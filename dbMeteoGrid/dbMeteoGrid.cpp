#include "dbMeteoGrid.h"


DbMeteoGrid::DbMeteoGrid()
{

}

bool DbMeteoGrid::parseXMLFile(QString xmlFileName, QDomDocument* xmlDoc)
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

bool DbMeteoGrid::parseXMLGrid(QString xmlFileName)
{

    QDomDocument xmlDoc;

     if (!parseXMLFile(xmlFileName, &xmlDoc))
    {
        qDebug() << "parseXMLFile error";
        return false;
    }

    QDomNode child;
    QDomNode secondChild;
    TXMLfield fieldCP;
    TXMLvar varTable;

    QDomNode ancestor = xmlDoc.documentElement().firstChild();
    QString myTag;
    QString mySecondTag;
    int nrTokens = 0;
    const int nrRequiredToken = 31;

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
                _gridStructure.isRegular = true;
            }
            else if (ancestor.toElement().attribute("isregular").toUpper() == "FALSE")
            {
                _gridStructure.isRegular = false;
            }
            if (ancestor.toElement().attribute("isutm").toUpper() == "TRUE")
            {
                _gridStructure.isUTM = true;
            }
            else if (ancestor.toElement().attribute("isutm").toUpper() == "FALSE")
            {
                _gridStructure.isUTM = false;
            }
            if (ancestor.toElement().attribute("istin").toUpper() == "TRUE")
            {
                _gridStructure.isTIN = true;
            }
            else if (ancestor.toElement().attribute("istin").toUpper() == "FALSE")
            {
                _gridStructure.isTIN = false;
            }
            if (ancestor.toElement().attribute("isFixedFields").toUpper() == "TRUE")
            {
                _gridStructure.isFixedFields = true;
            }
            else if (ancestor.toElement().attribute("isFixedFields").toUpper() == "FALSE")
            {
                _gridStructure.isFixedFields = false;
            }

            child = ancestor.firstChild();
            while( !child.isNull())
            {
                myTag = child.toElement().tagName().toUpper();
                if (myTag == "XLL")
                {
                    _gridStructure.gridStructure.header->llCorner->x= child.toElement().text().toFloat();
                    nrTokens++;
                }
                if (myTag == "YLL")
                {
                    _gridStructure.gridStructure.header->llCorner->y= child.toElement().text().toFloat();
                    nrTokens++;
                }
                if (myTag == "NROWS")
                {
                    _gridStructure.gridStructure.header->nrRows= child.toElement().text().toInt();
                    nrTokens++;
                }
                if (myTag == "NCOLS")
                {
                    _gridStructure.gridStructure.header->nrCols= child.toElement().text().toInt();
                    nrTokens++;
                }
                if (myTag == "XWIDTH")
                {
                    // ??? la cell size perchè non ha due dimensioni?
                    //_gridStructure.gridStructure.header->cellSize=
                    _gridStructure.xWidth = child.toElement().text().toFloat();
                    nrTokens++;
                }
                if (myTag == "YWIDTH")
                {
                    // ??? la cell size perchè non ha due dimensioni?
                    //_gridStructure.gridStructure.header->cellSize
                    _gridStructure.yWidth = child.toElement().text().toFloat();
                    nrTokens++;
                }
                child = child.nextSibling();
            }

        }

        else if (ancestor.toElement().tagName().toUpper() == "CELLSPROPERTIES")
        {
            child = ancestor.firstChild();
            while( !child.isNull())
            {
                myTag = child.toElement().tagName().toUpper();
                if (myTag == "TABLE")
                {
                    _cellsProperties.table = child.toElement().text();
                    // remove white spaces
                    _cellsProperties.table = _cellsProperties.table.simplified();
                    nrTokens++;

                }
                else if (myTag == "FIELD")
                {
                    secondChild = child.firstChild();
                    _cellsProperties.cellPropertiesField.push_back(fieldCP);
                    while( !secondChild.isNull())
                    {
                        mySecondTag = secondChild.toElement().tagName().toUpper();
                        if (mySecondTag == "NAME")
                        {
                            _cellsProperties.cellPropertiesField[_cellsProperties.cellPropertiesField.size()-1].name = secondChild.toElement().text();
                            // remove white spaces
                            _cellsProperties.cellPropertiesField[_cellsProperties.cellPropertiesField.size()-1].name = _cellsProperties.cellPropertiesField[_cellsProperties.cellPropertiesField.size()-1].name.simplified();
                            nrTokens++;

                        }

                        if (mySecondTag == "PRAGANAME")
                        {
                            _cellsProperties.cellPropertiesField[_cellsProperties.cellPropertiesField.size()-1].pragaName = secondChild.toElement().text();
                            // remove white spaces
                            _cellsProperties.cellPropertiesField[_cellsProperties.cellPropertiesField.size()-1].pragaName = _cellsProperties.cellPropertiesField[_cellsProperties.cellPropertiesField.size()-1].pragaName.simplified();
                            nrTokens++;
                        }

                        secondChild = secondChild.nextSibling();
                    }
                }

            }

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
                            nrTokens++;

                        }

                        if (mySecondTag == "VARPRAGANAME")
                        {
                            _tableDaily.varcode[_tableDaily.varcode.size()-1].varPragaName = secondChild.toElement().text();
                            // remove white spaces
                            _tableDaily.varcode[_tableDaily.varcode.size()-1].varPragaName = _tableDaily.varcode[_tableDaily.varcode.size()-1].varPragaName.simplified();
                            nrTokens++;
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
                            nrTokens++;

                        }

                        if (mySecondTag == "VARPRAGANAME")
                        {
                            _tableHourly.varcode[_tableHourly.varcode.size()-1].varPragaName = secondChild.toElement().text();
                            // remove white spaces
                            _tableHourly.varcode[_tableHourly.varcode.size()-1].varPragaName = _tableHourly.varcode[_tableHourly.varcode.size()-1].varPragaName.simplified();
                            nrTokens++;
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

    if (nrTokens < nrRequiredToken)
    {
        int missingTokens = nrRequiredToken - nrTokens;
        qDebug() << "Missing " + QString::number(missingTokens) + " key informations.";
        return false;
    }

    return true;
}

QString DbMeteoGrid::fileName() const
{
    return _fileName;
}

TXMLConnection DbMeteoGrid::connection() const
{
    return _connection;
}

MeteoGrid DbMeteoGrid::gridStructure() const
{
    return _gridStructure;
}

TXMLCellsProperties DbMeteoGrid::cellsProperties() const
{
    return _cellsProperties;
}

TXMLTable DbMeteoGrid::tableDaily() const
{
    return _tableDaily;
}

TXMLTable DbMeteoGrid::tableHourly() const
{
    return _tableHourly;
}

bool DbMeteoGrid::existHourlyData() const
{
    return _existHourlyData;
}

QString DbMeteoGrid::tableDailyPrefix() const
{
    return _tableDailyPrefix;
}

QString DbMeteoGrid::tableDailyPostfix() const
{
    return _tableDailyPostfix;
}

QString DbMeteoGrid::tableHourlyPrefix() const
{
    return _tableHourlyPrefix;
}

QString DbMeteoGrid::tableHourlyPostfix() const
{
    return _tableHourlyPostfix;
}

QString DbMeteoGrid::tableDailyModel() const
{
    return _tableDailyModel;
}

QString DbMeteoGrid::tableHourlyModel() const
{
    return _tableHourlyModel;
}




