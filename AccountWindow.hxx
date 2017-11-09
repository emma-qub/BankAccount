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

  inline CSVModel* getModel() const {return m_csvModel;}
  inline int getYear() const {return m_year;}
  inline int getMonth() const {return m_month;}

protected:
  QString getCurrentCSVFileName() const;

  void updateSummary();

protected slots:
  void goToPreviousYear();
  void goToPreviousMonth();
  void goToNextMonth();
  void goToNextYear();
  void updateMonth();
  void updateYear();
  void fillModel();
  void saveCategory(int p_row, const QString& p_group, const QString& p_category);
  void reloadFile();

signals:
  void monthChanged();
  void yearChanged();
  void updateModelRequested();

private:
  CSVModel* m_csvModel;
  QTableView* m_tableView;

  QLabel* m_monthLabel;
  QLabel* m_yearLabel;
  QPushButton* m_previousYear;
  QPushButton* m_previousMonth;
  QPushButton* m_nextMonth;
  QPushButton* m_nextYear;

  QLabel* m_salaryLabel;
  QLabel* m_fixedChargesLabel;
  QLabel* m_variableChargesLabel;
  QLabel* m_foodLabel;
  QLabel* m_savingLabel;
  QLabel* m_profitLabel;
  QLabel* m_balanceLabel;

  int m_month;
  int m_year;
};

#endif // ACCOUNTWINDOW_HXX
