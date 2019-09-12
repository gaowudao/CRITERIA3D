#ifndef DIALOGNEWSOIL_H
#define DIALOGNEWSOIL_H

#include <QtWidgets>


class DialogNewSoil : public QDialog
{
    Q_OBJECT

public:
    DialogNewSoil();
private:
    QLineEdit *idSoilValue;
    QLineEdit *codeSoilValue;
    QLineEdit *nameSoilValue;
};

#endif // DIALOGNEWSOIL_H
