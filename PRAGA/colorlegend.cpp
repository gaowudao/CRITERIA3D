#include "commonConstants.h"
#include "colorlegend.h"
#include "project.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

extern Project myProject;

ColorLegend::ColorLegend(QWidget *parent) :
    QWidget(parent)
{}

ColorLegend::~ColorLegend()
{}

void ColorLegend::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawColorLegend(&(myProject.DTM), &painter);
}


bool drawColorLegend(gis::Crit3DRasterGrid* myRaster, QPainter* myPainter)
{
    if ( myRaster == NULL) return false;
    if (! myRaster->isLoaded) return false;

    float minimum = myRaster->colorScale->minimum;
    float maximum = myRaster->colorScale->maximum;
    if ((minimum == NODATA) || (maximum == NODATA)) return false;

    const int DELTA = 20;
    int legendWidth = myPainter->window().width() - DELTA*2;
    int nrStep = myRaster->colorScale->nrColors;
    float step = float(maximum - minimum) / float(nrStep);
    float dx = float(legendWidth) / float(nrStep+1);
    int n = nrStep/4;

    gis::Crit3DColor* myColor;
    QColor myQColor;
    float value = minimum;

    for (int i = 0; i<=nrStep; i++)
    {
        myColor = myRaster->colorScale->getColor(value);
        myQColor = QColor(myColor->red, myColor->green, myColor->blue);
        myPainter->setBrush(myQColor);
        myPainter->fillRect(DELTA+dx*i, 0, ceil(dx), 20, myPainter->brush());
        if ((i % n) == 0)
        {
            if (int(value) == value)
                myPainter->drawText(DELTA*0.5 + dx*i, 40, QString::number(value));
            else
                myPainter->drawText(DELTA*0.5 + dx*i, 40, QString::number(value,'g',1));
        }
        value += step;
    }

    return true;
}
