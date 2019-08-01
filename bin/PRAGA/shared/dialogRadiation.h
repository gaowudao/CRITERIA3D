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

        QComboBox* comboAlgorithm;

        QCheckBox* checkRealSky;
        QCheckBox* checkUseTotalTransmiss;
        QLineEdit* editTransClearSky;

        QComboBox* comboLinkeMode;
        QLineEdit* editLinke;
        QLineEdit* editLinkeMap;

        QComboBox* comboAlbedoMode;
        QLineEdit* editAlbedo;
        QLineEdit* editAlbedoMap;

        QComboBox* tiltMode;
        QLineEdit* editTilt;
        QLineEdit* editTiltMap;
        QLineEdit* editAspect;

        QCheckBox* checkShadowing;

        void loadLinke();
        void loadAlbedo();

        void accept();

    protected:
        Project* project_;

    private:
        QDialogButtonBox *buttonBox;
};

#endif // DIALOGRADIATION_H
