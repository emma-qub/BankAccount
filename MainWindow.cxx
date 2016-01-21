#include "MainWindow.hxx"

MainWindow::MainWindow(QWidget* parent):
  QMainWindow(parent) {

  m_csvModel = new CSVModel(this);
  m_csvModel->setSource("../BankAccount/csv/E0133562.csv");

  m_tableView = new QTableView;
  m_tableView->setModel(m_csvModel);

  setCentralWidget(m_tableView);

  setWindowTitle("Bank Account Manager");
  setWindowState(Qt::WindowMaximized);
  setWindowIcon(QIcon("../BankAccount/icons/appIcon.png"));
}
