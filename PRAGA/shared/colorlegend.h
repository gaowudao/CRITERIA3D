#ifndef COLORLEGEND_H
#define COLORLEGEND_H

    #include <QWidget>
    #include "color.h"

    class ColorLegend : public QWidget
    {
        Q_OBJECT

    public:
        explicit ColorLegend(QWidget *parent = nullptr);
        ~ColorLegend();

        Crit3DColorScale *colorScale;

    private:
        void paintEvent(QPaintEvent *);
    };

    bool drawColorLegend(Crit3DColorScale* colorScale, QPainter* myPainter);

#endif // COLORLEGEND_H
