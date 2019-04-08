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

    QWidget *DBFWidget = new QWidget;
    QVBoxLayout* DBFLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;

    DBFWidget->resize(QDesktopWidget().availableGeometry().size());

    m_DBFTableWidget = new QTableWidget();
    DBFLayout->addWidget(m_DBFTableWidget);


    int colNumber = shapeHandler.getFieldNumbers();
    int rowNumber = shapeHandler.getShapeCount();
    m_DBFTableWidget->setRowCount(rowNumber);
    m_DBFTableWidget->setColumnCount(colNumber);
    QStringList m_DBFTableHeader;
    std::string nameField;
    int typeField;

    for (int i = 0; i < colNumber; i++)
    {
        nameField =  shapeHandler.getFieldName(i);
        typeField = shapeHandler.getFieldType(i);
        m_DBFTableHeader << QString::fromStdString(nameField);

        for (int j = 0; j < rowNumber; j++)
        {
            if (typeField == 0)
            {
                m_DBFTableWidget->setItem(j, i, new QTableWidgetItem( QString::fromStdString(shapeHandler.readStringAttribute(j,i) )));
            }
            else if (typeField == 1)
            {
                m_DBFTableWidget->setItem(j, i, new QTableWidgetItem( QString::number(shapeHandler.readIntAttribute(j,i) )));
            }
            else if (typeField == 2)
            {
                m_DBFTableWidget->setItem(j, i, new QTableWidgetItem( QString::number(shapeHandler.readDoubleAttribute(j,i) )));
            }

        }
    }
    m_DBFTableWidget->setHorizontalHeaderLabels(m_DBFTableHeader);
    m_DBFTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_DBFTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_DBFTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_DBFTableWidget->setShowGrid(false);
    m_DBFTableWidget->setStyleSheet("QTableView {selection-background-color: red;}");

    int offset = 100;
    m_DBFTableWidget->setMinimumWidth(DBFWidget->width());
    m_DBFTableWidget->setMaximumWidth(DBFWidget->width());
    m_DBFTableWidget->setMinimumHeight(DBFWidget->height() - offset);
    m_DBFTableWidget->setMaximumHeight(DBFWidget->height() - offset);

    const QSize BUTTON_SIZE = QSize(22, 22);
    m_addRowButton = new QPushButton();
    m_addRowButton->setText("+");
    m_addRowButton->setMaximumSize(BUTTON_SIZE);
    m_removeRowButton = new QPushButton();
    m_removeRowButton->setText("-");
    m_removeRowButton->setMaximumSize(BUTTON_SIZE);

    buttonLayout->addWidget(m_addRowButton);
    buttonLayout->addWidget(m_removeRowButton);
    DBFLayout->addLayout(buttonLayout);

    connect(m_addRowButton, &QPushButton::clicked, [=](){ this->addRowClicked(); });
    connect(m_removeRowButton, &QPushButton::clicked, [=](){ this->removeRowClicked(); });


    DBFWidget->setLayout(DBFLayout);
    DBFWidget->show();

}


void MainWindow::addRowClicked()
{
    qDebug() << "addRowClicked ";
}

void MainWindow::removeRowClicked()
{
    qDebug() << "removeRowClicked ";
}
