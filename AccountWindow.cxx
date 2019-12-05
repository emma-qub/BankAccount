#include "AccountWindow.hxx"

#include "CSVModel.hxx"
#include "CategoriesModel.hxx"
#include "MonthlyCSVGenerator.hxx"
#include "CategoryItemDelegate.hxx"
#include "Utils.hxx"
#include "BudgetItemDelegate.hxx"

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
#include <QTextStream>

AccountWindow::AccountWindow(CSVModel* p_csvModel, CategoriesModel* p_categoriesModel, QWidget* p_parent):
  QWidget(p_parent),
  m_csvModel(p_csvModel),
  m_categoriesModel(p_categoriesModel) {

  // Reload action
  auto reloadAction = new QAction("Reload data...", this);
  reloadAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  connect(reloadAction, &QAction::triggered, this, &AccountWindow::ReloadFile);
  addAction(reloadAction);

  // Toggle action
  auto toggleAction = new QAction("Toggle nul values", this);
  toggleAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_T));
  toggleAction->setCheckable(true);
  connect(toggleAction, &QAction::toggled, this, [this](bool p_toggle){
    p_toggle ?
      m_summaryView->expandAll():
      m_summaryView->expandToDepth(0);
  });
  addAction(toggleAction);

  // CSV
  connect(m_csvModel, &CSVModel::SaveCategoryRequested, this, &AccountWindow::SaveCategory);
  connect(m_csvModel, &CSVModel::UpdateSummaryRequested, this, &AccountWindow::UpdateSummary);
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
  m_tableView->setItemDelegateForColumn(CSVModel::eCategory, new CategoryItemDelegate(m_categoriesModel, m_tableView));
  m_tableView->setItemDelegateForColumn(CSVModel::eGroup, new CategoryItemDelegate(m_categoriesModel, m_tableView));
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
  m_monthLabel->setFixedWidth(fm.horizontalAdvance(QDate(1, 9, 1).toString("MMMM")));
  m_monthLabel->setAlignment(Qt::AlignCenter);
  m_yearLabel = new QLabel(currentDate.toString("yyyy"));
  m_yearLabel->setFont(font);
  m_yearLabel->setFixedWidth(fm.horizontalAdvance(m_yearLabel->text()));
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
  m_summaryView = new QTreeView;
  m_summaryModel = new QStandardItemModel;
  m_summaryModel->setColumnCount(4);
  m_summaryView->setModel(m_summaryModel);
  m_summaryView->setHeaderHidden(true);
  auto budgetItemDelegate = new BudgetItemDelegate(m_summaryView);
  m_summaryView->setItemDelegate(budgetItemDelegate);

  // In
  auto inItem = new QStandardItem("In");
  inItem->setBackground(Utils::GetFadedGreenColor());
  auto inItemFont = inItem->font();
  inItemFont.setBold(true);
  inItem->setFont(inItemFont);
  m_inItem = new QStandardItem(QString::number(0, 'f', 2));
  m_inItem->setBackground(Utils::GetFadedGreenColor());
  m_inItem->setTextAlignment(Qt::AlignRight);
  m_inItem->setFont(inItemFont);
  // Salary
  m_salaryItem = new QStandardItem(QString::number(0, 'f', 2));
  m_salaryItem->setTextAlignment(Qt::AlignRight);
  auto salaryLabelItem = new QStandardItem("Salary");
  salaryLabelItem->setData(QVariant::fromValue<CategoryType>(eSalary), eCategoryRole);
  inItem->appendRow({salaryLabelItem, m_salaryItem, new QStandardItem, new QStandardItem});
  // Profit
  m_profitItem = new QStandardItem(QString::number(0, 'f', 2));
  m_profitItem->setTextAlignment(Qt::AlignRight);
  auto profitLabelItem = new QStandardItem("Gain");
  profitLabelItem->setData(QVariant::fromValue<CategoryType>(eProfit), eCategoryRole);
  for (auto const& profit: m_categoriesModel->GetCategoriesByGroup("Gain")) {
    auto currentProfitItem = new QStandardItem(profit);
    auto currentProfitValueItem = new QStandardItem(QString::number(0, 'f', 2));
    currentProfitValueItem->setTextAlignment(Qt::AlignRight);
    profitLabelItem->appendRow({currentProfitItem, currentProfitValueItem, new QStandardItem, new QStandardItem});
    m_profitLabelsMap[profit] = currentProfitItem;
    m_profitValuesMap[profit] = currentProfitValueItem;
  }
  inItem->appendRow({profitLabelItem, m_profitItem, new QStandardItem, new QStandardItem});

  // Out
  auto outItem = new QStandardItem("Out");
  outItem->setBackground(Utils::GetFadedRedColor());
  auto outItemFont = outItem->font();
  outItemFont.setBold(true);
  outItem->setFont(outItemFont);
  m_outItem = new QStandardItem(QString::number(0, 'f', 2));
  m_outItem->setBackground(Utils::GetFadedRedColor());
  m_outItem->setTextAlignment(Qt::AlignRight);
  m_outItem->setFont(outItemFont);
  // Fixed charges
  m_fixedChargesItem = new QStandardItem(QString::number(0, 'f', 2));
  m_fixedChargesItem->setTextAlignment(Qt::AlignRight);
  auto fixedChargesLabelItem = new QStandardItem("Charges fixes");
  fixedChargesLabelItem->setData(QVariant::fromValue<CategoryType>(eFixedCharges), eCategoryRole);
  for (auto const& fixedCharge: m_categoriesModel->GetCategoriesByGroup("Charges fixes")) {
    auto currentFixedChargeItem = new QStandardItem(fixedCharge);
    currentFixedChargeItem->setForeground(QBrush(Utils::GetOrangeColor()));
    auto currentFixedChargeValueItem = new QStandardItem(QString::number(0, 'f', 2));
    currentFixedChargeValueItem->setTextAlignment(Qt::AlignRight);
    fixedChargesLabelItem->appendRow({currentFixedChargeItem, currentFixedChargeValueItem, new QStandardItem, new QStandardItem});
    m_fixedChargesLabelsMap[fixedCharge] = currentFixedChargeItem;
    m_fixedChargesValuesMap[fixedCharge] = currentFixedChargeValueItem;
  }
  outItem->appendRow({fixedChargesLabelItem, m_fixedChargesItem, new QStandardItem, new QStandardItem});
  // Variable charges
  m_variableChargesItem = new QStandardItem(QString::number(0, 'f', 2));
  m_variableChargesItem->setTextAlignment(Qt::AlignRight);
  auto variableChargesLabelItem = new QStandardItem("Charges variables");
  variableChargesLabelItem->setData(QVariant::fromValue<CategoryType>(eVariableCharges), eCategoryRole);
  outItem->appendRow({variableChargesLabelItem, m_variableChargesItem, new QStandardItem, new QStandardItem});
  // Food
  m_foodItem = new QStandardItem(QString::number(0, 'f', 2));
  m_foodItem->setTextAlignment(Qt::AlignRight);
  auto foodLabelItem = new QStandardItem("Nourriture");
  foodLabelItem->setData(QVariant::fromValue<CategoryType>(eFood), eCategoryRole);
  for (auto const& food: m_categoriesModel->GetCategoriesByGroup("Nourriture")) {
    auto currentFoodItem = new QStandardItem(food);
    auto currentFoodValueItem = new QStandardItem(QString::number(0, 'f', 2));
    currentFoodValueItem->setTextAlignment(Qt::AlignRight);

    foodLabelItem->appendRow({currentFoodItem, currentFoodValueItem, new QStandardItem, new QStandardItem});
    m_foodLabelsMap[food] = currentFoodItem;
    m_foodValuesMap[food] = currentFoodValueItem;
  }
  auto currentBudgetItem = new QStandardItem;
  currentBudgetItem->setData(true, eCanHaveBudgetRole);
  currentBudgetItem->setTextAlignment(Qt::AlignRight);
  auto currentPercentageItem = new QStandardItem;
  currentPercentageItem->setTextAlignment(Qt::AlignRight);
  outItem->appendRow({foodLabelItem, m_foodItem, currentBudgetItem, currentPercentageItem});
  // Saving
  m_savingItem = new QStandardItem(QString::number(0, 'f', 2));
  m_savingItem->setTextAlignment(Qt::AlignRight);
  auto savingLabelItem = new QStandardItem("Épargne");
  savingLabelItem->setData(QVariant::fromValue<CategoryType>(eSaving), eCategoryRole);
  outItem->appendRow({savingLabelItem, m_savingItem, new QStandardItem, new QStandardItem});

  // Balance
  m_balanceItem = new QStandardItem(QString::number(0, 'f', 2));
  m_balanceItem->setTextAlignment(Qt::AlignRight);
  auto balanceItemFont = m_balanceItem->font();
  balanceItemFont.setBold(true);
  m_balanceItem->setFont(balanceItemFont);

  // Model
  auto inColumn2 = new QStandardItem;
  inColumn2->setBackground(QBrush(Utils::GetFadedGreenColor()));
  auto inColumn3 = new QStandardItem;
  inColumn3->setBackground(QBrush(Utils::GetFadedGreenColor()));
  m_summaryModel->appendRow({inItem, m_inItem, inColumn2, inColumn3});
  AddSeparator();
  auto outColumn2 = new QStandardItem;
  outColumn2->setBackground(QBrush(Utils::GetFadedRedColor()));
  auto outColumn3 = new QStandardItem;
  outColumn3->setBackground(QBrush(Utils::GetFadedRedColor()));
  m_summaryModel->appendRow({outItem, m_outItem, outColumn2, outColumn3});
  auto separator2 = new QStandardItem("");
  separator2->setSelectable(false);
  AddSeparator();
  auto balanceItem = new QStandardItem("Équilibre");
  balanceItem->setFont(balanceItemFont);
  m_summaryModel->appendRow({balanceItem, m_balanceItem, new QStandardItem, new QStandardItem});

  // Summary layout
  auto summaryAndDateLayout = new QVBoxLayout;
  summaryAndDateLayout->addLayout(dateLayout);
  summaryAndDateLayout->addWidget(m_summaryView);

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
  connect(m_summaryView, &QTreeView::expanded, this, [this](){m_summaryView->resizeColumnToContents(0);});
  connect(budgetItemDelegate, &BudgetItemDelegate::BudgetUpdated, this, &AccountWindow::UpdatePercentage);
  connect(budgetItemDelegate, &BudgetItemDelegate::BudgetUpdated, this, &AccountWindow::SaveBudget);

  // Fill model
  FillModel();

  // Expand view
  m_summaryView->expandToDepth(0);
}

QString AccountWindow::GetCurrentCSVFileName() const {
  return QString("../BankAccount/csv/"+QDate(m_year, m_month, 1).toString("MM-yyyy")+"/operations.csv");
}

void AccountWindow::GoToPreviousYear() {
  m_year--;
  Q_EMIT YearChanged();
  Q_EMIT UpdateModelRequested();
}

void AccountWindow::GoToPreviousMonth() {
  //m_month==1?m_month=12,m_year--:m_month=(m_month+=11)%12;
  if (m_month == 1) {
    m_month = 12;
    m_year--;
    Q_EMIT YearChanged();
  } else
    m_month--;

  Q_EMIT MonthChanged();
  Q_EMIT UpdateModelRequested();
}

void AccountWindow::GoToNextMonth() {
  //m_month=(m_month+=13)%12==0?m_month=12:m_year+=(m_month==1);
  if (m_month == 12) {
    m_month = 1;
    m_year++;
    Q_EMIT YearChanged();
  } else
    m_month++;

  Q_EMIT MonthChanged();
  Q_EMIT UpdateModelRequested();
}

void AccountWindow::GoToNextYear() {
  m_year++;
  Q_EMIT YearChanged();
  Q_EMIT UpdateModelRequested();
}

void AccountWindow::FillModel() {
  m_csvModel->SetSource(GetCurrentCSVFileName());
  if (m_csvModel->rowCount() > 0) {
    m_tableView->horizontalHeader()->setSectionResizeMode(CSVModel::eLabel, QHeaderView::Stretch);
  }

  UpdateSummary();
  GetBudgetAmounts();
}

void AccountWindow::SaveCategory(int p_row, QString const& p_group, QString const& p_category) {
  MonthlyCSVGenerator::SaveCategory(p_row, p_group, p_category, GetCurrentCSVFileName());
}

void AccountWindow::GetBudgetAmounts() {
  m_budgetMap.clear();

  QString csvDirectoryPath = "../BankAccount/csv";
  QString accountDirectoryName = QDate(m_year, m_month, 1).toString("MM-yyyy");
  QFile budgetFile(csvDirectoryPath+QDir::separator()+accountDirectoryName+QDir::separator()+"budget.csv");

  if (budgetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream inBudgetFile(&budgetFile);
    inBudgetFile.setCodec("UTF-8");

    while (!inBudgetFile.atEnd()) {
      auto line = inBudgetFile.readLine();
      auto categoryBudgetPair = line.split(";");
      m_budgetMap[categoryBudgetPair.at(0)] = categoryBudgetPair.at(1);
    }
    budgetFile.close();
  }

  UpdateBuget();
}

void AccountWindow::UpdateBuget(QModelIndex const& p_parentIndex) {
  for (int row = 0; row < m_summaryModel->rowCount(p_parentIndex); ++row) {
    auto currentLabelIndex = m_summaryModel->index(row, 0, p_parentIndex);
    auto currentBudgetIndex = m_summaryModel->sibling(row, 2, currentLabelIndex);
    if (currentBudgetIndex.data(eCanHaveBudgetRole).toBool()) {
      auto currentLabel = currentLabelIndex.data().toString();
      QString budget;
      if (m_budgetMap.contains(currentLabel)) {
        budget = m_budgetMap[currentLabel];
      }
      m_summaryModel->itemFromIndex(currentBudgetIndex)->setText(budget);
      UpdatePercentage(currentBudgetIndex);
    }
    UpdateBuget(currentLabelIndex);
  }
}

void AccountWindow::UpdateSummary() {
  double salary = 0;
  double fixedCharges = 0;
  double variableCharges = 0;
  double food = 0;
  double saving = 0;
  double profit = 0;
  double balance = 0;
  double in = 0;
  double out = 0;

  // Clean fixed charges state
  for (auto const& fixedCharge: m_categoriesModel->GetCategoriesByGroup("")) {
    auto fixedChargeLabelItem = m_fixedChargesLabelsMap[fixedCharge];
    QFont validateFont(fixedChargeLabelItem->font());
    validateFont.setBold(false);
    fixedChargeLabelItem->setFont(validateFont);
  }

  // Clean values
  QModelIndex variableChargesIndex;
  for (int outterRow = 0; outterRow < m_summaryModel->rowCount(); ++outterRow) {
    auto outerItem = m_summaryModel->item(outterRow, 1);
    if (outerItem->data(eIsItemSeparatorRole).toBool()) {
      continue;
    }
    outerItem->setText(QString::number(0, 'f', 2));
    auto outterCurrentIndex = m_summaryModel->index(outterRow, 0);
    for (int row = 0; row < m_summaryModel->rowCount(outterCurrentIndex); ++row) {
      auto childIndex = m_summaryModel->index(row, 1, outterCurrentIndex);
      m_summaryModel->itemFromIndex(childIndex)->setText(QString::number(0, 'f', 2));
      auto currentIndex = m_summaryModel->index(row, 0, outterCurrentIndex);
      if (currentIndex.data(eCategoryRole).value<CategoryType>() == eVariableCharges) {
        variableChargesIndex = currentIndex;
        continue;
      }
      for (int innerRow = 0; innerRow < m_summaryModel->rowCount(currentIndex); ++innerRow) {
        auto childIndex0 = m_summaryModel->index(innerRow, 0, currentIndex);
        auto innerLabelItem = m_summaryModel->itemFromIndex(childIndex0);
        auto childIndex1 = m_summaryModel->index(innerRow, 1, currentIndex);
        auto innerValueItem = m_summaryModel->itemFromIndex(childIndex1);
        if (currentIndex.data(eCategoryRole).value<CategoryType>() == eFixedCharges) {
          innerValueItem->setText(QString("-"));
          innerLabelItem->setForeground(QBrush(Utils::GetOrangeColor()));
        } else {
          innerValueItem->setText(QString::number(0, 'f', 2));
        }
      }
    }
  }

  // Clean Variable charges
  Q_ASSERT_X(variableChargesIndex.isValid(), "AccountWindow::UpdateSummary()", "Could not find variableChargesIndex");
  QList<QStandardItem*> variableChargesItemsList;
  int rc = m_summaryModel->rowCount(variableChargesIndex);
  auto variableChargesItem = m_summaryModel->itemFromIndex(variableChargesIndex);
  for (int row = 0; row < rc; ++row) {
    variableChargesItemsList << variableChargesItem->takeRow(0);
  }
  qDeleteAll(variableChargesItemsList);
  m_variableChargesLabelsMap.clear();
  m_variableChargesValuesMap.clear();

  // Update amounts
  for (int row = 0; row < m_csvModel->rowCount(); ++row) {
    auto groupName = m_csvModel->index(row, CSVModel::eGroup).data().toString();
    auto amount = m_csvModel->GetCredit(row) + m_csvModel->GetDebit(row);
    auto group = CategoriesModel::GROUP_BY_NAME[groupName];
    auto categoryName = m_csvModel->index(row, CSVModel::eCategory).data().toString();

    switch(group) {
    case CategoriesModel::eSalary: {
      salary += amount;
      break;
    }
    case CategoriesModel::eFixedCharges: {
      if (!m_categoriesModel->GetCategoriesByGroup("Charges fixes").contains(categoryName))
        break;

      auto currentItem = m_fixedChargesLabelsMap[categoryName];
      auto currentValueItem = m_fixedChargesValuesMap[categoryName];
      currentItem->setForeground(currentValueItem->foreground());
      bool ok = false;
      auto oldAmount = currentValueItem->text().toDouble(&ok);
      if (!ok) {
        oldAmount = 0;
      }
      auto newAmount = amount + oldAmount;
      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      fixedCharges += amount;
      break;
    }
    case CategoriesModel::eVariableCharges: {
      QStandardItem* currentLabelItem = nullptr;
      QStandardItem* currentValueItem = nullptr;
      auto currentBudgetItem = new QStandardItem;
      currentBudgetItem->setData(true, eCanHaveBudgetRole);
      currentBudgetItem->setTextAlignment(Qt::AlignRight);
      QStandardItem* currentPercentageItem = nullptr;
      if (m_variableChargesLabelsMap.contains(categoryName)) {
        currentLabelItem = m_variableChargesLabelsMap[categoryName];
        currentValueItem = m_variableChargesValuesMap[categoryName];
        auto currentLabelIndex = currentLabelItem->index();
        currentPercentageItem = m_summaryModel->itemFromIndex(m_summaryModel->sibling(currentLabelIndex.row(), 3, currentLabelIndex));
      } else {
        currentLabelItem = new QStandardItem(categoryName);
        currentValueItem = new QStandardItem(QString::number(0, 'f', 2));
        currentValueItem->setTextAlignment(Qt::AlignRight);
        currentPercentageItem = new QStandardItem;
        currentPercentageItem->setTextAlignment(Qt::AlignRight);
        variableChargesItem->appendRow({currentLabelItem, currentValueItem, currentBudgetItem, currentPercentageItem});
        m_variableChargesLabelsMap[categoryName] = currentLabelItem;
        m_variableChargesValuesMap[categoryName] = currentValueItem;
      }
      auto oldAmount = currentValueItem->text().toDouble();
      auto newAmount = amount + oldAmount;
      auto budget = currentBudgetItem->data(Qt::DisplayRole).toDouble();

      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      if (budget != 0.) {
        auto percentage = QString::number(-newAmount/budget*100., 'f', 2)+"%";
        currentPercentageItem->setText(percentage);
      }

      variableCharges += amount;
      break;
    }
    case CategoriesModel::eFood: {
      if (!m_categoriesModel->GetCategoriesByGroup("Nourriture").contains(categoryName))
        break;

      auto currentValueItem = m_foodValuesMap[categoryName];
      auto oldAmount = currentValueItem->text().toDouble();
      auto newAmount = amount + oldAmount;
      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      food += amount;
      break;
    }
    case CategoriesModel::eSaving: {
      saving += amount;
      break;
    }
    case CategoriesModel::eProfit: {
      if (!m_categoriesModel->GetCategoriesByGroup("Gain").contains(categoryName))
        break;

      auto currentValueItem = m_profitValuesMap[categoryName];
      auto oldAmount = currentValueItem->text().toDouble();
      auto newAmount = amount + oldAmount;
      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      profit += amount;
      break;
    }
    case CategoriesModel::eUnknown: {
      break;
    }
    }
  }

  // Compute in, out and balance
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

  m_summaryView->setColumnWidth(2, 40);
  m_summaryView->setColumnWidth(3, 60);
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

void AccountWindow::UpdatePercentage(QModelIndex const& p_index) {
  auto row = p_index.row();

  auto amount = m_summaryModel->sibling(row, 1, p_index).data().toDouble();
  auto budget = m_summaryModel->sibling(row, 2, p_index).data().toDouble();

  QVariant newPercentage;
  if (budget != 0.) {
    newPercentage = QString::number(-amount/budget*100., 'f', 2)+"%";
  }

  auto percentageItem = m_summaryModel->itemFromIndex(m_summaryModel->sibling(row, 3, p_index));
  percentageItem->setData(newPercentage, Qt::DisplayRole);
}

void AccountWindow::SaveBudget(QModelIndex const& p_index) {
  auto budgetItem = m_summaryModel->itemFromIndex(p_index);
  auto labelItem = m_summaryModel->itemFromIndex(m_summaryModel->sibling(p_index.row(), 0, p_index));

  QString csvDirectoryPath = "../BankAccount/csv";
  QString accountDirectoryName = QDate(m_year, m_month, 1).toString("MM-yyyy");
  QString accountDirectoryPath = csvDirectoryPath+QDir::separator()+accountDirectoryName;
  QString inFilePath = accountDirectoryPath+QDir::separator()+"budget.csv";

  QFile budgetCSV(inFilePath);

  QTextStream in(&budgetCSV);
  in.setCodec("UTF-8");

  if (!budgetCSV.open(QIODevice::ReadWrite | QIODevice::Text)) {
    budgetItem->setText("Error");
    return;
  }

  bool found = false;
  QString line;
  QStringList budgetStringList;
  while (!in.atEnd()) {
    line = in.readLine();
    if (line.isEmpty()) {
      continue;
    }
    auto tokens = line.split(";");
    if (tokens.at(0) == labelItem->text()) {
      budgetStringList << tokens.at(0)+";"+p_index.data().toString();
      found = true;
    } else {
      budgetStringList << tokens.join(";");
    }
  }
  if (!found) {
    budgetStringList << labelItem->text()+";"+p_index.data().toString();
  }

  budgetCSV.close();

  QTextStream out(&budgetCSV);
  out.setCodec("UTF-8");

  if (!budgetCSV.open(QIODevice::WriteOnly | QIODevice::Text)) {
    budgetItem->setText("Error");
    return;
  }

  for (auto const& newLine: budgetStringList) {
    out << newLine << "\n";
  }
}

void AccountWindow::AddSeparator() {
  auto separator0 = new QStandardItem("");
  separator0->setFlags(Qt::NoItemFlags);
  separator0->setData(true, eIsItemSeparatorRole);
  auto separator1 = new QStandardItem("");
  separator1->setFlags(Qt::NoItemFlags);
  separator1->setData(true, eIsItemSeparatorRole);
  m_summaryModel->appendRow({separator0, separator1, new QStandardItem, new QStandardItem});
}
