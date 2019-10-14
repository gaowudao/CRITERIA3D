#ifndef CRIT3DDATE_H
#define CRIT3DDATE_H


    #ifndef _STRING_
        #include <string>
    #endif

    #define NO_DATE Crit3DDate(0,0,0);

    class Crit3DDate
    {
    public:
        int day;
        int month;
        int year;

        Crit3DDate();
        Crit3DDate(int myDay, int myMonth, int myYear);
        Crit3DDate(std::string myDate);

        friend bool operator == (const Crit3DDate& myFirstDate, const Crit3DDate& mySecondDate);
        friend bool operator != (const Crit3DDate& myFirstDate, const Crit3DDate& mySecondDate);
        friend bool operator >  (const Crit3DDate& myFirstDate, const Crit3DDate& mySecondDate);
        friend bool operator >= (const Crit3DDate& myFirstDate, const Crit3DDate& mySecondDate);
        friend bool operator <  (const Crit3DDate& myFirstDate, const Crit3DDate& mySecondDate);
        friend bool operator <= (const Crit3DDate& myFirstDate, const Crit3DDate& mySecondDate);

        friend Crit3DDate& operator ++ (Crit3DDate& myFirstDate);
        friend Crit3DDate& operator -- (Crit3DDate& myFirstDate);

        Crit3DDate addDays(int myLong) const;
        int daysTo(const Crit3DDate& myDate) const;
        std::string toStdString();
    };


    class Crit3DTime
    {
    public:
        Crit3DDate date;
        int time;

        Crit3DTime();
        Crit3DTime(Crit3DDate myDate, int myTime);

        friend bool operator > (const Crit3DTime& myFirstTime, const Crit3DTime& mySecondTime);
        friend bool operator < (const Crit3DTime& myFirstTime, const Crit3DTime& mySecondTime);
        friend bool operator >= (const Crit3DTime& myFirstTime, const Crit3DTime& mySecondTime);
        friend bool operator <= (const Crit3DTime& myFirstTime, const Crit3DTime& mySecondTime);
        friend bool operator == (const Crit3DTime& myFirstTime, const Crit3DTime& mySecondTime);
        friend bool operator != (const Crit3DTime& myFirstTime, const Crit3DTime& mySecondTime);

        Crit3DTime addSeconds(int mySeconds) const;
        bool isEqual(const Crit3DTime&) const;

        int getHour() const;
        int getMinutes() const;
        int getSeconds() const;
        std::string toStdString();
    };


    // functions
    Crit3DTime getNullTime();
    Crit3DDate getNullDate();
    bool isNullDate(Crit3DDate);
    bool isNullTime(const Crit3DTime& myTime);

    bool isLeapYear(int year);
    int getDaysInMonth(int month, int year);

    int getDoyFromDate(const Crit3DDate& myDate);
    Crit3DDate getDateFromDoy(int myYear, int myDoy);

    Crit3DDate max(const Crit3DDate& myDate1, const Crit3DDate& myDate2);
    Crit3DDate min(const Crit3DDate& myDate1, const Crit3DDate& myDate2);

    int difference(Crit3DDate myDate1, Crit3DDate myDate2);

#endif // CRIT3DDATE_H
