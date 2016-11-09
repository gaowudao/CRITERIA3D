/********************************************************************************
** Form generated from reading UI file 'formDate.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMDATE_H
#define UI_FORMDATE_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_formDate
{
public:
    QDialogButtonBox *buttonBox;
    QDateTimeEdit *dateTimeEditFirst;
    QDateTimeEdit *dateTimeEditLast;

    void setupUi(QDialog *formDate)
    {
        if (formDate->objectName().isEmpty())
            formDate->setObjectName(QStringLiteral("formDate"));
        formDate->resize(198, 126);
        buttonBox = new QDialogButtonBox(formDate);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(10, 80, 171, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        dateTimeEditFirst = new QDateTimeEdit(formDate);
        dateTimeEditFirst->setObjectName(QStringLiteral("dateTimeEditFirst"));
        dateTimeEditFirst->setGeometry(QRect(20, 10, 161, 22));
        dateTimeEditFirst->setDateTime(QDateTime(QDate(2014, 1, 1), QTime(1, 0, 0)));
        dateTimeEditLast = new QDateTimeEdit(formDate);
        dateTimeEditLast->setObjectName(QStringLiteral("dateTimeEditLast"));
        dateTimeEditLast->setGeometry(QRect(20, 40, 161, 22));
        dateTimeEditLast->setDateTime(QDateTime(QDate(2014, 1, 2), QTime(0, 0, 0)));
        dateTimeEditLast->setDate(QDate(2014, 1, 2));

        retranslateUi(formDate);
        QObject::connect(buttonBox, SIGNAL(accepted()), formDate, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), formDate, SLOT(reject()));

        QMetaObject::connectSlotsByName(formDate);
    } // setupUi

    void retranslateUi(QDialog *formDate)
    {
        formDate->setWindowTitle(QApplication::translate("formDate", "Dialog", 0));
        dateTimeEditFirst->setDisplayFormat(QApplication::translate("formDate", "dd/MM/yyyy HH.mm", 0));
        dateTimeEditLast->setDisplayFormat(QApplication::translate("formDate", "dd/MM/yyyy HH.mm", 0));
    } // retranslateUi

};

namespace Ui {
    class formDate: public Ui_formDate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMDATE_H
