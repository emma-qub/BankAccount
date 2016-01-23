#ifndef CHARTWIDGET_HXX
#define CHARTWIDGET_HXX

#include <QWidget>

class ChartWindow: public QWidget {
  Q_OBJECT

public:
  explicit ChartWindow(QWidget* p_parent = nullptr);
};

#endif // CHARTWIDGET_HXX
