#include "MonthlyCSVGenerator.hxx"

#include <exception>

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QRegularExpression>
#include <QDir>

#include <QDebug>

MonthlyCSVGenerator::MonthlyCSVGenerator() {
}

void MonthlyCSVGenerator::convertRawCSVToMonthlyCSV(QDate const& p_date, QChar const& p_delim) {

  QString csvDirectoryPath = "../BankAccount/csv";
  QString accountDirectoryName = p_date.toString("MM-yyyy");
  QString accountDirectoryPath = csvDirectoryPath+QDir::separator()+accountDirectoryName;
  QString inFilePath = accountDirectoryPath+QDir::separator()+"raw.csv";

  QFile rawCSV(inFilePath);
  if (!rawCSV.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw open_failure(rawCSV.errorString().toStdString().c_str());
  }

  QTextStream in(&rawCSV);
  in.setAutoDetectUnicode(true);
  QString line;
  QString newLine;

  while (!(line = in.readLine()).isEmpty()) {
    auto tokens = line.split(p_delim);

    // Handle date
    auto dateString = tokens.at(0);
    auto date = dateString.split('/');
    auto month = date.at(1).toInt();
    if (month != p_date.month()) {
      continue;
    }
    auto dayString = date.at(0);

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
      debit = QString::number(amount, 'f', 2)+"€";
    } else {
      credit = QString::number(amount, 'f', 2)+"€";
    }

    // Concat everything
    newLine += (QStringList() << dayString << category << operationType << label << debit << credit).join(';')+'\n';
  }

  rawCSV.close();

  QFile formatedCSV(accountDirectoryPath+QDir::separator()+"operations.csv");
  QTextStream out(&formatedCSV);
//  QString existingOperations;

//  if (formatedCSV.exists()) {
//    if (!formatedCSV.open(QIODevice::ReadOnly | QIODevice::Text)) {
//      throw open_failure(formatedCSV.errorString().toStdString().c_str(), true);
//    }

//    existingOperations = out.readAll();
//    formatedCSV.close();
//    out.flush();
//  }

  if (!formatedCSV.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw open_failure(formatedCSV.errorString().toStdString().c_str(), false);
  }

  out << newLine/* << existingOperations*/;

  formatedCSV.close();
}

void MonthlyCSVGenerator::saveCategory(int p_row, const QString& p_category, const QString& p_inFileName) {
  QFile inFile(p_inFileName);
  if (!inFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
    throw open_failure(inFile.errorString().toStdString().c_str());
  }

  auto fileLines = inFile.readAll().split('\n');
  auto concernedOperationTokens = fileLines.at(p_row).split(';');
  concernedOperationTokens.replace(1, p_category.toStdString().c_str());
  auto newLine = concernedOperationTokens.join(';');
  fileLines.replace(p_row, newLine);
  auto newText = fileLines.join('\n');

  inFile.seek(0);
  inFile.write(newText);

  inFile.close();
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

void MonthlyCSVGenerator::updateRawCSV(QDate const& p_date, QString const& p_inFileName, QChar const& p_delim, bool p_hasHeader) {
  QString csvDirectoryPath = "../BankAccount/csv";
  QString accountDirectoryName = p_date.toString("MM-yyyy");
  QString accountDirectoryPath = csvDirectoryPath+QDir::separator()+accountDirectoryName;

  QDir csvDirectory(csvDirectoryPath);
  QDir accountDirectory(accountDirectoryPath);
  if (!accountDirectory.exists()) {
    csvDirectory.mkdir(accountDirectoryName);
  }

  QString firstLine;
  QString rawCSVName = "raw.csv";
  QString rawCSVPath = accountDirectoryPath+QDir::separator()+rawCSVName;
  QFile existingRawCSV(rawCSVPath);
  auto rawCSVAlreadyExists = existingRawCSV.exists();
  if (rawCSVAlreadyExists) {
    qDebug() << "Raw CSV exists";
    if (!existingRawCSV.open(QIODevice::ReadOnly | QIODevice::Text)) {
      throw open_failure(existingRawCSV.errorString().toStdString().c_str());
    }
    firstLine = existingRawCSV.readLine();
  } else {
    qDebug() << "Raw CSV doesn't exist";
  }

  QFile inFile(p_inFileName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw open_failure(inFile.errorString().toStdString().c_str());
  }

  if (p_hasHeader) {
    inFile.readLine();
  }

  QByteArray newLines;
  QString currentLine;
  while (!(currentLine = inFile.readLine()).isEmpty()) {
    if (rawCSVAlreadyExists && currentLine == firstLine) {
      existingRawCSV.seek(0);
      newLines += existingRawCSV.readAll();
      existingRawCSV.close();
      break;
    }
    QStringList tokens = currentLine.split(p_delim);
    int currentMonth = tokens.at(0).split('/').at(1).toInt();
    if (currentMonth != p_date.month()) {
      continue;
    }
    newLines += currentLine;
  }

  existingRawCSV.close();
  inFile.close();

  if (!existingRawCSV.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw open_failure(existingRawCSV.errorString().toStdString().c_str(), false);
  }

  existingRawCSV.seek(0);
  existingRawCSV.write(newLines);

  existingRawCSV.close();
}
