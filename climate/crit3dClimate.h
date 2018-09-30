#ifndef DBCLIMATE_H
#define DBCLIMATE_H

    class QString;
    class QDate;
    class QSqlDatabase;

    #ifndef METEO_H
        #include "meteo.h"
    #endif
    #include "elaborationSettings.h"

    enum period{ dailyPeriod, decadalPeriod, monthlyPeriod, seasonalPeriod, annualPeriod, genericPeriod, noPeriodType};

    class Crit3DClimate
    {

    public:
        Crit3DClimate();
        //Crit3DClimate(const Crit3DClimate* reference);
        ~Crit3DClimate();

        void resetParam();

        QSqlDatabase db() const;
        void setDb(const QSqlDatabase &db);

        QString climateElab() const;
        void setClimateElab(const QString &climateElab);

        int yearStart() const;
        void setYearStart(int yearStart);

        int yearEnd() const;
        void setYearEnd(int yearEnd);

        meteoVariable variable() const;
        void setVariable(const meteoVariable &variable);

        QDate genericPeriodDateStart() const;
        void setGenericPeriodDateStart(const QDate &genericPeriodDateStart);

        QDate genericPeriodDateEnd() const;
        void setGenericPeriodDateEnd(const QDate &genericPeriodDateEnd);

        int nYears() const;
        void setNYears(int nYears);

        QString elab1() const;
        void setElab1(const QString &elab1);

        float param1() const;
        void setParam1(float param1);

        bool param1IsClimate() const;
        void setParam1IsClimate(bool param1IsClimate);

        QString param1ClimateField() const;
        void setParam1ClimateField(const QString &param1ClimateField);

        QString elab2() const;
        void setElab2(const QString &elab2);

        float param2() const;
        void setParam2(float param2);

        period periodType() const;
        void setPeriodType(const period &periodType);

        QString periodStr() const;
        void setPeriodStr(const QString &periodStr);

        Crit3DElaborationSettings *getElabSettings() const;
        void setElabSettings(Crit3DElaborationSettings *value);

    private:
        QSqlDatabase _db;
        QString _climateElab;
        int _yearStart;
        int _yearEnd;
        period _periodType;
        meteoVariable _variable;
        QString _periodStr;
        QDate _genericPeriodDateStart;
        QDate _genericPeriodDateEnd;
        int _nYears;
        QString _elab1;
        float _param1;
        bool _param1IsClimate;
        QString _param1ClimateField;
        QString _elab2;
        float _param2;
        Crit3DElaborationSettings *elabSettings;
        meteoVariable _currentVar;
        QString _currentElab1;
        int _currentYearStart;
        int _currentYearEnd;
    };


#endif // DBCLIMATE_H
