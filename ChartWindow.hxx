#ifndef CHARTWIDGET_HXX
#define CHARTWIDGET_HXX

#include "MonthlyChartGenerator.hxx"
#include "CSVModel.hxx"

#include <QWidget>
#include <QVBoxLayout>
#include <QChartView>

class ChartWindow: public QWidget {
  Q_OBJECT

public:
  explicit ChartWindow(CSVModel* p_model, int p_year, int p_month, QWidget* p_parent = nullptr);
  inline int getYear() const {return m_year;}
  inline int getMonth() const {return m_month;}

  void updateChart(int p_year, int p_month);

private:
  CSVModel* m_model;
  int m_year;
  int m_month;
  MonthlyChartGenerator* m_monthlyChartGenerator;
  QVBoxLayout* m_mainLayout;
  QtCharts::QChartView* m_chartView;
};

#endif // CHARTWIDGET_HXX
