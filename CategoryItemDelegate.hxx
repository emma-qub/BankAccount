#ifndef CATEGORYITEMDELEGATE_HXX
#define CATEGORYITEMDELEGATE_HXX

#include <QStyledItemDelegate>

class CategoryItemDelegate: public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit CategoryItemDelegate(QWidget* p_parent = nullptr);

  QWidget* createEditor(QWidget* p_parent, QStyleOptionViewItem const& p_option, QModelIndex const& p_index) const override;
  void setEditorData(QWidget* p_editor, QModelIndex const& p_index) const override;
  void setModelData(QWidget* p_editor, QAbstractItemModel* p_model, QModelIndex const& p_index) const override;
};

#endif
