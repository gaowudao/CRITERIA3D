#include "showProperties.h"


ShowProperties::ShowProperties(Crit3DShapeHandler* shapeHandler, QString filename)
    :shapeHandler(shapeHandler)
{
    setFixedSize(600,600);
    setWindowTitle(filename);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    QVBoxLayout* sxLayout = new QVBoxLayout;
    QVBoxLayout* dxLayout = new QVBoxLayout;

    QLabel typeLabel;
    typeLabel.setText("Type");
    shapeType = new QLineEdit();

    QLabel countLabel;
    countLabel.setText("Nr. of shapes");
    shapeEntityCount = new QLineEdit();

    QLabel fieldsLabel;
    fieldsLabel.setText("Nr. of attributes");
    fieldsCount = new QLineEdit();

    QLabel infoLabel;
    infoLabel.setText("Data");
    shapeData = new QTextEdit();

    sxLayout->addWidget(&typeLabel);
    sxLayout->addWidget(shapeType);
    sxLayout->addWidget(&countLabel);
    sxLayout->addWidget(shapeEntityCount);
    sxLayout->addWidget(&fieldsLabel);
    sxLayout->addWidget(fieldsCount);
    sxLayout->addWidget(&infoLabel);
    sxLayout->addWidget(shapeData);

    treeWidget = new QTreeWidget();
    treeWidget->setColumnCount(1);
    QStringList headerLabels;
    headerLabels.push_back("Shape");
    treeWidget->setHeaderLabels(headerLabels);
    dxLayout->addWidget(treeWidget);

    mainLayout->addLayout(sxLayout);
    mainLayout->addLayout(dxLayout);
    this->setLayout(mainLayout);

    int count = shapeHandler->getShapeCount();
    shapeEntityCount->setText(QString::number(count));
    QString typeString = QString::fromStdString(shapeHandler->getTypeString());
    shapeType->setText(typeString);
    int fieldNr = shapeHandler->getFieldNumbers();
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
            shapeHandler->getShape(index, object);

            shapeData->clear();
            QString myStr;

            shapeData->append("Shape nr. " + QString::number(index));

            /* print list of attributes */
            shapeData->append("\nAttributes: ");
            for (int i = 0; i < shapeHandler->getFieldNumbers(); i++)
            {
                std::string nameField =  shapeHandler->getFieldName(i);
                int typeField = shapeHandler->getFieldType(i);
                if (typeField == 0)
                {
                    myStr = QString::fromStdString(shapeHandler->readStringAttribute(index,i));
                }
                else if (typeField == 1)
                {
                    myStr = QString::number(shapeHandler->readIntAttribute(index,i));
                }
                else if (typeField == 2)
                {
                    myStr = QString::number(shapeHandler->readDoubleAttribute(index,i));
                }
                else
                {
                    myStr = "invalid field type ";
                }
                shapeData->append(QString::fromStdString(nameField) + " = " + myStr);
            }

            /* print list of vertices */
            int32_t vertexCount = int(object.getVertexCount());
            shapeData->append("\nNr. of vertices: " + QString::number(vertexCount));
            const Point<double> *p_ptr = object.getVertices();
            const Point<double> *p_end = p_ptr + (vertexCount - 1);

            bool isClosed = (p_ptr->x == p_end->x && p_ptr->y == p_end->y);
            // DEBUG
            for (int i = 0; i<object.getParts().size(); i++ )
            {
                //qDebug() << "part size " << object.getParts().size();
                //qDebug() << "part offset" << object.getParts().at(i).offset;
                //qDebug() << "part length" << object.getParts().at(i).length;
                //qDebug() << "part type" << object.getParts().at(i).type;
                //qDebug() << "polygonArea " << object.polygonArea(i);  // (HOLE: COUNTERCLOCK)
                //qDebug() << "**********";
                //qDebug() << "i " << i;
                bool res = object.isClockWise(i);
                qDebug() << "res " << res ;  // (HOLE: COUNTERCLOCK)
                //qDebug() << "polygonArea " << object.polygonArea(i);  // (HOLE: COUNTERCLOCK)
                //qDebug() << "**********";
                Point<double> UTMpoint(610479, 4980160);
                int shape = object.pointInPolygon(UTMpoint);
                qDebug() << "shape " << shape ;
            }
            //
            while (p_ptr <= p_end)
            {
                shapeData->append("x = "+QString::number(p_ptr->x) + " y = " + QString::number(p_ptr->y));
                p_ptr++;
            }
            if (isClosed) shapeData->append("First vertex == last vertex");
        }
    }
}

