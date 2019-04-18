#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

    #include <QMainWindow>
    #include <QTreeWidgetItem>
    #include <QDialogButtonBox>
    #include <QLineEdit>
    #include <QDesktopWidget>
    #include <QTableWidget>

    #include "shapeHandler.h"
    #include "dbfTableDialog.h"

    namespace Ui {
    class MainWindow;
    }

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    protected:
        Crit3DShapeHandler shapeHandler;

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;
        DbfTableDialog *DBFWidget;
        QString filepath;

    private slots:
        void onFileOpen();
        void onSelectShape(QTreeWidgetItem *item, int column);
        void on_dbfButton_clicked();

    };

#endif // MAINWINDOW_HPP
