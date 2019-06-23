#ifndef CHARTWIDGET_HXX
#define CHARTWIDGET_HXX

#include "MonthlyChartGenerator.hxx"
#include "CategoryChartGenerator.hxx"
#include "MonthlyBalanceGenerator.hxx"
#include "BalanceWindow.hxx"
#include "CSVModel.hxx"

#include <QWidget>

class QGridLayout;
class QComboBox;
class QTabWidget;
namespace QtCharts {
class QChartView;
}

class ChartWindow: public QWidget {
  Q_OBJECT

public:
  explicit ChartWindow(CSVModel* p_model, int p_year, int p_month, QWidget* p_parent = nullptr);
  inline int GetYear() const {return m_year;}
  inline int GetMonth() const {return m_month;}

  void UpdateMonthlyChart(int p_year, int p_month);
  void UpdateMonthlyBalance(int p_year, int p_month);

protected slots:
  void UpdateCategoryChart(const QString& p_category);

private:
  CSVModel* m_model;
  int m_year;
  int m_month;
  MonthlyChartGenerator* m_monthlyChartGenerator;
  QVBoxLayout* m_categoryChartLayout;
  QVBoxLayout* m_mainLayout;
  QtCharts::QChartView* m_monthlyChartView;
  QtCharts::QChartView* m_categoryChartView;
  QComboBox* m_categoryComboBox;
  CategoryChartGenerator* m_categoryChartGenerator;
  BalanceWindow* m_balanceWidget;
  MonthlyBalanceGenerator* m_monthlyBalanceGenerator;
  QTabWidget* m_tabWidget;
};

#endif
