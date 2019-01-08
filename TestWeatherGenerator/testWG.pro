
TARGET = WG

QT += core xml
QT -= gui

CONFIG += c++11

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../weatherGenerator

LIBS += -L../crit3dDate/release -lcrit3dDate
LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../weatherGenerator/release -lweatherGenerator

SOURCES += main.cpp
