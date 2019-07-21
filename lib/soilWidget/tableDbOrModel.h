#ifndef TABLEDB_H
#define TABLEDB_H

#include <QTableWidget>

class TableDbOrModel: public QTableWidget
{
Q_OBJECT
public:
    TableDbOrModel(QString name);
    void mouseMoveEvent(QMouseEvent *event);
private:
    QString name;

};

#endif // TABLEDB_H
