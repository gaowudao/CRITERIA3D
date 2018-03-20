/********************************************************************************
** Form generated from reading UI file 'CompositeTileSourceConfigurationWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPOSITETILESOURCECONFIGURATIONWIDGET_H
#define UI_COMPOSITETILESOURCECONFIGURATIONWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CompositeTileSourceConfigurationWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QToolButton *addSourceButton;
    QToolButton *removeSourceButton;
    QToolButton *moveDownButton;
    QToolButton *moveUpButton;
    QSlider *opacitySlider;
    QSpacerItem *horizontalSpacer;
    QListView *listView;

    void setupUi(QWidget *CompositeTileSourceConfigurationWidget)
    {
        if (CompositeTileSourceConfigurationWidget->objectName().isEmpty())
            CompositeTileSourceConfigurationWidget->setObjectName(QStringLiteral("CompositeTileSourceConfigurationWidget"));
        CompositeTileSourceConfigurationWidget->resize(393, 343);
        verticalLayout = new QVBoxLayout(CompositeTileSourceConfigurationWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        addSourceButton = new QToolButton(CompositeTileSourceConfigurationWidget);
        addSourceButton->setObjectName(QStringLiteral("addSourceButton"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/images/edit_add.png"), QSize(), QIcon::Normal, QIcon::Off);
        addSourceButton->setIcon(icon);
        addSourceButton->setPopupMode(QToolButton::InstantPopup);

        horizontalLayout->addWidget(addSourceButton);

        removeSourceButton = new QToolButton(CompositeTileSourceConfigurationWidget);
        removeSourceButton->setObjectName(QStringLiteral("removeSourceButton"));
        removeSourceButton->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/images/editdelete.png"), QSize(), QIcon::Normal, QIcon::Off);
        removeSourceButton->setIcon(icon1);

        horizontalLayout->addWidget(removeSourceButton);

        moveDownButton = new QToolButton(CompositeTileSourceConfigurationWidget);
        moveDownButton->setObjectName(QStringLiteral("moveDownButton"));
        moveDownButton->setEnabled(false);
        moveDownButton->setArrowType(Qt::DownArrow);

        horizontalLayout->addWidget(moveDownButton);

        moveUpButton = new QToolButton(CompositeTileSourceConfigurationWidget);
        moveUpButton->setObjectName(QStringLiteral("moveUpButton"));
        moveUpButton->setEnabled(false);
        moveUpButton->setArrowType(Qt::UpArrow);

        horizontalLayout->addWidget(moveUpButton);

        opacitySlider = new QSlider(CompositeTileSourceConfigurationWidget);
        opacitySlider->setObjectName(QStringLiteral("opacitySlider"));
        opacitySlider->setEnabled(false);
        opacitySlider->setMaximum(100);
        opacitySlider->setTracking(false);
        opacitySlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(opacitySlider);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        listView = new QListView(CompositeTileSourceConfigurationWidget);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setFocusPolicy(Qt::ClickFocus);
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setDragDropMode(QAbstractItemView::InternalMove);
        listView->setUniformItemSizes(true);

        verticalLayout->addWidget(listView);


        retranslateUi(CompositeTileSourceConfigurationWidget);

        QMetaObject::connectSlotsByName(CompositeTileSourceConfigurationWidget);
    } // setupUi

    void retranslateUi(QWidget *CompositeTileSourceConfigurationWidget)
    {
        CompositeTileSourceConfigurationWidget->setWindowTitle(QApplication::translate("CompositeTileSourceConfigurationWidget", "Form", 0));
        addSourceButton->setText(QApplication::translate("CompositeTileSourceConfigurationWidget", "+", 0));
        removeSourceButton->setText(QApplication::translate("CompositeTileSourceConfigurationWidget", "X", 0));
        moveDownButton->setText(QApplication::translate("CompositeTileSourceConfigurationWidget", "D", 0));
        moveUpButton->setText(QApplication::translate("CompositeTileSourceConfigurationWidget", "U", 0));
    } // retranslateUi

};

namespace Ui {
    class CompositeTileSourceConfigurationWidget: public Ui_CompositeTileSourceConfigurationWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPOSITETILESOURCECONFIGURATIONWIDGET_H
