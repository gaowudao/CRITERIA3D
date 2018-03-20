#include "dbMeteoGrid.h"
#include "utilities.h"
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

bool Crit3DMeteoGridDbHandler::parseXMLGrid(QString xmlFileName)
{

    QDomDocument xmlDoc;

     if (!parseXMLFile(xmlFileName, &xmlDoc))
    {
        qDebug() << "parseXMLFile error";
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
            if (ancestor.toElement().attribute("isFixedFields").toUpper() == "TRUE")
            {
                _gridStructure.setIsFixedFields(true);
                nrTokens++;
            }
            else if (ancestor.toElement().attribute("isFixedFields").toUpper() == "FALSE")
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
                        qDebug() << "UTM grid with dx != dy";
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
        qDebug() << "Missing daily and hourly var code";
        return false;
    }


    if (nrTokens < nrRequiredToken)
    {
        int missingTokens = nrRequiredToken - nrTokens;
        qDebug() << "Missing " + QString::number(missingTokens) + " key informations.";
        return false;
    }

    _meteoGrid->setGridStructure(_gridStructure);

    _meteoGrid->initMeteoPoints(nRow, nCol);

    return true;
}

void Crit3DMeteoGridDbHandler::openDatabase(QString dbName)
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(dbName);

    if (!_db.open())
    {
       qDebug() << "Error: connection with database fail";
    }
    else
    {
       qDebug() << "Database: connection ok";
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

bool Crit3DMeteoGridDbHandler::loadCellProperties(QString dbName)
{
    openDatabase(dbName);

    QSqlQuery qry(_db);

    qry.prepare( "SELECT * FROM CellsProperties ORDER BY Code" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            QString code = qry.value("Code").toString();
            QString name = qry.value("Name").toString();
            int row = qry.value("Row").toInt();
            int col = qry.value("Col").toInt();
            double x = qry.value(4).toDouble();
            double y = qry.value(5).toDouble();
            int height = qry.value(6).toInt();
            int area = qry.value(7).toInt();
            int active = qry.value(8).toInt();

            _meteoGrid->fillMeteoPoint(row, col, code.toStdString(), name.toStdString(), x, y, height, active, area);
        }
    }
    return true;


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




