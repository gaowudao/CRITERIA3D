#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

#ifndef INTERPOLATIONSETTINGS_H
    #include "interpolationSettings.h"
#endif

#include <QSettings>
#include <QDialog>
#include <QtWidgets>

class InterpolationDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit InterpolationDialog(QSettings *settings, Crit3DInterpolationSettings *myInterpolationSetting);

        QComboBox algorithmEdit;
        QLineEdit minRegressionR2Edit;
        QCheckBox lapseRateCodeEdit;
        QCheckBox thermalInversionEdit;
        QCheckBox optimalDetrendingEdit;
        QCheckBox topographicDistance;
        QCheckBox useDewPointEdit;
        QComboBox gridAggregationMethodEdit;
        QVector <QCheckBox*> proxy;

        QDialogButtonBox *buttonBox;

        void accept();
        void saveSettings();

    private:
        Crit3DInterpolationSettings *_interpolationSettings;

};

#endif // INTERPOLATIONDIALOG_H
