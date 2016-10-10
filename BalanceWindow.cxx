#include "BalanceWindow.hxx"
#include "Utils.hxx"

#include "CSVModel.hxx"

#include <QCheckBox>

#include <QDebug>

BalanceWindow::BalanceWindow(CSVModel* p_model, int p_year, int p_month, MonthlyBalanceGenerator* p_monthlyBalanceGenerator, QWidget* p_parent):
  QWidget(p_parent),
  m_model(p_model),
  m_year(p_year),
  m_month(p_month),
  m_monthlyBalanceGenerator(p_monthlyBalanceGenerator) {

  // Balance
  auto balanceLayout = new QVBoxLayout;
  m_balance = new QLabel;
  balanceLayout->addWidget(m_balance);
  balanceLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

  // Categories
  m_categoriesLayout = new QVBoxLayout;

  // Main
  auto mainLayout = new QHBoxLayout;
  mainLayout->addLayout(m_categoriesLayout);
  mainLayout->addLayout(balanceLayout);
  mainLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

  setLayout(mainLayout);
}

void BalanceWindow::updateBalance() {
  m_balance->setText(QString::number(m_monthlyBalanceGenerator->getBalance(m_categoriesChecked)));
}

void BalanceWindow::refreshCategories() {
  QLayoutItem* layoutItem = nullptr;
  while ((layoutItem = m_categoriesLayout->takeAt(0)) != nullptr) {
    QCheckBox* oldCheckBox = dynamic_cast<QCheckBox*>(layoutItem->widget());
    if (oldCheckBox != nullptr) {
      delete oldCheckBox;
      delete layoutItem;
    }
  }

  for (auto category: m_monthlyBalanceGenerator->createCategories()) {
    auto checkBox = new QCheckBox(category);
    m_categoriesLayout->addWidget(checkBox);
    connect(checkBox, &QCheckBox::clicked, this, [this, checkBox]() {
      if (checkBox->isChecked() && !m_categoriesChecked.contains(checkBox->text())) {
        m_categoriesChecked << checkBox->text();
      } else if (!checkBox->isChecked() && m_categoriesChecked.contains(checkBox->text())) {
        m_categoriesChecked.removeOne(checkBox->text());
      }
      updateBalance();
    });
  }
  m_categoriesLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
}
