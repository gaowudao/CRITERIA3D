#ifndef tableDBFDialog_H
#define tableDBFDialog_H

#include <QtWidgets>
#include "shapeHandler.h"
#include "newColDialog.h"

class TableDBFDialog : public QDialog
{
    Q_OBJECT

    private:

    Crit3DShapeHandler *shapeHandler;
    QTableWidget* m_DBFTableWidget;
    NewColDialog* newColDialog;
    QMenuBar *menuBar;
    QMenu *editMenu;
    QAction *addRow;
    QAction *deleteRow;
    QAction *addCol;
    QAction *deleteCol;
    QAction *save;
    QStringList labels;
    QStringList m_DBFTableHeader;;

    public:
        TableDBFDialog(Crit3DShapeHandler *shapeHandler);
        void addRowClicked();
        void removeRowClicked();
        void addColClicked();
        void removeColClicked();
        void cellChanged(int row, int column);
        void closeEvent(QCloseEvent *);
        void saveChangesClicked();
};

#endif // TableDBFDialog_H
