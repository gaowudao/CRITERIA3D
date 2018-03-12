#include "dbMeteoGrid.h"
#include "utilities.h"


Crit3DMeteoGridDbHandler::Crit3DMeteoGridDbHandler()
{

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
    const int nrRequiredToken = 29;

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
            }
            else if (ancestor.toElement().attribute("isregular").toUpper() == "FALSE")
            {
                _gridStructure.setIsRegular(false);
            }
            if (ancestor.toElement().attribute("isutm").toUpper() == "TRUE")
            {
                _gridStructure.setIsUTM(true);
            }
            else if (ancestor.toElement().attribute("isutm").toUpper() == "FALSE")
            {
                _gridStructure.setIsUTM(false);
            }
            if (ancestor.toElement().attribute("istin").toUpper() == "TRUE")
            {
                _gridStructure.setIsTIN(true);
            }
            else if (ancestor.toElement().attribute("istin").toUpper() == "FALSE")
            {
                _gridStructure.setIsTIN(false);
            }
            if (ancestor.toElement().attribute("isFixedFields").toUpper() == "TRUE")
            {
                _gridStructure.setIsFixedFields(true);
            }
            else if (ancestor.toElement().attribute("isFixedFields").toUpper() == "FALSE")
            {
                _gridStructure.setIsFixedFields(false);
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
                    nrTokens++;
                }
                if (myTag == "NCOLS")
                {
                    header.nrCols = child.toElement().text().toInt();
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

    if (_tableDaily.varcode.size() < 1)
    {
        qDebug() << "Missing daily var code";
        return false;
    }

    if (_tableHourly.varcode.size() < 1)
    {
        qDebug() << "Missing hourly var code";
        return false;
    }

    if (nrTokens < nrRequiredToken)
    {
        int missingTokens = nrRequiredToken - nrTokens;
        qDebug() << "Missing " + QString::number(missingTokens) + " key informations.";
        return false;
    }

    return true;
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




