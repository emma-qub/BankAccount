#include "MonthlyCSVGenerator.hxx"

#include "CSVModel.hxx"

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
  in.setCodec("UTF-8");
  QString line;
  QStringList newLines;

  QFile formatedCSV(accountDirectoryPath+QDir::separator()+p_outputFileName);
  QTextStream out(&formatedCSV);
  out.setCodec("UTF-8");

  if (!formatedCSV.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw OpenFailure(formatedCSV.errorString().toStdString().c_str(), false);
  }

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

    // Handle group
    QString group("Unknown");

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
    newLines << (QStringList() << dayString << group << category << operationType << label << debit << credit).join(';') + "\n";
  }

  // Write new lines
  for (auto const& newLine: newLines) {
    out << newLine;
  }

  formatedCSV.close();
  rawCSV.close();
}

void MonthlyCSVGenerator::saveCategory(int p_row, const QString& p_group, const QString& p_category, const QString& p_inFileName) {
  QFile inFile(p_inFileName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(inFile.errorString().toStdString().c_str());
  }

  QTextStream in(&inFile);
  in.setCodec("UTF-8");
  QStringList fileLines = in.readAll().split('\n');
  QStringList concernedOperationTokens = fileLines.at(p_row).split(';');
  concernedOperationTokens.replace(CSVModel::eGroup, p_group.toStdString().c_str());
  concernedOperationTokens.replace(CSVModel::eCategory, p_category.toStdString().c_str());
  QString newLine = concernedOperationTokens.join(';');
  fileLines.replace(p_row, newLine);

  inFile.close();

  QFile outFile(p_inFileName);
  if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw OpenFailure(outFile.errorString().toStdString().c_str());
  }

  QTextStream out(&outFile);
  out.setCodec("UTF-8");
  for (auto const& line: fileLines) {
    if (line.count(';') == CSVModel::eColumnCount-1)
    {
      out << line << "\n";
    }
  }

  outFile.close();
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
    QTextStream in(&existingRawCSV);
    in.setCodec("UTF-8");
    firstLine = in.readLine();
  }

  // Read bank operation csv file
  QFile inFile(p_inFileName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(inFile.errorString().toStdString().c_str());
  }

  QTextStream in(&inFile);
  in.setCodec("UTF-8");

  if (p_hasHeader) {
    in.readLine();
  }

  QByteArray newLines;
  QString currentLine;
  QByteArray existingRawLines;
  while (!in.atEnd()) {
    currentLine = in.readLine();
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
    newLines += currentLine + "\n";
  }

  existingRawCSV.close();
  inFile.close();

  // Write new lines in a temp raw csv file
  auto rawTempName = QString("rawTemp.csv");
  QFile rawTemp(accountDirectoryPath+QDir::separator()+rawTempName);

  if (!rawTemp.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw OpenFailure(rawTemp.errorString().toStdString().c_str(), false);
  }

  QTextStream inRawTemp(&rawTemp);
  inRawTemp.setCodec("UTF-8");

  inRawTemp << newLines;
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

  QTextStream inTempOperation(&tempOperation);
  inTempOperation.setCodec("UTF-8");

  auto newOperations = inTempOperation.readAll();
  tempOperation.close();

  QFile existingOperation(accountDirectoryPath+QDir::separator()+"operations.csv");
  if (!existingOperation.open(QIODevice::ReadWrite | QIODevice::Text)) {
    throw OpenFailure(existingOperation.errorString().toStdString().c_str());
  }

  QTextStream inExistingOperation(&existingOperation);
  inExistingOperation.setCodec("UTF-8");

  auto operations = inExistingOperation.readAll();

  inExistingOperation.seek(0);
  inExistingOperation << newOperations;
  inExistingOperation << operations;

  existingOperation.close();

  // Clean temp operation csv file
  accountDirectory.remove(operationTempName);

  // Clean operations
  cleanOperations(existingOperation.fileName());
}

QString MonthlyCSVGenerator::convertXLSToCSV(QString const& p_xlsFileName) {
  QFile xlsFile(p_xlsFileName);
  if (!xlsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(xlsFile.errorString().toStdString().c_str());
  }

  QTextStream inXlsFile(&xlsFile);
  inXlsFile.setCodec("UTF-8");

  QStringList newLines;

  while (inXlsFile.atEnd() == false) {
    auto byteArrayList = inXlsFile.readLine().split('\t');
    if (byteArrayList.at(4).contains("01056 102355U")) {
      continue;
    }

    auto label = byteArrayList.at(4).trimmed();
    if (label.isEmpty()) {
      label = byteArrayList.at(5).trimmed();
    }

    auto value = byteArrayList.at(1);
    value.replace(',', '.');

    QStringList newLineStringList;
    newLineStringList << byteArrayList.at(0).trimmed() << "_" << byteArrayList.at(2) << label << value;

    newLines.insert(0, newLineStringList.join(';')+"\n");
  }

  xlsFile.close();

  QString csvFileName = p_xlsFileName;
  csvFileName.chop(3);
  csvFileName += "csv";
  QFile csvFile(csvFileName);
  if (!csvFile.open(QIODevice::Append | QIODevice::Text)) {
    throw OpenFailure(csvFile.errorString().toStdString().c_str());
  }

  QTextStream inCsvFile(&csvFile);
  inCsvFile.setCodec("UTF-8");

  for (auto const newLine: newLines) {
    inCsvFile << newLine;
  }

  csvFile.close();

  return csvFileName;
}

void MonthlyCSVGenerator::cleanOperations(QString const& p_fileName) {
  QFile inFile(p_fileName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw OpenFailure(inFile.errorString().toStdString().c_str());
  }

  QTextStream in(&inFile);
  in.setCodec("UTF-8");

  QString currentLine;
  QStringList cleanContent;
  while (!in.atEnd()) {
    currentLine = in.readLine();
    if (currentLine.count(';') == CSVModel::eColumnCount-1) {
      cleanContent += currentLine;
    }
  }

  inFile.close();

  QFile outFile(p_fileName);
  if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw OpenFailure(outFile.errorString().toStdString().c_str(), false);
  }

  QTextStream out(&outFile);
  out.setCodec("UTF-8");

  for (auto const& cleanLine: cleanContent) {
    out << cleanLine << "\n";
  }

  outFile.close();
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
