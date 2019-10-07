#ifndef CRIT3DANOMALYLIST_H
#define CRIT3DANOMALYLIST_H

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


class Crit3DAnomalyList
{

public:
    Crit3DAnomalyList();
    ~Crit3DAnomalyList();

    bool isMeteoGrid() const;
    void setIsMeteoGrid(bool isMeteoGrid);

    QStringList listAnomaly() const;
    void setListAnomaly(const QStringList &listAnomaly);

    void reset();

    std::vector<int> listYearStart() const;
    void setListYearStart(const std::vector<int> &listYearStart);
    void insertYearStart(int yearStart);

    std::vector<int> listYearEnd() const;
    void setListYearEnd(const std::vector<int> &listYearEnd);
    void insertYearEnd(int yearEnd);

    std::vector<meteoVariable> listVariable() const;
    void setListVariable(const std::vector<meteoVariable> &listVariable);
    void insertVariable(meteoVariable variable);

    std::vector<QString> listPeriodStr() const;
    void setListPeriodStr(const std::vector<QString> &listPeriodStr);
    void insertPeriodStr(QString period);

    std::vector<period> listPeriodType() const;
    void setListPeriodType(const std::vector<period> &listPeriodType);
    void insertPeriodType(period period);

    std::vector<QDate> listDateStart() const;
    void setListDateStart(const std::vector<QDate> &listDateStart);
    void insertDateStart(QDate dateStart);

    std::vector<QDate> listDateEnd() const;
    void setListDateEnd(const std::vector<QDate> &listDateEnd);
    void insertDateEnd(QDate dateEnd);

    std::vector<int> listNYears() const;
    void setListNYears(const std::vector<int> &listNYears);
    void insertNYears(int nYears);

    std::vector<QString> listElab1() const;
    void setListElab1(const std::vector<QString> &listElab1);
    void insertElab1(QString elab1);

    std::vector<float> listParam1() const;
    void setListParam1(const std::vector<float> &listParam1);
    void insertParam1(float param1);

    std::vector<bool> listParam1IsClimate() const;
    void setListParam1IsClimate(const std::vector<bool> &listParam1IsClimate);
    void insertParam1IsClimate(bool param1IsClimate);

    std::vector<QString> listParam1ClimateField() const;
    void setListParam1ClimateField(const std::vector<QString> &listParam1ClimateField);
    void insertParam1ClimateField(QString param1ClimateField);

    std::vector<QString> listElab2() const;
    void setListElab2(const std::vector<QString> &listElab2);
    void insertElab2(QString elab2);

    std::vector<float> listParam2() const;
    void setListParam2(const std::vector<float> &listParam2);
    void insertParam2(float param2);

    //void parserElaboration();
    //bool parserGenericPeriodString(int index);

private:

    QStringList _listAll;
    bool _isMeteoGrid;
    std::vector<bool> _isPercentage;
    std::vector<bool> _isAnomalyFromDb; // VB RefType (Period o Clima)
    std::vector<QString> _listAnomalyClimateField;

    std::vector<int> _listYearStart;
    std::vector<int> _listYearEnd;
    std::vector<int> _listRefYearStart;
    std::vector<int> _listRefYearEnd;

    std::vector<meteoVariable> _listVariable;
    std::vector<QString> _listPeriodStr;
    std::vector<period> _listPeriodType;
    std::vector<QString> _listRefPeriodStr;
    std::vector<period> _listRefPeriodType;

    std::vector<QDate> _listDateStart;
    std::vector<QDate> _listDateEnd;
    std::vector<int> _listNYears;
    std::vector<QDate> _listRefDateStart;
    std::vector<QDate> _listRefDateEnd;
    std::vector<int> _listRefNYears;

    std::vector<QString> _listElab1;
    std::vector<float> _listParam1;
    std::vector<bool> _listParam1IsClimate;
    std::vector<QString> _listRefElab1;
    std::vector<float> _listRefParam1;
    std::vector<bool> _listRefParam1IsClimate;

    std::vector<QString> _listParam1ClimateField;
    std::vector<QString> _listElab2;
    std::vector<float> _listParam2;
    std::vector<QString> _listRefParam1ClimateField;
    std::vector<QString> _listRefElab2;
    std::vector<float> _listRefParam2;
};

#endif // CRIT3DANOMALYLIST_H
