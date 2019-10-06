#include <QString>
#include <QDate>
#include "crit3dElabList.h"
#include "commonConstants.h"
#include "climate.h"


Crit3DElabList::Crit3DElabList()
{

}

Crit3DElabList::~Crit3DElabList()
{
}

QStringList Crit3DElabList::listElab() const
{
    return _listElab;
}

void Crit3DElabList::setListElab(const QStringList &listElab)
{
    _listElab = listElab;
}

void Crit3DElabList::resetListElab()
{
    _listElab1.clear();
    _listElab2.clear();
    _listDateStart.clear();
    _listDateEnd.clear();
    _listNYears.clear();
    _listParam1.clear();
    _listParam1ClimateField.clear();
    _listParam1IsClimate.clear();
    _listParam2.clear();
    _listPeriodStr.clear();
    _listPeriodType.clear();
    _listVariable.clear();
    _listYearEnd.clear();
    _listYearStart.clear();
}

std::vector<int> Crit3DElabList::listYearStart() const
{
    return _listYearStart;
}

void Crit3DElabList::setListYearStart(const std::vector<int> &listYearStart)
{
    _listYearStart = listYearStart;
}

std::vector<int> Crit3DElabList::listYearEnd() const
{
    return _listYearEnd;
}

void Crit3DElabList::setListYearEnd(const std::vector<int> &listYearEnd)
{
    _listYearEnd = listYearEnd;
}

std::vector<meteoVariable> Crit3DElabList::listVariable() const
{
    return _listVariable;
}

void Crit3DElabList::setListVariable(const std::vector<meteoVariable> &listVariable)
{
    _listVariable = listVariable;
}

std::vector<QString> Crit3DElabList::listPeriodStr() const
{
    return _listPeriodStr;
}

void Crit3DElabList::setListPeriodStr(const std::vector<QString> &listPeriodStr)
{
    _listPeriodStr = listPeriodStr;
}

void Crit3DElabList::insertPeriodStr(QString period)
{
    _listPeriodStr.push_back(period);
}

std::vector<period> Crit3DElabList::listPeriodType() const
{
    return _listPeriodType;
}

void Crit3DElabList::setListPeriodType(const std::vector<period> &listPeriodType)
{
    _listPeriodType = listPeriodType;
}

void Crit3DElabList::insertPeriodType(period period)
{
    _listPeriodType.push_back(period);
}

std::vector<QDate> Crit3DElabList::listDateStart() const
{
    return _listDateStart;
}

void Crit3DElabList::setListDateStart(const std::vector<QDate> &listDateStart)
{
    _listDateStart = listDateStart;
}

std::vector<QDate> Crit3DElabList::listDateEnd() const
{
    return _listDateEnd;
}

void Crit3DElabList::setListDateEnd(const std::vector<QDate> &listDateEnd)
{
    _listDateEnd = listDateEnd;
}

std::vector<int> Crit3DElabList::listNYears() const
{
    return _listNYears;
}

void Crit3DElabList::setListNYears(const std::vector<int> &listNYears)
{
    _listNYears = listNYears;
}

std::vector<QString> Crit3DElabList::listElab1() const
{
    return _listElab1;
}

void Crit3DElabList::setListElab1(const std::vector<QString> &listElab1)
{
    _listElab1 = listElab1;
}

std::vector<float> Crit3DElabList::listParam1() const
{
    return _listParam1;
}

void Crit3DElabList::setListParam1(const std::vector<float> &listParam1)
{
    _listParam1 = listParam1;
}

std::vector<bool> Crit3DElabList::listParam1IsClimate() const
{
    return _listParam1IsClimate;
}

void Crit3DElabList::setListParam1IsClimate(const std::vector<bool> &listParam1IsClimate)
{
    _listParam1IsClimate = listParam1IsClimate;
}

std::vector<QString> Crit3DElabList::listParam1ClimateField() const
{
    return _listParam1ClimateField;
}

void Crit3DElabList::setListParam1ClimateField(const std::vector<QString> &listParam1ClimateField)
{
    _listParam1ClimateField = listParam1ClimateField;
}

std::vector<QString> Crit3DElabList::listElab2() const
{
    return _listElab2;
}

void Crit3DElabList::setListElab2(const std::vector<QString> &listElab2)
{
    _listElab2 = listElab2;
}

std::vector<float> Crit3DElabList::listParam2() const
{
    return _listParam2;
}

void Crit3DElabList::setListParam2(const std::vector<float> &listParam2)
{
    _listParam2 = listParam2;
}

std::vector<QString> Crit3DElabList::listUnit() const
{
    return _listUnit;
}

void Crit3DElabList::setListUnit(const std::vector<QString> &listUnit)
{
    _listUnit = listUnit;
}

/*
void Crit3DElabList::parserElaboration()
{

    for (int i = 0; i < _listElab.size(); i++)
    {
        int pos = 0;

        QString elaboration = _listElab[i];

        QStringList words = elaboration.split('_');

        if (words.isEmpty())
        {
            _listElab.replace(i, "NULL");
        }

        QString periodElabList = words.at(pos);
        QStringList myYearWords = periodElabList.split('-'); // ÷

        if (myYearWords[0].toInt() == false || myYearWords[1].toInt() == false)
        {
             _listElab.replace(i, "NULL");
        }

        _listYearStart.push_back(myYearWords[0].toInt());
        _listYearEnd.push_back(myYearWords[1].toInt());

        pos = pos + 1;

        if (words.size() == pos)
        {
             _listElab.replace(i, "NULL");
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

        _listVariable.push_back(var);

        pos = pos + 1;

        if (words.size() == pos)
        {
            _listElab.replace(i, "NULL");
        }

        QString periodTypeStr = words[pos];

        _listPeriodType.push_back(getPeriodTypeFromString(periodTypeStr));
        pos = pos + 1; // pos = 3

        if (words.size() == pos)
        {
             _listElab.replace(i, "NULL");
        }


        if ( (_listPeriodType[i] == genericPeriod) && ( (words[pos].at(0)).isDigit() ) )
        {
            _listPeriodStr.push_back(words[pos]);
            parserGenericPeriodString(i);
            pos = pos + 1; // pos = 4
        }
        else
        {
            _listPeriodStr.push_back(periodTypeStr);
            _listDateStart.push_back( QDate(0,  0,  0) );
            _listDateEnd.push_back( QDate(0,  0,  0) );
            _listNYears.push_back(0);
        }

        if (words.size() == pos)
        {
             _listElab.replace(i, "NULL");
        }

        QString elab = words[pos];
        bool param1IsClimate;
        QString param1ClimateField;

        meteoComputation elabMeteoComputation = MapMeteoComputation.at(elab.toStdString());

        float param = NODATA;
        int nrParam = nParameters(elabMeteoComputation);

        if (nrParam > 0)
        {
            pos = pos + 1;
            if ( words[pos].at(0) == "|" )
            {
                param1IsClimate = true;
                param1ClimateField = words[pos];
                param1ClimateField.remove(0,1);

                pos = pos + 1;
                if ( words[pos].right(2) == "||" )
                {
                     _listElab.replace(i, "NULL");
                }

                while ( words[pos].right(2) != "||" )
                {
                    param1ClimateField = param1ClimateField + "_" + words[pos];
                    pos = pos + 1;
                }
                param1ClimateField = param1ClimateField + "_" + words[pos].left(words[pos].size() - 2);
                param =  NODATA;
            }
            else
            {

                param1IsClimate = false;
                param1ClimateField = "";
                param = words[pos].toFloat();
            }
        }
        else
        {
            param1IsClimate = false;
            param1ClimateField = "";
            param =  NODATA;
        }

        pos = pos + 1;
        QString elab1;
        // second elab present
        if (words.size() > pos)
        {
            _listElab2.push_back(elab);
            _listParam2.push_back(param);


            elab1 = words[pos];
            _listElab1.push_back(elab1);
            elabMeteoComputation = MapMeteoComputation.at(elab1.toStdString());
            nrParam = nParameters(elabMeteoComputation);

            if (nrParam > 0)
            {
                pos = pos + 1;
                if ( words[pos].at(0) == "|" )
                {
                    param1IsClimate = true;
                    param1ClimateField = words[pos];
                    param1ClimateField.remove(0,1);

                    pos = pos + 1;
                    if ( words[pos].right(2) == "||" )
                    {
                         _listElab.replace(i, "NULL");
                    }

                    while ( words[pos].right(2) != "||" )
                    {
                        pos = pos + 1;
                        param1ClimateField = param1ClimateField + "_" + words[pos];
                    }
                    pos = pos + 1;
                    param1ClimateField = param1ClimateField + "_" + words[pos].left(words[pos].size() - 2);

                    _listParam1.push_back(NODATA);
                }
                else
                {
                    param1IsClimate = false;
                    param1ClimateField = "";
                    _listParam1.push_back( words[pos].toFloat() );
                }
            }
            else
            {
                param1IsClimate = false;
                param1ClimateField = "";
                _listParam1.push_back(NODATA);
            }
            _listParam1IsClimate.push_back(param1IsClimate);
            _listParam1ClimateField.push_back(param1ClimateField);

        }
        else
        {
            _listElab1.push_back(elab);
            _listParam1.push_back(param);
            _listElab2.push_back(nullptr);
            _listParam2.push_back(NODATA);

            _listParam1IsClimate.push_back(param1IsClimate);
            _listParam1ClimateField.push_back(param1ClimateField);
        }

    }

}
*/

/*
bool Crit3DElabList::parserGenericPeriodString(int index)
{

    QString periodString = _listPeriodStr.at(index);

    if ( periodString.isEmpty())
    {
        return false;
    }

    QString day = periodString.mid(0,2);
    QString month = periodString.mid(3,2);
    int year = 2000;
    _listDateStart.push_back( QDate(year,  month.toInt(),  day.toInt()) );

    //climaElabList->setDateStart( QDate(year,  month.toInt(),  day.toInt()) );

    day = periodString.mid(6,2);
    month = periodString.mid(9,2);

    //climaElabList->setDateEnd( QDate(year,  month.toInt(),  day.toInt()) );
    _listDateEnd.push_back( QDate(year,  month.toInt(),  day.toInt()) );

    if ( periodString.size() > 11 )
    {
        //climaElabList->setNYears( (periodString.mid(13,2)).toInt() );
        _listNYears.push_back((periodString.mid(13,2)).toInt());
    }
    return true;

}

*/
