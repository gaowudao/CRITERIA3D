#include "barHorizons.h"
#include "soil.h"
//#include <qdebug.h>

BarHorizons::BarHorizons(QWidget *parent)
{
    selected = false;
    this->setFrameStyle(QFrame::NoFrame);
    labelNumber = new QLabel;
    QHBoxLayout *layoutNumber = new QHBoxLayout;
    layoutNumber->setAlignment(Qt::AlignCenter);
    layoutNumber->addWidget(labelNumber);
    setLayout(layoutNumber);
}

void BarHorizons::setClass(int classUSDA)
{

    this->classUSDA = classUSDA;
    QPalette linePalette = palette();

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

void BarHorizons::mousePressEvent(QMouseEvent* event)
{

    // select the element
    if (selected == false)
    {
        selected = true;       
        setSelectedFrame();
    }
    // de-select the element
    else
    {
        selected = false;
        restoreFrame();
    }
    emit clicked(index);

}

void BarHorizons::setSelected(bool value)
{
    selected = value;
}

void BarHorizons::setSelectedFrame()
{
    this->setFrameStyle(QFrame::Box);
    this->setLineWidth(2);
}

void BarHorizons::restoreFrame()
{
    this->setFrameStyle(QFrame::NoFrame);
}

bool BarHorizons::getSelected() const
{
    return selected;
}

int BarHorizons::getIndex() const
{
    return index;
}

void BarHorizons::setIndex(int value)
{
    index = value;
    labelNumber->setText(QString::number( (value+1) ));
}


BarHorizonsList::BarHorizonsList()
{
    groupBox = new QGroupBox();
    groupBox->setMinimumWidth(90);
    groupBox->setTitle("Depth [cm]");

    layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignHCenter);
    groupBox->setLayout(layout);
}


void BarHorizonsList::draw(soil::Crit3DSoil *soil)
{
    int totHeight = int(groupBox->height() * 0.9);

    for (unsigned int i = 0; i < soil->nrHorizons; i++)
    {
        int length = int(totHeight * (soil->horizon[i].lowerDepth - soil->horizon[i].upperDepth) / soil->totalDepth);
        BarHorizons* newBar = new BarHorizons();
        newBar->setIndex(signed(i));
        newBar->setFixedWidth(25);
        newBar->setFixedHeight(length);
        newBar->setClass(soil->horizon[i].texture.classUSDA);
        layout->addWidget(newBar);
        list.push_back(newBar);
    }
}


void BarHorizonsList::clear()
{
    if (!list.isEmpty())
    {
        qDeleteAll(list);
        list.clear();
    }
}


void BarHorizonsList::selectItem(int index)
{
    for (int i = 0; i < list.size(); i++)
    {
        if (i != index)
        {
            list[i]->restoreFrame();
            list[i]->setSelected(false);
        }
        else
        {
            list[i]->setSelected(true);
            list[i]->setSelectedFrame();
        }
    }
}

