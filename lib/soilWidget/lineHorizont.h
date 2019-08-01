#ifndef LINEHORIZONT_H
#define LINEHORIZONT_H

#include <QLine>
#include <QObject>

class LineHorizont : public QObject, public QLine
{
    Q_OBJECT
public:
    LineHorizont(int x1, int y1, int x2, int y2, QString colorName);
private:
    QString colorName;
};

#endif // LINEHORIZONT_H
