#include "variableProperties.h"
#include "commonConstants.h"
#include "utilities.h"

bool loadVariableProperties(QString *myError, QSqlDatabase db, std::map<int, meteoVariable> mapIdMeteoVar)
{
    QSqlQuery qry(db);

    QString tableName = "variable_properties";
    int id_variable;
    QString variable;
    std::string stdVar;
    meteoVariable meteoVar;
    std::pair<std::map<int, meteoVariable>::iterator,bool> ret;

    QString statement = QString( "SELECT * FROM `%1` ").arg(tableName);
    if( !qry.exec(statement) )
    {
        *myError = qry.lastError().text();
        return false;
    }
    else
    {
        while (qry.next())
        {
            getValue(qry.value("id_variable"), &id_variable);
            getValue(qry.value("variable"), &variable);
            stdVar = variable.toStdString();
            try {
              meteoVar = MapDailyMeteoVar.at(stdVar);
            }
            catch (const std::out_of_range& ) {
                try {
                    meteoVar = MapHourlyMeteoVar.at(stdVar);
                }
                catch (const std::out_of_range& ) {
                    meteoVar = noMeteoVar;
                }
            }
            if (meteoVar != noMeteoVar)
            {
                ret = mapIdMeteoVar.insert(std::pair<int, meteoVariable>(id_variable,meteoVar));
                if (ret.second==false)
                {
                    *myError = "element 'z' already existed";
                }
            }
        }
    }
    return true;
}

int getIdfromMeteoVar(meteoVariable meteoVar, std::map<int, meteoVariable> mapIdMeteoVar)
{

    std::map<int, meteoVariable>::const_iterator it;
    int key = NODATA;

    for (it = mapIdMeteoVar.begin(); it != mapIdMeteoVar.end(); ++it)
    {
        if (it->second == meteoVar)
        {
            key = it->first;
            break;
        }
    }
    return key;
}
