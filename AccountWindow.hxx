#ifndef ACCOUNTWINDOW_HXX
#define ACCOUNTWINDOW_HXX

#include <QWidget>
#include <QMap>
#include <QModelIndex>

class CSVModel;
class QTableView;
class QStandardItemModel;
class QTreeView;
class QPushButton;
class QLabel;
class QStandardItem;
class QSortFilterProxyModel;

class AccountWindow: public QWidget {
  Q_OBJECT

public:
  enum ItemRoles {
    eIsItemSeparatorRole = Qt::UserRole+1,
    eCategoryRole,
    eCanHaveBudgetRole
  };

  enum CategoryType {
    eSalary,
    eFixedCharges,
    eVariableCharges,
    eFood,
    eProfit,
    eSaving
  };

  explicit AccountWindow(QWidget* parent = nullptr);

  inline CSVModel* GetModel() const {return m_csvModel;}
  inline int GetYear() const {return m_year;}
  inline int GetMonth() const {return m_month;}

protected:
  QString GetCurrentCSVFileName() const;
  void UpdateSummary();
  void GetBudgetAmounts();
  void AddSeparator();
  void UpdateBuget(const QModelIndex& p_parentIndex = QModelIndex());

protected slots:
  void GoToPreviousYear();
  void GoToPreviousMonth();
  void GoToNextMonth();
  void GoToNextYear();
  void FillModel();
  void SaveCategory(int p_row, const QString& p_group, const QString& p_category);
  void ReloadFile();
  void UpdatePercentage(QModelIndex const& p_index);
  void SaveBudget(QModelIndex const& p_index);

signals:
  void MonthChanged();
  void YearChanged();
  void UpdateModelRequested();

private:
  CSVModel* m_csvModel;
  QTableView* m_tableView;

  QLabel* m_monthLabel;
  QLabel* m_yearLabel;
  QPushButton* m_previousYear;
  QPushButton* m_previousMonth;
  QPushButton* m_nextMonth;
  QPushButton* m_nextYear;

  QStandardItemModel* m_categoryModel;
  QTreeView* m_categoryView;
  QStandardItem* m_salaryItem;
  QStandardItem* m_fixedChargesItem;
  QStandardItem* m_variableChargesItem;
  QStandardItem* m_foodItem;
  QStandardItem* m_savingItem;
  QStandardItem* m_profitItem;
  QStandardItem* m_balanceItem;
  QStandardItem* m_inItem;
  QStandardItem* m_outItem;

  int m_month;
  int m_year;

  QMap<QString, QString> m_budgetMap;

  QStringList m_fixedChargesList;
  QMap<QString, QStandardItem*> m_fixedChargesLabelsMap;
  QMap<QString, QStandardItem*> m_fixedChargesValuesMap;

  QStringList m_variableChargesList;
  QMap<QString, QStandardItem*> m_variableChargesLabelsMap;
  QMap<QString, QStandardItem*> m_variableChargesValuesMap;

  QStringList m_profitList;
  QMap<QString, QStandardItem*> m_profitLabelsMap;
  QMap<QString, QStandardItem*> m_profitValuesMap;

  QStringList m_foodList;
  QMap<QString, QStandardItem*> m_foodLabelsMap;
  QMap<QString, QStandardItem*> m_foodValuesMap;

};

Q_DECLARE_METATYPE(AccountWindow::CategoryType)

#endif
