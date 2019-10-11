#ifndef DIALOGXMLCOMPUTATION_H
#define DIALOGXMLCOMPUTATION_H

#include <QtWidgets>

class DialogXMLComputation : public QDialog
{
    Q_OBJECT
private:
    bool isAnomaly;
    QListWidget listXMLWidget;
    QStringList listXML;
public:
    DialogXMLComputation(bool isAnomaly, QStringList listXML);
    void elabClicked(QListWidgetItem* item);
};

#endif // DIALOGXMLCOMPUTATION_H
