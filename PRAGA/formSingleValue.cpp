#include "formSingleValue.h"
#include "ui_formSingleValue.h"
#include "commonConstants.h"

FormSingleValue::FormSingleValue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormSingleValue)
{
    ui->setupUi(this);
    this->value = "";
}

void FormSingleValue::on_buttonBox_rejected()
{
    this->value = "";
}

void FormSingleValue::on_buttonBox_accepted()
{
    this->value = this->ui->textEditValue->toPlainText();
}

void FormSingleValue::setValue(QString myValue)
{
    this->ui->textEditValue->setPlainText(myValue);
}

QString FormSingleValue::getValue()
{
    return(this->value);
}

