#include "ChartWindow.hxx"
#include "Utils.hxx"

ChartWindow::ChartWindow(CSVModel* p_model, int p_year, int p_month, QWidget* p_parent):
  QWidget(p_parent),
  m_model(p_model),
  m_year(p_year),
  m_month(p_month) {

  m_monthlyChartGenerator = new MonthlyChartGenerator(m_model, m_year, m_month, this);
  m_mainLayout = new QGridLayout;
  m_monthlyChartView = m_monthlyChartGenerator->createChartView();
  m_monthlyChartLayout = new QVBoxLayout;
  m_monthlyChartLayout->addWidget(m_monthlyChartView);
  m_mainLayout->addLayout(m_monthlyChartLayout, 0, 0);

  m_categoryComboBox = new QComboBox;
  m_categoryComboBox->addItems(Utils::CATEGORIES);
  connect(m_categoryComboBox, &QComboBox::currentTextChanged, this, &ChartWindow::updateCategoryChart);
  m_categoryChartGenerator = new CategoryChartGenerator(m_categoryComboBox->currentText(), QDate(2016, 1, 1), QDate(2016, 12, 1));
  m_categoryChartLayout = new QVBoxLayout;
  m_categoryChartLayout->addWidget(m_categoryComboBox, 0, Qt::AlignCenter);
  m_categoryChartView = m_categoryChartGenerator->createChartView();
  m_categoryChartLayout->addWidget(m_categoryChartView);

  m_mainLayout->addLayout(m_categoryChartLayout, 1, 0);

  setLayout(m_mainLayout);
}

void ChartWindow::updateMonthlyChart(int p_year, int p_month) {
  m_year = p_year;
  m_month = p_month;
  m_monthlyChartGenerator->setYear(m_year);
  m_monthlyChartGenerator->setMonth(m_month);

  m_monthlyChartLayout->removeWidget(m_monthlyChartView);
  delete m_monthlyChartView;
  m_monthlyChartView = m_monthlyChartGenerator->createChartView();
  m_monthlyChartLayout->addWidget(m_monthlyChartView);
}

void ChartWindow::updateCategoryChart(QString const& p_category) {
  m_categoryChartGenerator->setCategory(p_category);

  m_categoryChartLayout->removeWidget(m_categoryChartView);
  delete m_categoryChartView;
  m_categoryChartView = m_categoryChartGenerator->createChartView();
  m_categoryChartLayout->addWidget(m_categoryChartView);
}
