#include "CategoryChartGenerator.hxx"

#include "CSVModel.hxx"

#include <QFile>
#include <QTextStream>
#include <QtCharts/QBarSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE

CategoryChartGenerator::CategoryChartGenerator(QString const& p_category, QDate const& p_beginDate, QDate const& p_endDate, QObject* p_parent):
  QObject(p_parent),
  m_category(p_category),
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
  if (m_beginDate > m_endDate) {
    qt_assert_x("QtCharts::QChartView* CategoryChartGenerator::createChartView() const",
      "End date is superior to begin date", "CategoryChartGenerator", 25);
  }

  auto series = new QBarSeries;

  QDate currDate = m_beginDate;
  int monthsCount = 0;
  double totalAmount = 0.;
  while (currDate <= m_endDate) {
    auto barSet = new QBarSet(currDate.toString("MMMM yyyy"), this);
    auto amount = GetCategoryAmount(currDate);
    *barSet << amount;
    series->append(barSet);
    currDate = currDate.addMonths(1);

    totalAmount += amount;
    ++monthsCount;
  }

  m_totalAmount = totalAmount;
  m_averageAmount = totalAmount / monthsCount;
  auto averageSeries = new QLineSeries;
  averageSeries->append(0, m_averageAmount);
  averageSeries->append(monthsCount, m_averageAmount);

  auto chart = new QChart;
  chart->addSeries(series);
  chart->addSeries(averageSeries);
  chart->setTitle(QString("Debit for ") + m_category);
  chart->setAnimationOptions(QChart::SeriesAnimations);
  chart->createDefaultAxes();
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);

  auto chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  return chartView;
}

double CategoryChartGenerator::GetCategoryAmount(const QDate p_date) const {
  auto fileName = GetCurrentCSVFileName(p_date);
  double amount = 0;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    return amount;
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
    if (tokensList.at(CSVModel::eCategory) == m_category) {
      auto debitStr = tokensList.at(CSVModel::eDebit);
      amount -= debitStr.remove(debitStr.length()-1, 1).toDouble();
      auto creditStr = tokensList.at(CSVModel::eCredit);
      amount -= creditStr.remove(debitStr.length()-1, 1).toDouble();
    }
  }
  file.close();

  return amount;
}

QString CategoryChartGenerator::GetCurrentCSVFileName(QDate const& p_date) const {
  return QString("../BankAccount/csv/"+p_date.toString("MM-yyyy")+"/operations.csv");
}
