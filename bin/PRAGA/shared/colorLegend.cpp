#include "commonConstants.h"
#include "basicMath.h"
#include "colorLegend.h"
#include <QPainter>
#include <cmath>


ColorLegend::ColorLegend(QWidget *parent) :
    QWidget(parent)
{
    this->colorScale = nullptr;
}


ColorLegend::~ColorLegend()
{
    this->colorScale = nullptr;
}


void ColorLegend::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (this->colorScale == nullptr) return;
    if (isEqual(this->colorScale->minimum, NODATA)
        || isEqual(this->colorScale->maximum, NODATA)) return;

    QPainter painter(this);
    Crit3DColor* myColor;

    // clean widget
    painter.setBrush(Qt::white);
    painter.fillRect(0, 0, painter.window().width(), painter.window().height(), painter.brush());

    const int DELTA = 20;
    int legendWidth = painter.window().width() - DELTA*2;
    int nrStep = this->colorScale->nrColors;
    float step = (colorScale->maximum - colorScale->minimum) / float(nrStep);
    float dx = float(legendWidth) / float(nrStep+1);
    int stepText = MAXVALUE(nrStep / 4, 1);

    float value = this->colorScale->minimum;
    for (int i = 0; i<=nrStep; i++)
    {
        myColor = this->colorScale->getColor(value);
        painter.setBrush(QColor(myColor->red, myColor->green, myColor->blue));
        painter.fillRect(int(DELTA + dx*i), 0, int(ceil(dx)), 20, painter.brush());

        if ((i % stepText) == 0)
        {
            if (isEqual(int(value), value))
            {
                painter.drawText(int(DELTA*0.5f + dx*i), 36, QString::number(int(value)));
            }
            else if (fabs(int(value * 10) - (value * 10)) < 0.01f)
            {
                painter.drawText(int(DELTA*0.5f + dx*i), 36, QString::number(double(value),'f',1));
            }
            else
            {
                painter.drawText(int(DELTA*0.5f + dx*i), 36, QString::number(double(value),'f',2));
            }
        }
        value += step;
    }
}
