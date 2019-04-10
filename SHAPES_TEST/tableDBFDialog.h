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

    public:
        tableDBFDialog(Crit3DShapeHandler *shapeHandler);
        void addRowClicked();
        void removeRowClicked();
};

#endif // TABLEDBFDIALOG_H
