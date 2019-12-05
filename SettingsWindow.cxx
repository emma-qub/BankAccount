#include "SettingsWindow.hxx"

#include "CategoriesModel.hxx"

#include <QHeaderView>
#include <QTreeView>
#include <QVBoxLayout>

SettingsWindow::SettingsWindow(CategoriesModel* p_categoriesModel, QWidget* p_parent):
  QWidget(p_parent),
  m_categoriesModel(p_categoriesModel),
  m_categoriesView(new QTreeView) {

  m_categoriesView->setModel(m_categoriesModel);
  m_categoriesView->header()->hide();
  m_categoriesView->expandAll();

  auto mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_categoriesView);
  setLayout(mainLayout);
}
