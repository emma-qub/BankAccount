#ifndef CATEGORYITEMDELEGATE_HXX
#define CATEGORYITEMDELEGATE_HXX

#include <QStyledItemDelegate>

class CategoryItemDelegate: public QStyledItemDelegate {
public:
  explicit CategoryItemDelegate(QWidget* p_parent = nullptr);

  QWidget* createEditor(QWidget* p_parent, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const override;
  void setEditorData(QWidget* p_editor, const QModelIndex& p_index) const override;
  void setModelData(QWidget* p_editor, QAbstractItemModel* p_model, const QModelIndex& p_index) const override;
};

#endif
