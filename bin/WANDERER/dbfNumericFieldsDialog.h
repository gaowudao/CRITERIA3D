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
            DbfNumericFieldsDialog(Crit3DShapeHandler *shapeHandler, QString fileName, bool isRasterize);
            ~DbfNumericFieldsDialog();
            void acceptRasterize();
            void acceptSelection();
            QString getOutputName();
            QString getFieldSelected();
            double getCellSize() const;
    };

#endif // DBFNUMERICFIELDSDIALOG_H
