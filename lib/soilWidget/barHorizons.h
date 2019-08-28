#ifndef BARHORIZON_H
#define BARHORIZON_H

    #include <QWidget>
    #include <QFrame>
    #include <QPainter>
    #include <QLabel>

    //class BarHorizons : public QWidget
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

#endif // BARHORIZON_H
