#include "dbMeteoGrid.h"
#include "utilities.h"
#include "commonConstants.h"
#include <stdexcept>
#include <qdebug.h> //debug
#include <iostream> //debug


Crit3DMeteoGridDbHandler::Crit3DMeteoGridDbHandler()
{
    _meteoGrid = new Crit3DMeteoGrid();
}

Crit3DMeteoGridDbHandler::~Crit3DMeteoGridDbHandler()
{
    free(_meteoGrid);
}

bool Crit3DMeteoGridDbHandler::parseXMLFile(QString xmlFileName, QDomDocument* xmlDoc)
{
    if (xmlFileName == "")
    {
        qDebug() << "Missing XML file.";
        return(false);
    }

    QFile myFile(xmlFileName);
    if (!myFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Open XML failed:" << xmlFileName;
        qDebug() << myFile.errorString();
        return (false);
    }

    QString myError;
    int myErrLine, myErrColumn;
    if (!xmlDoc->setContent(&myFile, &myError, &myErrLine, &myErrColumn))
    {
       qDebug() << "Parse xml failed:" << xmlFileName
                << " Row: " << QString::number(myErrLine)
                << " - Column: " << QString::number(myErrColumn)
                << "\n" << myError;
        myFile.close();
        return(false);
    }

    myFile.close();
    return true;
}

bool Crit3DMeteoGridDbHandler::parseXMLGrid(QString xmlFileName, std::string *myError)
{

    QDomDocument xmlDoc;

     if (!parseXMLFile(xmlFileName, &xmlDoc))
    {
        *myError = "parseXMLFile error";
        return false;
    }

    QDomNode child;
    QDomNode secondChild;
    TXMLvar varTable;

    QDomNode ancestor = xmlDoc.documentElement().firstChild();
    QString myTag;
    QString mySecondTag;
    int nrTokens = 0;
    const int nrRequiredToken = 25;
    int nRow;
    int nCol;

    while(!ancestor.isNull())
    {
        if (ancestor.toElement().tagName().toUpper() == "CONNECTION")
        {
            child = ancestor.firstChild();
            while( !child.isNull())
            {
                myTag = child.toElement().tagName().toUpper();
                if (myTag == "PROVIDER")
                {
                    _connection.provider = child.toElement().text();
                    // remove white spaces
                    _connection.provider = _connection.provider.simplified();
                    nrTokens++;
                }
                else if (myTag == "SERVER")
                {
                    _connection.server = child.toElement().text();
                    // remove white spaces
                    _connection.server = _connection.server.simplified();
                    nrTokens++;
                }
                else if (myTag == "NAME")
                {
                    _connection.name = child.toElement().text();
                    // remove white spaces
                    _connection.server = _connection.server.simplified();
                    nrTokens++;
                }
                else if (myTag == "USER")
                {
                    _connection.user = child.toElement().text();
                    // remove white spaces
                    _connection.user = _connection.user.simplified();
                    nrTokens++;
                }
                else if (myTag == "PASSWORD")
                {
                    _connection.password = child.toElement().text();
                    // remove white spaces
                    _connection.password = _connection.password.simplified();
                    nrTokens++;
                }
                child = child.nextSibling();
            }

        }
        else if (ancestor.toElement().tagName().toUpper() == "GRIDSTRUCTURE")
        {
            if (ancestor.toElement().attribute("isregular").toUpper() == "TRUE")
            {
                _gridStructure.setIsRegular(true);
                nrTokens++;
            }
            else if (ancestor.toElement().attribute("isregular").toUpper() == "FALSE")
            {
                _gridStructure.setIsRegular(false);
                nrTokens++;
            }
            if (ancestor.toElement().attribute("isutm").toUpper() == "TRUE")
            {
                _gridStructure.setIsUTM(true);
                nrTokens++;
            }
            else if (ancestor.toElement().attribute("isutm").toUpper() == "FALSE")
            {
                _gridStructure.setIsUTM(false);
                nrTokens++;
            }
            if (ancestor.toElement().attribute("istin").toUpper() == "TRUE")
            {
                _gridStructure.setIsTIN(true);
                nrTokens++;
            }
            else if (ancestor.toElement().attribute("istin").toUpper() == "FALSE")
            {
                _gridStructure.setIsTIN(false);
                nrTokens++;
            }
            if (ancestor.toElement().attribute("isfixedfields").toUpper() == "TRUE")
            {
                _gridStructure.setIsFixedFields(true);
                nrTokens++;
            }
            else if (ancestor.toElement().attribute("isfixedfields").toUpper() == "FALSE")
            {
                _gridStructure.setIsFixedFields(false);
                nrTokens++;
            }

            child = ancestor.firstChild();
            gis::Crit3DGridHeader header;
            while( !child.isNull())
            {
                myTag = child.toElement().tagName().toUpper();
                if (myTag == "XLL")
                {
                    header.llCorner->longitude = child.toElement().text().toFloat();
                    nrTokens++;
                }
                if (myTag == "YLL")
                {
                    header.llCorner->latitude = child.toElement().text().toFloat();
                    nrTokens++;
                }
                if (myTag == "NROWS")
                {
                    header.nrRows = child.toElement().text().toInt();
                    nRow = header.nrRows;
                    nrTokens++;
                }
                if (myTag == "NCOLS")
                {
                    header.nrCols = child.toElement().text().toInt();
                    nCol = header.nrCols;
                    nrTokens++;
                }
                if (myTag == "XWIDTH")
                {
                    header.dx = child.toElement().text().toFloat();
                    nrTokens++;
                }
                if (myTag == "YWIDTH")
                {
                    header.dy = child.toElement().text().toFloat();
                    if (_gridStructure.isUTM() == true && header.dx != header.dy )
                    {
                        *myError = "UTM grid with dx != dy";
                        return false;
                    }
                    nrTokens++;
                }
                child = child.nextSibling();
            }
            _gridStructure.setHeader(header);

        }

        else if (ancestor.toElement().tagName().toUpper() == "TABLEDAILY")
        {
            child = ancestor.firstChild();
            while( !child.isNull())
            {
                myTag = child.toElement().tagName().toUpper();
                if (myTag == "INDEXTABLENAME")
                {
                    _tableDaily.indexTableName = child.toElement().text();
                    // remove white spaces
                    _tableDaily.indexTableName = _tableDaily.indexTableName.simplified();
                    nrTokens++;
                }
                if (myTag == "FIELDTIME")
                {
                    _tableDaily.fieldTime = child.toElement().text();
                    // remove white spaces
                    _tableDaily.fieldTime = _tableDaily.fieldTime.simplified();
                    nrTokens++;
                }
                if (myTag == "FIELDVARCODE")
                {
                    _tableDaily.fieldVarCode = child.toElement().text();
                    // remove white spaces
                    _tableDaily.fieldVarCode = _tableDaily.fieldVarCode.simplified();
                    nrTokens++;
                }
                if (myTag == "FIELDVALUE")
                {
                    _tableDaily.fieldValue = child.toElement().text();
                    // remove white spaces
                    _tableDaily.fieldValue = _tableDaily.fieldValue.simplified();
                    nrTokens++;
                }
                if (myTag == "POSTFIX")
                {
                    _tableDaily.postFix = child.toElement().text();
                    // remove white spaces
                    _tableDaily.postFix = _tableDaily.postFix.simplified();
                    nrTokens++;
                }
                if (myTag == "VARCODE")
                {
                    secondChild = child.firstChild();
                    _tableDaily.varcode.push_back(varTable);
                    while( !secondChild.isNull())
                    {
                        mySecondTag = secondChild.toElement().tagName().toUpper();
                        if (mySecondTag == "VARCODE")
                        {
                            _tableDaily.varcode[_tableDaily.varcode.size()-1].varCode = secondChild.toElement().text().toInt();

                        }

                        if (mySecondTag == "VARPRAGANAME")
                        {
                            _tableDaily.varcode[_tableDaily.varcode.size()-1].varPragaName = secondChild.toElement().text();
                            // remove white spaces
                            _tableDaily.varcode[_tableDaily.varcode.size()-1].varPragaName = _tableDaily.varcode[_tableDaily.varcode.size()-1].varPragaName.simplified();
                        }

                        secondChild = secondChild.nextSibling();
                    }
                }
                child = child.nextSibling();
            }

        }

        else if (ancestor.toElement().tagName().toUpper() == "TABLEHOURLY")
        {
            child = ancestor.firstChild();
            while( !child.isNull())
            {
                myTag = child.toElement().tagName().toUpper();
                if (myTag == "INDEXTABLENAME")
                {
                    _tableHourly.indexTableName = child.toElement().text();
                    // remove white spaces
                    _tableHourly.indexTableName = _tableHourly.indexTableName.simplified();
                    nrTokens++;
                }
                if (myTag == "FIELDTIME")
                {
                    _tableHourly.fieldTime = child.toElement().text();
                    // remove white spaces
                    _tableHourly.fieldTime = _tableHourly.fieldTime.simplified();
                    nrTokens++;
                }
                if (myTag == "FIELDVARCODE")
                {
                    _tableHourly.fieldVarCode = child.toElement().text();
                    // remove white spaces
                    _tableHourly.fieldVarCode = _tableHourly.fieldVarCode.simplified();
                    nrTokens++;
                }
                if (myTag == "FIELDVALUE")
                {
                    _tableHourly.fieldValue = child.toElement().text();
                    // remove white spaces
                    _tableHourly.fieldValue = _tableHourly.fieldValue.simplified();
                    nrTokens++;
                }
                if (myTag == "POSTFIX")
                {
                    _tableHourly.postFix = child.toElement().text();
                    // remove white spaces
                    _tableHourly.postFix = _tableHourly.postFix.simplified();
                    nrTokens++;
                }
                if (myTag == "VARCODE")
                {
                    secondChild = child.firstChild();
                    _tableHourly.varcode.push_back(varTable);
                    while( !secondChild.isNull())
                    {
                        mySecondTag = secondChild.toElement().tagName().toUpper();
                        if (mySecondTag == "VARCODE")
                        {
                            _tableHourly.varcode[_tableHourly.varcode.size()-1].varCode = secondChild.toElement().text().toInt();

                        }

                        if (mySecondTag == "VARPRAGANAME")
                        {
                            _tableHourly.varcode[_tableHourly.varcode.size()-1].varPragaName = secondChild.toElement().text();
                            // remove white spaces
                            _tableHourly.varcode[_tableHourly.varcode.size()-1].varPragaName = _tableHourly.varcode[_tableHourly.varcode.size()-1].varPragaName.simplified();
                        }

                        secondChild = secondChild.nextSibling();
                    }
                }

                child = child.nextSibling();
            }

        }

        ancestor = ancestor.nextSibling();
    }
    xmlDoc.clear();

    if (_tableDaily.varcode.size() < 1 && _tableHourly.varcode.size() < 1)
    {
        *myError = "Missing daily and hourly var code";
        return false;
    }


    if (nrTokens < nrRequiredToken)
    {
        int missingTokens = nrRequiredToken - nrTokens;
        QString errMess = QString("Missing %1 key informations.").arg(QString::number(missingTokens));
        *myError = errMess.toStdString();
        return false;
    }


    // create variable maps
    for (unsigned int i=0; i < _tableDaily.varcode.size(); i++)
    {
        try
        {
            meteoVariable gridMeteoKey = MapDailyMeteoVar.at(_tableDaily.varcode[i].varPragaName.toStdString());
            _gridDailyVar.insert(gridMeteoKey, _tableDaily.varcode[i].varCode);
        }
        catch (const std::out_of_range& oor)
        {
            QString errMess = QString("%1 does not exist" ).arg(_tableDaily.varcode[i].varPragaName);
            *myError = errMess.toStdString();
        }

    }

    for (unsigned int i=0; i < _tableHourly.varcode.size(); i++)
    {
        try
        {
            meteoVariable gridMeteoKey = MapHourlyMeteoVar.at(_tableHourly.varcode[i].varPragaName.toStdString());
            _gridHourlyVar.insert(gridMeteoKey, _tableHourly.varcode[i].varCode);
        }
        catch (const std::out_of_range& oor)
        {
            QString errMess = QString("%1 does not exist" ).arg(_tableHourly.varcode[i].varPragaName);
            *myError = errMess.toStdString();
        }
    }


    _meteoGrid->setGridStructure(_gridStructure);

    _meteoGrid->initMeteoPoints(nRow, nCol);

    return true;
}

int Crit3DMeteoGridDbHandler::getDailyVarCode(meteoVariable meteoGridDailyVar)
{

    int varCode = NODATA;
    //check
    if (meteoGridDailyVar == noMeteoVar)
    {
        return varCode;
    }
    if (_gridDailyVar.empty())
    {
        return varCode;
    }
    if(_gridDailyVar.contains(meteoGridDailyVar))
    {
        varCode = _gridDailyVar[meteoGridDailyVar];
    }

    return varCode;

}

meteoVariable Crit3DMeteoGridDbHandler::getDailyVarEnum(int varCode)
{

    if (varCode == NODATA)
    {
        return noMeteoVar;
    }

    QMapIterator<meteoVariable, int> i(_gridDailyVar);
    while (i.hasNext()) {
        i.next();
        if (i.value() == varCode)
        {
            return i.key();
        }
    }

    return noMeteoVar;
}

int Crit3DMeteoGridDbHandler::getHourlyVarCode(meteoVariable meteoGridHourlyVar)
{

    int varCode = NODATA;
    //check
    if (meteoGridHourlyVar == noMeteoVar)
    {
        return varCode;
    }
    if (_gridHourlyVar.empty())
    {
        return varCode;
    }
    if(_gridHourlyVar.contains(meteoGridHourlyVar))
    {
        varCode = _gridHourlyVar[meteoGridHourlyVar];
    }

    return varCode;

}

meteoVariable Crit3DMeteoGridDbHandler::getHourlyVarEnum(int varCode)
{

    if (varCode == NODATA)
    {
        return noMeteoVar;
    }

    QMapIterator<meteoVariable, int> i(_gridHourlyVar);
    while (i.hasNext()) {
        i.next();
        if (i.value() == varCode)
        {
            return i.key();
        }
    }

    return noMeteoVar;

}



bool Crit3DMeteoGridDbHandler::openDatabase(std::string *myError)
{

    if (_connection.provider.toUpper() == "MYSQL")
    {
        _db = QSqlDatabase::addDatabase("QMYSQL");
    }

    _db.setHostName(_connection.server);
    _db.setDatabaseName(_connection.name);
    _db.setUserName(_connection.user);
    _db.setPassword(_connection.password);


    if (!_db.open())
    {
       *myError = "Error: connection with database fail";
       return false;
    }
    else
    {
       qDebug() << "Database: connection ok";
       return true;
    }
}


void Crit3DMeteoGridDbHandler::closeDatabase()
{
    if ((_db.isValid()) && (_db.isOpen()))
    {
        _db.removeDatabase(_db.connectionName());
        _db.close();
    }
}

bool Crit3DMeteoGridDbHandler::loadCellProperties(std::string *myError)
{
    QSqlQuery qry(_db);
    int row, col, active, height;
    QString code, name;

    qry.prepare( "SELECT * FROM CellsProperties ORDER BY Code" );

    if( !qry.exec() )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }
    else
    {
        while (qry.next())
        {

            if (! getValue(qry.value("Code"), &code))
            {
                *myError = "Missing data: Code";
                return false;
            }

            // facoltativa
            if (! getValue(qry.value("Name"), &name))
            {
                name = code;
            }

            if (! getValue(qry.value("Row"), &row))
            {
                *myError = "Missing data: Row";
                return false;
            }

            if (! getValue(qry.value("Col"), &col))
            {
                *myError = "Missing data: Col";
                return false;
            }

            // facoltativa
            if (! getValue(qry.value("Height"), &height))
            {
                height = NODATA;
            }

            if (! getValue(qry.value("Active"), &active))
            {
                *myError = "Missing data: Active";
                return false;
            }


            if (row < _meteoGrid->gridStructure().header().nrRows
                && col < _meteoGrid->gridStructure().header().nrCols)
            {
                _meteoGrid->fillMeteoPoint(row, col, code.toStdString(), name.toStdString(), height, active);
            }
            else
            {
                *myError = "Row or Col > nrRows or nrCols";
                return false;
            }
        }
    }
    return true;
}


bool Crit3DMeteoGridDbHandler::updateGridDate(std::string *myError)
{

    QSqlQuery qry(_db);

    int row = 0;
    int col = 0;
    int tableNotFoundError = 1146;
    std::string id;

    QDate maxDateD(QDate(1800, 1, 1));
    QDate minDateD(QDate(7800, 12, 31));

    QDate maxDateH(QDate(1800, 1, 1));
    QDate minDateH(QDate(7800, 12, 31));

    QDate temp;


    if (!_meteoGrid->findFirstActiveMeteoPoint(&id, &row, &col))
    {
        *myError = "active cell not found";
        return false;
    }

    QString tableD = QString::fromStdString(id) + _tableDaily.postFix;
    QString tableH = QString::fromStdString(id) + _tableHourly.postFix;

    QString statement = QString("SELECT MIN(PragaTime) as minDate, MAX(PragaTime) as maxDate FROM `%1`").arg(tableD);
    if( !qry.exec(statement) )
    {
        while( qry.lastError().number() == tableNotFoundError)
        {

            if ( col < _gridStructure.header().nrCols)
            {
                col = col + 1;
            }
            else if( row < _gridStructure.header().nrRows)
            {
                row = row + 1;
                col = 0;
            }

            if (!_meteoGrid->findFirstActiveMeteoPoint(&id, &row, &col))
            {
                *myError = "active cell not found";
                return false;
            }
            tableD = QString::fromStdString(id) + _tableDaily.postFix;
            tableH = QString::fromStdString(id) + _tableHourly.postFix;

            statement = QString("SELECT MIN(PragaTime) as minDate, MAX(PragaTime) as maxDate FROM `%1`").arg(tableD);
            qry.exec(statement);
        }
        std::cout << "qry.lastError().number() " << qry.lastError().number() << std::endl;
        if ( !qry.lastError().type() == QSqlError::NoError && qry.lastError().number() != tableNotFoundError)
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    else
    {
        if (qry.next())
        {
            if (getValue(qry.value("minDate"), &temp))
            {
                if (temp < minDateD)
                    minDateD = temp;
            }
            else
            {
                *myError = "Missing daily PragaTime";
                return false;
            }

            if (getValue(qry.value("maxDate"), &temp))
            {
                if (temp > maxDateD)
                    maxDateD = temp;
            }
            else
            {
                *myError = "Missing daily PragaTime";
                return false;
            }

        }
        else
        {
            *myError = "Error: PragaTime not found" ;
            return false;
        }
    }


    statement = QString("SELECT MIN(PragaTime) as minDate, MAX(PragaTime) as maxDate FROM `%1`").arg(tableH);
    if( !qry.exec(statement) )
    {
        while( qry.lastError().number() == tableNotFoundError)
        {

            if ( col < _gridStructure.header().nrCols)
            {
                col = col + 1;
            }
            else if( row < _gridStructure.header().nrRows)
            {
                row = row + 1;
                col = 0;
            }

            if (!_meteoGrid->findFirstActiveMeteoPoint(&id, &row, &col))
            {
                *myError = "active cell not found";
                return false;
            }

            tableH = QString::fromStdString(id) + _tableHourly.postFix;

            statement = QString("SELECT MIN(PragaTime) as minDate, MAX(PragaTime) as maxDate FROM `%1`").arg(tableH);
            qry.exec(statement);
        }
        if ( !qry.lastError().type() == QSqlError::NoError && qry.lastError().number() != tableNotFoundError)
        {
            *myError = qry.lastError().text().toStdString();
            return false;
        }
    }
    else
    {
        if (qry.next())
        {
            if (getValue(qry.value("minDate"), &temp))
            {
                if (temp < minDateH)
                    minDateH = temp;
            }
            else
            {
                *myError = "Missing hourly PragaTime";
                return false;
            }

            if (getValue(qry.value("maxDate"), &temp))
            {
                if (temp > maxDateH)
                    maxDateH = temp;
            }
            else
            {
                *myError = "Missing hourly PragaTime";
                return false;
            }

        }
        else
        {
            *myError = "Error: PragaTime not found" ;
            return false;
        }
    }


    // the last hourly day is always incomplete, there is just 00.00 value
    maxDateH = maxDateH.addDays(-1);

    if (minDateD < minDateH)
    {
        _firstDate = minDateD;
    }
    else
    {
        _firstDate = minDateH;
    }

    if (maxDateD > maxDateH)
    {
        _lastDate = maxDateD;
    }
    else
    {
        _lastDate = maxDateH;
    }

    return true;

}


bool Crit3DMeteoGridDbHandler::loadGridDailyData(std::string *myError, QString meteoPoint, QDate first, QDate last)
{
    QSqlQuery qry(_db);
    QString tableD = meteoPoint + _tableDaily.postFix;
    QDate date;
    int varCode;
    float value;

    int row;
    int col;
    int initialize = 1;

    int numberOfDays = first.daysTo(last) + 1;

    if (!_meteoGrid->findMeteoPointFromId(&row, &col, meteoPoint.toStdString()) )
    {
        *myError = "Missing MeteoPoint id";
        return false;
    }

    QString statement = QString("SELECT * FROM `%1` WHERE PragaTime >= '%2' AND PragaTime <= '%3' ORDER BY PragaTime").arg(tableD).arg(first.toString("yyyy-MM-dd")).arg(last.toString("yyyy-MM-dd"));
    if( !qry.exec(statement) )
    {
        *myError = qry.lastError().text().toStdString();
        return false;
    }
    else
    {
        while (qry.next())
        {
            if (!getValue(qry.value("PragaTime"), &date))
            {
                *myError = "Missing PragaTime";
                return false;
            }

            if (!getValue(qry.value("VariableCode"), &varCode))
            {
                *myError = "Missing VariableCode";
                return false;
            }

            if (!getValue(qry.value("Value"), &value))
            {
                *myError = "Missing Value";
                return false;
            }

            meteoVariable variable = getDailyVarEnum(varCode);

            if (_meteoGrid->fillMeteoPointDailyValue(row, col, numberOfDays, initialize, Crit3DDate(date.day(), date.month(), date.year()), variable, value))
            {
                initialize = 0;
            }

        }

    }

    return true;
}


bool Crit3DMeteoGridDbHandler::loadGridHourlyData(std::string *myError, QString meteoPoint, QDateTime first, QDateTime last)
{

    QSqlQuery qry(_db);
    QString tableH = meteoPoint + _tableHourly.postFix;
    QDateTime date;
    int varCode;
    float value;

    int row;
    int col;
    int initialize = 1;

    int numberOfDays = first.date().daysTo(last.date()) + 1;

    if (!_meteoGrid->findMeteoPointFromId(&row, &col, meteoPoint.toStdString()) )
    {
        *myError = "Missing MeteoPoint id";
        return false;
    }

    QString statement = QString("SELECT * FROM `%1` WHERE PragaTime >= '%2' AND PragaTime <= '%3' ORDER BY PragaTime").arg(tableH).arg(first.toString("yyyy-MM-dd hh:mm")).arg(last.toString("yyyy-MM-dd hh:mm"));
    if( !qry.exec(statement) )
    {
        *myError = qry.lastError().text().toStdString();
    }
    else
    {
        while (qry.next())
        {
            if (!getValue(qry.value("PragaTime"), &date))
            {
                *myError = "Missing PragaTime";
                return false;
            }

            if (!getValue(qry.value("VariableCode"), &varCode))
            {
                *myError = "Missing VariableCode";
                return false;
            }

            if (!getValue(qry.value("Value"), &value))
            {
                *myError = "Missing Value";
                return false;
            }

            meteoVariable variable = getHourlyVarEnum(varCode);

            if ( _meteoGrid->fillMeteoPointHourlyValue(row, col, numberOfDays, initialize, Crit3DDate(date.date().year(), date.date().month(), date.date().day()), date.time().hour(), date.time().minute(), variable, value) )
            {
                initialize = 0;
            }

        }

    }


    return true;
}

QList<float> Crit3DMeteoGridDbHandler::loadGridDailyVar(std::string *myError, QString meteoPoint, meteoVariable variable, QDate first, QDate last, QDate* firstDateDB)
{
    QSqlQuery qry(_db);
    QString tableD = meteoPoint + _tableDaily.postFix;
    QDate date, previousDate;

    QList<float> dailyVarList;

    float value;
    int row;
    int col;
    int firstRow = 1;

    int varCode = getDailyVarCode(variable);

    if (varCode == NODATA)
    {
        *myError = "Variable not existing";
        return dailyVarList;
    }

    if (!_meteoGrid->findMeteoPointFromId(&row, &col, meteoPoint.toStdString()) )
    {
        *myError = "Missing MeteoPoint id";
        return dailyVarList;
    }

    QString statement = QString("SELECT * FROM `%1` WHERE VariableCode = '%2' AND PragaTime >= '%3' AND PragaTime <= '%4' ORDER BY PragaTime").arg(tableD).arg(varCode).arg(first.toString("yyyy-MM-dd")).arg(last.toString("yyyy-MM-dd"));
    if( !qry.exec(statement) )
    {
        *myError = qry.lastError().text().toStdString();
    }
    else
    {

        while (qry.next())
        {
            if (firstRow)
            {
                if (!getValue(qry.value("PragaTime"), firstDateDB))
                {
                    *myError = "Missing PragaTime";
                    return dailyVarList;
                }

                if (!getValue(qry.value("Value"), &value))
                {
                    *myError = "Missing Value";
                    return dailyVarList;
                }
                dailyVarList << value;
                previousDate = *firstDateDB;
                firstRow = 0;
            }
            else
            {
                if (!getValue(qry.value("PragaTime"), &date))
                {
                    *myError = "Missing PragaTime";
                    return dailyVarList;
                }

                int missingDate = previousDate.daysTo(date);
                for (int i =1; i<missingDate; i++)
                {
                    dailyVarList << NODATA;
                }

                if (!getValue(qry.value("Value"), &value))
                {
                    *myError = "Missing Value";
                    return dailyVarList;
                }
                dailyVarList << value;
                previousDate = date;
            }


        }

    }

    return dailyVarList;
}

QList<float> Crit3DMeteoGridDbHandler::loadGridHourlyVar(std::string *myError, QString meteoPoint, meteoVariable variable, QDateTime first, QDateTime last, QDateTime* firstDateDB)
{

    QSqlQuery qry(_db);
    QString tableH = meteoPoint + _tableHourly.postFix;
    QDateTime dateTime, previousDateTime;

    QList<float> hourlyVarList;

    float value;
    int row;
    int col;
    int firstRow = 1;

    int varCode = getHourlyVarCode(variable);

    if (varCode == NODATA)
    {
        *myError = "Variable not existing";
        return hourlyVarList;
    }

    if (!_meteoGrid->findMeteoPointFromId(&row, &col, meteoPoint.toStdString()) )
    {
        *myError = "Missing MeteoPoint id";
        return hourlyVarList;
    }

    QString statement = QString("SELECT * FROM `%1` WHERE VariableCode = '%2' AND PragaTime >= '%3' AND PragaTime <= '%4' ORDER BY PragaTime").arg(tableH).arg(varCode).arg(first.toString("yyyy-MM-dd hh:mm")).arg(last.toString("yyyy-MM-dd hh:mm"));
    if( !qry.exec(statement) )
    {
        *myError = qry.lastError().text().toStdString();
    }
    else
    {

        while (qry.next())
        {
            if (firstRow)
            {
                if (!getValue(qry.value("PragaTime"), firstDateDB))
                {
                    *myError = "Missing PragaTime";
                    return hourlyVarList;
                }

                if (!getValue(qry.value("Value"), &value))
                {
                    *myError = "Missing Value";
                    return hourlyVarList;
                }
                hourlyVarList << value;
                previousDateTime = *firstDateDB;
                firstRow = 0;
            }
            else
            {
                if (!getValue(qry.value("PragaTime"), &dateTime))
                {
                    *myError = "Missing PragaTime";
                    return hourlyVarList;
                }

                int missingDateTime = previousDateTime.msecsTo(dateTime)/(1000*3600);
                for (int i = 1; i < missingDateTime; i++)
                {
                    hourlyVarList << NODATA;
                }

                if (!getValue(qry.value("Value"), &value))
                {
                    *myError = "Missing Value";
                    return hourlyVarList;
                }
                hourlyVarList << value;
                previousDateTime = dateTime;
            }


        }

    }

    return hourlyVarList;

}


QDate Crit3DMeteoGridDbHandler::firstDate() const
{
    return _firstDate;
}

void Crit3DMeteoGridDbHandler::setFirstDate(const QDate &firstDate)
{
    _firstDate = firstDate;
}

QDate Crit3DMeteoGridDbHandler::lastDate() const
{
    return _lastDate;
}

void Crit3DMeteoGridDbHandler::setLastDate(const QDate &lastDate)
{
    _lastDate = lastDate;
}

Crit3DMeteoGrid *Crit3DMeteoGridDbHandler::meteoGrid() const
{
    return _meteoGrid;
}

void Crit3DMeteoGridDbHandler::setMeteoGrid(Crit3DMeteoGrid *meteoGrid)
{
    _meteoGrid = meteoGrid;
}

QSqlDatabase Crit3DMeteoGridDbHandler::db() const
{
    return _db;
}

void Crit3DMeteoGridDbHandler::setDb(const QSqlDatabase &db)
{
    _db = db;
}

QString Crit3DMeteoGridDbHandler::fileName() const
{
    return _fileName;
}

TXMLConnection Crit3DMeteoGridDbHandler::connection() const
{
    return _connection;
}

Crit3DMeteoGridStructure Crit3DMeteoGridDbHandler::gridStructure() const
{
    return _gridStructure;
}

TXMLTable Crit3DMeteoGridDbHandler::tableDaily() const
{
    return _tableDaily;
}

TXMLTable Crit3DMeteoGridDbHandler::tableHourly() const
{
    return _tableHourly;
}

QString Crit3DMeteoGridDbHandler::tableDailyPrefix() const
{
    return _tableDailyPrefix;
}

QString Crit3DMeteoGridDbHandler::tableDailyPostfix() const
{
    return _tableDailyPostfix;
}

QString Crit3DMeteoGridDbHandler::tableHourlyPrefix() const
{
    return _tableHourlyPrefix;
}

QString Crit3DMeteoGridDbHandler::tableHourlyPostfix() const
{
    return _tableHourlyPostfix;
}

QString Crit3DMeteoGridDbHandler::tableDailyModel() const
{
    return _tableDailyModel;
}

QString Crit3DMeteoGridDbHandler::tableHourlyModel() const
{
    return _tableHourlyModel;
}




