QT += core
QT -= gui

TARGET = PrecipitationErrorQuantile
CONFIG += console
CONFIG -= app_bundle


INCLUDEPATH += ../mathFunctions


LIBS += -L../mathFunctions/debug -lmathFunctions


TEMPLATE = app

SOURCES += main.cpp

