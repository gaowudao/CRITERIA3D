#ifndef DIALOGUCM_H
#define DIALOGUCM_H

    #include <QtWidgets>
    #include "shapeHandler.h"
    #include "mapGraphicsShapeObject.h"

    class DialogUCM : public QDialog
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
            DialogUCM(std::vector<MapGraphicsShapeObject *> shapeObjList);
            ~DialogUCM();

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


#endif // DIALOGUCM_H
