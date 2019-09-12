#ifndef DIALOGNEWSOIL_H
#define DIALOGNEWSOIL_H

#include <QtWidgets>


class DialogNewSoil : public QDialog
{
    Q_OBJECT

public:
    DialogNewSoil();
    int getIdSoilValue();
    QString getCodeSoilValue();
    QString getNameSoilValue();
    QString getInfoSoilValue();

private:
    QLineEdit *idSoilValue;
    QLineEdit *codeSoilValue;
    QLineEdit *nameSoilValue;
    QLineEdit *infoSoilValue;
};

#endif // DIALOGNEWSOIL_H
