#include "ChartWindow.hxx"

#include "qcustomplot.hxx"

ChartWindow::ChartWindow(QWidget* p_parent):
  QWidget(p_parent) {

  auto customPlot = new QCustomPlot;
  auto myBar = new QCPBars(customPlot->xAxis, customPlot->yAxis);
  customPlot->addPlottable(myBar);
  myBar->addData(
    QVector<double>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12,
    QVector<double>() << 140 << 137 << 209 << 176 << 150 << 157 << 152 << 137 << 149 << 148 << 152 << 187);
  customPlot->xAxis->setRange(0, 13);
  customPlot->yAxis->setRange(0, 210);
  customPlot->setFixedSize(600, 400);

  auto mainLayout = new QVBoxLayout;
  mainLayout->addWidget(customPlot);

  setLayout(mainLayout);
}
