#ifndef FORMINFO_H
#define FORMINFO_H

#include <QDialog>

namespace Ui {
class formInfo;
}

class formInfo : public QDialog
{
    Q_OBJECT

public:
    explicit formInfo(QWidget *parent = 0);
    ~formInfo();

    int start(QString info, int nrValues);
    void setValue(int myValue);
    void setText(QString myText);

private:
    Ui::formInfo *ui;
};


#endif // FORMINFO_H
