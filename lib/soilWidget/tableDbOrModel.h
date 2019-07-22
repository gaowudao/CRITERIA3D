#ifndef TABLEDB_H
#define TABLEDB_H

#include <QTableWidget>

enum tableType{dbTable, modelTable};

class TableDbOrModel: public QTableWidget
{
Q_OBJECT
public:
    TableDbOrModel(tableType type);
    void mouseMoveEvent(QMouseEvent *event);
private:
    tableType type;

};

#endif // TABLEDB_H
