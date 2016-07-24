#include "MonthlyChartGenerator.hxx"

#include "CSVModel.hxx"

#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QRegularExpression>

#include <QDebug>

QT_CHARTS_USE_NAMESPACE

MonthlyChartGenerator::MonthlyChartGenerator(CSVModel* p_model, int p_year, int p_month, QObject* p_object):
  m_model(p_model),
  m_year(p_year),
  m_month(p_month) {

  setParent(p_object);
}

QtCharts::QChartView* MonthlyChartGenerator::createChartView() {
  QVector<QBarSet*> barSetsVector;
  QStringList categoriesList;
  auto series = new QBarSeries;

  for (int k = 0; k < m_model->rowCount(); ++k) {
    auto category = m_model->index(k, CSVModel::eCategory).data().toString();
    auto categoryIndex = categoriesList.indexOf(QRegularExpression(category));
    if (categoryIndex == -1) {
      auto barSet = new QBarSet(category, this);
      *barSet << 0;
      barSetsVector.append(barSet);
      categoriesList << category;
      series->append(barSet);
      categoryIndex = categoriesList.size()-1;
    }

    auto newAmount = barSetsVector.at(categoryIndex)->at(0);
    auto debitStr = m_model->index(k, CSVModel::eDebit).data().toString();
    newAmount -= debitStr.remove(debitStr.length()-1, 1).toFloat();
    auto creditStr = m_model->index(k, CSVModel::eCredit).data().toString();
    newAmount -= creditStr.remove(creditStr.length()-1, 1).toFloat();
    barSetsVector.value(categoryIndex)->replace(0, newAmount);
  }

  auto chart = new QChart;
  chart->addSeries(series);
  chart->setTitle(QString("Debit for ") + QDate(m_year, m_month, 1).toString("MMMM yyyy"));
  chart->setAnimationOptions(QChart::SeriesAnimations);
  chart->createDefaultAxes();
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);

  auto chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  return chartView;
}
