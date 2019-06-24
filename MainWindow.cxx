#include "MainWindow.hxx"

#include <QTabWidget>

#include "AccountWindow.hxx"
#include "ChartWindow.hxx"
#include "SettingsWindow.hxx"

MainWindow::MainWindow(QWidget* parent):
  QMainWindow(parent) {

  m_accountWindow = new AccountWindow;
  m_chartWindow = new ChartWindow(m_accountWindow->GetModel(), m_accountWindow->GetYear(), m_accountWindow->GetMonth());
  m_settingsWindow = new SettingsWindow;

  m_tabWidget = new QTabWidget;
  m_tabWidget->addTab(m_accountWindow, QIcon("../BankAccount/icons/accountTab"), "");
  m_tabWidget->addTab(m_chartWindow, QIcon("../BankAccount/icons/chartTab"), "");
  m_tabWidget->addTab(m_settingsWindow, QIcon("../BankAccount/icons/settingsTab"), "");

  m_tabWidget->setIconSize(QSize(96, 96));
  m_tabWidget->setTabPosition(QTabWidget::West);
  m_tabWidget->setFocusPolicy(Qt::NoFocus);

  connect(m_tabWidget, &QTabWidget::currentChanged, this, [this](int p_tab) {
    if (p_tab == 1) {
      m_chartWindow->UpdateMonthlyChart(m_accountWindow->GetYear(), m_accountWindow->GetMonth());
      m_chartWindow->UpdateMonthlyBalance(m_accountWindow->GetYear(), m_accountWindow->GetMonth());
    }
  });

  QString tabBarStyle;
  tabBarStyle += "QTabBar::tab {";
  tabBarStyle += "    background-color: #ededed;";
  tabBarStyle += "}";
  tabBarStyle += "QTabWidget::pane {";
  tabBarStyle += "    border: solid 1px #fff;";
  tabBarStyle += "}";
  tabBarStyle += "QTabBar::tab:selected {";
  tabBarStyle += "    background-color: #80c342;";
  tabBarStyle += "}";

  setStyleSheet(tabBarStyle);

  setCentralWidget(m_tabWidget);

  setWindowTitle("Bank Account Manager");
  setWindowState(Qt::WindowMaximized);
  setWindowIcon(QIcon("../BankAccount/icons/appIcon.png"));
}
