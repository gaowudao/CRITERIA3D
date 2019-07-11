#ifndef SOILWIDGET_H
#define SOILWIDGET_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif

    #include <QWidget>
    #include <QComboBox>

    class Crit3DSoilWidget : public QWidget
    {
        Q_OBJECT

        public:
            Crit3DSoilWidget();

        protected:
            void mouseReleaseEvent(QMouseEvent *ev);

        private:
            QComboBox* soilListCombo;

            soil::Crit3DSoil soil;
            soil::Crit3DSoilClass soilClassList[13];

            void on_actionOpenSoilDB();
    };

#endif // SOILWIDGET_H
