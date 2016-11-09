#include "formSingleValue.h"
#include "ui_formSingleValue.h"
#include "commonConstants.h"

formSingleValue::formSingleValue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formSingleValue)
{
    ui->setupUi(this);
    myValue = NODATA;
}

formSingleValue::~formSingleValue()
{
    delete ui;
}

void formSingleValue::on_buttonBox_rejected()
{
    myValue = NODATA;
}

void formSingleValue::on_buttonBox_accepted()
{
    bool isOk;
    myValue = ui->textEditValue->toPlainText().toInt(&isOk, 10);
    if (! isOk) myValue = NODATA;
}

int formSingleValue::getValue()
{
    return(myValue);
}

