#ifndef ACCOUNTWINDOW_HXX
#define ACCOUNTWINDOW_HXX

#include <QWidget>

class CSVModel;
class QTableView;

class AccountWindow : public QWidget
{
  Q_OBJECT
public:
  explicit AccountWindow(QWidget *parent = 0);

signals:

public slots:

private:
  CSVModel* m_csvModel;
  QTableView* m_tableView;
};

#endif // ACCOUNTWINDOW_HXX
