#ifndef SHOWPROPERTIES_H
#define SHOWPROPERTIES_H

    #include <QtWidgets>
    #include "shapeHandler.h"
    #include "dbfTableDialog.h"

    class ShowProperties : public QDialog
    {
        Q_OBJECT

    public:
        explicit ShowProperties(Crit3DShapeHandler* shapeHandler, QString filename);
        ~ShowProperties();

    private:
        Crit3DShapeHandler* shapeHandler;
        QLineEdit* shapeType;
        QLineEdit* shapeEntityCount;
        QLineEdit* fieldsCount;
        QTextEdit* shapeData;
        QTreeWidget* treeWidget;

    private slots:
        void onSelectShape(QTreeWidgetItem *item, int column);
    };

#endif // SHOWPROPERTIES_H
