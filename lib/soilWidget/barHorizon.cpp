#include "barHorizon.h"
#include "soil.h"
#include <qdebug.h>

BarHorizon::BarHorizon(QWidget *parent)
{
    selected = false;
    this->setFrameStyle(QFrame::NoFrame);
    labelNumber = new QLabel;

    // font size
    QFont font = labelNumber->font();
    font.setPointSize(8);
    labelNumber->setFont(font);

    QHBoxLayout *layoutNumber = new QHBoxLayout;
    layoutNumber->setAlignment(Qt::AlignCenter);
    layoutNumber->addWidget(labelNumber);
    setLayout(layoutNumber);
}

void BarHorizon::setClass(int classUSDA)
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

void BarHorizon::mousePressEvent(QMouseEvent* event)
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

void BarHorizon::setSelected(bool value)
{
    selected = value;
}

void BarHorizon::setSelectedFrame()
{
    this->setFrameStyle(QFrame::Box);
    this->setLineWidth(2);
}

void BarHorizon::restoreFrame()
{
    this->setFrameStyle(QFrame::NoFrame);
}

bool BarHorizon::getSelected() const
{
    return selected;
}

int BarHorizon::getIndex() const
{
    return index;
}

void BarHorizon::setIndex(int value)
{
    index = value;
    labelNumber->setText(QString::number( (value+1) ));
}


BarHorizonList::BarHorizonList()
{
    groupBox = new QGroupBox();
    groupBox->setMinimumWidth(90);
    groupBox->setTitle("Depth [cm]");

    depthLayout = new QVBoxLayout;
    //depthLayout->setAlignment(Qt::AlignLeft);

    barLayout = new QVBoxLayout;
    barLayout->setAlignment(Qt::AlignHCenter);

    mainLayout = new QHBoxLayout;
    mainLayout->addLayout(depthLayout);
    mainLayout->addLayout(barLayout);
    groupBox->setLayout(mainLayout);
}


void BarHorizonList::draw(soil::Crit3DSoil *soil)
{
    int totHeight = int(groupBox->height() * 0.9);

    for (unsigned int i = 0; i < soil->nrHorizons; i++)
    {
        int length = int(totHeight * (soil->horizon[i].lowerDepth - soil->horizon[i].upperDepth) / soil->totalDepth);
        BarHorizon* newBar = new BarHorizon();
        newBar->setIndex(signed(i));
        newBar->setFixedWidth(28);
        newBar->setFixedHeight(length);
        newBar->setClass(soil->horizon[i].texture.classUSDA);
        barLayout->addWidget(newBar);
        barList.push_back(newBar);

        QLabel *startLabel = new QLabel();
        QLabel *endLabel = new QLabel();
        // font size
        QFont font = startLabel->font();
        font.setPointSize(6);
        startLabel->setFont(font);
        endLabel->setFont(font);
        startLabel->setText(QString::number( (soil->horizon[i].upperDepth*100) ));
        endLabel->setText(QString::number( (soil->horizon[i].lowerDepth*100) ));
        if (i == 0)
        {
            startLabel->setGeometry(0, 0, 10, 10);
            endLabel->setGeometry(0, startLabel->y() + 10, 10, 10);
        }
        else
        {
            startLabel->setGeometry(0, labelList.last()->y() - 10, 10, 10);
            endLabel->setGeometry(0, startLabel->y() + 10, 10, 10);
        }

        depthLayout->addWidget(startLabel);
        depthLayout->addWidget(endLabel);
        labelList.push_back(startLabel);
        labelList.push_back(endLabel);
    }
}


void BarHorizonList::clear()
{
    if (!barList.isEmpty())
    {
        qDeleteAll(barList);
        barList.clear();
        qDeleteAll(labelList);
        labelList.clear();
    }
}


void BarHorizonList::selectItem(int index)
{
    for (int i = 0; i < barList.size(); i++)
    {
        if (i != index)
        {
            barList[i]->restoreFrame();
            barList[i]->setSelected(false);
        }
        else
        {
            barList[i]->setSelected(true);
            barList[i]->setSelectedFrame();
        }
    }
}


void BarHorizonList::deselectAll(int index)
{
    for(int i = 0; i < barList.size(); i++)
    {
        if (i != index)
        {
            barList[i]->restoreFrame();
            barList[i]->setSelected(false);
        }
    }
}

