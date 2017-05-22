#ifndef RUBBERBAND_H
#define RUBBERBAND_H

#include <QObject>
#include <QRubberBand>
#include <QPainter>
#include <QPaintEvent>

class RubberBand : public QRubberBand
{
    Q_OBJECT
public:
    RubberBand(Shape s, QWidget *p = Q_NULLPTR);

signals:

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *);

public slots:

};

#endif // RUBBERBAND_H
