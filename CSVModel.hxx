#ifndef CSVMODEL_HXX
#define CSVMODEL_HXX

#include <QAbstractTableModel>

template<typename T>
class TableData {
public:
  TableData(int p_rowCount = 0, int p_columnCount = 0):
    m_rowCount(p_rowCount),
    m_columnCount(p_columnCount) {

    if (p_rowCount > 0 && p_columnCount > 0) {
      for (int i = 0; i < p_rowCount; ++i) {
        QList<T> rowData;
        for (int j = 0; j < p_columnCount; ++j) {
          rowData.insert(j, T());
        }
        m_data.insert(i, rowData);
      }
    }
  }

  T at(int p_row, int p_column) const {
    if (p_row > m_rowCount || p_column > m_columnCount) {
      return T();
    }

    return m_data.at(p_row).at(p_column);
  }

  QList<T>& operator[](int p_row) {
    return m_data[p_row];
  }

  bool setValue(int p_row, int p_column, T const& p_value) {
    if (p_column >= m_columnCount) {
      m_columnCount = p_column + 1;
      for (int i = 0; i < m_rowCount; ++i) {
        QList<T> rowData = m_data.at(i);
        while (rowData.size() < columnCount()) {
          rowData.append(T());
        }
        m_data[i] = rowData;
      }
    }
    if (p_row >= m_rowCount) {
      m_rowCount = p_row + 1;
      while (m_data.size() < rowCount()) {
        QList<T> rowData;
        while (rowData.size() < columnCount()) {
          rowData.append(T());
        }
        m_data.append(rowData);
      }
    }

    m_data[p_row][p_column] = p_value;
    return true;
  }

  int rowCount() const {
    return m_rowCount;
  }

  int columnCount() const {
    return m_columnCount;
  }

  void clear() {
    m_rowCount = 0;
    m_columnCount = 0;
    m_data.clear();
  }

  void append(QList<T> const& p_line) {
    if (p_line.size() > m_columnCount) {
      m_columnCount = p_line.size();
    }

    m_data.append(p_line);
    ++m_rowCount;
  }

  void appendEmptyLine() {
    QStringList emptyLine;
    for (int k = 0; k < m_columnCount; ++k)
      emptyLine << "";
    m_data.append(emptyLine);
  }

private:
  QList<QList<T>> m_data;
  int m_rowCount;
  int m_columnCount;
};

class CSVModel: public QAbstractTableModel {
  Q_OBJECT

public:
  enum ColumnName {
    eDate = 0,
    eGroup,
    eCategory,
    eOperationType,
    eLabel,
    eDebit,
    eCredit,

    eColumnCount
  };

  explicit CSVModel(QObject* p_parent = nullptr);

  int rowCount(QModelIndex const& = QModelIndex()) const override { return m_data.rowCount(); }
  int columnCount(QModelIndex const& = QModelIndex()) const override { return m_data.columnCount(); }

  QVariant headerData(int p_section, Qt::Orientation p_orientation, int p_role) const override;
  QVariant data(QModelIndex const& p_index, int p_role = Qt::DisplayRole) const override;
  bool setData(QModelIndex const& p_index, QVariant const& p_value, int p_role = Qt::EditRole) override;
  Qt::ItemFlags flags(QModelIndex const& p_index) const override;

  bool setSource(QString const& p_fileName, bool p_withHeader = false, QChar const& p_delim = ';');

signals:
  void saveCategoryRequested(int, QString const&, QString const&);

private:
  TableData<QString> m_data;
};

#endif // CSVMODEL_HXX
