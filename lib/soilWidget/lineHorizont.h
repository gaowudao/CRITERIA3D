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

private:
    int classUSDA;


};

#endif // LINEHORIZONT_H
