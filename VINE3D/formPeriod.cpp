#include "formPeriod.h"
#include "ui_formPeriod.h"

formPeriod::formPeriod(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formPeriod)
{
    ui->setupUi(this);
}

formPeriod::~formPeriod()
{
    delete ui;
}

void formPeriod::on_buttonBox_accepted()
{
    dateTimeFirst = ui->dateTimeEditFirst->dateTime();
    dateTimeLast = ui->dateTimeEditLast->dateTime();
}
