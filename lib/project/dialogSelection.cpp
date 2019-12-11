#include <QFileDialog>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListWidget>
#include <QRadioButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QDoubleValidator>
#include <QSettings>
#include <QGridLayout>
#include <QComboBox>
#include <QtWidgets>

#include "commonConstants.h"
#include "dialogSelection.h"
#include "color.h"
#include "utilities.h"


QString editValue(QString windowsTitle, QString defaultValue)
{
    QDialog myDialog;
    QVBoxLayout mainLayout;
    QHBoxLayout layoutValue;
    QHBoxLayout layoutOk;

    myDialog.setWindowTitle(windowsTitle);
    myDialog.setFixedWidth(300);

    QLabel *valueLabel = new QLabel("Value: ");
    QLineEdit valueEdit(defaultValue);
    valueEdit.setFixedWidth(150);

    layoutValue.addWidget(valueLabel);
    layoutValue.addWidget(&valueEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutValue);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() != QDialog::Accepted)
        return "";
    else
        return valueEdit.text();
}


meteoVariable chooseColorScale()
{
    QDialog myDialog;
    QVBoxLayout mainLayout;
    QVBoxLayout layoutVariable;
    QHBoxLayout layoutOk;

    myDialog.setWindowTitle("Choose color scale");
    myDialog.setFixedWidth(400);

    QRadioButton Dem("Elevation");
    QRadioButton Temp("Air temperature");
    QRadioButton Prec("Precipitation");
    QRadioButton RH("Air relative humidity");
    QRadioButton Rad("Solar radiation");
    QRadioButton Wind("Wind intensity");
    QRadioButton Anomaly("Anomaly");

    layoutVariable.addWidget(&Dem);
    layoutVariable.addWidget(&Temp);
    layoutVariable.addWidget(&Prec);
    layoutVariable.addWidget(&RH);
    layoutVariable.addWidget(&Rad);
    layoutVariable.addWidget(&Wind);
    layoutVariable.addWidget(&Anomaly);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutVariable);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() != QDialog::Accepted)
        return noMeteoVar;

    if (Dem.isChecked())
        return noMeteoTerrain;
    else if (Temp.isChecked())
        return airTemperature;
    else if (Prec.isChecked())
        return precipitation;
    else if (RH.isChecked())
        return airRelHumidity;
    else if (Rad.isChecked())
        return globalIrradiance;
    else if (Wind.isChecked())
        return windScalarIntensity;
    else if (Anomaly.isChecked())
        return anomaly;
    else
        return noMeteoTerrain;
}


frequencyType chooseFrequency()
{
    QDialog myDialog;
    QVBoxLayout mainLayout;
    QVBoxLayout layoutFrequency;
    QHBoxLayout layoutOk;

    myDialog.setWindowTitle("Choose frequency");
    myDialog.setFixedWidth(300);

    QRadioButton Daily("Daily");
    QRadioButton Hourly("Hourly");

    layoutFrequency.addWidget(&Daily);
    layoutFrequency.addWidget(&Hourly);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutFrequency);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() != QDialog::Accepted)
        return noFrequency;

   if (Daily.isChecked())
       return daily;
   else if (Hourly.isChecked())
       return hourly;
   else
       return noFrequency;

}


meteoVariable chooseMeteoVariable(Project* myProject)
{
    if (myProject->getCurrentFrequency() == noFrequency)
    {
        QMessageBox::information(nullptr, "No frequency", "Choose frequency before");
        return noMeteoVar;
    }

    QDialog myDialog;
    QVBoxLayout mainLayout;
    QVBoxLayout layoutVariable;
    QHBoxLayout layoutOk;

    myDialog.setWindowTitle("Choose variable");
    myDialog.setFixedWidth(300);

    QRadioButton Tavg("Average air temperature");
    QRadioButton Tmin("Minimum air temperature");
    QRadioButton Tmax("Maximum air temperature");
    QRadioButton Prec("Precipitation");
    QRadioButton RHavg("Average air relative humidity");
    QRadioButton RHmin("Minimum air relative humidity");
    QRadioButton RHmax("Maximum air relative humidity");
    QRadioButton Rad("Solar radiation");
    QRadioButton WindVAvg("Average wind vector intensity");
    QRadioButton WindVMax("Maximum wind vector intensity");
    QRadioButton WindVDir("Prevailing wind vector direction");
    QRadioButton WindSAvg("Average wind scalar intensity");
    QRadioButton WindSMax("Maximum wind scalar intensity");
    QRadioButton DewT("Air dew temperature (°C)");

    QRadioButton T("Air temperature");
    QRadioButton P("Precipitation");
    QRadioButton RH("Air relative humidity");
    QRadioButton Irr("Solar irradiance");
    QRadioButton WSInt("Wind scalar intensity");
    QRadioButton WVInt("Wind vector intensity");
    QRadioButton WVDir("Wind vector direction");

    if (myProject->getCurrentFrequency() == daily)
    {
        layoutVariable.addWidget(&Tmin);
        layoutVariable.addWidget(&Tavg);
        layoutVariable.addWidget(&Tmax);
        layoutVariable.addWidget(&Prec);
        layoutVariable.addWidget(&RHmin);
        layoutVariable.addWidget(&RHavg);
        layoutVariable.addWidget(&RHmax);
        layoutVariable.addWidget(&Rad);
        layoutVariable.addWidget(&WindSAvg);
        layoutVariable.addWidget(&WindSMax);
        layoutVariable.addWidget(&WindVAvg);
        layoutVariable.addWidget(&WindVMax);
        layoutVariable.addWidget(&WindVDir);
    }
    else if (myProject->getCurrentFrequency() == hourly)
    {
        layoutVariable.addWidget(&T);
        layoutVariable.addWidget(&P);
        layoutVariable.addWidget(&RH);
        layoutVariable.addWidget(&Irr);
        layoutVariable.addWidget(&WSInt);
        layoutVariable.addWidget(&WVInt);
        layoutVariable.addWidget(&WVDir);
    }
    else return noMeteoVar;

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutVariable);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() != QDialog::Accepted)
    {
        return noMeteoVar;
    }

   if (myProject->getCurrentFrequency() == daily)
   {
       if (Tmin.isChecked())
           return (dailyAirTemperatureMin);
       else if (Tmax.isChecked())
           return (dailyAirTemperatureMax);
       else if (Tavg.isChecked())
           return (dailyAirTemperatureAvg);
       else if (Prec.isChecked())
           return (dailyPrecipitation);
       else if (Rad.isChecked())
           return (dailyGlobalRadiation);
       else if (RHmin.isChecked())
           return (dailyAirRelHumidityMin);
       else if (RHmax.isChecked())
           return (dailyAirRelHumidityMax);
       else if (RHavg.isChecked())
           return (dailyAirRelHumidityAvg);
       else if (WindSAvg.isChecked())
           return (dailyWindScalarIntensityAvg);
       else if (WindSMax.isChecked())
           return (dailyWindScalarIntensityMax);
       else if (WindVAvg.isChecked())
           return (dailyWindVectorIntensityAvg);
       else if (WindVMax.isChecked())
           return (dailyWindVectorIntensityMax);
       else if (WindVDir.isChecked())
           return (dailyWindVectorDirectionPrevailing);
   }

   if (myProject->getCurrentFrequency() == hourly)
   {
       if (T.isChecked())
           return (airTemperature);
       else if (RH.isChecked())
           return (airRelHumidity);
       else if (P.isChecked())
           return (precipitation);
       else if (Irr.isChecked())
           return (globalIrradiance);
       else if (WSInt.isChecked())
           return (windScalarIntensity);
       else if (WVInt.isChecked())
           return windVectorIntensity;
       else if (WVDir.isChecked())
           return windVectorDirection;
       else if (DewT.isChecked())
           return (airDewTemperature);
   }

   return noMeteoVar;
}


#ifdef NETCDF
    bool chooseNetCDFVariable(NetCDFHandler* netCDF, int* varId, QDateTime* firstDateTime, QDateTime* lastDateTime)
    {
        // check
        if (! netCDF->isLoaded())
        {
            QMessageBox::information(nullptr, "No data", "Load NetCDF before");
            return false;
        }
        if (! netCDF->isTimeReadable())
        {
            QMessageBox::information(nullptr, "Wrong time", "Needs POSIX time (seconds since 1970-01-01)");
            return false;
        }

        QDialog myDialog;
        QVBoxLayout mainLayout;
        QVBoxLayout layoutVariable;
        QHBoxLayout layoutDate;
        QHBoxLayout layoutOk;
        QDateTime firstTime, lastTime;

        myDialog.setWindowTitle("Export NetCDF data series");

        // Variables
        QLabel *VariableLabel = new QLabel("<b>Variable:</b>");
        layoutVariable.addWidget(VariableLabel);

        unsigned int nrVariables = netCDF->getNrVariables();
        std::vector<QRadioButton*> buttonVars;

        for (unsigned int i = 0; i < nrVariables; i++)
        {
            QString varName = QString::fromStdString(netCDF->variables[i].getVarName());
            buttonVars.push_back(new QRadioButton(varName));

            layoutVariable.addWidget(buttonVars[i]);
        }

        // void space
        layoutVariable.addWidget(new QLabel());

        // date widgets
        Crit3DTime t0, t1;
        t0 = netCDF->getFirstTime();
        t1 = netCDF->getLastTime();
        firstTime = getQDateTime(t0);
        lastTime = getQDateTime(t1);
        QString str1, str2;
        str1 = firstTime.toString();
        str2 = lastTime.toString();

        QDateTimeEdit *firstDateEdit = new QDateTimeEdit;
        firstDateEdit->setDateTimeRange(firstTime, lastTime);
        firstDateEdit->setDateTime(firstTime);

        QLabel *firstDateLabel = new QLabel("<b>First Date:</b>");
        firstDateLabel->setBuddy(firstDateEdit);

        QDateTimeEdit *lastDateEdit = new QDateTimeEdit;
        lastDateEdit->setDateTimeRange(firstTime, lastTime);
        lastDateEdit->setDateTime(lastTime);

        QLabel *lastDateLabel = new QLabel("<b>Last Date:</b>");
        lastDateLabel->setBuddy(lastDateEdit);

        layoutDate.addWidget(firstDateLabel);
        layoutDate.addWidget(firstDateEdit);

        layoutDate.addWidget(lastDateLabel);
        layoutDate.addWidget(lastDateEdit);

        //void space
        layoutDate.addWidget(new QLabel());

        //Ok button
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
        myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));
        layoutOk.addWidget(&buttonBox);

        // Main layout
        mainLayout.addLayout(&layoutVariable);
        mainLayout.addLayout(&layoutDate);
        mainLayout.addLayout(&layoutOk);

        myDialog.setLayout(&mainLayout);
        myDialog.exec();

        if (myDialog.result() != QDialog::Accepted)
            return false;

        // assing values
        *firstDateTime = firstDateEdit->dateTime();
        *lastDateTime = lastDateEdit->dateTime();

        bool isVarSelected = false;
        unsigned int i = 0;
        while (i < nrVariables && ! isVarSelected)
        {
            if (buttonVars[i]->isChecked())
            {
               *varId = netCDF->variables[i].id;
                isVarSelected = true;
            }
            i++;
        }

        return isVarSelected;
    }

#endif




