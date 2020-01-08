TEMPLATE = subdirs

SUBDIRS =       ../../agrolib/soilFluxes3D  \
                ../../agrolib/crit3dDate ../../agrolib/mathFunctions  \
                ../../agrolib/gis ../../agrolib/meteo ../../agrolib/soil ../../agrolib/crop \
                ../../agrolib/interpolation ../../agrolib/solarRadiation  \
                ../../agrolib/utilities ../../agrolib/soilWidget  \
                ../../agrolib/dbMeteoPoints ../../agrolib/dbMeteoGrid  \
                ../../agrolib/project \
                ../CRITERIA3D

CONFIG += ordered

