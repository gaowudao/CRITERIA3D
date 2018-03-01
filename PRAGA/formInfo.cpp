#include "formInfo.h"
#include "ui_formInfo.h"

formInfo::formInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formInfo)
{
    ui->setupUi(this);
}

formInfo::~formInfo()
{
    delete ui;
}

int formInfo::start(QString info, int nrValues)
{
    this->ui->label->setText(info);

    if (nrValues <= 0)
        this->ui->progressBar->setVisible(false);
    else
    {
        this->ui->progressBar->setMaximum(nrValues);
        this->ui->progressBar->setValue(0);
        this->ui->progressBar->setVisible(true);
    }

    this->show();
    qApp->processEvents();

    return std::max(1, int(nrValues / 100));
}


void formInfo::setValue(int myValue)
{
    this->ui->progressBar->setValue(myValue);
    qApp->processEvents();
}


void formInfo::setText(QString myText)
{
    this->ui->label->setText(myText);
    qApp->processEvents();
}
