#ifndef SETTINGSWINDOW_HXX
#define SETTINGSWINDOW_HXX

#include <QWidget>

class SettingsWindow: public QWidget {
  Q_OBJECT

public:
  explicit SettingsWindow(QWidget* p_parent = nullptr);
};

#endif // SETTINGSWINDOW_HXX
