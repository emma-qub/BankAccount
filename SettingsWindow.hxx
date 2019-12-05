#ifndef SETTINGSWINDOW_HXX
#define SETTINGSWINDOW_HXX

#include <QWidget>

class CategoriesModel;
class QTreeView;

class SettingsWindow: public QWidget {
  Q_OBJECT

public:
  explicit SettingsWindow(CategoriesModel* p_categoriesModel, QWidget* p_parent = nullptr);

private:
  CategoriesModel* m_categoriesModel;
  QTreeView* m_categoriesView;
};

#endif
