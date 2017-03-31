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
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_FormSingleValue
{
public:
    QDialogButtonBox *buttonBox;
    QPlainTextEdit *textEditValue;
    QLabel *label;

    void setupUi(QDialog *FormSingleValue)
    {
        if (FormSingleValue->objectName().isEmpty())
            FormSingleValue->setObjectName(QStringLiteral("FormSingleValue"));
        FormSingleValue->setWindowModality(Qt::WindowModal);
        FormSingleValue->resize(250, 130);
        FormSingleValue->setModal(true);
        buttonBox = new QDialogButtonBox(FormSingleValue);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(70, 80, 161, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textEditValue = new QPlainTextEdit(FormSingleValue);
        textEditValue->setObjectName(QStringLiteral("textEditValue"));
        textEditValue->setGeometry(QRect(20, 40, 211, 31));
        textEditValue->setInputMethodHints(Qt::ImhPreferNumbers);
        label = new QLabel(FormSingleValue);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 71, 16));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(10);
        label->setFont(font);

        retranslateUi(FormSingleValue);
        QObject::connect(buttonBox, SIGNAL(accepted()), FormSingleValue, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FormSingleValue, SLOT(reject()));

        QMetaObject::connectSlotsByName(FormSingleValue);
    } // setupUi

    void retranslateUi(QDialog *FormSingleValue)
    {
        FormSingleValue->setWindowTitle(QApplication::translate("FormSingleValue", "Insert value", 0));
        label->setText(QApplication::translate("FormSingleValue", "Value:", 0));
    } // retranslateUi

};

namespace Ui {
    class FormSingleValue: public Ui_FormSingleValue {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMSINGLEVALUE_H
