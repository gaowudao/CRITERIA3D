#ifndef FORMDATE_H
#define FORMDATE_H

    #include <QDialog>
    #include <QDateTime>

    namespace Ui {
        class formDate;
    }

    class formDate : public QDialog
    {
        Q_OBJECT

    public:
        explicit formDate(QWidget *parent = 0);
        ~formDate();
        QDateTime dateTimeFirst;
        QDateTime dateTimeLast;

    private slots:
        void on_buttonBox_accepted();

    private:
        Ui::formDate *ui;
    };

#endif // FORMDATE_H
