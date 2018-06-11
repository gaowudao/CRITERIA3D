QT += core
QT -= gui

TARGET = percentileAnalysisInterface
CONFIG += console
CONFIG -= app_bundle


INCLUDEPATH += ../mathFunctions
INCLUDEPATH += ../percentileAnalysis


LIBS += -L../mathFunctions/debug -lmathFunctions
LIBS += -L../percentileAnalysis/debug -lpercentileAnalysis


TEMPLATE = app

SOURCES += main.cpp

