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
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::onFileOpen()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("shapefile (*.shp)"));
    if (! shapeHandler.open(filepath.toStdString()))
    {
        QMessageBox::warning(this, tr("Bad file"), tr("Something is wrong"));
	}
    else
    {
        int count = shapeHandler.getShapeCount();
		ui->shapeEntityCount->setText(QString::number(count));
        QString typeString = QString::fromStdString(shapeHandler.getTypeString());
		ui->shapeType->setText(typeString);

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

			qDebug() << "Shape " << index;
            int32_t vertexCount = int(object.getVertexCount());

			QFile vertexFile("vertex.txt");
			vertexFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
			{
				QTextStream vstrm(&vertexFile);
                const Point<double> *p_ptr = object.getVertices();
				const Point<double> *p_end = p_ptr + vertexCount;
				while (p_ptr < p_end) {
                    vstrm << p_ptr->x << ", " << p_ptr->y << "\n";
					p_ptr++;
				}
                p_ptr = object.getVertices();
				double x_first = p_ptr->x;
				double y_first = p_ptr->y;
				p_ptr += (vertexCount - 1);
				double x_last = p_ptr->x;
				double y_last = p_ptr->y;
                if (x_first == x_last && y_first == y_last)
                {
					qDebug() << "First == Last";
					vertexCount--;
				}
			}
			vertexFile.close();
		}
	}
}

