#include "CategoryItemDelegate.hxx"

#include <QComboBox>

CategoryItemDelegate::CategoryItemDelegate(QWidget* p_parent):
  QStyledItemDelegate(p_parent) {

}

QWidget* CategoryItemDelegate::createEditor(QWidget* p_parent, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const {
  if (p_index.isValid() && p_index.column() == 1) {
    auto editor = new QComboBox(p_parent);
    editor->addItems(
          QStringList()
          << "Assurance"
          << "Banque"
          << "Cadeau donné"
          << "Cadeau reçu"
          << "Coiffeur"
          << "Courses"
          << "Deezer"
          << "Dessin"
          << "Divers"
          << "Don"
          << "EDF"
          << "Escalade"
          << "Free"
          << "LEGO"
          << "Livres"
          << "Livret A"
          << "Loisirs"
          << "Loyer"
          << "Meubles"
          << "PEL"
          << "Restaurant"
          << "Salaire"
          << "SNCF"
          << "Qwerty"
          << "Transports"
          << "Unknown"
          );
    return editor;
  } else {
    return QStyledItemDelegate::createEditor(p_parent, p_option, p_index);
  }
}

void CategoryItemDelegate::setEditorData(QWidget* p_editor, const QModelIndex& p_index) const {
  if (p_index.isValid() && p_index.column() == 1) {
    auto editor = qobject_cast<QComboBox*>(p_editor);
    editor->setCurrentText(p_index.data().toString());
  } else {
    QStyledItemDelegate::setEditorData(p_editor, p_index);
  }
}

void CategoryItemDelegate::setModelData(QWidget* p_editor, QAbstractItemModel* p_model, const QModelIndex& p_index) const {
  if (p_index.isValid() && p_index.column() == 1) {
    auto editor = qobject_cast<QComboBox*>(p_editor);
    p_model->setData(p_index, editor->currentText());
  } else {
    QStyledItemDelegate::setEditorData(p_editor, p_index);
  }
}
