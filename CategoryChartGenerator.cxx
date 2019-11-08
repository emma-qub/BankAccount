#include "CategoryChartGenerator.hxx"

#include "CSVModel.hxx"

#include <QFile>
#include <QTextStream>
#include <QtCharts/QBarSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QStackedBarSeries>

QT_CHARTS_USE_NAMESPACE

CategoryChartGenerator::CategoryChartGenerator(QStringList const& p_categoriesList, QDate const& p_beginDate, QDate const& p_endDate, QObject* p_parent):
  QObject(p_parent),
  m_categoriesList(p_categoriesList),
  m_beginDate(p_beginDate),
  m_endDate(p_endDate),
  m_averageAmount(0.),
  m_totalAmount(0.) {

  if (m_beginDate > m_endDate) {
    QDate tmpDate = m_beginDate;
    m_beginDate = m_endDate;
    m_endDate = tmpDate;
  }
}

QtCharts::QChartView* CategoryChartGenerator::CreateChartView() {
  Q_ASSERT_X(m_beginDate <= m_endDate, "CategoryChartGenerator::CreateChartView()", "End date is superior to begin date");

  QDate currDate = m_beginDate;
  int monthsCount = 0;
  QMap<QString, double> totalAmounts;
  QStringList monthLabels;
  double totalAmount = 0.;

  QMap<QString, QBarSet*> barSets;
  auto stackedSeries = new QStackedBarSeries;
  for (auto const& category: m_categoriesList) {
    auto barSet = new QBarSet(category);
    barSets[category] = barSet;
    stackedSeries->append(barSet);
    totalAmounts[category] = 0.;
  }

  while (currDate <= m_endDate) {
    auto amounts = GetCategoryAmount(currDate);
    for (auto const& category: m_categoriesList) {
      *barSets[category] << amounts[category];
      totalAmounts[category] += amounts[category];
      totalAmount += amounts[category];
    }
    monthLabels << currDate.toString("MMMM yyyy");

    currDate = currDate.addMonths(1);
    ++monthsCount;
  }

  auto chart = new QChart;
  chart->addSeries(stackedSeries);

  auto amountAxis = new QValueAxis;
  chart->addAxis(amountAxis, Qt::AlignLeft);

  int count = 0;
  for (auto const& category: m_categoriesList) {
    auto averageSeries = new QLineSeries;
    averageSeries->setName(tr("Moyenne %1").arg(category));
    auto averageAmount = totalAmounts[category]/monthsCount;
    averageSeries->append(0, averageAmount);
    averageSeries->append(1, averageAmount);
    averageSeries->setColor(stackedSeries->barSets().at(count)->color());
    chart->addSeries(averageSeries);
    averageSeries->attachAxis(amountAxis);

    ++count;
  }

  m_totalAmount = totalAmount;
  m_averageAmount = totalAmount/monthsCount;

  auto averageSeries = new QLineSeries;
  averageSeries->append(0, m_averageAmount);
  averageSeries->append(1, m_averageAmount);
  averageSeries->setName(tr("Moyenne globale"));
  averageSeries->setColor(QColor("#333333"));
  chart->addSeries(averageSeries);
  averageSeries->attachAxis(amountAxis);

  chart->setTitle(QString("Debit"));
  chart->setAnimationOptions(QChart::SeriesAnimations);
  auto monthsAxis = new QBarCategoryAxis;
  monthsAxis->append(monthLabels);
  chart->addAxis(monthsAxis, Qt::AlignBottom);
  stackedSeries->attachAxis(monthsAxis);
  stackedSeries->attachAxis(amountAxis);
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);

  auto chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  return chartView;
}

QMap<QString, double> CategoryChartGenerator::GetCategoryAmount(QDate const& p_date) const {
  auto fileName = GetCurrentCSVFileName(p_date);
  QMap<QString, double> amounts;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    return amounts;
  }

  for (auto const& category: m_categoriesList) {
    amounts[category] = 0.;
  }

  QTextStream in(&file);
  in.setCodec("UTF-8");
  QString currentLine;
  while (!in.atEnd()) {
    currentLine = in.readLine();
    if (currentLine.isEmpty()) {
      continue;
    }
    auto tokensList = currentLine.split(';');
    auto category = tokensList.at(CSVModel::eCategory);
    if (m_categoriesList.contains(category)) {
      auto debitStr = tokensList.at(CSVModel::eDebit);
      amounts[category] -= debitStr.remove(debitStr.length()-1, 1).toDouble();
      auto creditStr = tokensList.at(CSVModel::eCredit);
      amounts[category] -= creditStr.remove(debitStr.length()-1, 1).toDouble();
    }
  }
  file.close();

  return amounts;
}

QString CategoryChartGenerator::GetCurrentCSVFileName(QDate const& p_date) const {
  return QString("../BankAccount/csv/"+p_date.toString("MM-yyyy")+"/operations.csv");
}
