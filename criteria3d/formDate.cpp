#include "formDate.h"
#include "ui_formDate.h"

formDate::formDate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formDate)
{
    ui->setupUi(this);
}

formDate::~formDate()
{
    delete ui;
}

void formDate::on_buttonBox_accepted()
{
    dateTimeFirst = ui->dateTimeEditFirst->dateTime();
    dateTimeLast = ui->dateTimeEditLast->dateTime();
}
