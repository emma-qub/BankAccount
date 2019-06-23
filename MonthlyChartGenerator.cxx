#include "MonthlyChartGenerator.hxx"

#include "CSVModel.hxx"

#include <QtCharts/QBarSeries>
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

QtCharts::QChartView* MonthlyChartGenerator::CreateChartView() {
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
      categoryIndex = categoriesList.size()-1;
    }

    auto newAmount = barSetsVector.at(categoryIndex)->at(0);
    auto debitStr = m_model->index(k, CSVModel::eDebit).data().toString();
    newAmount -= debitStr.remove(debitStr.length()-1, 1).toFloat();
    auto creditStr = m_model->index(k, CSVModel::eCredit).data().toString();
    newAmount -= creditStr.remove(creditStr.length()-1, 1).toFloat();
    barSetsVector.value(categoryIndex)->replace(0, newAmount);
  }

  qSort(barSetsVector.begin(), barSetsVector.end(), LessThan);
  for (auto barSet: barSetsVector)
    series->append(barSet);

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

bool LessThan(QBarSet const* p_barSet1, QBarSet const* p_barSet2) {
  return p_barSet1->at(0) < p_barSet2->at(0);
}
