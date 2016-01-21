#-------------------------------------------------
#
# Project created by QtCreator 2016-01-21T14:26:55
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BankAccount
TEMPLATE = app

SOURCES += main.cxx\
        MainWindow.cxx \
    CSVModel.cxx

HEADERS  += MainWindow.hxx \
    CSVModel.hxx

CONFIG += C++11
