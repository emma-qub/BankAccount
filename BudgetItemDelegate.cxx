#include "BudgetItemDelegate.hxx"

#include "AccountWindow.hxx"

#include <QStandardItemModel>
#include <QDoubleSpinBox>

BudgetItemDelegate::BudgetItemDelegate(QObject* p_parent):
  QStyledItemDelegate(p_parent) {
}

BudgetItemDelegate::~BudgetItemDelegate() = default;

QWidget* BudgetItemDelegate::createEditor(QWidget* p_parent, QStyleOptionViewItem const& p_option, QModelIndex const& p_index) const {
  Q_UNUSED(p_option)

  if (p_index.column() == 2) {
    if (p_index.data(AccountWindow::eCanHaveBudgetRole).toBool()) {
      auto editor = new QDoubleSpinBox(p_parent);
      editor->setRange(0, 10000);
      return editor;
    }
  }

  return nullptr;
}

void BudgetItemDelegate::setEditorData(QWidget* p_editor, QModelIndex const& p_index) const {
  if (p_index.column() == 2) {
    auto editor = qobject_cast<QDoubleSpinBox*>(p_editor);
    editor->setValue(p_index.data().toDouble());
  }
}

void BudgetItemDelegate::setModelData(QWidget* p_editor, QAbstractItemModel* p_model, QModelIndex const& p_index) const {
  if (p_index.column() == 2) {
    auto editor = qobject_cast<QDoubleSpinBox*>(p_editor);
    auto model = qobject_cast<QStandardItemModel*>(p_model);

    if (editor->value() != 0.) {
      model->setData(p_index, editor->value());
    } else {
      model->setData(p_index,  QVariant());
    }

    Q_EMIT BudgetUpdated(p_index);
  }
}
