#ifndef LINEHORIZONT_H
#define LINEHORIZONT_H

#include <QWidget>
#include <QPainter>

class LineHorizont : public QWidget
{
    Q_OBJECT
public:
    //LineHorizont(QWidget* parent, QPainter* painter);
    LineHorizont(QWidget* parent);
    void initialize(int x1, int y1, int width, int height, QString colorName);

    int getX1() const;
    void setX1(int value);

    int getY1() const;
    void setY1(int value);

    int getWidth() const;
    void setWidth(int value);

    int getHeight() const;
    void setHeight(int value);

    void draw();

protected:
    void paintEvent(QPaintEvent *event);

private:
    //QPainter *p;
    QString colorName;
    int x1;
    int y1;
    int width;
    int height;

};

#endif // LINEHORIZONT_H
