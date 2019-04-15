#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include "shapeHandler.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    ui->dbfButton->setEnabled(false);

}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::onFileOpen()
{
    shapeHandler.close();
    filepath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("shapefile (*.shp)"));
    if (!shapeHandler.open(filepath.toStdString()))
    {
        QMessageBox::warning(this, tr("Bad file"), tr("Something is wrong"));
	}
    else
    {
        int count = shapeHandler.getShapeCount();
		ui->shapeEntityCount->setText(QString::number(count));
        QString typeString = QString::fromStdString(shapeHandler.getTypeString());
		ui->shapeType->setText(typeString);
        int fieldNr = shapeHandler.getFieldNumbers();
        ui->fieldsCount->setText(QString::number(fieldNr));


		QList<QTreeWidgetItem *> items;
        for (int i = 0; i < count; i++)
        {
			QStringList list(QString::number(i));
            QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)nullptr, list);
			QVariant v(i);
			item->setData(0, Qt::UserRole, v);
			items.append(item);
		}
		ui->treeWidget->clear();
		ui->treeWidget->insertTopLevelItems(0, items);
        ui->dbfButton->setEnabled(true);

	}
}


void MainWindow::onSelectShape(QTreeWidgetItem *item, int)
{
    if (item != nullptr)
    {
		bool ok;
		int index = item->data(0, Qt::UserRole).toInt(&ok);

		if (ok) {
            ShapeObject object;
            shapeHandler.getShape(index, object);

            qDebug() << "\nShape" << index;
            int32_t vertexCount = int(object.getVertexCount());
            qDebug() << "Nr vertices:" << vertexCount;

            /* test print list of attributes*/
            qDebug() << "List of attributes: " ;
            for (int i = 0; i < shapeHandler.getFieldNumbers(); i++)
            {
                std::string nameField =  shapeHandler.getFieldName(i);
                int typeField = shapeHandler.getFieldType(i);
                qDebug() << QString::fromStdString(nameField) << " = ";
                if (typeField == 0)
                {
                    qDebug() << QString::fromStdString(shapeHandler.readStringAttribute(index,i)) << " ";
                }
                else if (typeField == 1)
                {
                    qDebug() << shapeHandler.readIntAttribute(index,i) << " ";
                }
                else if (typeField == 2)
                {
                    qDebug() << shapeHandler.readDoubleAttribute(index,i) << " ";
                }
                else
                {
                    qDebug() << "invalid field type ";
                }

            }
            /* */

            const Point<double> *p_ptr = object.getVertices();
            const Point<double> *p_end = p_ptr + (vertexCount - 1);

            if (p_ptr->x == p_end->x && p_ptr->y == p_end->y)
            {
                qDebug() << "First == Last";
            }

            while (p_ptr <= p_end)
            {
                qDebug() << p_ptr->x << p_ptr->y;
                p_ptr++;
            }
		}
	}
}


void MainWindow::on_dbfButton_clicked()
{

    QFileInfo filepathInfo(filepath);
    QString file_temp = filepathInfo.absolutePath()+"/"+filepathInfo.baseName()+"_temp.dbf";

    QFile::copy(filepathInfo.absolutePath()+"/"+filepathInfo.baseName()+".dbf", file_temp);

    shapeHandler.openDBF(filepath.toStdString());
    DBFWidget = new TableDBFDialog(&shapeHandler);

}





