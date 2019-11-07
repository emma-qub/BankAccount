#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>

class AccountWindow;
class ChartWindow;
class SettingsWindow;
class QTabWidget;

class MainWindow: public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);

private:
  QTabWidget* m_tabWidget;

  AccountWindow* m_accountWindow;
  ChartWindow* m_chartWindow;
  SettingsWindow* m_settingsWindow;

  QAction* m_updateOperationsAction;
};

#endif
