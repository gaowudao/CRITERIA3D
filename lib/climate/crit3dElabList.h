#ifndef CRIT3DELABLIST_H
#define CRIT3DELABLIST_H

#ifndef METEO_H
    #include "meteo.h"
#endif
#ifndef STATISTICS_H
    #include "statistics.h"
#endif

#ifndef CRIT3DCLIMATELIST_H
    #include "crit3dClimateList.h"
#endif

#ifndef QSTRINGLIST_H
    #include <QStringList>
#endif
#ifndef QDATETIME_H
    #include <QDateTime>
#endif


class Crit3DElabList
{

public:
    Crit3DElabList();
    ~Crit3DElabList();


    QStringList listElab() const;
    void setListElab(const QStringList &listClimateElab);

    void resetListElab();

    std::vector<int> listYearStart() const;
    void setListYearStart(const std::vector<int> &listYearStart);

    std::vector<int> listYearEnd() const;
    void setListYearEnd(const std::vector<int> &listYearEnd);

    std::vector<meteoVariable> listVariable() const;
    void setListVariable(const std::vector<meteoVariable> &listVariable);

    std::vector<QString> listPeriodStr() const;
    void setListPeriodStr(const std::vector<QString> &listPeriodStr);

    std::vector<period> listPeriodType() const;
    void setListPeriodType(const std::vector<period> &listPeriodType);

    std::vector<QDate> listDateStart() const;
    void setListDateStart(const std::vector<QDate> &listDateStart);

    std::vector<QDate> listDateEnd() const;
    void setListDateEnd(const std::vector<QDate> &listDateEnd);

    std::vector<int> listNYears() const;
    void setListNYears(const std::vector<int> &listNYears);

    std::vector<QString> listElab1() const;
    void setListElab1(const std::vector<QString> &listElab1);

    std::vector<float> listParam1() const;
    void setListParam1(const std::vector<float> &listParam1);

    std::vector<bool> listParam1IsClimate() const;
    void setListParam1IsClimate(const std::vector<bool> &listParam1IsClimate);

    std::vector<QString> listParam1ClimateField() const;
    void setListParam1ClimateField(const std::vector<QString> &listParam1ClimateField);

    std::vector<QString> listElab2() const;
    void setListElab2(const std::vector<QString> &listElab2);

    std::vector<float> listParam2() const;
    void setListParam2(const std::vector<float> &listParam2);

    std::vector<QString> listUnit() const;
    void setListUnit(const std::vector<QString> &listUnit);

    //void parserElaboration();
    //bool parserGenericPeriodString(int index);

private:

    QStringList _listElab;
    std::vector<int> _listYearStart;
    std::vector<int> _listYearEnd;
    std::vector<meteoVariable> _listVariable;
    std::vector<QString> _listPeriodStr;
    std::vector<period> _listPeriodType;
    std::vector<QDate> _listDateStart;
    std::vector<QDate> _listDateEnd;
    std::vector<int> _listNYears;
    std::vector<QString> _listElab1;
    std::vector<float> _listParam1;
    std::vector<bool> _listParam1IsClimate;
    std::vector<QString> _listParam1ClimateField;
    std::vector<QString> _listElab2;
    std::vector<float> _listParam2;
    std::vector<QString> _listUnit;

};

#endif // CRIT3DELABLIST_H
