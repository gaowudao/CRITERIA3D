#ifndef SOILWIDGET_H
#define SOILWIDGET_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #include <QWidget>
    #include <QComboBox>

    class SoilWidget : public QWidget
    {
        Q_OBJECT

        public:
            SoilWidget();

        protected:
            void mouseReleaseEvent(QMouseEvent *ev);

        private:
            QComboBox* soilCombo;
            std::vector <soil::Crit3DSoil> soilList;
            soil::Crit3DSoilClass soilClassList[13];

            void on_actionLoadSoil();
            bool loadSoilData(QString dbName, QString *error);
    };

#endif // SOILWIDGET_H
