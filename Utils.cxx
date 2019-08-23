#include "Utils.hxx"

Utils::Utils() {
}

QStringList const Utils::GAIN =
  QStringList()
    << "Amis"
    << "CE"
    << "Famille"
    << "Ventes";

QStringList const Utils::EPARGNE =
  QStringList()
    << "Compte courant"
    << "Livret A"
    << "PEL";

QStringList const Utils::CHARGES_FIXES =
  QStringList()
    << "Adobe"
    << "Assurance"
    << "Banque"
    << "Charges"
    << "Cotisations Elsa"
    << "Deezer"
    << "Disneyland"
    << "EDF"
    << "Escalade"
    << "Free"
    << "Garage"
    << "Impôts"
    << "Loyer"
    << "Patreon"
    << "TCL";

QStringList const Utils::NOURRITURE =
  QStringList()
    << "Courses"
    << "Restaurant";

QStringList const Utils::CHARGES_VARIABLES =
  QStringList()
    << "Beauté"
    << "Cadeau donné"
    << "Chat"
    << "Cinéma"
    << "Coiffeur"
    << "Déménagement"
    << "Dessin"
    << "Divers"
    << "Don"
    << "Électronique"
    << "Escalade"
    << "Immobilier"
    << "Jeux vidéo"
    << "LaPoste"
    << "LEGO"
    << "Livres"
    << "Loisirs"
    << "Médecin"
    << "Meubles"
    << "Musée"
    << "Note de frais"
    << "Pharmacie"
    << "Pokémon"
    << "Qwerty"
    << "Retrait"
    << "Salaire"
    << "SNCF"
    << "Tatouage"
    << "Transports"
    << "Travaux"
    << "Vêtements"
    << "Voyage";

QMap<QString, Utils::Group> const Utils::GROUP_BY_NAME =
    QMap<QString, Utils::Group>({
      {"Salaire", Utils::eSalary},
      {"Charges fixes", Utils::eFixedCharges},
      {"Charges variables", Utils::eVariableCharges},
      {"Nourriture", Utils::eFood},
      {"Épargne", Utils::eSaving},
      {"Gain", Utils::eProfit},
      {"Unknown", Utils::eUnknown}});

QMap<Utils::Group, QStringList> const Utils::CATEGORIES_BY_GROUP =
    QMap<Utils::Group, QStringList>({
      {Utils::eSalary, QStringList() << "Salaire"},
      {Utils::eFixedCharges, Utils::CHARGES_FIXES},
      {Utils::eVariableCharges, Utils::CHARGES_VARIABLES},
      {Utils::eFood, Utils::NOURRITURE},
      {Utils::eSaving, Utils::EPARGNE},
      {Utils::eProfit, Utils::GAIN},
      {Utils::eUnknown, QStringList() << "Unknown"}});

QStringList const Utils::GetCategories() {
  QStringList categories;
  for (auto categoriesPart: CATEGORIES_BY_GROUP.values()) {
    categories << categoriesPart;
  }
  categories.sort();
  return categories;
}

QStringList const Utils::GetGroups() {
  return GROUP_BY_NAME.keys();
}

QStringList const Utils::GetCategoriesByGroup(QString const& p_group) {
  return CATEGORIES_BY_GROUP.value(GROUP_BY_NAME.value(p_group));
}

Utils::Group Utils::GetGroupFromGroupName(QString const& p_groupName) {
  return GROUP_BY_NAME.value(p_groupName);
}

QColor Utils::GetRedColor()
{
  return QColor("#dc3545");
}

QColor Utils::GetFadedRedColor()
{
  auto color = GetRedColor();
  color.setAlpha(127);
  return color;
}

QColor Utils::GetGreenColor()
{
  return QColor("#28a745");
}

QColor Utils::GetFadedGreenColor()
{
  auto color = GetGreenColor();
  color.setAlpha(127);
  return color;
}

QColor Utils::GetOrangeColor()
{
  return QColor("#fd7e14");
}
