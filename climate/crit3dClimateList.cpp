#include <QString>
#include <QDate>

#include "commonConstants.h"
#include "crit3dClimateList.h"


Crit3DClimateList::Crit3DClimateList()
{

}

Crit3DClimateList::~Crit3DClimateList()
{
}

QStringList Crit3DClimateList::listClimateElab() const
{
    return _listClimateElab;
}

void Crit3DClimateList::setListClimateElab(const QStringList &listClimateElab)
{
    _listClimateElab = listClimateElab;
}

std::vector<int> Crit3DClimateList::listYearStart() const
{
    return _listYearStart;
}

void Crit3DClimateList::setListYearStart(const std::vector<int> &listYearStart)
{
    _listYearStart = listYearStart;
}

std::vector<int> Crit3DClimateList::listYearEnd() const
{
    return _listYearEnd;
}

void Crit3DClimateList::setListYearEnd(const std::vector<int> &listYearEnd)
{
    _listYearEnd = listYearEnd;
}

std::vector<meteoVariable> Crit3DClimateList::listVariable() const
{
    return _listVariable;
}

void Crit3DClimateList::setListVariable(const std::vector<meteoVariable> &listVariable)
{
    _listVariable = listVariable;
}

std::vector<QString> Crit3DClimateList::listPeriodStr() const
{
    return _listPeriodStr;
}

void Crit3DClimateList::setListPeriodStr(const std::vector<QString> &listPeriodStr)
{
    _listPeriodStr = listPeriodStr;
}

std::vector<period> Crit3DClimateList::listPeriodType() const
{
    return _listPeriodType;
}

void Crit3DClimateList::setListPeriodType(const std::vector<period> &listPeriodType)
{
    _listPeriodType = listPeriodType;
}

std::vector<QDate> Crit3DClimateList::listGenericPeriodDateStart() const
{
    return _listGenericPeriodDateStart;
}

void Crit3DClimateList::setListGenericPeriodDateStart(const std::vector<QDate> &listGenericPeriodDateStart)
{
    _listGenericPeriodDateStart = listGenericPeriodDateStart;
}

std::vector<QDate> Crit3DClimateList::listGenericPeriodDateEnd() const
{
    return _listGenericPeriodDateEnd;
}

void Crit3DClimateList::setListGenericPeriodDateEnd(const std::vector<QDate> &listGenericPeriodDateEnd)
{
    _listGenericPeriodDateEnd = listGenericPeriodDateEnd;
}

std::vector<int> Crit3DClimateList::listNYears() const
{
    return _listNYears;
}

void Crit3DClimateList::setListNYears(const std::vector<int> &listNYears)
{
    _listNYears = listNYears;
}

std::vector<QString> Crit3DClimateList::listElab1() const
{
    return _listElab1;
}

void Crit3DClimateList::setListElab1(const std::vector<QString> &listElab1)
{
    _listElab1 = listElab1;
}

std::vector<float> Crit3DClimateList::listParam1() const
{
    return _listParam1;
}

void Crit3DClimateList::setListParam1(const std::vector<float> &listParam1)
{
    _listParam1 = listParam1;
}

std::vector<bool> Crit3DClimateList::listParam1IsClimate() const
{
    return _listParam1IsClimate;
}

void Crit3DClimateList::setListParam1IsClimate(const std::vector<bool> &listParam1IsClimate)
{
    _listParam1IsClimate = listParam1IsClimate;
}

std::vector<QString> Crit3DClimateList::listParam1ClimateField() const
{
    return _listParam1ClimateField;
}

void Crit3DClimateList::setListParam1ClimateField(const std::vector<QString> &listParam1ClimateField)
{
    _listParam1ClimateField = listParam1ClimateField;
}

std::vector<QString> Crit3DClimateList::listElab2() const
{
    return _listElab2;
}

void Crit3DClimateList::setListElab2(const std::vector<QString> &listElab2)
{
    _listElab2 = listElab2;
}

std::vector<float> Crit3DClimateList::listParam2() const
{
    return _listParam2;
}

void Crit3DClimateList::setListParam2(const std::vector<float> &listParam2)
{
    _listParam2 = listParam2;
}

void Crit3DClimateList::parserElaboration()
{

    for (int i = 0; i < listClimateElab().size(); i++)
    {
        int pos = 0;

        QString climateElab = listClimateElab().at(i);

        QStringList words = climateElab.split('_');

        if (words.isEmpty())
        {
            listClimateElab().replace(i, "NULL");
        }

        QString periodElabList = words.at(pos);
        QStringList myYearWords = periodElabList.split('-'); // ÷

        if (myYearWords[0].toInt() == false || myYearWords[1].toInt() == false)
        {
             listClimateElab().replace(i, "NULL");
        }

//        clima->setYearStart(myYearWords[0].toInt());
//        clima->setYearEnd(myYearWords[1].toInt());
        listYearStart().push_back(myYearWords[0].toInt());
        listYearEnd().push_back(myYearWords[1].toInt());

        pos = pos + 1;

        if (words.size() == pos)
        {
             listClimateElab().replace(i, "NULL");
        }

        meteoVariable var;
        if (words[pos] == "")
        {
            var = noMeteoVar;
        }
        else
        {
            try
            {
                var = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, words[pos].toStdString());
            }
            catch (const std::out_of_range& )
            {
              var = noMeteoVar;
            }
        }

        //clima->setVariable(var);
        listVariable().push_back(var);

        pos = pos + 1;

        if (words.size() == pos)
        {
            listClimateElab().replace(i, "NULL");
        }

        QString periodTypeStr = words[pos];

//        clima->setPeriodStr(periodTypeStr);
//        clima->setPeriodType(getPeriodTypeFromString(periodTypeStr));

        listPeriodType().push_back(getPeriodTypeFromString(periodTypeStr));
        pos = pos + 1; // pos = 3

        if (words.size() == pos)
        {
             listClimateElab().replace(i, "NULL");
        }

//        if ( (clima->periodType() == genericPeriod) && ( (words[pos].at(0)).isDigit() ) )
//        {
//            clima->setPeriodStr(words[pos]);
//            parserGenericPeriodString(clima);
//            pos = pos + 1; // pos = 4
//        }

        if ( (listPeriodType().at(i) == genericPeriod) && ( (words[pos].at(0)).isDigit() ) )
        {
            //clima->setPeriodStr(words[pos]);
            listPeriodStr().push_back(words[pos]);
            parserGenericPeriodString(i);
            pos = pos + 1; // pos = 4
        }
        else
        {
            listPeriodStr().push_back(periodTypeStr);
            listGenericPeriodDateStart().push_back( QDate(0,  0,  0) );
            listGenericPeriodDateEnd().push_back( QDate(0,  0,  0) );
            listNYears().push_back(0);
        }

        if (words.size() == pos)
        {
             listClimateElab().replace(i, "NULL");
        }

        QString elab = words[pos];

        meteoComputation elabMeteoComputation = MapMeteoComputation.at(elab.toStdString());

        float param = NODATA;
        int nrParam = nParameters(elabMeteoComputation);

        if (nrParam > 0)
        {
            pos = pos + 1;
            if ( words[pos].at(0) == "|" )
            {
                listParam1IsClimate().push_back(true);
                QString param1ClimateField = words[pos];
                param1ClimateField.remove(0,1);

                pos = pos + 1;
                if ( words[pos].right(2) == "||" )
                {
                     listClimateElab().replace(i, "NULL");
                }

                while ( words[pos].right(2) != "||" )
                {
                    param1ClimateField = param1ClimateField + "_" + words[pos];
                    pos = pos + 1;
                }
                param1ClimateField = param1ClimateField + "_" + words[pos].left(words[pos].size() - 2);

                listParam1ClimateField().push_back(param1ClimateField);
                param =  NODATA;
            }
            else
            {
                listParam1IsClimate().push_back(false);
                listParam1ClimateField().push_back(NULL);
                param = words[pos].toFloat();
            }
        }
        else
        {
            listParam1IsClimate().push_back(false);
            listParam1ClimateField().push_back(NULL);
        }

        pos = pos + 1;
        QString elab1;
        if (words.size() > pos)
        {
            listElab2().push_back(elab);
            listParam2().push_back(param);

            elab1 = words[pos];
            listElab1().push_back(elab1);
            elabMeteoComputation = MapMeteoComputation.at(elab1.toStdString());
            nrParam = nParameters(elabMeteoComputation);

            if (nrParam > 0)
            {
                pos = pos + 1;
                if ( words[pos].at(0) == "|" )
                {
                    listParam1IsClimate().push_back(true);
                    QString param1ClimateField = words[pos];
                    param1ClimateField.remove(0,1);

                    pos = pos + 1;
                    if ( words[pos].right(2) == "||" )
                    {
                         listClimateElab().replace(i, "NULL");
                    }

                    while ( words[pos].right(2) != "||" )
                    {
                        pos = pos + 1;
                        param1ClimateField = param1ClimateField + "_" + words[pos];
                    }
                    pos = pos + 1;
                    param1ClimateField = param1ClimateField + "_" + words[pos].left(words[pos].size() - 2);

                    listParam1().push_back(NODATA);
                }
                else
                {
                    listParam1IsClimate().push_back(false);
                    listParam1ClimateField().push_back(NULL);
                    listParam1().push_back( words[pos].toFloat() );
                }
            }
            else
            {
                listParam1IsClimate().push_back(false);
                listParam1ClimateField().push_back(NULL);
            }

        }
        else
        {
            listElab1().push_back(elab1);
            listParam1().push_back(param);
        }

    }

}


bool Crit3DClimateList::parserGenericPeriodString(int index)
{

    QString periodString = listClimateElab().at(index);

    if ( periodString.isEmpty())
    {
        return false;
    }

    QString day = periodString.mid(0,2);
    QString month = periodString.mid(3,2);
    int year = 2000;
    listGenericPeriodDateStart().push_back( QDate(year,  month.toInt(),  day.toInt()) );

    //climaElabList->setGenericPeriodDateStart( QDate(year,  month.toInt(),  day.toInt()) );

    day = periodString.mid(6,2);
    month = periodString.mid(9,2);

    //climaElabList->setGenericPeriodDateEnd( QDate(year,  month.toInt(),  day.toInt()) );
    listGenericPeriodDateEnd().push_back( QDate(year,  month.toInt(),  day.toInt()) );

    if ( periodString.size() > 11 )
    {
        //climaElabList->setNYears( (periodString.mid(13,2)).toInt() );
        listNYears().push_back((periodString.mid(13,2)).toInt());
    }
    return true;

}


int Crit3DClimateList::nParameters(meteoComputation elab)
{
    switch(elab)
    {
    case average:
        return 0;
    case maxInList:
        return 0;
    case minInList:
        return 0;
    case sum:
        return 0;
    case avgAbove:
        return 1;
    case stdDevAbove:
        return 1;
    case sumAbove:
        return 1;
    case daysAbove:
        return 1;
    case daysBelow:
        return 1;
    case consecutiveDaysAbove:
        return 1;
    case consecutiveDaysBelow:
        return 1;
    case percentile:
        return 1;
    case prevailingWindDir:
        return 0;
    case correctedDegreeDaysSum:
        return 1;
    case trend:
        return 0;
    case mannKendall:
        return 0;
    case differenceWithThreshold:
        return 1;
    case lastDayBelowThreshold:
        return 1;
    default:
        return 0;
    }


}

period Crit3DClimateList::getPeriodTypeFromString(QString periodStr)
{

    if (periodStr == "Daily")
        return dailyPeriod;
    if (periodStr == "Decadal")
        return decadalPeriod;
    if (periodStr == "Monthly")
        return monthlyPeriod;
    if (periodStr == "Seasonal")
        return seasonalPeriod;
    if (periodStr == "Annual")
        return annualPeriod;
    if (periodStr == "Generic") // era generic_period
        return genericPeriod;

    return noPeriodType;

}
