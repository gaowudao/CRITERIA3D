#ifndef SHOWPROPERTIES_H
#define SHOWPROPERTIES_H

#include <QtWidgets>
    #include "shapeHandler.h"
    #include "dbfTableDialog.h"

class ShowProperties : public QDialog
{   
    Q_OBJECT

protected:
    Crit3DShapeHandler shapeHandler;

public:
    explicit ShowProperties(QString filepath);
    ~ShowProperties();

private:
    DbfTableDialog *DBFWidget;
    QLineEdit* shapeEntityCount;
    QLineEdit* shapeType;
    QLineEdit* fieldsCount;
    QTreeWidget* treeWidget;
    QPushButton *dbfButton;
    QString filepath;

private slots:
    void onSelectShape(QTreeWidgetItem *item, int column);
    void on_dbfButton_clicked();
};

#endif // SHOWPROPERTIES_H
