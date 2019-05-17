#include "showProperties.h"

ShowProperties::ShowProperties(QString filepath)
    :filepath(filepath)
{

    setFixedSize(800,600);
    setWindowTitle("ShowProperties");
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QFormLayout* formLayout = new QFormLayout;
    QVBoxLayout* verticalLayout = new QVBoxLayout;

    QLabel countLabel;
    countLabel.setText("Count");
    shapeEntityCount = new QLineEdit();

    QLabel typeLabel;
    typeLabel.setText("Type");
    shapeType = new QLineEdit();

    QLabel fields;
    fields.setText("Nr. of attributes");
    fieldsCount = new QLineEdit();

    formLayout->addWidget(&countLabel);
    formLayout->addWidget(shapeEntityCount);
    formLayout->addWidget(&typeLabel);
    formLayout->addWidget(shapeType);
    formLayout->addWidget(&fields);
    formLayout->addWidget(fieldsCount);

    treeWidget = new QTreeWidget();
    treeWidget->setColumnCount(1);
    QStringList headerLabels;
    headerLabels.push_back("Node");
    treeWidget->setHeaderLabels(headerLabels);
    verticalLayout->addWidget(treeWidget);

    DBFWidget = nullptr;

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(verticalLayout);
    this->setLayout(mainLayout);

    if (!shapeHandler.open(filepath.toStdString()))
    {
        QMessageBox::warning(this, tr("Bad file"), tr("Something is wrong"));
    }
    else
    {
        int count = shapeHandler.getShapeCount();
        shapeEntityCount->setText(QString::number(count));
        QString typeString = QString::fromStdString(shapeHandler.getTypeString());
        shapeType->setText(typeString);
        int fieldNr = shapeHandler.getFieldNumbers();
        fieldsCount->setText(QString::number(fieldNr));


        QList<QTreeWidgetItem *> items;
        for (int i = 0; i < count; i++)
        {
            QStringList list(QString::number(i));
            QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)nullptr, list);
            QVariant v(i);
            item->setData(0, Qt::UserRole, v);
            items.append(item);
        }
        treeWidget->clear();
        treeWidget->insertTopLevelItems(0, items);

    }
    connect(treeWidget, &QTreeWidget::itemClicked, [=](QTreeWidgetItem *item, int i){ this->onSelectShape(item, i); });

    exec();
}

ShowProperties::~ShowProperties()
{
    close();
}

void ShowProperties::onSelectShape(QTreeWidgetItem *item, int)
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


