TEMPLATE = subdirs

SUBDIRS =       ../../lib/soilFluxes3D  \
                ../../lib/crit3dDate ../../lib/mathFunctions  \
                ../../lib/gis ../../lib/meteo ../../lib/soil ../../lib/crop \
                ../../lib/interpolation ../../lib/solarRadiation  \
                ../../lib/utilities ../../lib/soilWidget  \
                ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid  \
                ../../lib/project ../CRITERIA3D

CONFIG += ordered

