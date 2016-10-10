#include "ChartWindow.hxx"
#include "Utils.hxx"

ChartWindow::ChartWindow(CSVModel* p_model, int p_year, int p_month, QWidget* p_parent):
  QWidget(p_parent),
  m_model(p_model),
  m_year(p_year),
  m_month(p_month) {

  m_tabWidget = new QTabWidget;

  // Monthly
  m_monthlyChartGenerator = new MonthlyChartGenerator(m_model, m_year, m_month, this);
  m_mainLayout = new QVBoxLayout;
  m_monthlyChartView = m_monthlyChartGenerator->createChartView();
  m_tabWidget->addTab(m_monthlyChartView, "Monthly");

  // Category
  m_categoryComboBox = new QComboBox;
  m_categoryComboBox->addItems(Utils::CATEGORIES);
  connect(m_categoryComboBox, &QComboBox::currentTextChanged, this, &ChartWindow::updateCategoryChart);
  m_categoryChartGenerator = new CategoryChartGenerator(m_categoryComboBox->currentText(), QDate(2016, 1, 1), QDate(2016, 12, 1));
  m_categoryChartLayout = new QVBoxLayout;
  m_categoryChartLayout->addWidget(m_categoryComboBox, 0, Qt::AlignCenter);
  m_categoryChartView = m_categoryChartGenerator->createChartView();
  m_categoryChartLayout->addWidget(m_categoryChartView);
  auto categoryWidget = new QWidget;
  categoryWidget->setLayout(m_categoryChartLayout);
  m_tabWidget->addTab(categoryWidget, "Categories");

  // Balance
  m_monthlyBalanceGenerator = new MonthlyBalanceGenerator(m_model, m_year, m_month, this);
  m_balanceWidget = new BalanceWindow(m_model, m_year, m_month, m_monthlyBalanceGenerator);
  m_tabWidget->addTab(m_balanceWidget, "Balance");

  m_mainLayout->addWidget(m_tabWidget);

  setLayout(m_mainLayout);
}

void ChartWindow::updateMonthlyChart(int p_year, int p_month) {
  m_year = p_year;
  m_month = p_month;
  m_monthlyChartGenerator->setYear(m_year);
  m_monthlyChartGenerator->setMonth(m_month);

  m_tabWidget->removeTab(0);
  delete m_monthlyChartView;
  m_monthlyChartView = m_monthlyChartGenerator->createChartView();
  m_tabWidget->insertTab(0, m_monthlyChartView, "Monthly");
}

void ChartWindow::updateCategoryChart(QString const& p_category) {
  m_categoryChartGenerator->setCategory(p_category);

  m_categoryChartLayout->removeWidget(m_categoryChartView);
  delete m_categoryChartView;
  m_categoryChartView = m_categoryChartGenerator->createChartView();
  m_categoryChartLayout->addWidget(m_categoryChartView);
}

void ChartWindow::updateMonthlyBalance(int p_year, int p_month) {
  m_year = p_year;
  m_month = p_month;
  m_monthlyBalanceGenerator->setYear(m_year);
  m_monthlyBalanceGenerator->setMonth(m_month);

  m_balanceWidget->refreshCategories();
}
