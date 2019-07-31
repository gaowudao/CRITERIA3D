#ifndef DIALOGRADIATION_H
#define DIALOGRADIATION_H

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>

class QDialogButtonBox;

class Project;

class DialogRadiation : public QDialog
{
    Q_OBJECT

    public:
        explicit DialogRadiation(Project* myProject);

        QComboBox algorithmEdit;

        QCheckBox checkRealSky;
        QComboBox comboTransmissAlgorithm;
        QComboBox comboTransmissPeriod;
        QCheckBox checkUseTotalTransmiss;
        QLineEdit editTransClearSky;

        QComboBox comboLinkeMode;
        QLineEdit editLinke;
        QLineEdit editLinkeMap;

        QComboBox comboAlbedoMode;
        QLineEdit editAlbedo;
        QLineEdit editAlbedoMap;

        QComboBox tiltMode;
        QLineEdit editTilt;
        QLineEdit editTiltMap;
        QLineEdit editAspect;

        QCheckBox checkShadowing;

        void accept();

    protected:
        Project* project_;

    private:
        QDialogButtonBox *buttonBox;
};

#endif // DIALOGRADIATION_H
