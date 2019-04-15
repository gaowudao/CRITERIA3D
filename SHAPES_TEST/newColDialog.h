#ifndef NEWCOLDIALOG_H
#define NEWCOLDIALOG_H

#include <QtWidgets>

class NewColDialog : public QDialog
{
    Q_OBJECT

    private:
        QLineEdit* name;
        QRadioButton *stringButton;
        QRadioButton *intButton;
        QRadioButton *doubleButton;
        QLineEdit* nWidth;
        QLineEdit* nDecimals;

    public:
        NewColDialog();
        void insertCol();
        void showDecimalEdit();
        void hideDecimalEdit();
};

#endif // NEWCOLDIALOG_H
