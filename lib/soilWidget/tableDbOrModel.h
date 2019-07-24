#ifndef TABLEDB_H
#define TABLEDB_H

#include <QTableWidget>

// custom QTableWidget to implement mouseMoveEvent and manage QToolTip timeout
// tables can be of 2 different types: dbTable and modelTable (each with specific header and background color)

enum tableType{dbTable, modelTable};

class TableDbOrModel: public QTableWidget
{
Q_OBJECT
public:
    TableDbOrModel(tableType type);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    tableType type;

};

#endif // TABLEDB_H
