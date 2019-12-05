#ifndef CATEGORIESMODEL_HXX
#define CATEGORIESMODEL_HXX

#include <QStandardItemModel>

class CategoriesModel: public QStandardItemModel {
  Q_OBJECT

public:
  enum Group {
    eSalary,
    eFixedCharges,
    eVariableCharges,
    eFood,
    eSaving,
    eProfit,
    eUnknown
  };

  static QMap<Group, QString> const GROUP_BY_FILENAME;
  static QMap<QString, Group> const GROUP_BY_NAME;

  CategoriesModel(QObject* p_parent);

  QStringList GetCategoriesByGroup(QString const& p_groupName) const;
  QStringList GetGroups() const;
};

#endif
