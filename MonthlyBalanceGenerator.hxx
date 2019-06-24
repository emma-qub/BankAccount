#ifndef MONTHLYBALANCEGENERATOR_HXX
#define MONTHLYBALANCEGENERATOR_HXX

#include <QString>
#include <QDate>
#include <QObject>

class CSVModel;

class MonthlyBalanceGenerator: public QObject {
public:
  MonthlyBalanceGenerator(CSVModel* p_model, int p_year, int p_month, QObject* p_parent = nullptr);

  inline void SetYear(int p_year) {m_year = p_year;}
  inline void SetMonth(int p_month) {m_month = p_month;}

  QStringList CreateCategories() const;

  float GetBalance(const QStringList& p_selectedCategories);

private:
  CSVModel* m_model;

  int m_year;
  int m_month;
};

#endif
