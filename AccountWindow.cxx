#include "AccountWindow.hxx"

#include "CSVModel.hxx"
#include "MonthlyCSVGenerator.hxx"
#include "CategoryItemDelegate.hxx"
#include "Utils.hxx"

#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QDate>
#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>

#include <QDebug>

AccountWindow::AccountWindow(QWidget* parent):
  QWidget(parent) {

  // Reload action
  auto reloadAction = new QAction("Reload data...", this);
  reloadAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  connect(reloadAction, &QAction::triggered, this, &AccountWindow::ReloadFile);
  addAction(reloadAction);

  // Toggle expand
  auto toggleExpandAction = new QAction("Toggle expand", this);
  toggleExpandAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
  toggleExpandAction->setCheckable(true);
  connect(toggleExpandAction, &QAction::toggled, this, [this](bool p_toggle){
    p_toggle ?
    m_categoryView->expandAll():
    m_categoryView->expandToDepth(0);
  });
  addAction(toggleExpandAction);

  // Toggle action
  auto toggleAction = new QAction("Toggle nul values", this);
  toggleAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_T));
  toggleAction->setCheckable(true);
  connect(toggleAction, &QAction::toggled, this, [this, toggleExpandAction](bool p_toggle){
    p_toggle ?
      m_proxyModel->setFilterRegularExpression(QRegularExpression("^$|^-?[1-9].*$")):
      m_proxyModel->setFilterRegularExpression(QRegularExpression(".*"));
    toggleExpandAction->isChecked() ?
      m_categoryView->expandAll():
      m_categoryView->expandToDepth(0);
  });
  addAction(toggleAction);

  // CSV
  m_csvModel = new CSVModel(this);
  connect(m_csvModel, &CSVModel::SaveCategoryRequested, this, &AccountWindow::SaveCategory);
  auto proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(m_csvModel);
  m_tableView = new QTableView;
  m_tableView->setModel(proxyModel);
  m_tableView->setAlternatingRowColors(true);
  m_tableView->verticalHeader()->hide();
  if (m_csvModel->rowCount() > 0) {
    m_tableView->horizontalHeader()->setSectionResizeMode(CSVModel::eLabel, QHeaderView::Stretch);
  }
  m_tableView->setFocusPolicy(Qt::NoFocus);
  m_tableView->setSelectionMode(QTableView::NoSelection);
  m_tableView->setItemDelegateForColumn(CSVModel::eCategory, new CategoryItemDelegate);
  m_tableView->setItemDelegateForColumn(CSVModel::eGroup, new CategoryItemDelegate);
  m_tableView->setSortingEnabled(true);

  // Current date
  QDate currentDate = QDate::currentDate();
  m_month = currentDate.month();
  m_year = currentDate.year();

  // Date UI
  QFont font;
  font.setPixelSize(25);
  font.setCapitalization(QFont::Capitalize);
  font.setBold(99);
  QFontMetrics fm(font);
  m_monthLabel = new QLabel(currentDate.toString("MMMM"));
  m_monthLabel->setFont(font);
  m_monthLabel->setFixedWidth(fm.width(QDate(1, 9, 1).toString("MMMM")));
  m_monthLabel->setAlignment(Qt::AlignCenter);
  m_yearLabel = new QLabel(currentDate.toString("yyyy"));
  m_yearLabel->setFont(font);
  m_yearLabel->setFixedWidth(fm.width(m_yearLabel->text()));
  auto dateLabelLayout = new QHBoxLayout;
  dateLabelLayout->addWidget(m_monthLabel);
  dateLabelLayout->addWidget(m_yearLabel);
  m_previousYear = new QPushButton("<<");
  m_previousYear->setFont(font);
  m_previousYear->setFixedSize(m_previousYear->sizeHint().height(), m_previousYear->sizeHint().height());
  m_previousMonth = new QPushButton("<");
  m_previousMonth->setFont(font);
  m_previousMonth->setFixedSize(m_previousMonth->sizeHint().height(), m_previousMonth->sizeHint().height());
  m_nextMonth = new QPushButton(">");
  m_nextMonth->setFont(font);
  m_nextMonth->setFixedSize(m_nextMonth->sizeHint().height(), m_nextMonth->sizeHint().height());
  m_nextYear = new QPushButton(">>");
  m_nextYear->setFont(font);
  m_nextYear->setFixedSize(m_nextYear->sizeHint().height(), m_nextYear->sizeHint().height());

  // Date layout
  auto dateLayout = new QHBoxLayout;
  dateLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
  dateLayout->addWidget(m_previousYear);
  dateLayout->setAlignment(m_previousYear, Qt::AlignCenter);
  dateLayout->addWidget(m_previousMonth);
  dateLayout->setAlignment(m_previousMonth, Qt::AlignCenter);
  dateLayout->addLayout(dateLabelLayout);
  dateLayout->setAlignment(dateLabelLayout, Qt::AlignCenter);
  dateLayout->addWidget(m_nextMonth);
  dateLayout->setAlignment(m_nextMonth, Qt::AlignCenter);
  dateLayout->addWidget(m_nextYear);
  dateLayout->setAlignment(m_nextYear, Qt::AlignCenter);
  dateLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));

  // Summary model view
  m_categoryView = new QTreeView;
  m_categoryModel = new QStandardItemModel;
  m_proxyModel = new QSortFilterProxyModel;
  m_proxyModel->setSourceModel(m_categoryModel);
  m_proxyModel->setFilterKeyColumn(1);
  m_categoryView->setModel(m_proxyModel);
  m_categoryView->setHeaderHidden(true);
  m_categoryView->setEditTriggers(QTreeView::NoEditTriggers);

  // In
  auto inItem = new QStandardItem("In");
  inItem->setBackground(Utils::GetFadedGreenColor());
  m_inItem = new QStandardItem(QString::number(0, 'f', 2));
  m_inItem->setBackground(Utils::GetFadedGreenColor());
  m_inItem->setTextAlignment(Qt::AlignRight);
  // Salary
  m_salaryItem = new QStandardItem(QString::number(0, 'f', 2));
  m_salaryItem->setTextAlignment(Qt::AlignRight);
  inItem->appendRow({new QStandardItem("Salary"), m_salaryItem});
  // Profit
  m_profitItem = new QStandardItem(QString::number(0, 'f', 2));
  m_profitItem->setTextAlignment(Qt::AlignRight);
  auto profitLabelItem = new QStandardItem("Gain");
  m_profitList = Utils::NOURRITURE;
  for (auto const& profit: m_profitList) {
    auto currentProfitItem = new QStandardItem(profit);
    auto currentProfitValueItem = new QStandardItem(QString::number(0, 'f', 2));
    currentProfitValueItem->setTextAlignment(Qt::AlignRight);
    profitLabelItem->appendRow({currentProfitItem, currentProfitValueItem});
    m_profitLabelsMap[profit] = currentProfitItem;
    m_profitValuesMap[profit] = currentProfitValueItem;
  }
  inItem->appendRow({profitLabelItem, m_profitItem});

  // Out
  auto outItem = new QStandardItem("Out");
  outItem->setBackground(Utils::GetFadedRedColor());
  m_outItem = new QStandardItem(QString::number(0, 'f', 2));
  m_outItem->setBackground(Utils::GetFadedRedColor());
  m_outItem->setTextAlignment(Qt::AlignRight);
  // Fixed charges
  m_fixedChargesItem = new QStandardItem(QString::number(0, 'f', 2));
  m_fixedChargesItem->setTextAlignment(Qt::AlignRight);
  auto fixedChargesLabelItem = new QStandardItem("Charges fixes");
  m_fixedChargesList = Utils::CHARGES_FIXES;
  for (auto const& fixedCharge: m_fixedChargesList) {
    auto currentFixedChargeItem = new QStandardItem(fixedCharge);
    auto currentFixedChargeValueItem = new QStandardItem(QString::number(0, 'f', 2));
    currentFixedChargeValueItem->setTextAlignment(Qt::AlignRight);
    fixedChargesLabelItem->appendRow({currentFixedChargeItem, currentFixedChargeValueItem});
    m_fixedChargesLabelsMap[fixedCharge] = currentFixedChargeItem;
    m_fixedChargesValuesMap[fixedCharge] = currentFixedChargeValueItem;
  }
  outItem->appendRow({fixedChargesLabelItem, m_fixedChargesItem});
  // Variable charges
  m_variableChargesItem = new QStandardItem(QString::number(0, 'f', 2));
  m_variableChargesItem->setTextAlignment(Qt::AlignRight);
  auto variableChargesLabelItem = new QStandardItem("Charges variables");
  m_variableChargesList = Utils::CHARGES_VARIABLES;
  for (auto const& variableCharge: m_variableChargesList) {
    auto currentVariableChargeItem = new QStandardItem(variableCharge);
    auto currentVariableChargeValueItem = new QStandardItem(QString::number(0, 'f', 2));
    currentVariableChargeValueItem->setTextAlignment(Qt::AlignRight);
    variableChargesLabelItem->appendRow({currentVariableChargeItem, currentVariableChargeValueItem});
    m_variableChargesLabelsMap[variableCharge] = currentVariableChargeItem;
    m_variableChargesValuesMap[variableCharge] = currentVariableChargeValueItem;
  }
  outItem->appendRow({variableChargesLabelItem, m_variableChargesItem});
  // Food
  m_foodItem = new QStandardItem(QString::number(0, 'f', 2));
  m_foodItem->setTextAlignment(Qt::AlignRight);
  auto foodLabelItem = new QStandardItem("Nourriture");
  m_foodList = Utils::NOURRITURE;
  for (auto const& food: m_foodList) {
    auto currentFoodItem = new QStandardItem(food);
    auto currentFoodValueItem = new QStandardItem(QString::number(0, 'f', 2));
    currentFoodValueItem->setTextAlignment(Qt::AlignRight);
    foodLabelItem->appendRow({currentFoodItem, currentFoodValueItem});
    m_foodLabelsMap[food] = currentFoodItem;
    m_foodValuesMap[food] = currentFoodValueItem;
  }
  outItem->appendRow({foodLabelItem, m_foodItem});
  // Saving
  m_savingItem = new QStandardItem(QString::number(0, 'f', 2));
  m_savingItem->setTextAlignment(Qt::AlignRight);
  outItem->appendRow({new QStandardItem("Épargne"), m_savingItem});

  // Balance
  m_balanceItem = new QStandardItem(QString::number(0, 'f', 2));
  m_balanceItem->setTextAlignment(Qt::AlignRight);

  // Model
  m_categoryModel->appendRow({inItem, m_inItem});
  AddSeparator();
  m_categoryModel->appendRow({outItem, m_outItem});
  auto separator2 = new QStandardItem("");
  separator2->setSelectable(false);
  AddSeparator();
  m_categoryModel->appendRow({new QStandardItem("Équilibre"), m_balanceItem});

  // Summary layout
  auto summaryAndDateLayout = new QVBoxLayout;
  summaryAndDateLayout->addLayout(dateLayout);
  summaryAndDateLayout->addWidget(m_categoryView);

  // Main layout
  auto mainLayout = new QHBoxLayout;
  mainLayout->addLayout(summaryAndDateLayout);
  mainLayout->addWidget(m_tableView);
  mainLayout->setStretchFactor(summaryAndDateLayout, 1);
  mainLayout->setStretchFactor(m_tableView, 5);
  setLayout(mainLayout);

  // Connect
  connect(m_previousYear, &QPushButton::clicked, this, &AccountWindow::GoToPreviousYear);
  connect(m_previousMonth, &QPushButton::clicked, this, &AccountWindow::GoToPreviousMonth);
  connect(m_nextMonth, &QPushButton::clicked, this, &AccountWindow::GoToNextMonth);
  connect(m_nextYear, &QPushButton::clicked, this, &AccountWindow::GoToNextYear);
  connect(this, &AccountWindow::YearChanged, this, [this](){m_yearLabel->setText(QString::number(m_year));});
  connect(this, &AccountWindow::MonthChanged, this, [this](){m_monthLabel->setText(QDate(1, m_month, 1).toString("MMMM"));});
  connect(this, &AccountWindow::UpdateModelRequested, this, &AccountWindow::FillModel);
  connect(m_categoryView, &QTreeView::expanded, this, [this](){m_categoryView->resizeColumnToContents(0);});

  // Fill model
  FillModel();
}

QString AccountWindow::GetCurrentCSVFileName() const {
  return QString("../BankAccount/csv/"+QDate(m_year, m_month, 1).toString("MM-yyyy")+"/operations.csv");
}

void AccountWindow::GoToPreviousYear() {
  m_year--;
  emit YearChanged();
  emit UpdateModelRequested();
}

void AccountWindow::GoToPreviousMonth() {
  //m_month==1?m_month=12,m_year--:m_month=(m_month+=11)%12;
  if (m_month == 1) {
    m_month = 12;
    m_year--;
    emit YearChanged();
  } else
    m_month--;

  emit MonthChanged();
  emit UpdateModelRequested();
}

void AccountWindow::GoToNextMonth() {
  //m_month=(m_month+=13)%12==0?m_month=12:m_year+=(m_month==1);
  if (m_month == 12) {
    m_month = 1;
    m_year++;
    emit YearChanged();
  } else
    m_month++;

  emit MonthChanged();
  emit UpdateModelRequested();
}

void AccountWindow::GoToNextYear() {
  m_year++;
  emit YearChanged();
  emit UpdateModelRequested();
}

void AccountWindow::FillModel() {
  m_csvModel->SetSource(GetCurrentCSVFileName());
  if (m_csvModel->rowCount() > 0) {
    m_tableView->horizontalHeader()->setSectionResizeMode(CSVModel::eLabel, QHeaderView::Stretch);
  }

  UpdateSummary();
}

void AccountWindow::SaveCategory(int p_row, const QString& p_group, QString const& p_category) {
  MonthlyCSVGenerator::SaveCategory(p_row, p_group, p_category, GetCurrentCSVFileName());
}

void AccountWindow::UpdateSummary() {
  float salary = 0;
  float fixedCharges = 0;
  float variableCharges = 0;
  float food = 0;
  float saving = 0;
  float profit = 0;
  float balance = 0;
  float in = 0;
  float out = 0;

  for (auto const& fixedCharge: m_fixedChargesList) {
    auto currentLabel = m_fixedChargesLabelsMap[fixedCharge];
    m_fixedChargesValuesMap[fixedCharge]->setText(QString::number(0, 'f', 2));
    QFont validateFont(currentLabel->font());
    validateFont.setBold(false);
    currentLabel->setFont(validateFont);
  }

  for (int row = 0; row < m_csvModel->rowCount(); ++row) {
    QString groupName = m_csvModel->index(row, CSVModel::eGroup).data().toString();
    float amount = m_csvModel->GetCredit(row) + m_csvModel->GetDebit(row);
    Utils::Group group = Utils::GetGroupFromGroupName(groupName);
    QString categoryName = m_csvModel->index(row, CSVModel::eCategory).data().toString();

    switch(group) {
    case Utils::eSalary: {
      salary += amount;
      break;
    }
    case Utils::eFixedCharges: {
      Q_ASSERT_X(m_fixedChargesList.contains(categoryName), "AccountWindow::updateSummary()", tr("Unknown fixed charge: %1").arg(categoryName).toStdString().c_str());
      auto currentItem = m_fixedChargesLabelsMap[categoryName];
      QFont validateFont(currentItem->font());
      validateFont.setBold(true);
      currentItem->setFont(validateFont);
      auto currentValueItem = m_fixedChargesValuesMap[categoryName];
      auto newAmount = amount + currentValueItem->text().toFloat();
      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      fixedCharges += amount;
      break;
    }
    case Utils::eVariableCharges: {
      Q_ASSERT_X(m_variableChargesList.contains(categoryName), "AccountWindow::updateSummary()", tr("Unknown variable charge: %1").arg(categoryName).toStdString().c_str());
      auto currentValueItem = m_variableChargesValuesMap[categoryName];
      auto newAmount = amount + currentValueItem->text().toFloat();
      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      variableCharges += amount;
      break;
    }
    case Utils::eFood: {
      Q_ASSERT_X(m_foodList.contains(categoryName), "AccountWindow::updateSummary()", tr("Unknown variable charge: %1").arg(categoryName).toStdString().c_str());
      auto currentValueItem = m_foodValuesMap[categoryName];
      auto newAmount = amount + currentValueItem->text().toFloat();
      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      food += amount;
      break;
    }
    case Utils::eSaving: {
      saving += amount;
      break;
    }
    case Utils::eProfit: {
      Q_ASSERT_X(m_profitList.contains(categoryName), "AccountWindow::updateSummary()", tr("Unknown variable charge: %1").arg(categoryName).toStdString().c_str());
      auto currentValueItem = m_profitValuesMap[categoryName];
      auto newAmount = amount + currentValueItem->text().toFloat();
      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      profit += amount;
      break;
    }
    case Utils::eUnknown:
    default: {
      break;
    }
    }
  }

  in = salary + profit;
  out = fixedCharges + variableCharges + food + saving;
  balance = in + out;

  m_salaryItem->setText(QString::number(salary, 'f', 2));
  m_profitItem->setText(QString::number(profit, 'f', 2));
  m_inItem->setText(QString::number(in, 'f', 2));

  m_fixedChargesItem->setText(QString::number(fixedCharges, 'f', 2));
  m_variableChargesItem->setText(QString::number(variableCharges, 'f', 2));
  m_foodItem->setText(QString::number(food, 'f', 2));
  m_savingItem->setText(QString::number(saving, 'f', 2));
  m_outItem->setText(QString::number(out, 'f', 2));

  m_balanceItem->setText(QString::number(balance, 'f', 2));
  QColor color;
  balance < 0 ? color = Utils::GetRedColor() : color = Utils::GetGreenColor();
  m_balanceItem->setForeground(QBrush(color));

  m_categoryView->expandToDepth(0);
}

void AccountWindow::ReloadFile() {
  auto xlsFileName = QFileDialog::getOpenFileName(this, "Import CSV file", "../BankAccount/csv", "EXEL files (*.xls *.XLS);;CSV files (*.csv *.CSV)");
  if (xlsFileName.isEmpty()) {
    return;
  }

  bool hasHeader = !(xlsFileName.endsWith("xls"));
  auto csvFileName = xlsFileName;

  if (xlsFileName.endsWith("xls")) {
    csvFileName = MonthlyCSVGenerator::ConvertXLSToCSV(xlsFileName);
  }

  auto currentDate = QDate(m_year, m_month, 1);
  MonthlyCSVGenerator::UpdateRawCSV(currentDate, csvFileName, ';', hasHeader);

  FillModel();
}

void AccountWindow::AddSeparator()
{
  auto separator0 = new QStandardItem("");
  separator0->setFlags(Qt::NoItemFlags);
  auto separator1 = new QStandardItem("");
  separator1->setFlags(Qt::NoItemFlags);
  m_categoryModel->appendRow({separator0, separator1});
}
