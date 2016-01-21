#include "AccountWindow.hxx"

#include "CSVModel.hxx"

#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>

AccountWindow::AccountWindow(QWidget *parent) : QWidget(parent)
{
  m_csvModel = new CSVModel(this);
  m_csvModel->setSource("../BankAccount/csv/E0213562.csv");

  m_tableView = new QTableView;
  m_tableView->setModel(m_csvModel);
  m_tableView->setAlternatingRowColors(true);
  m_tableView->verticalHeader()->hide();
  m_tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
  m_tableView->setFocusPolicy(Qt::NoFocus);
  m_tableView->setSelectionMode(QTableView::NoSelection);

  auto mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_tableView);

  setLayout(mainLayout);
}
