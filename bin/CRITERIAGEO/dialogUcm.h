#ifndef DIALOGUCM_H
#define DIALOGUCM_H

    #include <QtWidgets>
    #ifndef SHAPEHANDLER_H
        #include "shapeHandler.h"
    #endif

    class DialogUCM : public QDialog
    {
        Q_OBJECT

        private:
            std::vector<Crit3DShapeHandler*> shapeObjList;
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
            DialogUCM(std::vector<Crit3DShapeHandler*> shapeObjList);

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
