#include "formDownload.h"
#include "ui_formDownload.h"

FormDownload::FormDownload(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormDownload)
{
    ui->setupUi(this);
}

FormDownload::~FormDownload()
{
    delete ui;
}
