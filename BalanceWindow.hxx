#ifndef BALANCEWINDOW_HXX
#define BALANCEWINDOW_HXX

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "MonthlyBalanceGenerator.hxx"

class CSVModel;

class BalanceWindow: public QWidget {
  Q_OBJECT

public:
  explicit BalanceWindow(CSVModel* p_model, int p_year, int p_month, MonthlyBalanceGenerator* p_monthlyBalanceGenerator, QWidget* p_parent = nullptr);
  inline int getYear() const {return m_year;}
  inline int getMonth() const {return m_month;}

  void refreshCategories();

signals:

public slots:

protected:
  void updateBalance();

private:
  CSVModel* m_model;
  int m_year;
  int m_month;
  QStringList m_categoriesChecked;
  QLabel* m_balance;
  MonthlyBalanceGenerator* m_monthlyBalanceGenerator;
  QVBoxLayout* m_categoriesLayout;
};

#endif // BALANCEWINDOW_HXX
