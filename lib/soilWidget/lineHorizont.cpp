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
    // sand
    case 1:
    {
        pal.setColor(QPalette::Background, QColor(255,194,129));
        break;
    }
    // loamy sand
    case 2:
    {
        pal.setColor(QPalette::Background, QColor(255,159,159));
        break;
    }
    // sandy loam
    case 3:
    {
        pal.setColor(QPalette::Background, Qt::magenta);
        break;
    }
    // silt loam
    case 4:
    {
        pal.setColor(QPalette::Background, QColor(000,128,000));
        break;
    }
    // loam
    case 5:
    {
        pal.setColor(QPalette::Background, QColor(161,063,000));
        break;
    }
    // silt
    case 6:
    {
        pal.setColor(QPalette::Background, QColor(000,255,000));
        break;
    }
    // sandy clayloam
    case 7:
    {
        pal.setColor(QPalette::Background, QColor(194,18,18));
        break;
    }
    // silty clayloam
    case 8:
    {
        pal.setColor(QPalette::Background, QColor(000,130,80));
        break;
    }
    // clayloam
    case 9:
    {
        pal.setColor(QPalette::Background, QColor(065,255,050));
        break;
    }
    // sandy clay
    case 10:
    {
        pal.setColor(QPalette::Background, Qt::red);
        break;
    }
    // silty clay
    case 11:
    {
        pal.setColor(QPalette::Background, QColor(128,255,128));
        break;
    }
    // clay
    case 12:
    {
        pal.setColor(QPalette::Background, QColor(255,255,128));
        break;
    }


    }
    this->setAutoFillBackground(true);
    this->setPalette(pal);


}


