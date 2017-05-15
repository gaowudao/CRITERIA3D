#ifndef FORMDOWNLOAD_H
#define FORMDOWNLOAD_H

#include <QDialog>

namespace Ui {
class FormDownload;
}

class FormDownload : public QDialog
{
    Q_OBJECT

public:
    explicit FormDownload(QWidget *parent = 0);
    ~FormDownload();

private:
    Ui::FormDownload *ui;
};

#endif // FORMDOWNLOAD_H
