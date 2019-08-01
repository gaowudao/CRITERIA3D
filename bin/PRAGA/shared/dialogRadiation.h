#ifndef DIALOGRADIATION_H
#define DIALOGRADIATION_H

#include <QtWidgets>

class QDialogButtonBox;

class Project;

class DialogRadiation : public QDialog
{
    Q_OBJECT

    public:
        explicit DialogRadiation(Project* myProject);

        QComboBox* comboAlgorithm;

        QCheckBox* checkRealSky;
        QComboBox* comboRealSky;
        QLineEdit* editTransClearSky;

        QGroupBox* groupLinke;
        QComboBox* comboLinkeMode;
        QLineEdit* editLinke;
        QLineEdit* editLinkeMap;

        QComboBox* comboAlbedoMode;
        QLineEdit* editAlbedo;
        QLineEdit* editAlbedoMap;

        QComboBox* comboTiltMode;
        QLineEdit* editTilt;
        QLineEdit* editAspect;

        QCheckBox* checkShadowing;

        void loadLinke();
        void loadAlbedo();
        void updateAlgorithm(const QString myString);
        void updateLinke();
        void updateAlbedo();
        void updateTilt();

        void accept();

    protected:
        Project* project_;

    private:
        QDialogButtonBox *buttonBox;
};

#endif // DIALOGRADIATION_H
