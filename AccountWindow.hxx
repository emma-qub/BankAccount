#ifndef ACCOUNTWINDOW_HXX
#define ACCOUNTWINDOW_HXX

#include <QWidget>
#include <QMap>
#include <QModelIndex>

class CSVModel;
class CategoriesModel;
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

  explicit AccountWindow(CSVModel* p_csvModel, CategoriesModel* p_categoriesModel, QWidget* parent = nullptr);

protected:
  QString GetCurrentCSVFileName() const;
  void UpdateSummary();
  void GetBudgetAmounts();
  void AddSeparator();
  void UpdateBuget(QModelIndex const& p_parentIndex = QModelIndex());
  void TryToFillClassicOperations();

protected Q_SLOTS:
  void GoToPreviousYear();
  void GoToPreviousMonth();
  void GoToNextMonth();
  void GoToNextYear();
  void FillModel();
  void SaveCategory(int p_row, QString const& p_group, QString const& p_category);
  void ReloadFile();
  void UpdatePercentage(QModelIndex const& p_index);
  void SaveBudget(QModelIndex const& p_index);

Q_SIGNALS:
  void MonthChanged();
  void YearChanged();
  void UpdateModelRequested();

private:
  CSVModel* m_csvModel;
  CategoriesModel* m_categoriesModel;
  QTableView* m_tableView;

  QLabel* m_monthLabel;
  QLabel* m_yearLabel;
  QPushButton* m_previousYear;
  QPushButton* m_previousMonth;
  QPushButton* m_nextMonth;
  QPushButton* m_nextYear;

  QStandardItemModel* m_summaryModel;
  QTreeView* m_summaryView;
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

  QMap<QString, QStandardItem*> m_fixedChargesLabelsMap;
  QMap<QString, QStandardItem*> m_fixedChargesValuesMap;

  QMap<QString, QStandardItem*> m_variableChargesLabelsMap;
  QMap<QString, QStandardItem*> m_variableChargesValuesMap;

  QMap<QString, QStandardItem*> m_profitLabelsMap;
  QMap<QString, QStandardItem*> m_profitValuesMap;

  QMap<QString, QStandardItem*> m_foodLabelsMap;
  QMap<QString, QStandardItem*> m_foodValuesMap;

};

Q_DECLARE_METATYPE(AccountWindow::CategoryType)

#endif
