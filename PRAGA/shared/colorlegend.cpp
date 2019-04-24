#include "commonConstants.h"
#include "colorlegend.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

ColorLegend::ColorLegend(QWidget *parent) :
    QWidget(parent)
{
    colorScale = nullptr;
}

ColorLegend::~ColorLegend()
{
    colorScale = nullptr;
}

void ColorLegend::redraw()
{
    QPainter painter(this);
    drawColorLegend(colorScale, &painter);
}

void ColorLegend::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawColorLegend(colorScale, &painter);
}

bool drawColorLegend(Crit3DColorScale* colorScale, QPainter* myPainter)
{
    if (colorScale == nullptr) return false;

    // clean
    myPainter->setBrush(Qt::white);
    myPainter->fillRect(0, 0, myPainter->window().width(), myPainter->window().height(), myPainter->brush());

    float minimum = colorScale->minimum;
    float maximum = colorScale->maximum;
    if ((minimum == NODATA) || (maximum == NODATA)) return false;

    const int DELTA = 20;
    int legendWidth = myPainter->window().width() - DELTA*2;
    int nrStep = colorScale->nrColors;
    float step = float(maximum - minimum) / float(nrStep);
    float dx = float(legendWidth) / float(nrStep+1);
    int n = maxValue(nrStep/4, 1);

    Crit3DColor* myColor;
    QColor myQColor;
    float value = minimum;

    for (int i = 0; i<=nrStep; i++)
    {
        myColor = colorScale->getColor(value);
        myQColor = QColor(myColor->red, myColor->green, myColor->blue);
        myPainter->setBrush(myQColor);
        myPainter->fillRect(int(DELTA + dx*i), 0, int(ceil(dx)), 20, myPainter->brush());

        if ((i % n) == 0)
        {
            if (fabs(int(value) - value) < float(EPSILON))
            {
                myPainter->drawText(int(DELTA*0.5f + dx*i), 36, QString::number(int(value)));
            }
            else if (fabs(int(value * 10) - (value * 10)) < 0.01f)
            {
                myPainter->drawText(int(DELTA*0.5f + dx*i), 36, QString::number(double(value),'f',1));
            }
            else
            {
                myPainter->drawText(int(DELTA*0.5f + dx*i), 36, QString::number(double(value),'f',2));
            }
        }
        value += step;
    }

    return true;
}
