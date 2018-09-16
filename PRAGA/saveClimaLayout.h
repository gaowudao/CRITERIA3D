#ifndef SAVECLIMALAYOUT_H
#define SAVECLIMALAYOUT_H

#include <QString>
#include <QGridLayout>
#include <QtWidgets>



class SaveClimaLayout : public QDialog
{

    Q_OBJECT

    public:
        SaveClimaLayout();

        QString getFirstYear() const;
        void setFirstYear(const QString &value);
        QString getLastYear() const;
        void setLastYear(const QString &value);
        QString getVariable() const;
        void setVariable(const QString &value);
        QString getPeriod() const;
        void setPeriod(const QString &value);
        QString getGenericPeriodEnd() const;
        void setGenericPeriodEnd(const QString &value);
        QString getGenericNYear() const;
        void setGenericNYear(const QString &value);
        QString getSecondElab() const;
        void setSecondElab(const QString &value);
        QString getElab2Param() const;
        void setElab2Param(const QString &value);
        QString getElab() const;
        void setElab(const QString &value);
        QString getElab1Param() const;
        void setElab1Param(const QString &value);
        QString getGenericPeriodStartDay() const;
        void setGenericPeriodStartDay(const QString &value);
        QString getGenericPeriodStartMonth() const;
        void setGenericPeriodStartMonth(const QString &value);
        QString getGenericPeriodEndDay() const;
        void setGenericPeriodEndDay(const QString &value);
        QString getGenericPeriodEndMonth() const;
        void setGenericPeriodEndMonth(const QString &value);

        void addElab();
        void deleteRaw();
        void deleteAll();

        //bool eventFilter(QObject *obj, QEvent *event);
        void saveElabList();
        void loadElabList();

private:

        QVBoxLayout mainLayout;
        QVBoxLayout listLayout;
        QHBoxLayout saveButtonLayout;

        QListView listView;
        QStringListModel model;
        QStringList list;
        QModelIndex index;

        QPushButton saveList;
        QPushButton loadList;

        QString firstYear;
        QString lastYear;
        QString variable;
        QString period;

        QString genericPeriodStartDay;
        QString genericPeriodStartMonth;
        QString genericPeriodEndDay;
        QString genericPeriodEndMonth;
        QString genericNYear;

        QString secondElab;
        QString elab2Param;

        QString elab;
        QString elab1Param;


};


#endif // SAVECLIMALAYOUT_H
