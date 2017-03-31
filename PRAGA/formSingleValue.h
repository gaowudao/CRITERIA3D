#ifndef FORMSINGLEVALUE_H
#define FORMSINGLEVALUE_H

    #include <QDialog>
    #include <QString>

    namespace Ui
    {
        class FormSingleValue;
    }

    class FormSingleValue : public QDialog
    {
        Q_OBJECT

    public:
        explicit FormSingleValue(QWidget *parent = 0);
        QString getValue();
        void setValue(QString myValue);

    private slots:
        void on_buttonBox_rejected();
        void on_buttonBox_accepted();

    private:
        Ui::FormSingleValue* ui;
        QString value;
    };

#endif // FORMSINGLEVALUE_H
