#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtWidgets>
#include <QTabWidget>
#include <QDialogButtonBox>


class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        SettingsDialog();

    private:
        QTabWidget *tabWidget;
        QDialogButtonBox *buttonBox;
};

#endif // SETTINGSDIALOG_H
