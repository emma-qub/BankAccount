#include "MonthlyCSVGenerator.hxx"

#include <exception>

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QRegularExpression>
#include <QDir>
#include <QByteArrayList>

#include <QDebug>

MonthlyCSVGenerator::MonthlyCSVGenerator() {
}

void MonthlyCSVGenerator::convertRawCSVToMonthlyCSV(QDate const& p_date, QChar const& p_delim,
  const QString& p_outputFileName, const QString& p_rawFileName) {

  QString csvDirectoryPath = "../BankAccount/csv";
  QString accountDirectoryName = p_date.toString("MM-yyyy");
  QString accountDirectoryPath = csvDirectoryPath+QDir::separator()+accountDirectoryName;
  QString inFilePath = accountDirectoryPath+QDir::separator()+p_rawFileName;

  QFile rawCSV(inFilePath);
  if (!rawCSV.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(rawCSV.errorString().toStdString().c_str());
  }

  QTextStream in(&rawCSV);
  in.setAutoDetectUnicode(true);
  QString line;
  QString newLine;

  while (!in.atEnd()) {
    line = in.readLine();
    if (line.isEmpty()) {
      continue;
    }
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

  QFile formatedCSV(accountDirectoryPath+QDir::separator()+p_outputFileName);
  QTextStream out(&formatedCSV);
//  QString existingOperations;

//  if (formatedCSV.exists()) {
//    if (!formatedCSV.open(QIODevice::ReadOnly | QIODevice::Text)) {
//      throw OpenFailure(formatedCSV.errorString().toStdString().c_str(), true);
//    }

//    existingOperations = out.readAll();
//    formatedCSV.close();
//    out.flush();
//  }

  if (!formatedCSV.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw OpenFailure(formatedCSV.errorString().toStdString().c_str(), false);
  }

  out << newLine/* << existingOperations*/;

  formatedCSV.close();
}

void MonthlyCSVGenerator::saveCategory(int p_row, const QString& p_category, const QString& p_inFileName) {
  QFile inFile(p_inFileName);
  if (!inFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
    throw OpenFailure(inFile.errorString().toStdString().c_str());
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

void MonthlyCSVGenerator::updateRawCSV(QDate const& p_date, QString const& p_inFileName, QChar const& p_delim, bool p_hasHeader) {
  QString csvDirectoryPath = "../BankAccount/csv";
  QString accountDirectoryName = p_date.toString("MM-yyyy");
  QString accountDirectoryPath = csvDirectoryPath+QDir::separator()+accountDirectoryName;

  QDir csvDirectory(csvDirectoryPath);
  QDir accountDirectory(accountDirectoryPath);
  if (!accountDirectory.exists()) {
    csvDirectory.mkdir(accountDirectoryName);
  }

  // Get first line of raw csv if it already exists
  QString firstLine;
  QString rawCSVName = "raw.csv";
  QString rawCSVPath = accountDirectoryPath+QDir::separator()+rawCSVName;
  QFile existingRawCSV(rawCSVPath);
  auto rawCSVAlreadyExists = existingRawCSV.exists();
  if (rawCSVAlreadyExists) {
    if (!existingRawCSV.open(QIODevice::ReadOnly | QIODevice::Text)) {
      throw OpenFailure(existingRawCSV.errorString().toStdString().c_str());
    }
    firstLine = existingRawCSV.readLine();
  }

  // Read bank operation csv file
  QFile inFile(p_inFileName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(inFile.errorString().toStdString().c_str());
  }

  if (p_hasHeader) {
    inFile.readLine();
  }

  QByteArray newLines;
  QString currentLine;
  QByteArray existingRawLines;
  while (!inFile.atEnd()) {
    currentLine = inFile.readLine();
    if (currentLine.isEmpty()) {
      continue;
    }
    if (rawCSVAlreadyExists && currentLine == firstLine) {
      existingRawLines = existingRawCSV.readAll();
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

  // Write new lines in a temp raw csv file
  auto rawTempName = QString("rawTemp.csv");
  QFile rawTemp(accountDirectoryPath+QDir::separator()+rawTempName);

  if (!rawTemp.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw OpenFailure(rawTemp.errorString().toStdString().c_str(), false);
  }

  rawTemp.write(newLines);
  rawTemp.close();

  // Convert operation from temp raw csv file to temp operation csv file
  auto operationTempName = QString("operationTemp.csv");
  convertRawCSVToMonthlyCSV(p_date, p_delim, operationTempName, rawTempName);

  // Add temp raw csv file content at the beginning of existing raw csv file
  if (!existingRawCSV.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw OpenFailure(existingRawCSV.errorString().toStdString().c_str(), false);
  }

  existingRawCSV.seek(0);
  existingRawCSV.write(newLines);
  existingRawCSV.write(existingRawLines);

  existingRawCSV.close();

  // Clean temp raw csv file
  accountDirectory.remove(rawTempName);

  // Add new operations at the beginning of operation csv file
  QFile tempOperation(accountDirectoryPath+QDir::separator()+operationTempName);
  if (!tempOperation.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(tempOperation.errorString().toStdString().c_str());
  }

  auto newOperations = tempOperation.readAll();
  tempOperation.close();

  QFile existingOperation(accountDirectoryPath+QDir::separator()+"operations.csv");
  if (!existingOperation.open(QIODevice::ReadWrite | QIODevice::Text)) {
    throw OpenFailure(existingOperation.errorString().toStdString().c_str());
  }

  auto operations = existingOperation.readAll();

  existingOperation.seek(0);
  existingOperation.write(newOperations);
  existingOperation.write(operations);

  existingOperation.close();

  // Clean temp operation csv file
  accountDirectory.remove(operationTempName);

  // Clean operations
  CleanOperations(accountDirectoryPath+QDir::separator()+"operations.csv");
}

void MonthlyCSVGenerator::convertXLSToCSV(QString& p_csvFileName) {
  QFile xlsFile(p_csvFileName);
  if (!xlsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(xlsFile.errorString().toStdString().c_str());
  }

  p_csvFileName.replace("xls", "csv");
  QFile csvFile(p_csvFileName);
  if (!csvFile.open(QIODevice::Append | QIODevice::Text)) {
    throw OpenFailure(csvFile.errorString().toStdString().c_str());
  }

  while (xlsFile.atEnd() == false) {
    auto byteArrayList = xlsFile.readLine().split('\t');
    if (byteArrayList.at(4).contains("01056 102355U")) {
      continue;
    }

    QByteArray label = byteArrayList.at(4).trimmed();
    if (label.isEmpty()) {
      label = byteArrayList.at(5).trimmed();
    }

    QByteArray value = byteArrayList.at(1);
    value.replace(',', '.');

    QByteArrayList newLineStringList;
    newLineStringList << byteArrayList.at(0).trimmed() << "_" << byteArrayList.at(2) << label << value;

    csvFile.write(newLineStringList.join(';')+"\n");
  }
}

void MonthlyCSVGenerator::CleanOperations(QString const& p_fileName) {
  QFile in(p_fileName);
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(in.errorString().toStdString().c_str());
  }

  QString currentLine;
  QByteArray cleanContent;
  while (!in.atEnd()) {
    currentLine = in.readLine();
    if (currentLine.count(';') == 5) {
      cleanContent += currentLine;
    }
  }

  in.close();

  QFile out(p_fileName);
  if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw OpenFailure(out.errorString().toStdString().c_str(), false);
  }

  out.write(cleanContent);
  out.close();
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
