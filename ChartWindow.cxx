#include "ChartWindow.hxx"

#include "CategoriesModel.hxx"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDateEdit>
#include <QStandardItemModel>
#include <QPushButton>
#include <QMenu>
#include <QRadioButton>
#include <QButtonGroup>

#include <QDebug>

QT_CHARTS_USE_NAMESPACE

ChartWindow::ChartWindow(CSVModel* p_model, CategoriesModel* p_categoriesModel, QWidget* p_parent):
  QWidget(p_parent),
  m_model(p_model),
  m_categoriesModel(p_categoriesModel),
  m_categoryChartView(new QChartView) {

  m_categoryChartView->setRenderHint(QPainter::Antialiasing);

  // Category
  m_beginDateCalendar = new QDateEdit;
  m_beginDateCalendar->setDate(QDate(QDate::currentDate().year(), 1, 1));
  m_beginDateCalendar->setCalendarPopup(true);
  connect(m_beginDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);
  m_endDateCalendar = new QDateEdit;
  m_endDateCalendar->setDate(QDate::currentDate());
  m_endDateCalendar->setCalendarPopup(true);
  connect(m_endDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);

  m_categoryRadioButton = new QRadioButton;
  m_categoryButton = new QPushButton(tr("Select categories"));
  m_categoryMenu = new QMenu;
  m_categoryButton->setMenu(m_categoryMenu);

  m_groupRadioButton = new QRadioButton;
  m_groupButton = new QPushButton(tr("Select a group"));
  m_groupButton->setEnabled(false);
  m_groupMenu = new QMenu;
  m_groupButton->setMenu(m_groupMenu);

  m_balanceRadioButton = new QRadioButton(tr("Balance"));

  bool noAction = true;
  for (int groupRow = 0; groupRow < m_categoriesModel->rowCount(); ++groupRow) {
    auto const& groupIndex = m_categoriesModel->index(groupRow, 0);
    auto groupName = groupIndex.data().toString();

    // Categories menu
    if (!noAction) {
      m_categoryMenu->addSeparator();
    }

    auto groupActionForCategoriesMenu = new QAction(groupName);
    m_categoryMenu->addAction(groupActionForCategoriesMenu);
    auto groupFont = groupActionForCategoriesMenu->font();
    groupFont.setBold(true);
    groupActionForCategoriesMenu->setFont(groupFont);
    groupActionForCategoriesMenu->setCheckable(true);
    connect(groupActionForCategoriesMenu, &QAction::toggled, this, [this, groupActionForCategoriesMenu](){Q_EMIT GroupToggled(groupActionForCategoriesMenu);});
    m_actionsByGroup[groupActionForCategoriesMenu] = QList<QAction*>();

    for (int categoryRow = 0; categoryRow < m_categoriesModel->rowCount(groupIndex); ++ categoryRow) {
      auto const& categoryIndex = m_categoriesModel->index(categoryRow, 0, groupIndex);

      auto action = new QAction(categoryIndex.data().toString());
      action->setCheckable(true);
      m_categoryMenu->addAction(action);
      connect(action, &QAction::toggled, this, &ChartWindow::UpdateCategoryChart);
      m_actionsByGroup[groupActionForCategoriesMenu] << action;
    }
    noAction = false;

    // Groups menu
    auto groupActionForGroupsMenu = new QAction(groupName);
    groupActionForGroupsMenu->setCheckable(true);
    connect(groupActionForGroupsMenu, &QAction::toggled, this, &ChartWindow::UpdateGroupChart);
    m_groupMenu->addAction(groupActionForGroupsMenu);
  }

  connect(m_balanceRadioButton, &QRadioButton::toggled, this, &ChartWindow::UpdateBalanceChart);

  connect(m_categoryRadioButton, &QRadioButton::toggled, m_categoryButton, &QPushButton::setEnabled);
  m_categoryRadioButton->toggle();

  connect(m_groupRadioButton, &QRadioButton::toggled, m_groupButton, &QPushButton::setEnabled);

  auto radioGroup = new QButtonGroup(this);
  radioGroup->addButton(m_categoryRadioButton);
  radioGroup->addButton(m_groupRadioButton);
  radioGroup->addButton(m_balanceRadioButton);

  m_categoryChartGenerator = new CategoryChartGenerator(m_categoryChartView, m_beginDateCalendar->date(), m_endDateCalendar->date());
  m_averageLabel = new QLabel(tr("Average: 0.00"));
  m_totalLabel = new QLabel(tr("Total: 0.00"));
  m_hoveredAverageLabel = new QLabel(tr(""));
  m_hoveredCumulLabel = new QLabel(tr(""));
  m_chartOptionsLayout = new QHBoxLayout;
  m_chartOptionsLayout->addWidget(m_beginDateCalendar);
  m_chartOptionsLayout->addWidget(m_endDateCalendar);
  m_chartOptionsLayout->addWidget(m_categoryRadioButton);
  m_chartOptionsLayout->addWidget(m_categoryButton);
  m_chartOptionsLayout->addWidget(m_groupRadioButton);
  m_chartOptionsLayout->addWidget(m_groupButton);
  m_chartOptionsLayout->addWidget(m_balanceRadioButton);
  m_chartOptionsLayout->addWidget(m_averageLabel);
  m_chartOptionsLayout->addWidget(m_totalLabel);
  m_chartOptionsLayout->addWidget(m_hoveredAverageLabel);
  m_chartOptionsLayout->addWidget(m_hoveredCumulLabel);
  m_chartOptionsLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
  m_categoryChartLayout = new QVBoxLayout;
  m_categoryChartLayout->addLayout(m_chartOptionsLayout);
  m_categoryChartLayout->setAlignment(m_chartOptionsLayout, Qt::AlignTop);
  m_categoryChartLayout->addWidget(m_categoryChartView);

  connect(this, &ChartWindow::GroupToggled, this, &ChartWindow::SelectGroupAndUpdateCategoryChart);
  connect(m_categoryChartGenerator, &CategoryChartGenerator::HoveredAverageChanged, this, [this](double p_average, QString const& p_category) {
    m_hoveredAverageLabel->setVisible(!p_category.isEmpty());
    m_hoveredAverageLabel->setText(tr("%1: %2€").arg(p_category, QString::number(p_average, 'f', 2)));
  });
  connect(m_categoryChartGenerator, &CategoryChartGenerator::HoveredCumulChanged, this, [this](double p_cumul, QString const& p_category) {
    m_hoveredCumulLabel->setVisible(!p_category.isEmpty());
    m_hoveredCumulLabel->setText(tr("%1 | Cumul: %2€").arg(p_category, QString::number(p_cumul, 'f', 2)));
  });

  auto useCurrentYearAction = new QAction(this);
  addAction(useCurrentYearAction);
  useCurrentYearAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  connect(useCurrentYearAction, &QAction::triggered, this, &ChartWindow::UseCurrentYear);

  auto decreaseOneYearAction = new QAction(this);
  addAction(decreaseOneYearAction);
  decreaseOneYearAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
  connect(decreaseOneYearAction, &QAction::triggered, this, [this](){ ChangeYear(false); });

  auto increaseOneYearAction = new QAction(this);
  addAction(increaseOneYearAction);
  increaseOneYearAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
  connect(increaseOneYearAction, &QAction::triggered, this, [this](){ ChangeYear(true); });

  setLayout(m_categoryChartLayout);
}

void ChartWindow::UpdateCategoryChart() {
  QStringList categories;
  for (auto action: m_categoryMenu->actions()) {
    if (action->isChecked()) {
      categories << action->text();
    }
  }

  m_categoryChartGenerator->SetChartType(CategoryChartGenerator::eCategories);
  m_categoryChartGenerator->SetGroups({});
  m_categoryChartGenerator->SetCategories(categories);
  m_categoryChartGenerator->SetBeginDate(m_beginDateCalendar->date());
  m_categoryChartGenerator->SetEndDate(m_endDateCalendar->date());

  m_categoryChartGenerator->UpdateChartView();

  m_averageLabel->setText(tr("Average: %1€").arg(QString::number(m_categoryChartGenerator->GetAverageAmount(), 'f', 2)));
  m_totalLabel->setText(tr("Total: %1€").arg(QString::number(m_categoryChartGenerator->GetTotalAmount(), 'f', 2)));
}

void ChartWindow::UpdateGroupChart() {
  QStringList groups;
  for (auto action: m_groupMenu->actions()) {
    if (action->isChecked()) {
      groups << action->text();
    }
  }

  m_categoryChartGenerator->SetChartType(CategoryChartGenerator::eGroups);
  m_categoryChartGenerator->SetGroups(groups);
  m_categoryChartGenerator->SetCategories({});
  m_categoryChartGenerator->SetBeginDate(m_beginDateCalendar->date());
  m_categoryChartGenerator->SetEndDate(m_endDateCalendar->date());

  m_categoryChartGenerator->UpdateChartView();

  m_averageLabel->setText(tr("Average: %1€").arg(QString::number(m_categoryChartGenerator->GetAverageAmount(), 'f', 2)));
  m_totalLabel->setText(tr("Total: %1€").arg(QString::number(m_categoryChartGenerator->GetTotalAmount(), 'f', 2)));
}

void ChartWindow::UpdateBalanceChart() {
  m_categoryChartGenerator->SetChartType(CategoryChartGenerator::eBalance);
  m_categoryChartGenerator->SetGroups({});
  m_categoryChartGenerator->SetCategories({});
  m_categoryChartGenerator->SetBeginDate(m_beginDateCalendar->date());
  m_categoryChartGenerator->SetEndDate(m_endDateCalendar->date());

  m_categoryChartGenerator->UpdateChartView();

  m_averageLabel->setText(tr("Average: %1€").arg(QString::number(m_categoryChartGenerator->GetAverageAmount(), 'f', 2)));
  m_totalLabel->setText(tr("Total: %1€").arg(QString::number(m_categoryChartGenerator->GetTotalAmount(), 'f', 2)));
}

void ChartWindow::UpdateChartAccordingToType() {
  if (m_endDateCalendar->date() < m_beginDateCalendar->date()) {
    disconnect(m_beginDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);
    m_beginDateCalendar->setDate(m_endDateCalendar->date());
    connect(m_beginDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);
  }

  if (m_balanceRadioButton->isChecked()) {
    UpdateBalanceChart();
  } else if (m_categoryRadioButton->isChecked()) {
    UpdateCategoryChart();
  } else if (m_groupRadioButton->isChecked()) {
    UpdateGroupChart();
  }
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

void ChartWindow::UseCurrentYear() {
  disconnect(m_beginDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);
  m_beginDateCalendar->setDate(QDate(QDate::currentDate().year(), 1, 1));
  connect(m_beginDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);
  m_endDateCalendar->setDate(QDate(QDate::currentDate().year(), 12, 31));
}

void ChartWindow::ChangeYear(bool p_add) {
  auto increment = p_add? 1: -1;

  disconnect(m_beginDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);
  disconnect(m_endDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);
  auto beginDate = m_beginDateCalendar->date();
  m_beginDateCalendar->setDate(QDate(beginDate.year()+increment, beginDate.month(), beginDate.day()));
  auto endDate = m_endDateCalendar->date();
  m_endDateCalendar->setDate(QDate(endDate.year()+increment, endDate.month(), endDate.day()));
  connect(m_beginDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);
  connect(m_endDateCalendar, &QDateEdit::dateChanged, this, &ChartWindow::UpdateChartAccordingToType);

  UpdateChartAccordingToType();
}
