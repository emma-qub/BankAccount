#include "CSVModel.hxx"

#include <QFile>
#include <QTextStream>
#include <QColor>
#include <QFont>

CSVModel::CSVModel(QObject* p_parent):
  QAbstractTableModel(p_parent) {
}

QVariant CSVModel::data(QModelIndex const& p_index, int p_role) const {
  if (p_index.isValid()) {
    auto column = p_index.column();
    if (p_role == Qt::DisplayRole || p_role == Qt::EditRole) {
      return m_data.at(p_index.row(), column);
    } else if (p_role == Qt::ForegroundRole && column == eCredit) {
      return QColor("#80c342");
    } else if (p_role == Qt::FontRole && (column == eDebit|| column == eCredit)) {
      QFont font;
      font.setBold(true);
      return font;
    } else if (p_role == Qt::TextAlignmentRole) {
      if (column == eDate || column == eCategory) {
        return Qt::AlignCenter;
      } else if (column == eDebit || column == eCredit) {
        return Qt::AlignRight;
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
    case eDate:
      headerTitle = QString("Date");
      break;
    case eCategory:
      headerTitle = QString("Category");
      break;
    case eOperationType:
      headerTitle = QString("Operation");
      break;
    case eLabel:
      headerTitle = QString("Label");
      break;
    case eDebit:
      headerTitle = QString("Debit");
      break;
    case eCredit:
      headerTitle = QString("Credit");
      break;
    default:
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

//bool CSVModel::setData(QModelIndex const& p_index, QVariant const& p_value, int p_role) {
//  if (p_index.isValid() && p_role == Qt::EditRole) {
//    return m_data.setValue(p_index.row(), p_index.column(), p_value.toString());
//  }
//  return false;
//}

Qt::ItemFlags CSVModel::flags(QModelIndex const& p_index) const {
  Qt::ItemFlags flags = QAbstractItemModel::flags(p_index);
  if (p_index.column() == eCategory)
    flags |= Qt::ItemIsEditable;
  else
    flags &= ~Qt::ItemIsEditable;
  return flags;
}

bool CSVModel::setSource(const QString& p_fileName, bool p_withHeader, const QChar& p_delim) {
  m_data.clear();

  QFile file(p_fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    return false;
  }

  QTextStream in(&file);
  QString currentLine;
  if (p_withHeader) {
    currentLine = in.readLine();
  }
  while (!(currentLine = in.readLine()).isEmpty()) {
    auto tokensList = currentLine.split(p_delim);
    m_data.append(tokensList);
  }
  file.close();

  return true;
}
