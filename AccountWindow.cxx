#include "AccountWindow.hxx"

#include "CSVModel.hxx"
#include "MonthlyCSVGenerator.hxx"

#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>

#include <QSortFilterProxyModel>

#include <QDebug>

AccountWindow::AccountWindow(QWidget* parent) : QWidget(parent) {
  m_csvModel = new CSVModel(this);
  fillModel();

  auto proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(m_csvModel);

  m_tableView = new QTableView;
  m_tableView->setModel(proxyModel);
  m_tableView->setAlternatingRowColors(true);
  m_tableView->verticalHeader()->hide();
  m_tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
  m_tableView->setFocusPolicy(Qt::NoFocus);
  m_tableView->setSelectionMode(QTableView::NoSelection);
  m_tableView->setSortingEnabled(true);
  m_tableView->sortByColumn(0, Qt::AscendingOrder);

  auto mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_tableView);

  setLayout(mainLayout);
}

void AccountWindow::fillModel() {
  MonthlyCSVGenerator::convertRawCSVToMonthlyCSV("../BankAccount/csv/E0213562.csv", "../BankAccount/csv/012016.csv");
  m_csvModel->setSource("../BankAccount/csv/012016.csv");
}
