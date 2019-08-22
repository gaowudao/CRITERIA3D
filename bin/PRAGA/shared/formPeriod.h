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
        formPeriod(QWidget *parent = nullptr);
        formPeriod(QDateTime timeIni, QDateTime timeFin, QWidget * parent = nullptr);
        ~formPeriod();


        QDateTime getDateTimeFirst() const;
        void setDateTimeFirst(const QDateTime &value);
        QDateTime getDateTimeLast() const;
        void setDateTimeLast(const QDateTime &value);

    private slots:
        void on_buttonBox_accepted();

    private:
        Ui::formPeriod *ui;

        QDateTime dateTimeFirst;
        QDateTime dateTimeLast;
    };

#endif // FORMDATE_H
