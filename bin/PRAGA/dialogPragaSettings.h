#ifndef DIALOGPRAGASETTINGS_H
#define DIALOGPRAGASETTINGS_H

#include <QWidget>

#ifndef ELABORATIONSETTINGS_H
    #include "elaborationSettings.h"
#endif

#ifndef DIALOGSETTINGS_H
    #include "dialogSettings.h"
#endif

class ElaborationTab : public QWidget
{
    Q_OBJECT

public:
    explicit ElaborationTab(Crit3DElaborationSettings *elabSettings);

    QLineEdit anomalyPtsMaxDisEdit;
    QLineEdit anomalyPtsMaxDeltaZEdit;
    QLineEdit gridMinCoverageEdit;
    QCheckBox automaticTmedEdit;
    QCheckBox automaticETPEdit;
    QCheckBox mergeJointStationsEdit;

private:
};

class DialogPragaSettings : public DialogSettings
{
    Q_OBJECT

    public:
        explicit DialogPragaSettings(QSettings *projectSettings,
                                     QSettings *settings,
                                     gis::Crit3DGisSettings *gisSettings,
                                     Crit3DQuality *quality,
                                     Crit3DMeteoSettings *meteoSettings,
                                     Crit3DElaborationSettings *elabSettings);

        bool acceptPragaValues();
        void savePragaSettings();
        void accept();

    private:
        Crit3DElaborationSettings *_elabSettings;
        ElaborationTab* elabTab;
};


#endif // DIALOGPRAGASETTINGS_H
