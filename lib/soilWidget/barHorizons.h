#ifndef BARHORIZON_H
#define BARHORIZON_H

    #include <QWidget>
    #include <QFrame>
    #include <QPainter>

    //class BarHorizons : public QWidget
    class BarHorizons : public QFrame
    {
        Q_OBJECT
    public:
        //BarHorizons();
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
        int index;
        int classUSDA;
        bool selected;

    signals:
           void clicked(int index);

    };

#endif // BARHORIZON_H
