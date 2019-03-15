#include <stdio.h>
#include <math.h>
#include "meteoPoint.h"

float sommatoriaGradiGiorno(int anno, TObsDataD** osservato, int lengthSeries, int fineMese, int fineGiorno,int fineAnno, int stazione)
{
    int somma = 0;
    int dataIniziale=0;
    int dataFinale = 0;
    bool stop = false;

    while (osservato[stazione][dataIniziale].date.year < anno)
    {
        dataIniziale++;
        dataFinale++;
        //printf("oseervato %d %d %d\n", osservato[stazione][dataIniziale].date.day, osservato[stazione][dataIniziale].date.month, osservato[stazione][dataIniziale].date.year);
    }
    while (osservato[stazione][dataIniziale].date.year <= anno && osservato[stazione][dataIniziale].date.month != 7)
    {
        dataIniziale++;
        dataFinale++;
        //printf("oseervato %d %d %d\n", osservato[stazione][dataIniziale].date.day, osservato[stazione][dataIniziale].date.month, osservato[stazione][dataIniziale].date.year);
    }
    for (int j=dataIniziale; j<lengthSeries ; j++)
    {
        if (stop == false)
        {
            dataFinale++;
            if (osservato[stazione][dataFinale].date.year == fineAnno && osservato[stazione][dataFinale].date.month == fineMese && osservato[stazione][dataFinale].date.day == fineGiorno)
            {
                stop = true;
            }

        }
    }
    float tempUltimi3Giorni[3] = {30,30,30};
    bool periodoAttivo = false;
    for (int i=dataIniziale; i<=dataFinale;i++)
    {
        float tmed = (osservato[stazione][i].tMin + osservato[stazione][i].tMax)*0.5;
        tempUltimi3Giorni[0]=tempUltimi3Giorni[1];
        tempUltimi3Giorni[1]=tempUltimi3Giorni[2];
        tempUltimi3Giorni[2]= tmed;
        if (tempUltimi3Giorni[0] <12 && tempUltimi3Giorni[1]<12 && tempUltimi3Giorni[2]< 12)
        {
            periodoAttivo = true;
        }
        if (osservato[stazione][i].date.month == 12 && osservato[stazione][i].date.month == 1)
        {
            periodoAttivo = true;
        }
        if (periodoAttivo == true && tempUltimi3Giorni[0] >12 && tempUltimi3Giorni[1]>12 && tempUltimi3Giorni[2]> 12 && osservato[stazione][i].date.month != 1 && osservato[stazione][i].date.month != 2 && osservato[stazione][i].date.month != 12 && osservato[stazione][i].date.month != 11)
        {
            periodoAttivo = false;
        }
        if (periodoAttivo && tmed < 20)
        {
            somma += (20.0 - tmed);
        }
    }
    return somma;
}
