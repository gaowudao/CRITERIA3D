#include "commonConstants.h"
#include "colorlegend.h"
#include "project.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

ColorLegend::ColorLegend(QWidget *parent) :
    QWidget(parent)
{
    colorScale = NULL;
}

ColorLegend::~ColorLegend()
{
    colorScale = NULL;
}

void ColorLegend::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawColorLegend(colorScale, &painter);
}

bool drawColorLegend(Crit3DColorScale* colorScale, QPainter* myPainter)
{
    if (colorScale == NULL) return false;

    float minimum = colorScale->minimum;
    float maximum = colorScale->maximum;
    if ((minimum == NODATA) || (maximum == NODATA)) return false;

    const int DELTA = 20;
    int legendWidth = myPainter->window().width() - DELTA*2;
    int nrStep = colorScale->nrColors;
    float step = float(maximum - minimum) / float(nrStep);
    float dx = float(legendWidth) / float(nrStep+1);
    int n = nrStep/4;

    Crit3DColor* myColor;
    QColor myQColor;
    float value = minimum;

    for (int i = 0; i<=nrStep; i++)
    {
        myColor = colorScale->getColor(value);
        myQColor = QColor(myColor->red, myColor->green, myColor->blue);
        myPainter->setBrush(myQColor);
        myPainter->fillRect(DELTA+dx*i, 0, ceil(dx), 20, myPainter->brush());
        if ((i % n) == 0)
        {
            if (int(value) == value)
                myPainter->drawText(DELTA*0.5 + dx*i, 40, QString::number(value));
            else
                myPainter->drawText(DELTA*0.5 + dx*i, 40, QString::number(value,'f',1));
        }
        value += step;
    }

    return true;
}
