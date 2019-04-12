#ifndef TABLEDBFDIALOG_H
#define TABLEDBFDIALOG_H

#include <QtWidgets>
#include "shapeHandler.h"

class tableDBFDialog : public QDialog
{
    Q_OBJECT

    private:

    Crit3DShapeHandler *shapeHandler;
    QTableWidget* m_DBFTableWidget;
    QMenuBar *menuBar;
    QMenu *editMenu;
    QAction *addRow;
    QAction *deleteRow;
    QAction *addCol;
    QAction *deleteCol;
    QAction *save;
    QStringList labels;

    public:
        tableDBFDialog(Crit3DShapeHandler *shapeHandler);
        void addRowClicked();
        void removeRowClicked();
        void cellChanged(int row, int column);
        void closeEvent(QCloseEvent *);
};

#endif // TABLEDBFDIALOG_H
