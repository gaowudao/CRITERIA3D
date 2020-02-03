#include "shapeUtilities.h"
#include "unitDb.h"
#include "formInfo.h"

bool extractUCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error, bool showInfo)
{
    FormInfo formInfo;

    UnitDb* unitList = new UnitDb(dbName);

    QStringList idCase;
    QStringList idCrop;
    QStringList idMeteo;
    QList<float> idSoil;

    int nShape = shapeHandler->getShapeCount();

    formInfo.start("Extract list...", nShape);

    for (int i = 0; i < nShape; i++)
    {
        formInfo.setValue(i);

        QString key = QString::fromStdString(shapeHandler->getStringValue(signed(i), "ID_CASE"));
        if (!key.isEmpty() && !idCase.contains(key))
        {
            idCase << key;
            idCrop << QString::fromStdString(shapeHandler->getStringValue(signed(i), "ID_CROP"));
            idMeteo << QString::fromStdString(shapeHandler->getStringValue(signed(i), "ID_METEO"));
            idSoil << shapeHandler->getNumericValue(signed(i), "ID_SOIL");
        }
    }
    bool res = unitList->writeListToUnitsTable(idCase, idCrop, idMeteo, idSoil);
    *error = unitList->getError().toStdString();

    formInfo.close();
    delete unitList;

    return res;

}

bool createShapeFromCSV()
{

}
