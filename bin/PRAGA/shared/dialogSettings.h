#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QtWidgets>

#ifndef PROJECT_H
    #include "project.h"
#endif

#ifndef GIS_H
    #include "gis.h"
#endif

#ifndef QUALITY_H
    #include "quality.h"
#endif

#ifndef METEO_H
    #include "meteo.h"
#endif


class QDialogButtonBox;
class QFileInfo;
class QTabWidget;


class GeoTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeoTab(gis::Crit3DGisSettings *gisSettings);

    QLineEdit startLocationLatEdit;
    QLineEdit startLocationLonEdit;
    QLineEdit utmZoneEdit;
    QLineEdit timeZoneEdit;
    QCheckBox utc;
    QCheckBox localTime ;


private:

};

class QualityTab : public QWidget
{
    Q_OBJECT

public:
    explicit QualityTab(Crit3DQuality *quality);

    QLineEdit referenceClimateHeightEdit;
    QLineEdit deltaTSuspectEdit;
    QLineEdit deltaTWrongEdit;
    QLineEdit humidityToleranceEdit;

private:

};

class MeteoTab : public QWidget
{
    Q_OBJECT

public:
    explicit MeteoTab(Crit3DMeteoSettings *meteoSettings);

    QLineEdit minimumPercentageEdit;
    QLineEdit rainfallThresholdEdit;
    QLineEdit thomThresholdEdit;
    QLineEdit transSamaniCoefficientEdit;

private:
};

class DialogSettings : public QDialog
{
    Q_OBJECT

    public:
        explicit DialogSettings(Project* myProject);
        bool acceptValues();
        void accept();

        QTabWidget *getTabWidget() const;
        void setTabWidget(QTabWidget *value);

    protected:
        Project* project_;

    private:
        QTabWidget *tabWidget;
        QDialogButtonBox *buttonBox;
        GeoTab* geoTab;
        QualityTab* qualityTab;
        MeteoTab* metTab;
};

#endif // DIALOGSETTINGS_H
