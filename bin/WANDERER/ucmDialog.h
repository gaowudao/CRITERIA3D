#ifndef UCMDIALOG_H
#define UCMDIALOG_H

    #include <QtWidgets>
    #include "shapeHandler.h"
    #include "mapGraphicsShapeObject.h"

    class UcmDialog : public QDialog
    {
        Q_OBJECT

        private:
            std::vector<MapGraphicsShapeObject *> shapeObjList;
            QListWidget* cropShape;
            QListWidget* soilShape;
            QListWidget* meteoShape;
            QListWidget* cropField;
            QListWidget* soilField;
            QListWidget* meteoField;
            QLineEdit* outputName;
            QLineEdit* cellSize;
            Crit3DShapeHandler *crop;
            Crit3DShapeHandler *soil;
            Crit3DShapeHandler *meteo;
            QString idCrop;
            QString idSoil;
            QString idMeteo;
        public:
            UcmDialog(std::vector<MapGraphicsShapeObject *> shapeObjList);
            ~UcmDialog();
            void shapeCropClicked(QListWidgetItem* item);
            void shapeSoilClicked(QListWidgetItem* item);
            void shapeMeteoClicked(QListWidgetItem* item);
            void ucm();
            QString getIdCrop() const;
            QString getIdSoil() const;
            QString getIdMeteo() const;
            Crit3DShapeHandler *getCrop() const;
            Crit3DShapeHandler *getSoil() const;
            Crit3DShapeHandler *getMeteo() const;
            QString getOutputName() const;
            double getCellSize() const;
    };

#endif // UCMDIALOG_H
