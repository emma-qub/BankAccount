#ifndef CHARTWIDGET_HXX
#define CHARTWIDGET_HXX

#include "CategoryChartGenerator.hxx"
#include "CSVModel.hxx"

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QTabWidget;
class QLabel;
class QDateEdit;
class QPushButton;
class QMenu;

namespace QtCharts {
class QChartView;
}

class ChartWindow: public QWidget {
  Q_OBJECT

public:
  explicit ChartWindow(CSVModel* p_model, QWidget* p_parent = nullptr);

protected:
  void UpdateCategoryChart();
  void SelectGroupAndUpdateCategoryChart(QAction* p_action);

Q_SIGNALS:
  void GroupToggled(QAction* p_action);

private:
  CSVModel* m_model;
  QDateEdit* m_beginDateCalendar;
  QDateEdit* m_endDateCalendar;
  QHBoxLayout* m_chartOptionsLayout;
  QVBoxLayout* m_categoryChartLayout;
  QVBoxLayout* m_mainLayout;
  QtCharts::QChartView* m_categoryChartView;
  QLabel* m_averageLabel;
  QLabel* m_totalLabel;
  CategoryChartGenerator* m_categoryChartGenerator;
  QPushButton* m_categoryButton;
  QMenu* m_categoryMenu;
  QMap<QAction*, QList<QAction*>> m_actionsByGroup;
};

#endif
