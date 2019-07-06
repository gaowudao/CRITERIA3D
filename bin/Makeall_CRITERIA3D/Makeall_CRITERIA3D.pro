TEMPLATE = subdirs

SUBDIRS =       ../../lib/soilFluxes3D  \
                ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/gis ../../lib/meteo \
                ../../lib/soil ../../lib/crop ../../lib/interpolation ../../lib/solarRadiation  \
                ../../lib/utilities ../../lib/soilWidget ../../lib/cropWidget  \
                ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid  \
                ../CRITERIA3D \

CONFIG += ordered

