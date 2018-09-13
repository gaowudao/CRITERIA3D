#ifndef SAVECLIMALAYOUT_H
#define SAVECLIMALAYOUT_H

#include <QString>
#include <QGridLayout>
#include <QtWidgets>


class SaveClimaLayout : public QDialog
{

    Q_OBJECT

    private:

        QVBoxLayout mainLayout;
        QHBoxLayout buttonLayout;
        QVBoxLayout listLayout;
        QHBoxLayout saveButtonLayout;

        QListView listview;
        QPushButton add;
        QPushButton del;
        QPushButton delAll;

        QPushButton saveList;
        QPushButton loadList;



    public:
        SaveClimaLayout();

};


#endif // SAVECLIMALAYOUT_H
