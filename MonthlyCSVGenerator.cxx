#include "MonthlyCSVGenerator.hxx"

#include <exception>

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QRegularExpression>

#include <QDebug>

MonthlyCSVGenerator::MonthlyCSVGenerator() {
}

void MonthlyCSVGenerator::convertRawCSVToMonthlyCSV(
  QString const& p_inFileName, QString const& p_outFileName, QChar const& p_delim, bool p_hasHeader) {

  QFile rawCSV(p_inFileName);
  if (!rawCSV.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw open_failure(p_inFileName.toStdString().c_str());
  }

  QTextStream in(&rawCSV);
  QString line;
  QString newLine;
  if (p_hasHeader) {
    line = in.readLine();
  }

  while (!(line = in.readLine()).isEmpty()) {
    auto tokens = line.split(p_delim);

    // Handle date
    auto dateString = tokens.at(0);
    auto dayString = dateString.split('/').at(0);

    // Handle category
    QString category("Unknown");

    // Handle operation type
    auto operationType = getOperationType(tokens.at(2));

    // Handle label
    auto label = tokens.at(3);
    if (label.startsWith("DU ")) {
      label.mid(10);
    }

    // Handle debit and credit
    auto amountString = tokens.at(4);
    auto amount = amountString.replace(QRegularExpression(QString("\\s+")), "").toDouble();
    QString debit;
    QString credit;
    if (amount < 0) {
      debit = QString::number(amount, 'f', 2);
    } else {
      credit = QString::number(amount, 'f', 2);
    }

    // Concat everything
    newLine += (QStringList() << dayString << category << operationType << label << debit << credit).join(';')+'\n';
  }

  rawCSV.close();

  QFile formatedCSV(p_outFileName);
  if (!formatedCSV.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw open_failure(p_inFileName.toStdString().c_str(), false);
  }

  QTextStream out(&formatedCSV);
  out << newLine;

  formatedCSV.close();
}

QString MonthlyCSVGenerator::getOperationType(QString const& p_operationType) {
  QString operationType = p_operationType;

  if (p_operationType == "FACTURE CARTE") {
    operationType = "CB";
  } else if (p_operationType == "CHEQUE") {
    operationType = "Chèque";
  } else if (p_operationType == "VIR SEPA EMIS") {
    operationType = "Virement émis";
  } else if (p_operationType == "PRLV SEPA") {
    operationType = "Prélèvement";
  } else if (p_operationType == "COMMISSIONS") {
    operationType = "Commissions";
  } else if (p_operationType == "VIR SEPA RECU") {
    operationType = "Virement reçu";
  } else if (p_operationType == "VIRT CPTE A CPTE EMIS") {
    operationType = "Transfert émis";
  } else if (p_operationType == "VIRT CPTE A CPTE RECU") {
    operationType = "Transfert reçu";
  }

  return operationType;
}

