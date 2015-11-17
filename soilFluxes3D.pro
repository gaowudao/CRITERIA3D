#-------------------------------------------------
#
# CRITERIA 3D QT Project
#
#-------------------------------------------------

QT       -= core gui

TARGET = criteria3D
TEMPLATE = lib

CONFIG += staticlib

SOURCES +=  \
    boundary.cpp \
    balance.cpp \
    water.cpp \
    solver.cpp \
    memory.cpp \
    soilPhysics.cpp \
    soilFluxes3D.cpp


HEADERS += \
    header/types.h \
    header/parameters.h \
    header/boundary.h \
    header/balance.h \
    header/water.h \
    header/solver.h \
    header/memory.h \
    header/soilPhysics.h \
    header/soilFluxes3D.h
