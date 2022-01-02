#include "CategoryChartGenerator.hxx"

#include "CategoriesModel.hxx"

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

#include <limits>

#include <QDebug>

QT_CHARTS_USE_NAMESPACE

CategoryChartGenerator::CategoryChartGenerator(QtCharts::QChartView* p_categoryChartView, QDate const& p_beginDate, QDate const& p_endDate, QObject* p_parent):
  QObject(p_parent),
  m_chartType(eNone),
  m_groupsList({}),
  m_categoriesList({}),
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

  if (m_chartType == eCategories || m_chartType == eGroups) {
    auto const& list = GetList();

    for (auto const& category: list) {
      auto barSet = new QBarSet(category);
      barSets[category] = barSet;
      m_stackedSeries->append(barSet);
      totalAmounts[category] = 0.;
    }

    while (currDate <= m_endDate) {
      auto amounts = GetCategoryAmount(currDate);
      for (auto const& label: list) {
        *barSets[label] << amounts[label];
        totalAmounts[label] += amounts[label];
        totalAmount += amounts[label];
      }
      monthLabels << currDate.toString("MM/yy");

      currDate = currDate.addMonths(1);
      ++monthsCount;
    }

    auto amountAxis = new QValueAxis;
    m_categoryChart->addAxis(amountAxis, Qt::AlignLeft);
    m_categoryChart->addSeries(m_stackedSeries);

    int count = 0;
    for (auto const& label: list) {
      auto averageSeries = new QLineSeries;
      averageSeries->setName(tr("Moyenne %1").arg(label));
      auto averageAmount = totalAmounts[label]/monthsCount;
      averageSeries->append(0, averageAmount);
      averageSeries->append(1, averageAmount);
      averageSeries->setColor(m_stackedSeries->barSets().at(count)->color());
      m_categoryChart->addSeries(averageSeries);
      averageSeries->attachAxis(amountAxis);
      connect(averageSeries, &QLineSeries::hovered, this, [this, averageAmount, label](QPointF const&, bool p_state) {
        p_state? Q_EMIT HoveredAverageChanged(averageAmount, label): Q_EMIT HoveredAverageChanged(std::numeric_limits<double>::min(), "");
      });

      ++count;
    }

    m_totalAmount = totalAmount;
    m_averageAmount = totalAmount/monthsCount;

    auto averageSeries = new QLineSeries;
    averageSeries->append(0, m_averageAmount);
    averageSeries->append(1, m_averageAmount);
    averageSeries->setName(tr("Global Average"));
    averageSeries->setColor(QColor(333333));
    connect(averageSeries, &QLineSeries::hovered, this, [this](QPointF const&, bool p_state) {
      p_state? Q_EMIT HoveredAverageChanged(m_averageAmount, tr("Global Average")): Q_EMIT HoveredAverageChanged(std::numeric_limits<double>::min(), "");
    });
    m_categoryChart->addSeries(averageSeries);
    averageSeries->attachAxis(amountAxis);

    auto monthsAxis = new QBarCategoryAxis;
    monthsAxis->append(monthLabels);
    monthsAxis->setLabelsAngle(-90);
    m_categoryChart->addAxis(monthsAxis, Qt::AlignBottom);
    m_stackedSeries->attachAxis(monthsAxis);
    m_stackedSeries->attachAxis(amountAxis);
  } else if (m_chartType == eBalance) {

    // Init balance bar set
    QString balanceLabel("Balance");
    auto balanceBarSet = new QBarSet(balanceLabel);

    // Init cumul bar set
    QString cumulLabel("Cumul");
    auto cumulBarSet = new QBarSet(cumulLabel);

    // Init bar series
    auto barSeries = new QBarSeries;
    barSeries->append(balanceBarSet);
    barSeries->append(cumulBarSet);

    // Init average series
    auto averageSeries = new QLineSeries;

    // Compute balance, cumul and average
    auto cumulAmount = 0.;
    auto minAxis = 0.;
    auto maxAxis = 0.;

    while (currDate <= m_endDate) {
      auto amounts = GetCategoryAmount(currDate);
      auto balanceAmount = 0.;
      for (auto const& label: CategoriesModel::GROUP_BY_NAME.keys()) {
        balanceAmount -= amounts[label];
      }
      balanceBarSet->append(balanceAmount);
      cumulAmount += balanceAmount;
      cumulBarSet->append(cumulAmount);
      monthLabels << currDate.toString("MM/yy");

      minAxis = qMin(minAxis, qMin(balanceAmount, cumulAmount));
      maxAxis = qMax(maxAxis, qMax(balanceAmount, cumulAmount));

      currDate = currDate.addMonths(1);
      ++monthsCount;
    }

    m_totalAmount = cumulAmount;
    m_averageAmount = cumulAmount/monthsCount;

    averageSeries->append(0, m_averageAmount);
    averageSeries->append(1, m_averageAmount);
    averageSeries->setName(tr("Global Average"));
    averageSeries->setColor(QColor(333333));
    connect(averageSeries, &QLineSeries::hovered, this, [this](QPointF const&, bool p_state) {
      p_state? Q_EMIT HoveredAverageChanged(m_averageAmount, tr("Global Average")): Q_EMIT HoveredAverageChanged(std::numeric_limits<double>::min(), "");
    });

    // Create axis
    auto amountAxis = new QValueAxis;
    amountAxis->setMin(minAxis);
    amountAxis->setMax(maxAxis);
    auto monthsAxis = new QBarCategoryAxis;
    monthsAxis->append(monthLabels);
    monthsAxis->setLabelsAngle(-90);

    // Add axis to chart
    m_categoryChart->addAxis(amountAxis, Qt::AlignLeft);
    m_categoryChart->addAxis(monthsAxis, Qt::AlignBottom);

    // Add series to chart
    m_categoryChart->addSeries(barSeries);
    m_categoryChart->addSeries(averageSeries);

    // Attach axis to series
    barSeries->attachAxis(monthsAxis);
    barSeries->attachAxis(amountAxis);
    averageSeries->attachAxis(amountAxis);
  }
}

QMap<QString, double> CategoryChartGenerator::GetCategoryAmount(QDate const& p_date) const {
  auto fileName = GetCurrentCSVFileName(p_date);
  QMap<QString, double> amounts;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    return amounts;
  }

  QStringList list;
  if (m_chartType == eBalance)
    list = CategoriesModel::GROUP_BY_NAME.keys();
  else
    list = GetList();

  for (auto const& category: list) {
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
    auto label = tokensList.at(GetColumnName());
    if (list.contains(label)) {
      auto debitStr = tokensList.at(CSVModel::eDebit);
      amounts[label] -= debitStr.remove(debitStr.length()-1, 1).toDouble();
      auto creditStr = tokensList.at(CSVModel::eCredit);
      amounts[label] -= creditStr.remove(debitStr.length()-1, 1).toDouble();
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
    Q_EMIT HoveredCumulChanged(cumul, m_stackedSeries->barSets().at(m_stackedSeries->barSets().indexOf(p_barset))->label());
  } else {
    Q_EMIT HoveredCumulChanged(std::numeric_limits<double>::min(), "");
  }
}

QStringList const& CategoryChartGenerator::GetList() const {
  switch (m_chartType) {
  case eGroups:
    return m_groupsList;
  case eCategories:
    return m_categoriesList;
  case eBalance:
  case eNone:
    Q_ASSERT_X(false, "CategoryChartGenerator::GetChartTypeList", "Chart type is not set");
  }
}

CSVModel::ColumnName CategoryChartGenerator::GetColumnName() const {
  switch (m_chartType) {
  case eGroups:
    return CSVModel::eGroup;
  case eBalance:
    return CSVModel::eGroup;
  case eCategories:
    return CSVModel::eCategory;
  case eNone:
    Q_ASSERT_X(false, "CategoryChartGenerator::GetChartTypeLabel", "Chart type is not set");
  }
}
