#include "lineHorizont.h"
#include <qdebug.h>

LineHorizont::LineHorizont()
{

}

void LineHorizont::setClass(int classUSDA)
{

    this->classUSDA = classUSDA;
    QPalette pal = palette();

    switch (classUSDA) {
    case 1:
    {
        pal.setColor(QPalette::Background, Qt::darkYellow);
        break;
    }
    case 2:
    {
        pal.setColor(QPalette::Background, Qt::magenta);
        break;
    }
    case 3:
    {
        pal.setColor(QPalette::Background, Qt::darkMagenta);
        break;
    }
    case 4:
    {
        pal.setColor(QPalette::Background, Qt::darkGreen);
        break;
    }
    case 5:
    {
        pal.setColor(QPalette::Background, Qt::green);
        break;
    }
    case 6:
    {
        pal.setColor(QPalette::Background, Qt::cyan);
        break;
    }
    case 7:
    {
        pal.setColor(QPalette::Background, Qt::darkCyan);
        break;
    }
    case 8:
    {
        pal.setColor(QPalette::Background, Qt::red);
        break;
    }
    case 9:
    {
        pal.setColor(QPalette::Background, Qt::darkRed);
        break;
    }
    case 10:
    {
        pal.setColor(QPalette::Background, Qt::blue);
        break;
    }
    case 11:
    {
        pal.setColor(QPalette::Background, Qt::darkBlue);
        break;
    }
    case 12:
    {
        pal.setColor(QPalette::Background, Qt::black);
        break;
    }


    }
    this->setAutoFillBackground(true);
    this->setPalette(pal);


}


