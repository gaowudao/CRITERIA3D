#include "lineHorizont.h"
#include <qdebug.h>

LineHorizont::LineHorizont()
{
    selected = false;
}

void LineHorizont::setClass(int classUSDA)
{

    this->classUSDA = classUSDA;
    linePalette = palette();

    switch (classUSDA) {
    // sand
    case 1:
    {
        linePalette.setColor(QPalette::Background, QColor(255,206,156));
        break;
    }
    // loamy sand
    case 2:
    {
        linePalette.setColor(QPalette::Background, QColor(255,206,206));
        break;
    }
    // sandy loam
    case 3:
    {
        linePalette.setColor(QPalette::Background, QColor(255,206,255));
        break;
    }
    // silt loam
    case 4:
    {
        linePalette.setColor(QPalette::Background, QColor(156,206,000));
        break;
    }
    // loam
    case 5:
    {
        linePalette.setColor(QPalette::Background, QColor(206,156,000));
        break;
    }
    // silt
    case 6:
    {
        linePalette.setColor(QPalette::Background, QColor(000,255,49));
        break;
    }
    // sandy clayloam
    case 7:
    {
        linePalette.setColor(QPalette::Background, QColor(255,156,156));
        break;
    }
    // silty clayloam
    case 8:
    {
        linePalette.setColor(QPalette::Background, QColor(99,206,156));
        break;
    }
    // clayloam
    case 9:
    {
        linePalette.setColor(QPalette::Background, QColor(206,255,99));
        break;
    }
    // sandy clay
    case 10:
    {
        linePalette.setColor(QPalette::Background, QColor(255,000,000));
        break;
    }
    // silty clay
    case 11:
    {
        linePalette.setColor(QPalette::Background, QColor(156,255,206));
        break;
    }
    // clay
    case 12:
    {
        linePalette.setColor(QPalette::Background, QColor(255,255,156));
        break;
    }


    }
    this->setAutoFillBackground(true);
    this->setPalette(linePalette);

}

void LineHorizont::mousePressEvent(QMouseEvent* event)
{

    QPalette pal(this->palette());
    // select the element
    if (selected == false)
    {
        selected = true;
        pal.setColor( QPalette::Background, pal.background().color().lighter(130));
        this->setPalette(pal);
        repaint();
    }
    // de-select the element
    else
    {
        selected = false;
        restorePalette();
    }
    emit clicked(index);

}

void LineHorizont::setSelected(bool value)
{
    selected = value;
}

void LineHorizont::restorePalette()
{
    this->setPalette(linePalette);
    repaint();
}

bool LineHorizont::getSelected() const
{
    return selected;
}

int LineHorizont::getIndex() const
{
    return index;
}

void LineHorizont::setIndex(int value)
{
    index = value;
}

