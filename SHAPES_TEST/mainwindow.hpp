#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

    #include <QMainWindow>
    #include <QTreeWidgetItem>

    #include "shapeHandler.h"

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

    private slots:
        void onFileOpen();
        void onSelectShape(QTreeWidgetItem *item, int column);
    };

#endif // MAINWINDOW_HPP
