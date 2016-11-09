/********************************************************************************
** Form generated from reading UI file 'formSingleValue.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMSINGLEVALUE_H
#define UI_FORMSINGLEVALUE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_formSingleValue
{
public:
    QDialogButtonBox *buttonBox;
    QPlainTextEdit *textEditValue;

    void setupUi(QDialog *formSingleValue)
    {
        if (formSingleValue->objectName().isEmpty())
            formSingleValue->setObjectName(QStringLiteral("formSingleValue"));
        formSingleValue->setWindowModality(Qt::WindowModal);
        formSingleValue->resize(214, 111);
        formSingleValue->setModal(true);
        buttonBox = new QDialogButtonBox(formSingleValue);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(20, 60, 181, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textEditValue = new QPlainTextEdit(formSingleValue);
        textEditValue->setObjectName(QStringLiteral("textEditValue"));
        textEditValue->setGeometry(QRect(40, 20, 131, 31));
        textEditValue->setInputMethodHints(Qt::ImhPreferNumbers);

        retranslateUi(formSingleValue);
        QObject::connect(buttonBox, SIGNAL(accepted()), formSingleValue, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), formSingleValue, SLOT(reject()));

        QMetaObject::connectSlotsByName(formSingleValue);
    } // setupUi

    void retranslateUi(QDialog *formSingleValue)
    {
        formSingleValue->setWindowTitle(QApplication::translate("formSingleValue", "Insert value", 0));
    } // retranslateUi

};

namespace Ui {
    class formSingleValue: public Ui_formSingleValue {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMSINGLEVALUE_H
