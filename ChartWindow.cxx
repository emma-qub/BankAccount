#include "ChartWindow.hxx"

#include "MonthlyChartGenerator.hxx"

ChartWindow::ChartWindow(CSVModel* p_model, int p_year, int p_month, QWidget* p_parent):
  QWidget(p_parent),
  m_model(p_model),
  m_year(p_year),
  m_month(p_month) {

  m_monthlyChartGenerator = new MonthlyChartGenerator(m_model, m_year, m_month, this);
  m_mainLayout = new QVBoxLayout;
  m_chartView = m_monthlyChartGenerator->createChartView();

  m_mainLayout->addWidget(m_chartView);

  setLayout(m_mainLayout);
}

void ChartWindow::updateChart(int p_year, int p_month) {
  m_year = p_year;
  m_month = p_month;
  m_monthlyChartGenerator->setYear(m_year);
  m_monthlyChartGenerator->setMonth(m_month);

  m_mainLayout->removeWidget(m_chartView);
  delete m_chartView;
  m_chartView = m_monthlyChartGenerator->createChartView();
  m_mainLayout->addWidget(m_chartView);
}
