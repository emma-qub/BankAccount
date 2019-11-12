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
#include <QtCharts/QChart>

QT_CHARTS_USE_NAMESPACE

CategoryChartGenerator::CategoryChartGenerator(QtCharts::QChartView* p_categoryChartView, QStringList const& p_categoriesList, QDate const& p_beginDate, QDate const& p_endDate, QObject* p_parent):
  QObject(p_parent),
  m_categoriesList(p_categoriesList),
  m_beginDate(p_beginDate),
  m_endDate(p_endDate),
  m_averageAmount(0.),
  m_totalAmount(0.),
  m_categoryChartView(p_categoryChartView),
  m_categoryChart(new QChart),
  m_stackedSeries(nullptr){

  m_categoryChartView->setChart(m_categoryChart);
  m_categoryChart->setTitle(QString("Debit"));
  m_categoryChart->setAnimationOptions(QChart::SeriesAnimations);
  m_categoryChart->legend()->setVisible(true);
  m_categoryChart->legend()->setAlignment(Qt::AlignBottom);

  if (m_beginDate > m_endDate) {
    QDate tmpDate = m_beginDate;
    m_beginDate = m_endDate;
    m_endDate = tmpDate;
  }
}

void CategoryChartGenerator::UpdateChartView() {
  Q_ASSERT_X(m_beginDate <= m_endDate, "CategoryChartGenerator::CreateChartView()", "End date is superior to begin date");

  QDate currDate = m_beginDate;
  int monthsCount = 0;
  QMap<QString, double> totalAmounts;
  QStringList monthLabels;
  double totalAmount = 0.;

  m_categoryChart->removeAllSeries();
  for (auto axe: m_categoryChart->axes()) {
    m_categoryChart->removeAxis(axe);
  }

  QMap<QString, QBarSet*> barSets;
  m_stackedSeries = new QStackedBarSeries;
  m_stackedSeries->setLabelsVisible(true);
  m_stackedSeries->setLabelsFormat("@value");
  connect(m_stackedSeries, &QStackedBarSeries::hovered, this, &CategoryChartGenerator::UpdateCurrentCumul);
  for (auto const& category: m_categoriesList) {
    auto barSet = new QBarSet(category);
    barSets[category] = barSet;
    m_stackedSeries->append(barSet);
    totalAmounts[category] = 0.;
  }

  while (currDate <= m_endDate) {
    auto amounts = GetCategoryAmount(currDate);
    for (auto const& category: m_categoriesList) {
      *barSets[category] << amounts[category];
      totalAmounts[category] += amounts[category];
      totalAmount += amounts[category];
    }
    monthLabels << currDate.toString("MM/yy");

    currDate = currDate.addMonths(1);
    ++monthsCount;
  }

  auto amountAxis = new QValueAxis;
  m_categoryChart->addAxis(amountAxis, Qt::AlignLeft);
  m_categoryChart->addSeries(m_stackedSeries);

  int count = 0;
  for (auto const& category: m_categoriesList) {
    auto averageSeries = new QLineSeries;
    averageSeries->setName(tr("Moyenne %1").arg(category));
    auto averageAmount = totalAmounts[category]/monthsCount;
    averageSeries->append(0, averageAmount);
    averageSeries->append(1, averageAmount);
    averageSeries->setColor(m_stackedSeries->barSets().at(count)->color());
    m_categoryChart->addSeries(averageSeries);
    averageSeries->attachAxis(amountAxis);
    connect(averageSeries, &QLineSeries::hovered, this, [this, averageAmount](QPointF const&, bool p_state) {
      p_state? Q_EMIT HoveredAverageChanged(averageAmount): Q_EMIT HoveredAverageChanged(0.);
    });

    ++count;
  }

  m_totalAmount = totalAmount;
  m_averageAmount = totalAmount/monthsCount;

  auto averageSeries = new QLineSeries;
  averageSeries->append(0, m_averageAmount);
  averageSeries->append(1, m_averageAmount);
  averageSeries->setName(tr("Moyenne globale"));
  averageSeries->setColor(QColor("#333333"));
  connect(averageSeries, &QLineSeries::hovered, this, [this](QPointF const&, bool p_state) {
    p_state? Q_EMIT HoveredAverageChanged(m_averageAmount): Q_EMIT HoveredAverageChanged(0.);
  });
  m_categoryChart->addSeries(averageSeries);
  averageSeries->attachAxis(amountAxis);

  auto monthsAxis = new QBarCategoryAxis;
  monthsAxis->append(monthLabels);
  m_categoryChart->addAxis(monthsAxis, Qt::AlignBottom);
  m_stackedSeries->attachAxis(monthsAxis);
  m_stackedSeries->attachAxis(amountAxis);
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

void CategoryChartGenerator::UpdateCurrentCumul(bool p_status, int p_index, QBarSet* p_barset) {
  if (p_status) {
    auto cumul = 0.;
    for (int i = 0; i <= m_stackedSeries->barSets().indexOf(p_barset); ++i) {
      cumul += m_stackedSeries->barSets().at(i)->at(p_index);
    }

    Q_EMIT HoveredCumulChanged(cumul);
  } else {
    Q_EMIT HoveredCumulChanged(0.);
  }
}
