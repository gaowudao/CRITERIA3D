#-------------------------------------------------
#
# CRITERIA3D
# soil fluxes library
#
#-------------------------------------------------

QT       -= core gui

TARGET = soilFluxes3D
TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += ../mathFunctions

SOURCES +=  \
    boundary.cpp \
    balance.cpp \
    water.cpp \
    solver.cpp \
    memory.cpp \
    soilPhysics.cpp \
    soilFluxes3D.cpp \
    heat.cpp \
    physics.cpp


HEADERS += \
    header/types.h \
    header/parameters.h \
    header/boundary.h \
    header/balance.h \
    header/water.h \
    header/solver.h \
    header/memory.h \
    header/soilPhysics.h \
    header/soilFluxes3D.h \
    header/extra.h \
    header/heat.h \
    header/physics.h
