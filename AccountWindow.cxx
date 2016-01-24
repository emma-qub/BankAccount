#include "AccountWindow.hxx"

#include "CSVModel.hxx"
#include "MonthlyCSVGenerator.hxx"

#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QDate>

#include <QDebug>

AccountWindow::AccountWindow(QWidget* parent):
  QWidget(parent) {

  auto currentDate = QDate::currentDate();
  QFont font;
  font.setPixelSize(25);
  font.setCapitalization(QFont::Capitalize);
  font.setBold(99);
  m_monthLabel = new QLabel(currentDate.toString("MMMM"));
  m_monthLabel->setFont(font);
  m_yearLabel = new QLabel(currentDate.toString("yyyy"));
  m_yearLabel->setFont(font);
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

  auto dateLayout = new QHBoxLayout;
  dateLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
  dateLayout->addWidget(m_previousYear);
  dateLayout->setAlignment(m_previousYear, Qt::AlignCenter);
  dateLayout->addWidget(m_previousMonth);
  dateLayout->setAlignment(m_previousMonth, Qt::AlignCenter);
  dateLayout->addWidget(m_monthLabel);
  dateLayout->setAlignment(m_monthLabel, Qt::AlignCenter);
  dateLayout->addWidget(m_yearLabel);
  dateLayout->setAlignment(m_yearLabel, Qt::AlignCenter);
  dateLayout->addWidget(m_nextMonth);
  dateLayout->setAlignment(m_nextMonth, Qt::AlignCenter);
  dateLayout->addWidget(m_nextYear);
  dateLayout->setAlignment(m_nextYear, Qt::AlignCenter);
  dateLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));

  m_csvModel = new CSVModel(this);
  fillModel();

  auto proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(m_csvModel);

  m_tableView = new QTableView;
  m_tableView->setModel(proxyModel);
  m_tableView->setAlternatingRowColors(true);
  m_tableView->verticalHeader()->hide();
  if (m_csvModel->rowCount() > 0) {
    m_tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
  }
  m_tableView->setFocusPolicy(Qt::NoFocus);
  m_tableView->setSelectionMode(QTableView::NoSelection);
  //m_tableView->setSortingEnabled(true);
  //m_tableView->sortByColumn(0, Qt::AscendingOrder);

  auto mainLayout = new QVBoxLayout;
  mainLayout->addLayout(dateLayout);
  mainLayout->addWidget(m_tableView);

  setLayout(mainLayout);
}

void AccountWindow::fillModel() {
  MonthlyCSVGenerator::convertRawCSVToMonthlyCSV(QDate::currentDate(), "../BankAccount/csv/raw.csv");
  m_csvModel->setSource("../BankAccount/csv/"+QDate::currentDate().toString("MM-yyyy")+"/operations.csv");
}
