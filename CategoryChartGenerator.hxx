#ifndef CATEGORYCHARTGENERATOR_HXX
#define CATEGORYCHARTGENERATOR_HXX

#include <QObject>
#include <QtCharts/QChartView>
#include <QDate>
#include <QString>

class CategoryChartGenerator: public QObject {
  Q_OBJECT

public:
  CategoryChartGenerator(QString const& p_category, QDate const& p_beginDate, QDate const& p_endDate, QObject* p_parent = nullptr);

  QtCharts::QChartView* CreateChartView();

  inline void SetCategory(QString const& p_category) { m_category = p_category; }
  inline void SetBeginDate(QDate const& p_beginDate) { m_beginDate = p_beginDate; }
  inline void SetEndDate(QDate const& p_endDate) { m_endDate = p_endDate; }

  inline double GetAverageAmount() const { return m_averageAmount; }
  inline double GetTotalAmount() const { return m_totalAmount; }

protected:
  double GetCategoryAmount(QDate const& p_date) const;
  QString GetCurrentCSVFileName(QDate const& p_date) const;

private:
  QString m_category;
  QDate m_beginDate;
  QDate m_endDate;
  double m_averageAmount;
  double m_totalAmount;
};

#endif
