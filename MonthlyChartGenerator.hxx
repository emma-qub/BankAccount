#ifndef MONTHLYCHARTGENERATOR_HXX
#define MONTHLYCHARTGENERATOR_HXX

#include <QString>
#include <QDate>
#include <QtCharts/QChartView>

class CSVModel;

class MonthlyChartGenerator: public QObject {
public:
  MonthlyChartGenerator(CSVModel* p_model, int p_year, int p_month, QObject* p_object = nullptr);

  inline void setYear(int p_year) {m_year = p_year;}
  inline void setMonth(int p_month) {m_month = p_month;}

  QtCharts::QChartView* createChartView();

private:
  CSVModel* m_model;

  int m_year;
  int m_month;
};

#endif // MONTHLYCHARTGENERATOR_HXX
