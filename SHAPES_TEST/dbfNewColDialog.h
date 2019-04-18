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
        bool insertOK;


    public:

        enum eType {
            FTString = 0,
            FTInteger,
            FTDouble
        };

        NewColDialog();
        bool insertCol();
        void showDecimalEdit();
        void hideDecimalEdit();
        bool checkValidData();
        bool getInsertOK() const;
        QString getName();
        int getType();
        int getWidth();
        int getDecimals();
};

#endif // NEWCOLDIALOG_H
