#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

#ifndef PROJECT_H
    #include "project.h"
#endif

#include <QSettings>
#include <QDialog>
#include <QtWidgets>
#include <QPushButton>
#include <QVBoxLayout>

class InterpolationDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit InterpolationDialog(Project *myProject);

        QComboBox algorithmEdit;
        QLineEdit minRegressionR2Edit;
        QCheckBox* lapseRateCodeEdit;
        QCheckBox* thermalInversionEdit;
        QCheckBox* optimalDetrendingEdit;
        QCheckBox* topographicDistanceEdit;
        QCheckBox* useDewPointEdit;
        QComboBox gridAggregationMethodEdit;
        QVBoxLayout *layoutProxyList;
        QListWidget *proxyListCheck;

        void redrawProxies();
        void writeInterpolationSettings();
        void accept();

    private:
        Project* _project;
        QSettings* _paramSettings;
        Crit3DInterpolationSettings* _interpolationSettings;
        Crit3DInterpolationSettings* _qualityInterpolationSettings;

    private slots:
        void editProxies();

};

class ProxyDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit ProxyDialog(Project *myProject);

        QComboBox _proxyCombo;
        QComboBox _field;
        QComboBox _table;
        QLineEdit _proxyGridName;
        QCheckBox _forQuality;

        bool edited(Project* myProject);
        void changedProxy();
        void changedTable();
        void getGridFile();
        void redrawProxies();
        void addProxy();
        void deleteProxy();
        void writeProxies();
        void saveProxies();
        void saveProxy();
        bool checkProxies(QString *error);
        void accept();

    private:
        Project *_project;
        std::vector <Crit3DProxy> _proxy;

};

#endif // INTERPOLATIONDIALOG_H
