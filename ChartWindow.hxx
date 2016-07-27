#ifndef CHARTWIDGET_HXX
#define CHARTWIDGET_HXX

#include "MonthlyChartGenerator.hxx"
#include "CategoryChartGenerator.hxx"
#include "CSVModel.hxx"

#include <QWidget>
#include <QGridLayout>
#include <QComboBox>
#include <QtCharts/QChartView>

class ChartWindow: public QWidget {
  Q_OBJECT

public:
  explicit ChartWindow(CSVModel* p_model, int p_year, int p_month, QWidget* p_parent = nullptr);
  inline int getYear() const {return m_year;}
  inline int getMonth() const {return m_month;}

  void updateMonthlyChart(int p_year, int p_month);

protected slots:
  void updateCategoryChart(const QString& p_category);

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
  QTabWidget* m_tabWidget;
};

#endif // CHARTWIDGET_HXX
