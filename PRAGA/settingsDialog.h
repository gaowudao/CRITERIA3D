#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QFileInfo;
class QTabWidget;


class GeoTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeoTab();
};

class QualityTab : public QWidget
{
    Q_OBJECT

public:
    explicit QualityTab();
};

class ElaborationTab : public QWidget
{
    Q_OBJECT

public:
    explicit ElaborationTab();
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit SettingsDialog();

    private:
        QTabWidget *tabWidget;
        QDialogButtonBox *buttonBox;
        GeoTab* geoTab;
        QualityTab* qualityTab;
        ElaborationTab* elabTab;
};

#endif // SETTINGSDIALOG_H
