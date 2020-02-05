#include "CSVModel.hxx"

#include "CategoriesModel.hxx"
#include "Utils.hxx"

#include <QFile>
#include <QTextStream>
#include <QColor>
#include <QFont>
#include <QRegularExpression>

CSVModel::CSVModel(CategoriesModel* p_categoriesModel, QObject* p_parent):
  QAbstractTableModel(p_parent),
  m_categoriesModel(p_categoriesModel) {
}

QVariant CSVModel::data(QModelIndex const& p_index, int p_role) const {
  if (p_index.isValid()) {
    auto column = p_index.column();
    if (p_role == Qt::DisplayRole || p_role == Qt::EditRole) {
      return m_data.at(p_index.row(), column);
    } else if (p_role == Qt::ForegroundRole) {
      if (column == eCredit) {
        return Utils::GetGreenColor();
      } else if ((column == eCategory || column == eGroup)) {
        if (p_index.data() == "Unknown") {
          return Utils::GetOrangeColor();
        } else {
          if (column == eCategory && !CategoryBelongsToGroup(p_index)) {
            return Utils::GetRedColor();
          } else if (m_autoFilledMap[p_index.row()]) {
            return Utils::GetAutoFilledColor();
          }
        }
      }
    } else if (p_role == Qt::FontRole && IsBold(p_index)) {
      QFont font;
      font.setBold(true);
      return font;
    } else if (p_role == Qt::TextAlignmentRole) {
      if (column == eDate || column == eCategory || column == eGroup) {
        return Qt::AlignCenter;
      } else if (column == eDebit || column == eCredit) {
        return QVariant(Qt::AlignVCenter | Qt::AlignRight);
      }
    }
  }
  return QVariant();
}

QVariant CSVModel::headerData(int p_section, Qt::Orientation p_orientation, int p_role) const {
  if (p_orientation != Qt::Horizontal) {
    return QVariant();
  } else if (p_role == Qt::DisplayRole) {
    QString headerTitle;

    switch (p_section) {
    case eDate: {
      headerTitle = QString("Date");
      break;
    } case eGroup: {
      headerTitle = QString("Group");
      break;
    } case eCategory: {
      headerTitle = QString("Category");
      break;
    } case eOperationType: {
      headerTitle = QString("Operation");
      break;
    } case eLabel: {
      headerTitle = QString("Label");
      break;
    } case eDebit: {
      headerTitle = QString("Debit");
      break;
    } case eCredit: {
      headerTitle = QString("Credit");
      break;
    } default:
      break;
    }

    return headerTitle;
  } else if (p_role == Qt::FontRole) {
    QFont font;
    font.setBold(true);
    return font;
  }

  return QVariant();
}

bool CSVModel::setData(QModelIndex const& p_index, QVariant const& p_value, int p_role) {
  if (p_index.isValid() && p_role == Qt::EditRole) {
    QString group;
    QString category;
    if (p_index.column() == eGroup) {
      group = p_value.toString();
      category = data(p_index.sibling(p_index.row(), CSVModel::eCategory), Qt::DisplayRole).toString();
    } else if (p_index.column() == eCategory) {
      group = data(p_index.sibling(p_index.row(), CSVModel::eGroup), Qt::DisplayRole).toString();
      category = p_value.toString();
    } else {
      return false;
    }
    auto result = m_data.setValue(p_index.row(), p_index.column(), p_value.toString());
    if (result) {
      Q_EMIT SaveCategoryRequested(p_index.row(), group, category);
      if (p_index.column() == eCategory) {
        Q_EMIT UpdateSummaryRequested();
      }
      Q_EMIT dataChanged(p_index, p_index);
    }
    return result;
  }
  return false;
}

Qt::ItemFlags CSVModel::flags(QModelIndex const& p_index) const {
  Qt::ItemFlags flags = QAbstractItemModel::flags(p_index);
  (p_index.column() == eCategory || p_index.column() == eGroup) ?
    flags |= Qt::ItemIsEditable:
    flags &= ~Qt::ItemIsEditable;
  return flags;
}

bool CSVModel::SetSource(QString const& p_fileName, bool p_withHeader, QChar const& p_delim) {
  beginResetModel();
  m_data.clear();
  m_autoFilledMap.clear();

  QFile file(p_fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    // Create empty line, in order to prevent view from being reset
    m_data.AppendEmptyLine();
    endResetModel();
    return false;
  }

  QTextStream in(&file);
  in.setCodec("UTF-8");
  QString currentLine;
  if (p_withHeader) {
    currentLine = in.readLine();
  }
  while (!in.atEnd()) {
    currentLine = in.readLine();
    if (currentLine.isEmpty()) {
      continue;
    }
    auto tokensList = currentLine.split(p_delim);
    m_data.append(tokensList);
  }
  file.close();

  endResetModel();

  return true;
}

double CSVModel::GetCredit(int p_row) {
  return GetAmount(p_row, eCredit);
}

double CSVModel::GetDebit(int p_row) {
  return GetAmount(p_row, eDebit);
}

void CSVModel::ActivateAutoFilledAt(int p_row) {
  m_autoFilledMap[p_row] = true;
}

double CSVModel::GetAmount(int p_row, ColumnName p_column) {
  return m_data[p_row][p_column].remove("€").toDouble();
}

bool CSVModel::CategoryBelongsToGroup(QModelIndex const& p_categoryIndex) const {
  QModelIndex groupIndex = p_categoryIndex.siblingAtColumn(eGroup);

  return m_categoriesModel->GetCategoriesByGroup(groupIndex.data().toString()).contains(p_categoryIndex.data().toString());
}

bool CSVModel::IsBold(QModelIndex const& p_index) const {
  auto column = p_index.column();

  auto isDebit = (column == eDebit);
  auto isCredit = (column == eCredit);
  auto isUnknown = ((column == eCategory || column == eGroup) && p_index.data() == "Unknown");
  auto isNotACategory = (column == eCategory && !CategoryBelongsToGroup(p_index));
  auto isAutoFilled = m_autoFilledMap.contains(p_index.row());
  return isDebit || isCredit || isUnknown || isNotACategory || isAutoFilled;
}
