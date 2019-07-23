/*!
    CRITERIA3D

    \copyright 2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    You should have received a copy of the GNU General Public License
    along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by A.R.P.A. Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/

#include <math.h>

#include "crit3dDate.h"
#include "commonConstants.h"


const long daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


// index: 1 - 12
int getDaysInMonth(int month, int year)
{
    if (month < 1 || month > 12)
        return (-1);

    if(month == 2 && isLeapYear(year))
        return 29;

    else return daysInMonth[month-1];
}


Crit3DDate::Crit3DDate()
{
    day = 0; month = 0; year = 0;
}

Crit3DDate::Crit3DDate(int myDay, int myMonth, int myYear)
{
    day = myDay; month = myMonth; year = myYear;
}

bool operator == (const Crit3DDate& myDate1, const Crit3DDate& myDate2)
{
    return myDate1.year == myDate2.year && myDate1.month == myDate2.month && myDate1.day == myDate2.day;
}

bool operator != (const Crit3DDate& myDate1, const Crit3DDate& myDate2)
{
    return ! ( myDate1.year == myDate2.year && myDate1.month == myDate2.month && myDate1.day == myDate2.day );
}

bool operator > (const Crit3DDate& myDate1, const Crit3DDate& myDate2)
{
    return myDate1.year > myDate2.year ||
        ( myDate1.year == myDate2.year && myDate1.month > myDate2.month ) ||
        ( myDate1.year == myDate2.year && myDate1.month == myDate2.month && myDate1.day > myDate2.day );
}

bool operator >= (const Crit3DDate& myDate1, const Crit3DDate& myDate2)
{
    return myDate1.year > myDate2.year ||
         ( myDate1.year == myDate2.year && myDate1.month > myDate2.month ) ||
         ( myDate1.year == myDate2.year && myDate1.month == myDate2.month && myDate1.day >= myDate2.day );
}

bool operator < (const Crit3DDate& myDate1, const Crit3DDate& myDate2)
{
    return myDate1.year < myDate2.year ||
        ( myDate1.year == myDate2.year && myDate1.month < myDate2.month ) ||
        ( myDate1.year == myDate2.year && myDate1.month == myDate2.month && myDate1.day < myDate2.day );
}

bool operator <= (const Crit3DDate& myDate1, const Crit3DDate& myDate2)
{
    return myDate1.year < myDate2.year ||
           ( myDate1.year == myDate2.year && myDate1.month < myDate2.month ) ||
           ( myDate1.year == myDate2.year && myDate1.month == myDate2.month && myDate1.day <= myDate2.day );
}

Crit3DDate& operator ++ (Crit3DDate& myDate)
{
    long leap = 0;

    if(myDate.month == 2 && isLeapYear(myDate.year))
      leap = 1; // anni bisestili

    if( myDate.day < daysInMonth[myDate.month - 1] + leap )
        myDate.day++;
    else
    {
        if( myDate.month < 12)
        {
            myDate.day = 1;
            myDate.month++;
        }
        else
        {
            myDate.day = 1;
            myDate.month = 1;
            myDate.year++;
        }
    }
    return myDate;
}

Crit3DDate& operator -- (Crit3DDate& myDate)
{
    long leap = 0;

    if( myDate.month == 3 && isLeapYear(myDate.year))
      leap = 1; // anni bisestili

    if( myDate.day > 1 )
        myDate.day--;
    else
    {
        if( myDate.month > 1 )
        {
            myDate.month--;
            myDate.day = daysInMonth[myDate.month-1] + leap;
        }
        else
        {
            myDate.month = 12;
            myDate.day = daysInMonth[myDate.month-1];
            myDate.year--;
        }
    }

    return myDate;
}

Crit3DDate Crit3DDate::addDays(int nrDays) const
{
    Crit3DDate myDate = *this;
    if (nrDays >= 0)
    {
        for (int i=0; i<nrDays; i++)
            ++myDate;
    }
    else
    {
        for (int i=0; i>nrDays; i--)
            --myDate;
    }

    return (myDate);
}

int Crit3DDate::daysTo(const Crit3DDate& myDate) const
{
    Crit3DDate myDateMin = min(*this, myDate);
    Crit3DDate myDateMax = max(*this, myDate);

    int myDiff = 0;
    while (myDateMin < myDateMax)
    {
        myDiff++;
        ++myDateMin;
    }

    if (myDateMax == myDate)
        return (myDiff);
    else
        return (-myDiff);
}

Crit3DDate max(const Crit3DDate& myDate1, const Crit3DDate& myDate2)
{
    if( myDate1.year > myDate2.year ||
        ( myDate1.year == myDate2.year && myDate1.month > myDate2.month ) ||
        ( myDate1.year == myDate2.year && myDate1.month == myDate2.month && myDate1.day > myDate2.day ) )
        return myDate1;
    else
        return myDate2;
}

Crit3DDate min(const Crit3DDate& myDate1, const Crit3DDate& myDate2)
{
    if( myDate1.year < myDate2.year ||
        ( myDate1.year == myDate2.year && myDate1.month < myDate2.month ) ||
        ( myDate1.year == myDate2.year && myDate1.month == myDate2.month && myDate1.day < myDate2.day ) )
        return myDate1;
    else
        return myDate2;
}

Crit3DDate getDateFromDoy(int myYear, int myDoy)
{
    if (myDoy > 366)
    {
        return NO_DATE;
    }
    if (myDoy == 366 && isLeapYear(myYear) == false)
    {
        return NO_DATE;
    }
    Crit3DDate myDate(1, 1, myYear);
    myDate = myDate.addDays(myDoy - 1);
    return (myDate);
}

Crit3DDate getNullDate()
{
    Crit3DDate* myDate = new Crit3DDate();
    return (*myDate);
}

bool isNullDate(Crit3DDate myDate)
{
    return ((myDate.day == 0) && (myDate.month == 0) && (myDate.year == 0));
}


int difference(Crit3DDate myDatefirst, Crit3DDate myDatelast)
{

    int myDiff = 0;
    while (myDatefirst < myDatelast)
    {
        myDiff++;
        ++myDatefirst;
    }

    return myDiff;
}


bool isLeapYear(int year)
{
    bool leap = false ;
    if (year%4 == 0)
    {
      leap = true;
      if (year%100 == 0)
          if (! (year%400 == 0)) leap = false;
    }
    return leap ;
}


int getDoyFromDate(const Crit3DDate& myDate)
{
    int myDoy = 0;

    if(myDate.month > 2 && isLeapYear(myDate.year))
      myDoy = 1;

    for(int month = 0; month < myDate.month - 1; month++)
      myDoy += daysInMonth[month];

    myDoy += myDate.day;

    return myDoy;
}


std::string Crit3DDate::toStdString()
{
    char myStr[11];
    sprintf (myStr, "%d-%02d-%02d", this->year, this->month, this->day);

    return std::string(myStr);

}

