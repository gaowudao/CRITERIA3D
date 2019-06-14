#ifndef DBFNUMERICFIELDSDIALOG_H
#define DBFNUMERICFIELDSDIALOG_H

    #include <QtWidgets>
    #include "shapeHandler.h"

    class DbfNumericFieldsDialog : public QDialog
    {
        Q_OBJECT

        private:
            Crit3DShapeHandler *shapeHandler;
            QListWidget* listFields;
            QLineEdit* outputName;
            QLineEdit* cellSize;

        public:
            DbfNumericFieldsDialog(Crit3DShapeHandler *shapeHandler, QString fileName);
            ~DbfNumericFieldsDialog();
            void fieldToRaster();
            QString getOutputName();
            QString getFieldSelected();
            double getCellSize() const;
    };

#endif // DBFNUMERICFIELDSDIALOG_H
