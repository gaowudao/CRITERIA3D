#ifndef UNITDB_H
#define UNITDB_H

#ifndef QOBJECT_H
    #include <QObject>
#endif

#include <QString>
#include <QtSql>

class UnitDb : public QObject
{
    Q_OBJECT

public:
    UnitDb(QString dbname);
    ~UnitDb();
    void createUnitTable();

private:
    QString error;
    QSqlDatabase db;
};

#endif // UNITDB_H
