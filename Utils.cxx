#include "Utils.hxx"

Utils::Utils() {
}

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
    << "Deezer"
    << "EDF"
    << "Escalade"
    << "Free"
    << "Impôts"
    << "Loyer";

QStringList const Utils::NOURRITURE =
  QStringList()
    << "Courses"
    << "Restaurant";

QStringList const Utils::CHARGES_VARIABLES =
  QStringList()
    << "Cadeau donné"
    << "Cadeau reçu"
    << "Cinéma"
    << "Coiffeur"
    << "Déménagement"
    << "Dessin"
    << "Divers"
    << "Don"
    << "Électronique"
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
      {"Unknown", Utils::eUnknown}});

QMap<Utils::Group, QStringList> const Utils::CATEGORIES_BY_GROUP =
    QMap<Utils::Group, QStringList>({
      {Utils::eSalary, QStringList() << "Salaire"},
      {Utils::eFixedCharges, CHARGES_FIXES},
      {Utils::eVariableCharges, CHARGES_VARIABLES},
      {Utils::eFood, NOURRITURE},
      {Utils::eSaving, EPARGNE},
      {Utils::eUnknown, QStringList() << "Unknown"}});

QStringList const Utils::getCategories() {
  QStringList categories;
  for (auto categoriesPart: CATEGORIES_BY_GROUP.values()) {
    categories << categoriesPart;
  }
  categories.sort();
  return categories;
}

QStringList const Utils::getGroups() {
  return GROUP_BY_NAME.keys();
}

QStringList const Utils::getCategoriesByGroup(QString const& p_group) {
  return CATEGORIES_BY_GROUP.value(GROUP_BY_NAME.value(p_group));
}

Utils::Group Utils::getGroupFromGroupName(QString const& p_groupName) {
  return GROUP_BY_NAME.value(p_groupName);
}
