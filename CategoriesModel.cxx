#include "CategoriesModel.hxx"

#include <QFile>
#include <QTextStream>

QMap<CategoriesModel::Group, QString> const CategoriesModel::GROUP_BY_FILENAME =
    QMap<Group, QString>({
      {eSalary, "../BankAccount/categories/salary.txt"},
      {eFixedCharges, "../BankAccount/categories/fixed_charges.txt"},
      {eVariableCharges, "../BankAccount/categories/variable_charges.txt"},
      {eFood, "../BankAccount/categories/food.txt"},
      {eSaving, "../BankAccount/categories/saving.txt"},
      {eProfit, "../BankAccount/categories/profit.txt"},
      {eUnknown, "../BankAccount/categories/unknown.txt"}});

QMap<QString, CategoriesModel::Group> const CategoriesModel::GROUP_BY_NAME =
    QMap<QString, Group>({
      {"Salaire", eSalary},
      {"Charges fixes", eFixedCharges},
      {"Charges variables", eVariableCharges},
      {"Nourriture", eFood},
      {"Épargne", eSaving},
      {"Gain", eProfit},
      {"Unknown", eUnknown}});

CategoriesModel::CategoriesModel(QObject* p_parent):
  QStandardItemModel(p_parent) {

  for (auto const& group: GROUP_BY_NAME) {
    auto categoriesFileName = GROUP_BY_FILENAME[group];

    QFile categoryFile(categoriesFileName);
    Q_ASSERT_X(categoryFile.open(QIODevice::ReadOnly | QIODevice::Text), "CategoriesModel::CategoriesModel",
      tr("Cannot open category file %1").arg(categoriesFileName).toStdString().c_str());

    auto groupName = GROUP_BY_NAME.key(group);
    auto groupItem = new QStandardItem(groupName);
    appendRow(groupItem);

    QTextStream inCategoryFile(&categoryFile);

    while (!inCategoryFile.atEnd()) {
      auto categoryName = inCategoryFile.readLine();
      if (!categoryName.isEmpty()) {
        auto categoryItem = new QStandardItem(categoryName);
        groupItem->appendRow(categoryItem);
      }
    }
  }
}

QStringList CategoriesModel::GetCategoriesByGroup(const QString& p_groupName) const {
  QStringList categories;

  for (int groupRow = 0; groupRow < rowCount(); ++ groupRow) {
    auto groupIndex = index(groupRow, 0);
    if (groupIndex.data().toString() == p_groupName) {
      for (int categoryRow = 0; categoryRow < rowCount(groupIndex); ++ categoryRow) {
        auto categoryIndex = index(categoryRow, 0, groupIndex);
        categories << categoryIndex.data().toString();
      }
    }
  }

  return categories;
}

QStringList CategoriesModel::GetGroups() const {
  QStringList groups;

  for (int groupRow = 0; groupRow < rowCount(); ++ groupRow) {
    auto groupIndex = index(groupRow, 0);
    groups << groupIndex.data().toString();
  }

  return groups;
}
