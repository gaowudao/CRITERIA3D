#-------------------------------------------------
#
# soilFluxes3D
# numerical solution for flow equations
# of water and heat in the soil
# in a three-dimensional domain
#
# this library is part of CRITERIA3D distribution
#
#-------------------------------------------------

QT          -= gui

TARGET = soilFluxes3D
TEMPLATE = lib
CONFIG += staticlib

SOURCES +=  \
    boundary.cpp \
    balance.cpp \
    water.cpp \
    solver.cpp \
    memory.cpp \
    soilPhysics.cpp \
    soilFluxes3D.cpp \
    heat.cpp \
    extra.cpp


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
    header/heat.h
