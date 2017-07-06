#include "Utils.hxx"

Utils::Utils() {
}

QStringList const Utils::EPARGNE =
  QStringList()
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
    << "Unknown";

QMap<QString, QStringList> const Utils::CATEGORIES_BY_GROUP =
    QMap<QString, QStringList>({
      {"Charges fixes", CHARGES_FIXES},
      {"Salaire", QStringList() << "Salaire"},
      {"Nourriture", NOURRITURE},
      {"Épargne", EPARGNE},
      {"Charges variables", CHARGES_VARIABLES},
      {"Unknown", QStringList() << "Unknown"}});

QStringList const Utils::getCategories() {
  QStringList categories;
  for (auto categoriesPart: CATEGORIES_BY_GROUP.values()) {
    categories << categoriesPart;
  }
  categories.sort();
  return categories;
}

QStringList const Utils::getGroups() {
  return CATEGORIES_BY_GROUP.keys();
}

QStringList const Utils::getCategoriesByGroup(QString const& p_group) {
  return CATEGORIES_BY_GROUP.value(p_group);
}
