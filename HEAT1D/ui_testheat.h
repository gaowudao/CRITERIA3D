/********************************************************************************
** Form generated from reading UI file 'testheat.ui'
**
** Created: Mon 20. Feb 11:28:28 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTHEAT_H
#define UI_TESTHEAT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_testHeat
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *testHeat)
    {
        if (testHeat->objectName().isEmpty())
            testHeat->setObjectName(QString::fromUtf8("testHeat"));
        testHeat->resize(400, 300);
        menuBar = new QMenuBar(testHeat);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        testHeat->setMenuBar(menuBar);
        mainToolBar = new QToolBar(testHeat);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        testHeat->addToolBar(mainToolBar);
        centralWidget = new QWidget(testHeat);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        testHeat->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(testHeat);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        testHeat->setStatusBar(statusBar);

        retranslateUi(testHeat);

        QMetaObject::connectSlotsByName(testHeat);
    } // setupUi

    void retranslateUi(QMainWindow *testHeat)
    {
        testHeat->setWindowTitle(QApplication::translate("testHeat", "testHeat", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class testHeat: public Ui_testHeat {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTHEAT_H
