#ifndef tableDBFDialog_H
#define tableDBFDialog_H

#include <QtWidgets>
#include "shapeHandler.h"
#include "dbfNewColDialog.h"

#define DEFAULT_INTEGER_MAX_DIGITS 9
#define DEFAULT_DOUBLE_MAX_DIGITS 13
#define DEFAULT_DOUBLE_MAX_DECIMALS 3

class DbfTableDialog : public QDialog
{
    Q_OBJECT

    private:

    Crit3DShapeHandler *shapeHandler;
    QTableWidget* m_DBFTableWidget;
    DbfNewColDialog* newColDialog;
    QMenuBar *menuBar;
    QMenu *editMenu;
    QAction *addRow;
    QAction *deleteRow;
    QAction *addCol;
    QAction *deleteCol;
    QAction *save;
    QStringList labels;
    QStringList m_DBFTableHeader;

    public:
        DbfTableDialog(Crit3DShapeHandler *shapeHandler);
        void addRowClicked();
        void removeRowClicked();
        void addColClicked();
        void removeColClicked();
        void cellChanged(int row, int column);
        void closeEvent(QCloseEvent *);
        void saveChangesClicked();
};

#endif // TableDBFDialog_H
