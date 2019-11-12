#ifndef CATEGORYCHARTGENERATOR_HXX
#define CATEGORYCHARTGENERATOR_HXX

#include <QObject>
#include <QtCharts/QChartView>
#include <QDate>
#include <QString>

namespace QtCharts {
class QChart;
class QBarSet;
class QStackedBarSeries;
}

class CategoryChartGenerator: public QObject {
  Q_OBJECT

public:
  CategoryChartGenerator(QtCharts::QChartView* p_categoryChartView, QStringList const& p_categoriesList, QDate const& p_beginDate, QDate const& p_endDate, QObject* p_parent = nullptr);

  void UpdateChartView();

  inline void SetCategory(QStringList const& p_categoriesList) { m_categoriesList = p_categoriesList; }
  inline void SetBeginDate(QDate const& p_beginDate) { m_beginDate = p_beginDate; }
  inline void SetEndDate(QDate const& p_endDate) { m_endDate = p_endDate; }

  inline double GetAverageAmount() const { return m_averageAmount; }
  inline double GetTotalAmount() const { return m_totalAmount; }

protected:
  QMap<QString, double> GetCategoryAmount(QDate const& p_date) const;
  QString GetCurrentCSVFileName(QDate const& p_date) const;
  void UpdateCurrentCumul(bool p_status, int p_index, QtCharts::QBarSet* p_barset);

Q_SIGNALS:
  void HoveredAverageChanged(double p_cumul, QString const& p_category);
  void HoveredCumulChanged(double p_cumul, QString const& p_category);

private:
  QStringList m_categoriesList;
  QDate m_beginDate;
  QDate m_endDate;
  double m_averageAmount;
  double m_totalAmount;
  QtCharts::QChartView* m_categoryChartView;
  QtCharts::QChart* m_categoryChart;
  QtCharts::QStackedBarSeries* m_stackedSeries;
};

#endif
