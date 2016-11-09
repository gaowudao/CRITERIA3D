#ifndef FORMSINGLEVALUE_H
#define FORMSINGLEVALUE_H

    #include <QDialog>

    namespace Ui {
    class formSingleValue;
    }

    class formSingleValue : public QDialog
    {
        Q_OBJECT

    public:
        explicit formSingleValue(QWidget *parent = 0);
        ~formSingleValue();
        int getValue();

    private slots:
        void on_buttonBox_rejected();
        void on_buttonBox_accepted();

    private:
        Ui::formSingleValue *ui;
        int myValue;
    };

#endif // FORMSINGLEVALUE_H
