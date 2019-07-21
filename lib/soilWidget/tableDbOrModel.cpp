#include "tableDbOrModel.h"
#include <QWidget>
#include <qevent.h>
#include <qtooltip.h>
#include <qdebug.h>

#include "tableDelegate.h"

TableDbOrModel::TableDbOrModel(QString name) : name(name)
{
    this->setMouseTracking(true);
    this->viewport()->setMouseTracking(true);

    QStringList tableHeader;
    if (name == "Db")
    {
        this->setColumnCount(10);
        tableHeader << "Upper depth [cm]" << "Lower depth [cm]" << "Sand [%]" << "Silt  [%]" << "Clay [%]" << "Coarse frag. [%]" << "Org. matter [%]"
                        << "Bulk density [g/cm3]" << "K Sat [cm/d]" << "Theta S [-]";
    }
    else if (name == "Model")
    {
        this->setColumnCount(11);
        tableHeader << "USDA Texture" << "Coarse frag. [%]" << "Org. matter [%]"
                        << "Bulk density [g/cm3]" << "K Sat [cm/d]" << "Theta S [-]" << "Theta R [-]" << "Air entry [KPa]"
                        << "alpha [KPa^-1]" << "  n  [-] " << " m   [-] ";
    }

    this->setHorizontalHeaderLabels(tableHeader);
    this->resizeColumnsToContents();
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setShowGrid(true);
    this->setStyleSheet("QTableView {selection-background-color: green;}");
    if (name == "Db")
    {
        this->setItemDelegate(new TableDelegate(this));
    }
    else
    {
        this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

}

void TableDbOrModel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QTableWidgetItem *item = this->itemAt(pos);
    if(!item)
        return;
    if (item->backgroundColor() == "red")
    {
        if (name == "Db")
        {
            QToolTip::showText(this->viewport()->mapToGlobal(pos), "wrong value", this, QRect(pos,QSize(100,100)), 1000);
        }
        else
        {
            QToolTip::showText(this->viewport()->mapToGlobal(pos), "wrong horizon", this, QRect(pos,QSize(100,100)), 1000);
        }

    }
    else if(item->backgroundColor() == "yellow")
    {
        if (name == "Db")
        {
            QToolTip::showText(this->viewport()->mapToGlobal(pos), "missing data", this, QRect(pos,QSize(100,100)), 1000);
        }
        else
        {
            QToolTip::showText(this->viewport()->mapToGlobal(pos), "estimated value", this, QRect(pos,QSize(100,100)), 1000);
        }

    }
}
