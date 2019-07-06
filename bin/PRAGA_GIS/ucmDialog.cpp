#include "ucmDialog.h"

QString UcmDialog::getIdCrop() const
{
    return idCrop;
}

QString UcmDialog::getIdSoil() const
{
    return idSoil;
}

QString UcmDialog::getIdMeteo() const
{
    return idMeteo;
}

Crit3DShapeHandler *UcmDialog::getCrop() const
{
    return crop;
}

Crit3DShapeHandler *UcmDialog::getSoil() const
{
    return soil;
}

Crit3DShapeHandler *UcmDialog::getMeteo() const
{
    return meteo;
}

QString UcmDialog::getOutputName() const
{
    return outputName->text();
}

double UcmDialog::getCellSize() const
{
    QString cellString = cellSize->text();
    cellString.replace(",", ".");
    return cellString.toDouble();
}

UcmDialog::UcmDialog(std::vector<MapGraphicsShapeObject *> shapeObjList)
    :shapeObjList(shapeObjList)
{

    this->setWindowTitle("Unit Crop Map");
    this->setFixedSize(800,600);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* boxLayout = new QHBoxLayout;
    QVBoxLayout* shapeLayout = new QVBoxLayout;
    QVBoxLayout* fieldLayout = new QVBoxLayout;

    cropShape = new QListWidget();
    soilShape = new QListWidget();
    meteoShape = new QListWidget();

    QLabel *cropLabel = new QLabel();
    cropLabel->setText("Select shape CROP:");
    QLabel *soilLabel = new QLabel();
    soilLabel->setText("Select shape SOIL:");
    QLabel *meteoLabel = new QLabel();
    meteoLabel->setText("Select shape METEO:");
    shapeLayout->addWidget(cropLabel);
    shapeLayout->addWidget(cropShape);
    shapeLayout->addWidget(soilLabel);
    shapeLayout->addWidget(soilShape);
    shapeLayout->addWidget(meteoLabel);
    shapeLayout->addWidget(meteoShape);


    QStringList shapeLabel;

    for (int i = 0; i < shapeObjList.size(); i++)
    {
        QString filepath = QString::fromStdString(shapeObjList.at(i)->getShapePointer()->getFilepath());
        QFileInfo file(filepath);
        shapeLabel << file.baseName();
    }
    cropShape->addItems(shapeLabel);
    soilShape->addItems(shapeLabel);
    meteoShape->addItems(shapeLabel);

    cropField = new QListWidget();
    soilField = new QListWidget();
    meteoField = new QListWidget();

    QLabel *cropFieldLabel = new QLabel();
    cropFieldLabel->setText("Select ID CROP:");
    QLabel *soilFieldLabel = new QLabel();
    soilFieldLabel->setText("Select ID SOIL:");
    QLabel *meteoFieldLabel = new QLabel();
    meteoFieldLabel->setText("Select ID METEO:");

    fieldLayout->addWidget(cropFieldLabel);
    fieldLayout->addWidget(cropField);
    fieldLayout->addWidget(soilFieldLabel);
    fieldLayout->addWidget(soilField);
    fieldLayout->addWidget(meteoFieldLabel);
    fieldLayout->addWidget(meteoField);


    crop = shapeObjList.at(0)->getShapePointer();
    QStringList fieldLabel;
    for (int i = 0; i < crop->getFieldNumbers(); i++)
    {
        std::string nameField =  crop->getFieldName(i);
        fieldLabel << QString::fromStdString(nameField);
    }
    cropField->addItems(fieldLabel);
    soilField->addItems(fieldLabel);
    meteoField->addItems(fieldLabel);

    boxLayout->addLayout(shapeLayout);
    boxLayout->addLayout(fieldLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);

    connect(cropShape, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->shapeCropClicked(item); });
    connect(soilShape, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->shapeSoilClicked(item); });
    connect(meteoShape, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->shapeMeteoClicked(item); });
    connect(buttonBox, &QDialogButtonBox::accepted, [=](){ this->ucm(); });
    connect(buttonBox, &QDialogButtonBox::rejected, [=](){ QDialog::done(QDialog::Rejected); });


    mainLayout->addLayout(boxLayout);
    cellSize = new QLineEdit();
    cellSize->setPlaceholderText("cell size [m]");
    cellSize->setValidator(new QDoubleValidator(0, 9999.0, 2)); //LC accetta double con 2 cifre decimali da 0 a 9999
    outputName = new QLineEdit();
    outputName->setPlaceholderText("Output Name");
    mainLayout->addWidget(cellSize);
    mainLayout->addWidget(outputName);

    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    exec();
}


UcmDialog::~UcmDialog()
{

}

void UcmDialog::shapeCropClicked(QListWidgetItem* item)
{
    cropField->clear();
    int pos = cropShape->row(item);
    crop = shapeObjList.at(unsigned(pos))->getShapePointer();

    QStringList fieldLabel;
    for (int i = 0; i < crop->getFieldNumbers(); i++)
    {
        std::string nameField =  crop->getFieldName(i);
        fieldLabel << QString::fromStdString(nameField);
    }
    cropField->addItems(fieldLabel);
}

void UcmDialog::shapeSoilClicked(QListWidgetItem* item)
{
    soilField->clear();
    int pos = soilShape->row(item);
    soil = shapeObjList.at(unsigned(pos))->getShapePointer();

    QStringList fieldLabel;
    for (int i = 0; i < soil->getFieldNumbers(); i++)
    {
        std::string nameField =  soil->getFieldName(i);
        fieldLabel << QString::fromStdString(nameField);
    }
    soilField->addItems(fieldLabel);
}

void UcmDialog::shapeMeteoClicked(QListWidgetItem* item)
{
    meteoField->clear();
    int pos = meteoShape->row(item);
    meteo = shapeObjList.at(unsigned(pos))->getShapePointer();

    QStringList fieldLabel;
    for (int i = 0; i < meteo->getFieldNumbers(); i++)
    {
        std::string nameField =  meteo->getFieldName(i);
        fieldLabel << QString::fromStdString(nameField);
    }
    meteoField->addItems(fieldLabel);
}


void UcmDialog::ucm()
{
    int pos;
    QListWidgetItem* itemSelected;

    // check shape selection
    itemSelected = cropShape->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No crop shape selected", "Select a shape");
        return;
    }
    pos = cropShape->row(itemSelected);
    crop = shapeObjList.at(unsigned(pos))->getShapePointer();

    itemSelected = soilShape->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No soil shape selected", "Select a shape");
        return;
    }
    pos = soilShape->row(itemSelected);
    soil = shapeObjList.at(unsigned(pos))->getShapePointer();

    itemSelected = meteoShape->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No meteo shape selected", "Select a shape");
        return;
    }
    pos = meteoShape->row(itemSelected);
    meteo = shapeObjList.at(unsigned(pos))->getShapePointer();

    // check field selection

    itemSelected = cropField->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No id crop selected", "Select a field");
        return;
    }
    idCrop = itemSelected->text();

    itemSelected = soilField->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No id soil selected", "Select a field");
        return;
    }
    idSoil = itemSelected->text();

    itemSelected = meteoField->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No id meteo selected", "Select a field");
        return;
    }
    idMeteo = itemSelected->text();

    // check cell Size
    if (cellSize->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Empty cellSize", "Insert cell size");
        return;
    }

    // check output name
    if (outputName->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Empty name", "Insert output name");
        return;
    }
    QDialog::done(QDialog::Accepted);
}
