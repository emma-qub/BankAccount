#ifndef CHARTWIDGET_HXX
#define CHARTWIDGET_HXX

#include "CategoryChartGenerator.hxx"
#include "CSVModel.hxx"

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QComboBox;
class QTabWidget;
class QLabel;
class QDateEdit;

namespace QtCharts {
class QChartView;
}

class ChartWindow: public QWidget {
  Q_OBJECT

public:
  explicit ChartWindow(CSVModel* p_model, QWidget* p_parent = nullptr);

protected:
  void UpdateCategoryChart();

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
  QComboBox* m_categoryComboBox;
  CategoryChartGenerator* m_categoryChartGenerator;
};

#endif
