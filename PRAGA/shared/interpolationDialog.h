#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

#ifndef INTERPOLATIONSETTINGS_H
    #include "interpolationSettings.h"
#endif

#include <QSettings>
#include <QDialog>
#include <QtWidgets>
#include <QPushButton>

class Crit3DMeteoPointsDbHandler;

class InterpolationDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit InterpolationDialog(QSettings *settings, Crit3DInterpolationSettings *myInterpolationSetting);

        QComboBox algorithmEdit;
        QLineEdit minRegressionR2Edit;
        QCheckBox* lapseRateCodeEdit;
        QCheckBox* thermalInversionEdit;
        QCheckBox* optimalDetrendingEdit;
        QCheckBox* topographicDistanceEdit;
        QCheckBox* useDewPointEdit;
        QComboBox gridAggregationMethodEdit;
        QVector <QCheckBox*> proxy;

        QDialogButtonBox *buttonBox;

        void writeInterpolationSettings();
        void accept();

    private:
        QSettings* _paramSettings;
        Crit3DInterpolationSettings *_interpolationSettings;

};

class ProxyDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit ProxyDialog(QSettings *settings, Crit3DMeteoPointsDbHandler *myMeteoPointsHandler);
        void showProxyProperties();

        QComboBox _proxy;
        QComboBox _field;
        QComboBox _table;
        QLineEdit _proxyGridName;

        QDialogButtonBox *buttonBox;

        void accept();

    private:
        QSettings* _paramSettings;
        Crit3DMeteoPointsDbHandler *_meteoPointsHandler;

};

#endif // INTERPOLATIONDIALOG_H
