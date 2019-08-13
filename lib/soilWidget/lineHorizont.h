#ifndef LINEHORIZONT_H
#define LINEHORIZONT_H

    #include <QWidget>
    #include <QPainter>

    class LineHorizont : public QWidget
    {
        Q_OBJECT
    public:
        LineHorizont();
        void setClass(int classUSDA);
        int getIndex() const;
        void setIndex(int value);
        bool getSelected() const;
        void restorePalette();
        void setSelected(bool value);
        void setSelectedPalette();

    protected:
        void mousePressEvent(QMouseEvent* event);

    private:
        int index;
        int classUSDA;
        bool selected;
        QPalette linePalette;

    signals:
           void clicked(int index);

    };

#endif // LINEHORIZONT_H
