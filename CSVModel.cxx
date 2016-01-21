#include "CSVModel.hxx"

#include <QFile>
#include <QTextStream>

CSVModel::CSVModel(QObject* p_parent):
QAbstractTableModel(p_parent) {
}

QVariant CSVModel::data(QModelIndex const& p_index, int p_role) const {
  if (p_index.isValid()) {
    if (p_role == Qt::DisplayRole || p_role == Qt::EditRole) {
      return m_data.at(p_index.row(), p_index.column());
    }
  }
  return QVariant();
}

bool CSVModel::setData(QModelIndex const& p_index, QVariant const& p_value, int p_role) {
  if (p_index.isValid() && p_role == Qt::EditRole) {
    return m_data.setValue(p_index.row(), p_index.column(), p_value.toString());
  }
  return false;
}

Qt::ItemFlags CSVModel::flags(QModelIndex const& p_index) const {
  Qt::ItemFlags flags = QAbstractItemModel::flags(p_index);
  flags |= Qt::ItemIsEditable;
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
