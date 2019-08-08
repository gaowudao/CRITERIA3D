#include "lineHorizont.h"
#include <qdebug.h>

LineHorizont::LineHorizont()
{
    state = false;
}

void LineHorizont::setClass(int classUSDA)
{

    this->classUSDA = classUSDA;
    QPalette pal = palette();

    switch (classUSDA) {
    // sand
    case 1:
    {
        pal.setColor(QPalette::Background, QColor(255,206,156));
        break;
    }
    // loamy sand
    case 2:
    {
        pal.setColor(QPalette::Background, QColor(255,206,206));
        break;
    }
    // sandy loam
    case 3:
    {
        pal.setColor(QPalette::Background, QColor(255,206,255));
        break;
    }
    // silt loam
    case 4:
    {
        pal.setColor(QPalette::Background, QColor(156,206,000));
        break;
    }
    // loam
    case 5:
    {
        pal.setColor(QPalette::Background, QColor(206,156,000));
        break;
    }
    // silt
    case 6:
    {
        pal.setColor(QPalette::Background, QColor(000,255,49));
        break;
    }
    // sandy clayloam
    case 7:
    {
        pal.setColor(QPalette::Background, QColor(255,156,156));
        break;
    }
    // silty clayloam
    case 8:
    {
        pal.setColor(QPalette::Background, QColor(99,206,156));
        break;
    }
    // clayloam
    case 9:
    {
        pal.setColor(QPalette::Background, QColor(206,255,99));
        break;
    }
    // sandy clay
    case 10:
    {
        pal.setColor(QPalette::Background, QColor(255,000,000));
        break;
    }
    // silty clay
    case 11:
    {
        pal.setColor(QPalette::Background, QColor(156,255,206));
        break;
    }
    // clay
    case 12:
    {
        pal.setColor(QPalette::Background, QColor(255,255,156));
        break;
    }


    }
    this->setAutoFillBackground(true);
    this->setPalette(pal);


}

void LineHorizont::mousePressEvent(QMouseEvent* event)
{
         state = true;
         repaint();
         emit pressed();
}

void LineHorizont::mouseReleaseEvent(QMouseEvent* event)
{
         state = false;
         repaint();
         emit released();
         emit clicked();
}

void LineHorizont::paintEvent(QMouseEvent* event)
{
         if(state)
         {
             QString BorderThickness("2");
             QString hexColorCode("#FF00FF");
             QString color = this->palette().background().color().name();
             this->setStyleSheet("ChildWidget { border: " + BorderThickness + " solid " +
                                               hexColorCode + ";"
                                                              "background-color: "+ color + ";"
                                                              "color: white; }");
         }
         else
         {

         }
}


