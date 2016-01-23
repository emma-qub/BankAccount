#ifndef ACCOUNTWINDOW_HXX
#define ACCOUNTWINDOW_HXX

#include <QWidget>

class CSVModel;
class QTableView;

class AccountWindow: public QWidget {
  Q_OBJECT

public:
  explicit AccountWindow(QWidget* parent = nullptr);

private:
  void fillModel();

  CSVModel* m_csvModel;
  QTableView* m_tableView;
};

#endif // ACCOUNTWINDOW_HXX
