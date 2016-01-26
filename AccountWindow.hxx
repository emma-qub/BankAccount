#ifndef ACCOUNTWINDOW_HXX
#define ACCOUNTWINDOW_HXX

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class CSVModel;
class QTableView;

class AccountWindow: public QWidget {
  Q_OBJECT

public:
  explicit AccountWindow(QWidget* parent = nullptr);

private slots:
  void goToPreviousYear();
  void goToPreviousMonth();
  void goToNextMonth();
  void goToNextYear();
  void updateMonth();
  void updateYear();

signals:
  void monthChanged();
  void yearChanged();

private:
  void fillModel();

  CSVModel* m_csvModel;
  QTableView* m_tableView;

  QLabel* m_monthLabel;
  QLabel* m_yearLabel;
  QPushButton* m_previousYear;
  QPushButton* m_previousMonth;
  QPushButton* m_nextMonth;
  QPushButton* m_nextYear;

  int m_month;
  int m_year;
};

#endif // ACCOUNTWINDOW_HXX
