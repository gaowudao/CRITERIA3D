/********************************************************************************
** Form generated from reading UI file 'formPeriod.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMPERIOD_H
#define UI_FORMPERIOD_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_formPeriod
{
public:
    QDialogButtonBox *buttonBox;
    QDateTimeEdit *dateTimeEditFirst;
    QDateTimeEdit *dateTimeEditLast;

    void setupUi(QDialog *formPeriod)
    {
        if (formPeriod->objectName().isEmpty())
            formPeriod->setObjectName(QString::fromUtf8("formPeriod"));
        formPeriod->resize(198, 126);
        buttonBox = new QDialogButtonBox(formPeriod);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 80, 171, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        dateTimeEditFirst = new QDateTimeEdit(formPeriod);
        dateTimeEditFirst->setObjectName(QString::fromUtf8("dateTimeEditFirst"));
        dateTimeEditFirst->setGeometry(QRect(20, 10, 161, 22));
        dateTimeEditFirst->setDateTime(QDateTime(QDate(2014, 1, 1), QTime(1, 0, 0)));
        dateTimeEditLast = new QDateTimeEdit(formPeriod);
        dateTimeEditLast->setObjectName(QString::fromUtf8("dateTimeEditLast"));
        dateTimeEditLast->setGeometry(QRect(20, 40, 161, 22));
        dateTimeEditLast->setDateTime(QDateTime(QDate(2014, 1, 2), QTime(0, 0, 0)));
        dateTimeEditLast->setDate(QDate(2014, 1, 2));

        retranslateUi(formPeriod);
        QObject::connect(buttonBox, SIGNAL(accepted()), formPeriod, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), formPeriod, SLOT(reject()));

        QMetaObject::connectSlotsByName(formPeriod);
    } // setupUi

    void retranslateUi(QDialog *formPeriod)
    {
        formPeriod->setWindowTitle(QCoreApplication::translate("formPeriod", "Dialog", nullptr));
        dateTimeEditFirst->setDisplayFormat(QCoreApplication::translate("formPeriod", "dd/MM/yyyy HH.mm", nullptr));
        dateTimeEditLast->setDisplayFormat(QCoreApplication::translate("formPeriod", "dd/MM/yyyy HH.mm", nullptr));
    } // retranslateUi

};

namespace Ui {
    class formPeriod: public Ui_formPeriod {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMPERIOD_H
