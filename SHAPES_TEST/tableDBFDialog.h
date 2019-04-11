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
    QPushButton *m_addRowButton;
    QPushButton *m_removeRowButton;
    QStringList labels;

    public:
        tableDBFDialog(Crit3DShapeHandler *shapeHandler);
        void addRowClicked();
        void removeRowClicked();
        void cellChanged(int row, int column);
        void closeEvent(QCloseEvent *);
};

#endif // TABLEDBFDIALOG_H
