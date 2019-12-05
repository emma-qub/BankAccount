#ifndef CHARTWIDGET_HXX
#define CHARTWIDGET_HXX

#include "CategoryChartGenerator.hxx"

#include <QWidget>

class CSVModel;
class CategoriesModel;
class QVBoxLayout;
class QHBoxLayout;
class QTabWidget;
class QLabel;
class QDateEdit;
class QPushButton;
class QMenu;
class QRadioButton;

#include <QtCharts/QStackedBarSeries>

class ChartWindow: public QWidget {
  Q_OBJECT

public:
  explicit ChartWindow(CSVModel* p_model, CategoriesModel* p_categoriesModel, QWidget* p_parent = nullptr);

protected:
  void UpdateCategoryChart();
  void UpdateGroupChart();
  void SelectGroupAndUpdateCategoryChart(QAction* p_action);

Q_SIGNALS:
  void GroupToggled(QAction* p_action);

private:
  CSVModel* m_model;
  CategoriesModel* m_categoriesModel;

  QDateEdit* m_beginDateCalendar;
  QDateEdit* m_endDateCalendar;

  QHBoxLayout* m_chartOptionsLayout;
  QVBoxLayout* m_categoryChartLayout;
  QVBoxLayout* m_mainLayout;
  QtCharts::QChartView* m_categoryChartView;
  QLabel* m_averageLabel;
  QLabel* m_totalLabel;
  QLabel* m_hoveredAverageLabel;
  QLabel* m_hoveredCumulLabel;
  CategoryChartGenerator* m_categoryChartGenerator;
  QRadioButton* m_categoryRadioButton;
  QPushButton* m_categoryButton;
  QMenu* m_categoryMenu;
  QRadioButton* m_groupRadioButton;
  QPushButton* m_groupButton;
  QMenu* m_groupMenu;
  QMap<QAction*, QList<QAction*>> m_actionsByGroup;
};

#endif
