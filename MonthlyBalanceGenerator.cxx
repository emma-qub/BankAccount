#include "MonthlyBalanceGenerator.hxx"

#include "CSVModel.hxx"

MonthlyBalanceGenerator::MonthlyBalanceGenerator(CSVModel* p_model, int p_year, int p_month, QObject* p_parent):
  m_model(p_model),
  m_year(p_year),
  m_month(p_month) {

  setParent(p_parent);
}

QStringList MonthlyBalanceGenerator::CreateCategories() const {
  QStringList columnList;

  for (int k = 0; k < m_model->rowCount(); ++k) {
    auto category = m_model->index(k, CSVModel::eCategory).data().toString();
    if (!columnList.contains(category))
      columnList << category;
  }

  return columnList;
}

float MonthlyBalanceGenerator::GetBalance(QStringList const& p_selectedCategories) {
  float balance = 0.f;

  if (p_selectedCategories.isEmpty())
    return balance;

  for (int k = 0; k < m_model->rowCount(); ++k) {
    if (p_selectedCategories.contains(m_model->index(k, CSVModel::eCategory).data().toString())) {
      auto debitStr = m_model->index(k, CSVModel::eDebit).data().toString();
      balance -= debitStr.remove(debitStr.length()-1, 1).toFloat();
      auto creditStr = m_model->index(k, CSVModel::eCredit).data().toString();
      balance -= creditStr.remove(creditStr.length()-1, 1).toFloat();
    }
  }

  return balance;
}
