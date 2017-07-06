#include "AccountWindow.hxx"

#include "CSVModel.hxx"
#include "MonthlyCSVGenerator.hxx"
#include "CategoryItemDelegate.hxx"

#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QDate>
#include <QAction>
#include <QFileDialog>

#include <QDebug>

AccountWindow::AccountWindow(QWidget* parent):
  QWidget(parent) {

  auto reloadAction = new QAction("Reload data...", this);
  reloadAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  connect(reloadAction, &QAction::triggered, this, &AccountWindow::reloadFile);
  addAction(reloadAction);

  QDate currentDate = QDate::currentDate();
  m_month = currentDate.month();
  m_year = currentDate.year();

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

  connect(m_previousYear, &QPushButton::clicked, this, &AccountWindow::goToPreviousYear);
  connect(m_previousMonth, &QPushButton::clicked, this, &AccountWindow::goToPreviousMonth);
  connect(m_nextMonth, &QPushButton::clicked, this, &AccountWindow::goToNextMonth);
  connect(m_nextYear, &QPushButton::clicked, this, &AccountWindow::goToNextYear);
  connect(this, &AccountWindow::yearChanged, this, &AccountWindow::updateYear);
  connect(this, &AccountWindow::monthChanged, this, &AccountWindow::updateMonth);
  connect(this, &AccountWindow::updateModelRequested, this, &AccountWindow::fillModel);

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

  m_csvModel = new CSVModel(this);
  fillModel();
  connect(m_csvModel, &CSVModel::saveCategoryRequested, this, &AccountWindow::saveCategory);

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
  //m_tableView->setSortingEnabled(true);
  //m_tableView->sortByColumn(0, Qt::AscendingOrder);

  auto mainLayout = new QVBoxLayout;
  mainLayout->addLayout(dateLayout);
  mainLayout->addWidget(m_tableView);

  setLayout(mainLayout);
}

QString AccountWindow::getCurrentCSVFileName() const {
  return QString("../BankAccount/csv/"+QDate(m_year, m_month, 1).toString("MM-yyyy")+"/operations.csv");
}

void AccountWindow::goToPreviousYear() {
  m_year--;
  emit yearChanged();
  emit updateModelRequested();
}

void AccountWindow::goToPreviousMonth() {
  //m_month==1?m_month=12,m_year--:m_month=(m_month+=11)%12;
  if (m_month == 1) {
    m_month = 12;
    m_year--;
    emit yearChanged();
  } else
    m_month--;

  emit monthChanged();
  emit updateModelRequested();
}

void AccountWindow::goToNextMonth() {
  //m_month=(m_month+=13)%12==0?m_month=12:m_year+=(m_month==1);
  if (m_month == 12) {
    m_month = 1;
    m_year++;
    emit yearChanged();
  } else
    m_month++;

  emit monthChanged();
  emit updateModelRequested();
}

void AccountWindow::goToNextYear() {
  m_year++;
  emit yearChanged();
  emit updateModelRequested();
}

void AccountWindow::updateMonth() {
  m_monthLabel->setText(QDate(1, m_month, 1).toString("MMMM"));
}

void AccountWindow::updateYear() {
  m_yearLabel->setText(QString::number(m_year));
}

void AccountWindow::fillModel() {
  m_csvModel->setSource(getCurrentCSVFileName());
  if (m_csvModel->rowCount() > 0)
    m_tableView->horizontalHeader()->setSectionResizeMode(CSVModel::eLabel, QHeaderView::Stretch);
}

void AccountWindow::saveCategory(int p_row, const QString& p_group, QString const& p_category) {
  MonthlyCSVGenerator::saveCategory(p_row, p_group, p_category, getCurrentCSVFileName());
}

void AccountWindow::reloadFile() {
  auto csvFileName = QFileDialog::getOpenFileName(this, "Import CSV file", "../BankAccount/csv", "EXEL files (*.xls *.XLS);;CSV files (*.csv *.CSV)");
  if (csvFileName.isEmpty()) {
    return;
  }

  if (csvFileName.endsWith("xls")) {
    MonthlyCSVGenerator::convertXLSToCSV(csvFileName);
  }

  auto currentDate = QDate(m_year, m_month, 1);
  MonthlyCSVGenerator::updateRawCSV(currentDate, csvFileName);

  fillModel();
}
