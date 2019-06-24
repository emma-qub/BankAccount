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

  // Toggle action
  auto toggleAction = new QAction("Toggle nul values", this);
  toggleAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_T));
  toggleAction->setCheckable(true);
  connect(toggleAction, &QAction::toggled, this, [this](bool p_toggle){
    p_toggle ?
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
  inItem->appendRow({salaryLabelItem, m_salaryItem});
  // Profit
  m_profitItem = new QStandardItem(QString::number(0, 'f', 2));
  m_profitItem->setTextAlignment(Qt::AlignRight);
  auto profitLabelItem = new QStandardItem("Gain");
  profitLabelItem->setData(QVariant::fromValue<CategoryType>(eProfit), eCategoryRole);
  m_profitList = Utils::GAIN;
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
  m_fixedChargesList = Utils::CHARGES_FIXES;
  for (auto const& fixedCharge: m_fixedChargesList) {
    auto currentFixedChargeItem = new QStandardItem(fixedCharge);
    currentFixedChargeItem->setForeground(QBrush(Utils::GetOrangeColor()));
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
  variableChargesLabelItem->setData(QVariant::fromValue<CategoryType>(eVariableCharges), eCategoryRole);
  outItem->appendRow({variableChargesLabelItem, m_variableChargesItem});
  // Food
  m_foodItem = new QStandardItem(QString::number(0, 'f', 2));
  m_foodItem->setTextAlignment(Qt::AlignRight);
  auto foodLabelItem = new QStandardItem("Nourriture");
  foodLabelItem->setData(QVariant::fromValue<CategoryType>(eFood), eCategoryRole);
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
  auto savingLabelItem = new QStandardItem("Épargne");
  savingLabelItem->setData(QVariant::fromValue<CategoryType>(eSaving), eCategoryRole);
  outItem->appendRow({savingLabelItem, m_savingItem});

  // Balance
  m_balanceItem = new QStandardItem(QString::number(0, 'f', 2));
  m_balanceItem->setTextAlignment(Qt::AlignRight);
  auto balanceItemFont = m_balanceItem->font();
  balanceItemFont.setBold(true);
  m_balanceItem->setFont(balanceItemFont);

  // Model
  m_categoryModel->appendRow({inItem, m_inItem});
  AddSeparator();
  m_categoryModel->appendRow({outItem, m_outItem});
  auto separator2 = new QStandardItem("");
  separator2->setSelectable(false);
  AddSeparator();
  auto balanceItem = new QStandardItem("Équilibre");
  balanceItem->setFont(balanceItemFont);
  m_categoryModel->appendRow({balanceItem, m_balanceItem});

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

  // Expand view
  m_categoryView->expandToDepth(0);
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
  for (auto const& fixedCharge: m_fixedChargesList) {
    auto fixedChargeLabelItem = m_fixedChargesLabelsMap[fixedCharge];
    QFont validateFont(fixedChargeLabelItem->font());
    validateFont.setBold(false);
    fixedChargeLabelItem->setFont(validateFont);
  }

  // Clean values
  QModelIndex variableChargesIndex;
  for (int outterRow = 0; outterRow < m_categoryModel->rowCount(); ++outterRow) {
    auto outerItem = m_categoryModel->item(outterRow, 1);
    if (outerItem->data(eIsItemSeparatorRole).toBool()) {
      continue;
    }
    outerItem->setText(QString::number(0, 'f', 2));
    auto outterCurrentIndex = m_categoryModel->index(outterRow, 0);
    for (int row = 0; row < m_categoryModel->rowCount(outterCurrentIndex); ++row) {
      m_categoryModel->itemFromIndex(outterCurrentIndex.child(row, 1))->setText(QString::number(0, 'f', 2));
      auto currentIndex = m_categoryModel->index(row, 0, outterCurrentIndex);
      if (currentIndex.data(eCategoryRole).value<CategoryType>() == eVariableCharges) {
        variableChargesIndex = currentIndex;
        continue;
      }
      for (int innerRow = 0; innerRow < m_categoryModel->rowCount(currentIndex); ++innerRow) {
        auto innerLabelItem = m_categoryModel->itemFromIndex(currentIndex.child(innerRow, 0));
        auto innerValueItem = m_categoryModel->itemFromIndex(currentIndex.child(innerRow, 1));
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
  int rc = m_categoryModel->rowCount(variableChargesIndex);
  auto variableChargesItem = m_categoryModel->itemFromIndex(variableChargesIndex);
  for (int row = 0; row < rc; ++row) {
    variableChargesItemsList << variableChargesItem->takeRow(0);
  }
  qDeleteAll(variableChargesItemsList);
  m_variableChargesLabelsMap.clear();
  m_variableChargesValuesMap.clear();

  // Update amounts
  for (int row = 0; row < m_csvModel->rowCount(); ++row) {
    QString groupName = m_csvModel->index(row, CSVModel::eGroup).data().toString();
    auto amount = m_csvModel->GetCredit(row) + m_csvModel->GetDebit(row);
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
    case Utils::eVariableCharges: {
      QStandardItem* currentLabelItem = nullptr;
      QStandardItem* currentValueItem = nullptr;
      if (m_variableChargesLabelsMap.contains(categoryName)) {
        currentLabelItem = m_variableChargesLabelsMap[categoryName];
        currentValueItem = m_variableChargesValuesMap[categoryName];
      } else {
        currentLabelItem = new QStandardItem(categoryName);
        currentValueItem = new QStandardItem(QString::number(0, 'f', 2));
        currentValueItem->setTextAlignment(Qt::AlignRight);
        variableChargesItem->appendRow({currentLabelItem, currentValueItem});
        m_variableChargesLabelsMap[categoryName] = currentLabelItem;
        m_variableChargesValuesMap[categoryName] = currentValueItem;
      }
      auto oldAmount = currentValueItem->text().toDouble();
      auto newAmount = amount + oldAmount;

      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      variableCharges += amount;
      break;
    }
    case Utils::eFood: {
      Q_ASSERT_X(m_foodList.contains(categoryName), "AccountWindow::updateSummary()", tr("Unknown food: %1").arg(categoryName).toStdString().c_str());
      auto currentValueItem = m_foodValuesMap[categoryName];
      auto oldAmount = currentValueItem->text().toDouble();
      auto newAmount = amount + oldAmount;
      currentValueItem->setText(QString::number(newAmount, 'f', 2));
      food += amount;
      break;
    }
    case Utils::eSaving: {
      saving += amount;
      break;
    }
    case Utils::eProfit: {
      Q_ASSERT_X(m_profitList.contains(categoryName), "AccountWindow::updateSummary()", tr("Unknown profit: %1").arg(categoryName).toStdString().c_str());
      auto currentValueItem = m_profitValuesMap[categoryName];
      auto oldAmount = currentValueItem->text().toDouble();
      auto newAmount = amount + oldAmount;
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

  ///m_categoryView->expandToDepth(0);
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
  separator0->setData(true, eIsItemSeparatorRole);
  auto separator1 = new QStandardItem("");
  separator1->setFlags(Qt::NoItemFlags);
  separator1->setData(true, eIsItemSeparatorRole);
  m_categoryModel->appendRow({separator0, separator1});
}
