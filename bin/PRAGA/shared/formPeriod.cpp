#include "formPeriod.h"
#include "ui_formPeriod.h"

formPeriod::formPeriod(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formPeriod)
{
    ui->setupUi(this);
}

formPeriod::formPeriod(QDateTime timeIni, QDateTime timeFin, QWidget * parent) :
    QDialog(parent),
    ui(new Ui::formPeriod)
{
    ui->setupUi(this);
    ui->dateTimeEditFirst->setDateTime(timeIni);
    ui->dateTimeEditLast->setDateTime(timeFin);
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

QDateTime formPeriod::getDateTimeLast() const
{
    return dateTimeLast;
}

void formPeriod::setDateTimeLast(const QDateTime &value)
{
    dateTimeLast = value;
}

QDateTime formPeriod::getDateTimeFirst() const
{
    return dateTimeFirst;
}

void formPeriod::setDateTimeFirst(const QDateTime &value)
{
    dateTimeFirst = value;
}
