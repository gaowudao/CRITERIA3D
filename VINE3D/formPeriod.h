#ifndef FORMPERIOD_H
#define FORMPERIOD_H

    #include <QDialog>
    #include <QDateTime>

    namespace Ui {
        class formPeriod;
    }

    class formPeriod : public QDialog
    {
        Q_OBJECT

    public:
        explicit formPeriod(QWidget *parent = 0);
        ~formPeriod();
        QDateTime dateTimeFirst;
        QDateTime dateTimeLast;

    private slots:
        void on_buttonBox_accepted();

    private:
        Ui::formPeriod *ui;
    };

#endif // FORMDATE_H
