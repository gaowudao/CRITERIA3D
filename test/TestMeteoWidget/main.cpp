#include <QApplication>

#include "meteoWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Crit3DMeteoWidget w;
    w.show();

    return a.exec();
}
 
