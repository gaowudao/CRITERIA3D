#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

class Plot;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_pushInitialize_clicked();
    void on_pushRunNextHour_clicked();
    void on_pushRunAllPeriod_clicked();  
    void on_pushLoadFileSoil_clicked();
    void on_pushLoadFileMeteo_clicked();
    void on_chkBoxWater_clicked();
    void on_chkBoxHeat_clicked();

    bool initializeModel();

    void on_pushCopyOutput_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *selItem);

private:
    Ui::MainWindow *ui;

private:
    Plot * outPlot;

};

#endif // MAINWINDOW_H
