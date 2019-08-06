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
            soil::Crit3DSoil SoilFromDb;
            soil::Crit3DTextureClass textureClassList[13];
            int dbSoilType;

            QGroupBox *infoGroup;
            QString soilName;
            QLineEdit* soilCodeValue;
            QLineEdit* satValue;
            QLineEdit* fcValue;
            QLineEdit* wpValue;
            QLineEdit* awValue;
            QLineEdit* potFCValue;
            QAction* restoreData;
            QAction* addHorizon;
            QAction* deleteHorizon;
            QAction* useData;
            QAction* airEntry;

            QPixmap pic;
            QString picPath;
            QPainter painter;
            QLabel *labelPic;

            // USDA textural triangle size inside picture pic
            constexpr static const double widthTriangle = 271.0;
            constexpr static const double heightTriangle = 236.0;

            void on_actionOpenSoilDB();
            void on_actionSave();
            void on_actionNewSoil();
            void on_actionDeleteSoil();
            void on_actionUseData();
            void on_actionAirEntry();
            void on_actionChooseSoil(QString);
            void on_actionAddHorizon();
            void on_actionDeleteHorizon();
            void on_actionRestoreData();
            void tabChanged(int index);

            private slots:
            void setInfoTextural(int);
    };

#endif // SOILWIDGET_H
