#ifndef VARIABLESLIST_H
#define VARIABLESLIST_H

#include <QList>

class VariablesList
{

public:
    explicit VariablesList(int id, int arkId, QString varName, int frequency);

    int id() const;
    int arkId() const;
    QString varName() const;
    int frequency() const;

private:
    int _id;
    int _arkId;
    QString _varName;
    int _frequency;
};

#endif // VARIABLESLIST_H
