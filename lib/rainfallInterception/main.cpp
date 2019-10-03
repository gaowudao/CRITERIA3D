#include <QCoreApplication>
#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "models.h"
#define NODATA -9999
void readWeatherData(int*, double*, double*,double*);
void readWeatherDataValidation(int*, double*, double*,double*);
double laiEstimation(int doy);
int main()
{
    //double test;
    //test = rainfallCanopyInterceptionHydrall(5.0,0.5,0.9,12.);
    //printf("%f \n", test*24);
    //bool waterManagement(double* StoredWater, double* throughfallWater, double rainfall,double waterFreeEvaporation,double lai,double extinctionCoefficient,double leafStorage,double stemStorage);
    int dateCampionamento[16]={218,225,252,281,307,62,103,110,126,159,188,216,250,271,300,327};
    double cumulatoCampionamento[15];
    double cumulatoFreeThroughFall[15];
    double cumulatoStemFlow[15];
    int timeSerieLength = 504;
    int* dayOfYear = (int*)calloc(timeSerieLength, sizeof(int));
    double* precipitationDaily = (double*)calloc(timeSerieLength, sizeof(double));
    double* evaporationDaily = (double*)calloc(timeSerieLength, sizeof(double));
    double* precipitationHourly = (double*)calloc(timeSerieLength*24, sizeof(double));
    readWeatherData(dayOfYear,precipitationDaily,evaporationDaily,precipitationHourly);
    double drainage;
    FILE *fp;
    fp = fopen("calibration.csv","w");
    fprintf(fp,"Extcoef,leafStorage,stemStorage,MBE,RMSE\n");

    bool calibrazione;
    calibrazione = true;
    if (calibrazione)
    {
        for (int cal1=-0;cal1<1;cal1++)
        {
        for (int cal2=-0;cal2<1;cal2++)
        {
        for (int cal3=-0;cal3<1;cal3++)
        {
            //int cal1,cal2,cal3;
            double rainfall;
            double freeWaterEvaporation;
            double lai;
            double laiMin = 1.5;
            //int doy;
            double storedWater = 0.1;
            double throughfallWater = 0;
            double extinctionCoefficient = 0.69+cal1*0.01;
            double leafStorage = 0.3+cal2*0.01;
            double stemStorage = 0.105+cal3*0.01; // per gli alberi deve essere molto di più fino a 0.2 per il tiglio vedi pubblicazione
            //double stemStorage = 1;
            double maxStemFlowRate = 0.15;
            double freeRainfallCumulated = 0;
            double rainfallCumulated = 0;
            double stemflowCumulated = 0;
            double freeRainfall;
            double stemFlow;
            double throughfallCumulated=0;
            int contatoreCumulato = 0;


            //leafStorage = 0.49;
            //stemStorage = 0.385;
            //extinctionCoefficient = 0.99;
                for (int i=0;i<15;i++)
                {
                    cumulatoCampionamento[i]=0;
                    cumulatoFreeThroughFall[i]=0;
                    cumulatoStemFlow[i]=0.;
                }
                for (int day=0; day<timeSerieLength; day++)
                //for (int day=0; day<2; day++)
                {
                    lai = laiEstimation(dayOfYear[day]);
                    //printf("doy = %d, lai = %.1f\n",dayOfYear[day],lai);
                    //rainfall = precipitationDaily[day]/24.;

                    freeWaterEvaporation = evaporationDaily[day]/24.;
                    for(int hour=0;hour<24;hour++)
                    {
                        rainfall = precipitationHourly[day*24+hour];
                        //printf("doy = %d, hour = %d, prec = %.1f\n",dayOfYear[day],hour,precipitationHourly[day*24+hour]);
                        canopy::waterManagementCanopy(&storedWater,&throughfallWater,rainfall,freeWaterEvaporation,lai,extinctionCoefficient,leafStorage,stemStorage,maxStemFlowRate,&freeRainfall,&drainage,&stemFlow,laiMin);
                        rainfallCumulated += rainfall;
                        freeRainfallCumulated += freeRainfall;
                        throughfallCumulated += throughfallWater;
                        stemflowCumulated += drainage - throughfallWater + freeRainfall;
                        cumulatoCampionamento[contatoreCumulato]+= rainfall;
                        cumulatoFreeThroughFall[contatoreCumulato]+= throughfallWater;
                        cumulatoStemFlow[contatoreCumulato]+= stemFlow;
                    }
                    double variable;
                    if (dayOfYear[day]-dateCampionamento[contatoreCumulato+1] > 0)
                        variable = dayOfYear[day]-dateCampionamento[contatoreCumulato+1];
                    else
                        variable = -dayOfYear[day]+dateCampionamento[contatoreCumulato+1];

                    if (variable< 90)
                    {
                        if (dayOfYear[day]> dateCampionamento[contatoreCumulato+1]) contatoreCumulato++;
                    }
                    //printf("%d  %d\n",dayOfYear[day],contatoreCumulato);
                    //getchar();
                    //printf("tot. %.1f, freeflow %.1f, ground %.1f, evap. %.2f\n",rainfallCumulated,freeRainfallCumulated,throughfallCumulated,rainfallCumulated-throughfallCumulated);
                }
                double somma=0;
                for (int i=0;i<15;i++)
                {
                    somma += cumulatoCampionamento[i];
                    //printf("doy %d tag %d rain %.1f t %.1f \n",dateCampionamento[i+1],i,cumulatoCampionamento[i],cumulatoFreeThroughFall[i]);
                }

                double experimentalData[7]={59.2,43.4,15.7,29.8,14.4,25.5,20.9};
                double experimentalDataStemFlow[7]={NODATA,NODATA,0.8,NODATA,1.4,5,4.2};
                double waterFunnel[7]={85.4,52.7,18.7,41.4,18.1,32.8,25.7};
                double simulatedData[7]={cumulatoFreeThroughFall[1],cumulatoFreeThroughFall[3],cumulatoFreeThroughFall[7],cumulatoFreeThroughFall[10],cumulatoFreeThroughFall[12],cumulatoFreeThroughFall[13],cumulatoFreeThroughFall[14]};
                double waterMonticolo[7]={cumulatoCampionamento[1],cumulatoCampionamento[3],cumulatoCampionamento[7],cumulatoCampionamento[10],cumulatoCampionamento[12],cumulatoCampionamento[13],cumulatoCampionamento[14]};
                double simulatedDataStemFlow[7]={cumulatoStemFlow[1],cumulatoStemFlow[3],cumulatoStemFlow[7],cumulatoStemFlow[10],cumulatoStemFlow[12],cumulatoStemFlow[13],cumulatoStemFlow[14]};
                double MBE=0;
                double RMSE=0;

                for (int i=0;i<7;i++)
                {
                    //printf("exp. %.1f sim. %.1f\n",experimentalData[i],simulatedData[i]);
                    //printf("exp. %.1f sim. %.1f\n",waterFunnel[i],(waterMonticolo[i]-waterFunnel[i])/waterFunnel[i]*100);
                    simulatedData[i] *= (1-(waterMonticolo[i]-waterFunnel[i])/waterFunnel[i]);
                    simulatedDataStemFlow[i]  *= (1-(waterMonticolo[i]-waterFunnel[i])/waterFunnel[i]);
                    //printf("exp. %.1f sim. %.1f stemFlow %.2f\n",experimentalData[i],simulatedData[i],simulatedDataStemFlow[i]);
                    printf("%f %f %f %f %f\n",waterFunnel[i],experimentalData[i],simulatedData[i],experimentalDataStemFlow[i],simulatedDataStemFlow[i]);
                    //printf("exp. %.1f sim. %.1f stemFlow exp %.2f sim %.2f\n",experimentalData[i],simulatedData[i],experimentalDataStemFlow[i],simulatedDataStemflow[i]);

                    //MBE += simulatedData[i] - experimentalData[i];
                    //RMSE += (simulatedData[i] - experimentalData[i])*(simulatedData[i] - experimentalData[i])/6.;
                    if (experimentalDataStemFlow[i] != NODATA)
                    {
                        MBE += (simulatedDataStemFlow[i] - experimentalDataStemFlow[i])/experimentalDataStemFlow[i]*100 ;
                        RMSE += (simulatedDataStemFlow[i] - experimentalDataStemFlow[i])*(simulatedDataStemFlow[i] - experimentalDataStemFlow[i])/4.;
                    }
                }
                RMSE = sqrt(RMSE);
                //fprintf(fp,"%f,%f,%f,%.3f,%.3f\n",extinctionCoefficient,leafStorage,stemStorage,MBE/6,RMSE);

        /*
            // test for litter
            for (int hours=0; hours<3; hours++)
            {
                // entrambe le funzioni sono da chiamare la prima calcola quanto è la traspirazione risparmiata la seconda il management della pioggia
                litter::evaporationMulching(litter::storageCapacity(doy));
                litter::waterManagementLitter(&storedWater,&throughfallWater,rainfall,freeWaterEvaporation,litter::storageCapacity(doy));

                //printf("stored=%f\t throughfall=%f\n",storedWater,throughfallWater);
                rainfall = 0;
            }
        */

            }
            }
            }
    }

    else
    {
        /* procedura per validazione:
         * 1)calcolare il cumulato di Monticolo pluviometro
         * 2) comparare con i "pluviometri" fatti solo con l'imbuto
         * 3) quando questi sono simili si fa l'analisi
         * 4) calcolo MBE,RMSE

*/
        for (int cal1=-0;cal1<1;cal1++)
        {
        for (int cal2=-0;cal2<1;cal2++)
        {
        for (int cal3=-0;cal3<1;cal3++)
        {


            double rainfall;
            double freeWaterEvaporation;
            double lai;
            double laiMin = 1.5;
            //int doy;
            double storedWater = 0.1;
            double throughfallWater = 0;
            double extinctionCoefficient = 0.69 + 0.02*cal1;
            double leafStorage = 0.3 + 0.03*cal2;
            double stemStorage = 0.105 + 0.01*cal3; // per gli alberi deve essere molto di più fino a 0.2 per il tiglio vedi pubblicazione
            double maxStemFlowRate = 0.15;
            double freeRainfallCumulated = 0;
            double stemFlowCumulated = 0;
            double rainfallCumulated = 0;
            double freeRainfall;
            double stemFlow;
            double throughfallCumulated=0;
            int contatoreCumulato = 0;
            int nrDateCampionamento = 8;

            int* dateCampionamentoValidazione=(int*)calloc(nrDateCampionamento, sizeof(int));
            dateCampionamentoValidazione[0]=357;
            dateCampionamentoValidazione[1]=67;
            dateCampionamentoValidazione[2]=130;
            dateCampionamentoValidazione[3]=165;
            dateCampionamentoValidazione[4]=185;
            dateCampionamentoValidazione[5]=212;
            dateCampionamentoValidazione[6]=253;
            dateCampionamentoValidazione[7]=256;
            double* cumulatoCampionamentoValidazione =(double*)calloc(nrDateCampionamento-1, sizeof(double));
            double* cumulatoFreeThroughFallValidazione =(double*)calloc(nrDateCampionamento-1, sizeof(double));
            int timeSerieLengthValidazione = 296; //14208

            int* dayOfYearValidation = (int*)calloc(timeSerieLengthValidazione, sizeof(int));
            double* precipitationDailyValidation = (double*)calloc(timeSerieLengthValidazione, sizeof(double));
            double* evaporationDailyValidation = (double*)calloc(timeSerieLengthValidazione, sizeof(double));
            double* precipitationHourlyValidation = (double*)calloc(timeSerieLengthValidazione*24, sizeof(double));


            readWeatherDataValidation(dayOfYearValidation,precipitationDailyValidation,evaporationDailyValidation,precipitationHourlyValidation);

            for (int i=0;i<nrDateCampionamento-1;i++)
            {
                cumulatoCampionamentoValidazione[i]=0;
                cumulatoFreeThroughFallValidazione[i]=0;
                cumulatoStemFlow[i]=0;
            }

            for (int day=0; day<timeSerieLengthValidazione; day++)
            //for (int day=0; day<2; day++)
            {
                lai = laiEstimation(dayOfYearValidation[day]);
                //printf("doy = %d, lai = %.1f\n",dayOfYearValidation[day],lai);
                //rainfall = precipitationDailyValidation[day]/24.;

                freeWaterEvaporation = evaporationDailyValidation[day]/24.;
                for(int hour=0;hour<24;hour++)
                {
                    rainfall = precipitationHourlyValidation[day*24+hour];
                    //printf("doy = %d, hour = %d, prec = %.1f\n",dayOfYearValidation[day],hour,precipitationHourlyValidation[day*24+hour]);
                    canopy::waterManagementCanopy(&storedWater,&throughfallWater,rainfall,freeWaterEvaporation,lai,extinctionCoefficient,leafStorage,stemStorage,maxStemFlowRate,&freeRainfall,&drainage,&stemFlow,laiMin);
                    rainfallCumulated += rainfall;
                    freeRainfallCumulated += freeRainfall;
                    throughfallCumulated += throughfallWater;
                    stemFlowCumulated += drainage - throughfallWater + freeRainfall;
                    cumulatoCampionamentoValidazione[contatoreCumulato]+= rainfall;
                    cumulatoFreeThroughFallValidazione[contatoreCumulato]+= throughfallWater;
                    cumulatoStemFlow[contatoreCumulato]+= stemFlow;

                }
                double variable;
                if ((dayOfYearValidation[day]-dateCampionamentoValidazione[contatoreCumulato+1])>0)
                    variable = dayOfYearValidation[day]-dateCampionamentoValidazione[contatoreCumulato+1];
                else
                    variable = -dayOfYearValidation[day]+dateCampionamentoValidazione[contatoreCumulato+1];

                if (variable< 90)
                {
                    if (dayOfYearValidation[day]> dateCampionamentoValidazione[contatoreCumulato+1]) contatoreCumulato++;
                }
                //printf("%d  %f %f \n",dayOfYearValidation[day],rainfallCumulated,cumulatoCampionamentoValidazione[contatoreCumulato]);
                //getchar();
                //printf("tot. %.1f, freeflow %.1f, ground %.1f, evap. %.1f\n",rainfallCumulated,freeRainfallCumulated,throughfallCumulated,rainfallCumulated-throughfallCumulated);
            }

            double experimentalData[4]={31.9,48.1,45.8,104.6};
            double experimentalDataStemFlow[4]={4.5,7.5,3.7,NODATA};
            double waterFunnel[4]={43.5,68.2,68.7,147.4};
            double simulatedData[4]={cumulatoFreeThroughFallValidazione[0],cumulatoFreeThroughFallValidazione[3],cumulatoFreeThroughFallValidazione[4],cumulatoFreeThroughFallValidazione[5]};
            double waterMonticolo[4]={cumulatoCampionamentoValidazione[0],cumulatoCampionamentoValidazione[3],cumulatoCampionamentoValidazione[4],cumulatoCampionamentoValidazione[5]};
            double simulatedDataStemflow[4]= {cumulatoStemFlow[0],cumulatoStemFlow[3],cumulatoStemFlow[4],cumulatoStemFlow[5]};
            //printf("%f %f %f %f\n",cumulatoStemFlow[0],cumulatoStemFlow[3],cumulatoStemFlow[4],cumulatoStemFlow[5]);
            //double MBE=0;
            //double RMSE=0;

            for (int i=0;i<4;i++)
            {
                //printf("misurato %.1f pluviometro %.1f\n",waterFunnel[i],waterMonticolo[i]);
                //printf("exp. %.1f sim. %.1f\n",experimentalData[i],simulatedData[i]);
                //printf("exp. %.1f sim. %.1f\n",waterFunnel[i],(waterMonticolo[i]-waterFunnel[i])/waterFunnel[i]*100);
                simulatedData[i] *= (1-(waterMonticolo[i]-waterFunnel[i])/waterFunnel[i]);
                simulatedDataStemflow[i] *= (1-(waterMonticolo[i]-waterFunnel[i])/waterFunnel[i]);
                //printf("exp. %.1f sim. %.1f stemFlow exp %.2f sim %.2f\n",experimentalData[i],simulatedData[i],experimentalDataStemFlow,simulatedDataStemflow[i]);
                printf("%f %f %f %f %f\n",waterFunnel[i],experimentalData[i],simulatedData[i],experimentalDataStemFlow[i],simulatedDataStemflow[i]);
                //MBE += simulatedData[i] - experimentalData[i];
                //RMSE += (simulatedData[i] - experimentalData[i])*(simulatedData[i] - experimentalData[i])/3.;
                //if (experimentalDataStemFlow[i] != NODATA)
                //{
                    //MBE += (simulatedDataStemflow[i] - experimentalDataStemFlow[i]);
                    //RMSE += (simulatedDataStemflow[i] - experimentalDataStemFlow[i])*(simulatedDataStemflow[i] - experimentalDataStemFlow[i])/3;
                //}
            }
            //RMSE = sqrt(RMSE);
            //fprintf(fp,"%f,%f,%f,%.3f,%.3f\n",extinctionCoefficient,leafStorage,stemStorage,MBE/3,RMSE);



    }
    }
    }


    } // chiusura dell'else



    fclose(fp);
    return 0;
}

double laiEstimation(int doy)
{
    double lai;
    if (doy <= 100)
    {
        lai = 0.1;
    }
    else if (doy >= 330)
    {
        lai = 0.1;
    }
    else if (doy >= 150 && doy <= 250)
    {
        lai = 3.6;
    }
    else
    {
        if (doy < 150)
        {
            lai = 0.1 + 0.07*(doy-100);
        }
        else
        {
            lai = 3.6 - 0.043*(doy-250);
        }
    }
    return lai;

}
void readWeatherData(int* DOY, double* PREC, double* EVAP, double* precipitationHourly)
{
    int doy[504];
    double prec[504];
    double evap[504];
    for (int i=0; i<504;i++)
    {
        PREC[i] = prec[i];
        DOY[i] = doy[i];
        EVAP[i] = evap[i];
    }
    double halfHourlyPrec[24192];    for (int i=0;i<12096;i++)
    {
        precipitationHourly[i]=halfHourlyPrec[2*i]+halfHourlyPrec[2*i+1];
    }
}

void readWeatherDataValidation(int* DOY, double* PREC, double* EVAP, double* precipitationHourly)
{
    double halfHourlyPrec[14208];
    for (int i=0;i<7104;i++)
    {
        precipitationHourly[i]=halfHourlyPrec[2*i]+halfHourlyPrec[2*i+1];
    }
    int doy[296];
    double prec[296];
    double evap[296];
    for (int i=0; i<296;i++)
    {
        PREC[i] = prec[i];
        DOY[i] = doy[i];
        EVAP[i] = evap[i];
    }
}


