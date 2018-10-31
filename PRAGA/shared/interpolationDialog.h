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
class Crit3DProxyMeteoPoint;

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
        explicit ProxyDialog(QSettings *settings,
                             Crit3DInterpolationSettings *myInterpolationSettings,
                             Crit3DMeteoPointsDbHandler *myMeteoPointsHandler);

        QComboBox _proxyCombo;
        QComboBox _field;
        QComboBox _table;
        QLineEdit _proxyGridName;

        void changedProxy();
        void changedTable();
        void getGridFile();
        void redrawProxies();
        void addProxy();
        void deleteProxy();
        void saveProxies();

        void accept();

    private:
        QSettings* _paramSettings;
        Crit3DInterpolationSettings *_interpolationSettings;
        Crit3DMeteoPointsDbHandler *_meteoPointsHandler;
        std::vector <Crit3DProxyMeteoPoint> _proxy;

};

#endif // INTERPOLATIONDIALOG_H
