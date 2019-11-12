#include "ChartWindow.hxx"
#include "Utils.hxx"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDateEdit>
#include <QStandardItemModel>
#include <QPushButton>
#include <QMenu>

QT_CHARTS_USE_NAMESPACE

ChartWindow::ChartWindow(CSVModel* p_model, QWidget* p_parent):
  QWidget(p_parent),
  m_model(p_model),
  m_categoryChartView(new QChartView) {

  m_categoryChartView->setRenderHint(QPainter::Antialiasing);

  // Category
  m_beginDateCalendar = new QDateEdit;
  m_beginDateCalendar->setDate(QDate(QDate::currentDate().year(), 1, 1));
  m_beginDateCalendar->setCalendarPopup(true);
  connect(m_beginDateCalendar, &QDateEdit::dateTimeChanged, this, &ChartWindow::UpdateCategoryChart);
  m_endDateCalendar = new QDateEdit;
  m_endDateCalendar->setDate(QDate::currentDate());
  connect(m_endDateCalendar, &QDateEdit::dateTimeChanged, this, &ChartWindow::UpdateCategoryChart);

  m_categoryButton = new QPushButton("Select categories");
  m_categoryMenu = new QMenu;
  m_categoryButton->setMenu(m_categoryMenu);
  bool noAction = true;
  for (auto const& group: Utils::GROUP_BY_NAME.keys()) {
    if (!noAction) {
      m_categoryMenu->addSeparator();
    }
    auto groupAction = new QAction(group);
    m_categoryMenu->addAction(groupAction);
    auto groupFont = groupAction->font();
    groupFont.setBold(true);
    groupAction->setFont(groupFont);
    groupAction->setCheckable(true);
    connect(groupAction, &QAction::toggled, this, [this, groupAction](){Q_EMIT GroupToggled(groupAction);});
    m_actionsByGroup[groupAction] = QList<QAction*>();
    for (auto const& category: Utils::CATEGORIES_BY_GROUP[Utils::GROUP_BY_NAME[group]]) {
      auto action = new QAction(category);
      action->setCheckable(true);
      m_categoryMenu->addAction(action);
      connect(action, &QAction::toggled, this, &ChartWindow::UpdateCategoryChart);
      m_actionsByGroup[groupAction] << action;
    }
    noAction = false;
  }

  m_categoryChartGenerator = new CategoryChartGenerator(m_categoryChartView, {}, m_beginDateCalendar->date(), m_endDateCalendar->date());
  m_averageLabel = new QLabel(tr("Average: 0.00"));
  m_totalLabel = new QLabel(tr("Total: 0.00"));
  m_hoveredCumulLabel = new QLabel(tr("Hovered Cumul: 0.00"));
  m_hoveredAverageLabel = new QLabel(tr("Hovered Average: 0.00"));
  m_chartOptionsLayout = new QHBoxLayout;
  m_chartOptionsLayout->addWidget(m_beginDateCalendar);
  m_chartOptionsLayout->addWidget(m_endDateCalendar);
  m_chartOptionsLayout->addWidget(m_categoryButton);
  m_chartOptionsLayout->addWidget(m_averageLabel);
  m_chartOptionsLayout->addWidget(m_totalLabel);
  m_chartOptionsLayout->addWidget(m_hoveredCumulLabel);
  m_chartOptionsLayout->addWidget(m_hoveredAverageLabel);
  m_chartOptionsLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
  m_categoryChartLayout = new QVBoxLayout;
  m_categoryChartLayout->addLayout(m_chartOptionsLayout);
  m_categoryChartLayout->setAlignment(m_chartOptionsLayout, Qt::AlignTop);
  m_categoryChartLayout->addWidget(m_categoryChartView);

  connect(this, &ChartWindow::GroupToggled, this, &ChartWindow::SelectGroupAndUpdateCategoryChart);
  connect(m_categoryChartGenerator, &CategoryChartGenerator::HoveredCumulChanged, this, [this](double p_cumul) {
    m_hoveredCumulLabel->setText(tr("Hovered Cumul: %1").arg(QString::number(p_cumul, 'f', 2)));
  });
  connect(m_categoryChartGenerator, &CategoryChartGenerator::HoveredAverageChanged, this, [this](double p_average) {
    m_hoveredAverageLabel->setText(tr("Hovered Average: %1").arg(QString::number(p_average, 'f', 2)));
  });

  setLayout(m_categoryChartLayout);
}

void ChartWindow::UpdateCategoryChart() {
  QStringList categories;
  for (auto action: m_categoryMenu->actions()) {
    if (action->isChecked()) {
      categories << action->text();
    }
  }

  m_categoryChartGenerator->SetCategory(categories);
  m_categoryChartGenerator->SetBeginDate(m_beginDateCalendar->date());
  m_categoryChartGenerator->SetEndDate(m_endDateCalendar->date());

  m_categoryChartGenerator->UpdateChartView();

  m_averageLabel->setText(tr("Average: %1€").arg(QString::number(m_categoryChartGenerator->GetAverageAmount(), 'f', 2)));
  m_totalLabel->setText(tr("Total: %1€").arg(QString::number(m_categoryChartGenerator->GetTotalAmount(), 'f', 2)));
}

void ChartWindow::SelectGroupAndUpdateCategoryChart(QAction* p_action) {
  auto actionsList = m_actionsByGroup[p_action];
  for (auto action: actionsList) {
    disconnect(action, &QAction::toggled, this, &ChartWindow::UpdateCategoryChart);
    action->setChecked(p_action->isChecked());
    connect(action, &QAction::toggled, this, &ChartWindow::UpdateCategoryChart);
  }

  UpdateCategoryChart();
}
