#ifndef BUDGETITEMDELEGATE_HXX
#define BUDGETITEMDELEGATE_HXX

#include <QStyledItemDelegate>

class BudgetItemDelegate: public QStyledItemDelegate {
  Q_OBJECT

public:
  BudgetItemDelegate(QObject* p_parent = nullptr);

  ~BudgetItemDelegate() override;

  QWidget* createEditor(QWidget* p_parent, QStyleOptionViewItem const& p_option, QModelIndex const& p_index) const override;

  void setEditorData(QWidget* p_editor, QModelIndex const& p_index) const override;

  void setModelData(QWidget* p_editor, QAbstractItemModel* p_model, QModelIndex const& p_index) const override;

Q_SIGNALS:
  void BudgetUpdated(QModelIndex const& p_index) const;
};

#endif
