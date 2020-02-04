#ifndef UNITDB_H
#define UNITDB_H

#ifndef QOBJECT_H
    #include <QObject>
#endif

#include <QString>
#include <QtSql>

class UcmDb : public QObject
{
    Q_OBJECT

public:
    UcmDb(QString dbname);
    ~UcmDb();
    void createUnitsTable();
    bool writeUnitsTable(QString idCase, QString idCrop, QString idMeteo, float idSoil);
    bool writeListToUnitsTable(QStringList idCase, QStringList idCrop, QStringList idMeteo, QList<float> idSoil);
    QString getError() const;

private:
    QString error;
    QSqlDatabase db;
};

#endif // UNITDB_H
