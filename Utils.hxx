#ifndef UTILS_HXX
#define UTILS_HXX

#include <QStringList>
#include <QMap>

class Utils
{
public:
  Utils();

  static QStringList const EPARGNE;
  static QStringList const CHARGES_FIXES;
  static QStringList const NOURRITURE;
  static QStringList const CHARGES_VARIABLES;
  static QMap<QString, QStringList> const CATEGORIES_BY_GROUP;

  static QStringList const getCategories();
  static QStringList const getGroups();
  static QStringList const getCategoriesByGroup(const QString& p_group);
};

#endif // UTILS_HXX
