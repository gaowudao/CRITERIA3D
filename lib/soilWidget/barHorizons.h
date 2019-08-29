#ifndef BARHORIZON_H
#define BARHORIZON_H

    #include <QWidget>
    #include <QFrame>
    #include <QPainter>
    #include <QLabel>
    #include <QGroupBox>
    #include <QLayout>
    #include "soil.h"

    class BarHorizons : public QFrame
    {
        Q_OBJECT
    public:
        BarHorizons(QWidget *parent = nullptr);
        void setClass(int classUSDA);
        int getIndex() const;
        void setIndex(int value);
        bool getSelected() const;
        void restoreFrame();
        void setSelected(bool value);
        void setSelectedFrame();

    protected:
        void mousePressEvent(QMouseEvent* event);

    private:
        QLabel *labelNumber;
        int index;
        int classUSDA;
        bool selected;

    signals:
           void clicked(int index);

    };


    class BarHorizonsList
    {
    public:
        QGroupBox* groupBox;
        QList<BarHorizons*> list;
        QVBoxLayout* layout;

        BarHorizonsList();

        void clear();
        void selectItem(int index);
        void draw(int totHeight, soil::Crit3DSoil *soil);
    };


#endif // BARHORIZON_H
