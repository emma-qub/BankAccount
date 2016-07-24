#-------------------------------------------------
#
# Project created by QtCreator 2016-01-21T14:26:55
#
#-------------------------------------------------

QT += \
    core \
    gui \
    charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = BankAccount
TEMPLATE = app

SOURCES += \
    main.cxx\
    MainWindow.cxx \
    CSVModel.cxx \
    AccountWindow.cxx \
    ChartWindow.cxx \
    SettingsWindow.cxx \
    MonthlyCSVGenerator.cxx \
    CategoryItemDelegate.cxx \
    MonthlyChartGenerator.cxx

HEADERS  += \
    MainWindow.hxx \
    CSVModel.hxx \
    AccountWindow.hxx \
    ChartWindow.hxx \
    SettingsWindow.hxx \
    MonthlyCSVGenerator.hxx \
    CategoryItemDelegate.hxx \
    MonthlyChartGenerator.hxx

CONFIG += C++11
