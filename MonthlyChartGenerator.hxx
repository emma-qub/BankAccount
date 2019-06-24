#ifndef MONTHLYCHARTGENERATOR_HXX
#define MONTHLYCHARTGENERATOR_HXX

#include <QString>
#include <QDate>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>

class CSVModel;

class MonthlyChartGenerator: public QObject {
public:
  MonthlyChartGenerator(CSVModel* p_model, int p_year, int p_month, QObject* p_object = nullptr);

  inline void SetYear(int p_year) {m_year = p_year;}
  inline void SetMonth(int p_month) {m_month = p_month;}

  QtCharts::QChartView* CreateChartView();

private:
  CSVModel* m_model;

  int m_year;
  int m_month;
};

bool LessThan(QtCharts::QBarSet const* p_barSet1, QtCharts::QBarSet const* p_barSet2);

#endif
