#ifndef BALANCEWINDOW_HXX
#define BALANCEWINDOW_HXX

#include <QWidget>

class CSVModel;
class MonthlyBalanceGenerator;

class QLabel;
class QVBoxLayout;

class BalanceWindow: public QWidget {
  Q_OBJECT

public:
  explicit BalanceWindow(CSVModel* p_model, int p_year, int p_month, MonthlyBalanceGenerator* p_monthlyBalanceGenerator, QWidget* p_parent = nullptr);
  inline int GetYear() const {return m_year;}
  inline int GetMonth() const {return m_month;}

  void RefreshCategories();

protected:
  void UpdateBalance();

private:
  CSVModel* m_model;
  int m_year;
  int m_month;
  QStringList m_categoriesChecked;
  QLabel* m_balance;
  MonthlyBalanceGenerator* m_monthlyBalanceGenerator;
  QVBoxLayout* m_categoriesLayout;
};

#endif
