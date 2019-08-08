#ifndef LINEHORIZONT_H
#define LINEHORIZONT_H

#include <QWidget>
#include <QPainter>

class LineHorizont : public QWidget
{
    Q_OBJECT
public:
    LineHorizont();
    void setClass(int classUSDA);

protected:
     void mousePressEvent(QMouseEvent* event);
     void mouseReleaseEvent(QMouseEvent* event);
     void paintEvent(QMouseEvent* event);

private:
    int classUSDA;
    bool state;

signals:
       void clicked();
       void released();
       void pressed();

};

#endif // LINEHORIZONT_H
