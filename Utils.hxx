#ifndef UTILS_HXX
#define UTILS_HXX

#include <QStringList>
#include <QMap>
#include <QColor>

class Utils {
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

  Utils() = delete;
  Utils(Utils const&) = delete;
  Utils& operator=(Utils const&) = delete;

  static QStringList const GAIN;
  static QStringList const EPARGNE;
  static QStringList const CHARGES_FIXES;
  static QStringList const NOURRITURE;
  static QStringList const CHARGES_VARIABLES;
  static QMap<Group, QStringList> const CATEGORIES_BY_GROUP;
  static QMap<QString, Utils::Group> const GROUP_BY_NAME;

  static QStringList const GetCategories();
  static QStringList const GetGroups();
  static QStringList const GetCategoriesByGroup(QString const& p_group);

  static Group GetGroupFromGroupName(QString const& p_groupName);

  static QColor GetRedColor();
  static QColor GetFadedRedColor();
  static QColor GetGreenColor();
  static QColor GetFadedGreenColor();
  static QColor GetOrangeColor();
};

#endif
