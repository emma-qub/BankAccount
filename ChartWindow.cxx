#include "ChartWindow.hxx"
#include "Utils.hxx"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDateEdit>
#include <QtCharts/QChartView>

ChartWindow::ChartWindow(CSVModel* p_model, QWidget* p_parent):
  QWidget(p_parent),
  m_model(p_model) {

  // Category
  m_beginDateCalendar = new QDateEdit;
  m_beginDateCalendar->setDate(QDate(2017, 1, 1));
  m_beginDateCalendar->setCalendarPopup(true);
  connect(m_beginDateCalendar, &QDateEdit::dateTimeChanged, this, &ChartWindow::UpdateCategoryChart);
  m_endDateCalendar = new QDateEdit;
  m_endDateCalendar->setDate(QDate::currentDate());
  connect(m_endDateCalendar, &QDateEdit::dateTimeChanged, this, &ChartWindow::UpdateCategoryChart);
  m_categoryComboBox = new QComboBox;
  m_categoryComboBox->addItems(Utils::GetCategories());
  connect(m_categoryComboBox, &QComboBox::currentTextChanged, this, &ChartWindow::UpdateCategoryChart);
  m_categoryChartGenerator = new CategoryChartGenerator(
    m_categoryComboBox->currentText(), m_beginDateCalendar->date(), m_endDateCalendar->date());
  m_averageLabel = new QLabel;
  m_totalLabel = new QLabel;
  m_chartOptionsLayout = new QHBoxLayout;
  m_chartOptionsLayout->addWidget(m_beginDateCalendar);
  m_chartOptionsLayout->addWidget(m_endDateCalendar);
  m_chartOptionsLayout->addWidget(m_categoryComboBox);
  m_chartOptionsLayout->addWidget(m_averageLabel);
  m_chartOptionsLayout->addWidget(m_totalLabel);
  m_categoryChartLayout = new QVBoxLayout;
  m_categoryChartLayout->addLayout(m_chartOptionsLayout);
  m_categoryChartView = m_categoryChartGenerator->CreateChartView();
  m_categoryChartLayout->addWidget(m_categoryChartView);

  setLayout(m_categoryChartLayout);
}

void ChartWindow::UpdateCategoryChart() {
  m_categoryChartGenerator->SetCategory(m_categoryComboBox->currentText());
  m_categoryChartGenerator->SetBeginDate(m_beginDateCalendar->date());
  m_categoryChartGenerator->SetEndDate(m_endDateCalendar->date());

  m_categoryChartLayout->removeWidget(m_categoryChartView);
  delete m_categoryChartView;
  m_categoryChartView = m_categoryChartGenerator->CreateChartView();
  m_categoryChartLayout->addWidget(m_categoryChartView);

  m_averageLabel->setText(tr("Average: %1").arg(QString::number(m_categoryChartGenerator->GetAverageAmount())));
  m_totalLabel->setText(tr("Total: %1").arg(QString::number(m_categoryChartGenerator->GetTotalAmount())));
}
