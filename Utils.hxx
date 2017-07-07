#ifndef UTILS_HXX
#define UTILS_HXX

#include <QStringList>
#include <QMap>

class Utils
{
public:
  enum Group {
    eSalary,
    eFixedCharges,
    eVariableCharges,
    eFood,
    eSaving,
    eUnknown
  };

  Utils();

  static QStringList const EPARGNE;
  static QStringList const CHARGES_FIXES;
  static QStringList const NOURRITURE;
  static QStringList const CHARGES_VARIABLES;
  static QMap<Group, QStringList> const CATEGORIES_BY_GROUP;
  static QMap<QString, Utils::Group> const GROUP_BY_NAME;

  static QStringList const getCategories();
  static QStringList const getGroups();
  static QStringList const getCategoriesByGroup(const QString& p_group);

  static Group getGroupFromGroupName(QString const& p_groupName);
};

#endif // UTILS_HXX
