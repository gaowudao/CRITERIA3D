#include <QApplication>
#include <QFile>
#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();
	
	return app.exec();
}
