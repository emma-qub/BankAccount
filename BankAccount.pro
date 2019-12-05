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
QMAKE_CXXFLAGS += "-fno-sized-deallocation"

SOURCES += \
    CategoriesModel.cxx \
    main.cxx\
    MainWindow.cxx \
    CSVModel.cxx \
    AccountWindow.cxx \
    ChartWindow.cxx \
    SettingsWindow.cxx \
    MonthlyCSVGenerator.cxx \
    CategoryItemDelegate.cxx \
    CategoryChartGenerator.cxx \
    Utils.cxx \
    BudgetItemDelegate.cxx

HEADERS  += \
    CategoriesModel.hxx \
    MainWindow.hxx \
    CSVModel.hxx \
    AccountWindow.hxx \
    ChartWindow.hxx \
    SettingsWindow.hxx \
    MonthlyCSVGenerator.hxx \
    CategoryItemDelegate.hxx \
    CategoryChartGenerator.hxx \
    Utils.hxx \
    BudgetItemDelegate.hxx

CONFIG += C++17
