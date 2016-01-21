#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>

#include "CSVModel.hxx"
#include <QTableView>

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);

private:
  CSVModel* m_csvModel;
  QTableView* m_tableView;

};

#endif // MAINWINDOW_HXX
