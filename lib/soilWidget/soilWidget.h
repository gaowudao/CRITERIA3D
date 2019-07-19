#ifndef SOILWIDGET_H
#define SOILWIDGET_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef TABHORIZONS_H
        #include "tabHorizons.h"
    #endif
    #ifndef TABHYDRAULICCONDUCTIVITYCURVE_H
        #include "tabHydraulicConductivityCurve.h"
    #endif
    #ifndef TABWATERRETENTIONCURVE_H
        #include "tabWaterRetentionCurve.h"
    #endif
    #ifndef TABWATERRETENTIONDATA_H
        #include "tabWaterRetentionData.h"
    #endif


    #include <QWidget>
    #include <QComboBox>
    #include <QTextEdit>
    #include <QSqlDatabase>


    class Crit3DSoilWidget : public QWidget
    {
        Q_OBJECT

        public:
            Crit3DSoilWidget();

        protected:
            void mouseReleaseEvent(QMouseEvent *ev);

        private:
            QComboBox soilListComboBox;
            QTabWidget* tabWidget;
            TabHorizons* horizonsTab;
            TabWaterRetentionData* wrDataTab;
            TabWaterRetentionCurve* wrCurveTab;
            TabHydraulicConductivityCurve* hydraConducCurveTab;

            QSqlDatabase dbSoil;
            soil::Crit3DSoil mySoil;
            soil::Crit3DTextureClass textureClassList[13];
            int dbSoilType;

            QAction* useData;
            QAction* airEntry;

            void on_actionOpenSoilDB();
            void on_actionSave();
            void on_actionNewSoil();
            void on_actionDeleteSoil();
            void on_actionUseData();
            void on_actionAirEntry();
            void on_actionChooseSoil(QString);
    };

#endif // SOILWIDGET_H
