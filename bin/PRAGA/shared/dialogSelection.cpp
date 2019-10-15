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

    QRadioButton Dem("Elevation m");
    QRadioButton Temp("Temperature °C");
    QRadioButton Prec("Precipitation mm");
    QRadioButton RH("Relative humidity %");
    QRadioButton Rad("Solar radiation MJ m-2");
    QRadioButton Wind("Wind intensity m s-1");
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
        return windIntensity;
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

    QRadioButton Tavg("Average temperature (°C)");
    QRadioButton Tmin("Minimum temperature (°C)");
    QRadioButton Tmax("Maximum temperature (°C)");
    QRadioButton Prec("Precipitation (mm)");
    QRadioButton RHavg("Average relative humidity (%)");
    QRadioButton RHmin("Minimum relative humidity (%)");
    QRadioButton RHmax("Maximum relative humidity (%)");
    QRadioButton Rad("Solar radiation (MJ m-2)");
    QRadioButton Wind("Average wind intensity (m s-1)");
    QRadioButton DewT("Air dew temperature (°C)");

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
        layoutVariable.addWidget(&Wind);
    }
    else if (myProject->getCurrentFrequency() == hourly)
    {
        Tavg.setText("Average temperature (°C)");
        Prec.setText("Precipitation (mm)");
        RHavg.setText("Average relative humidity (%)");
        Rad.setText("Solar irradiance (W m-2)");

        layoutVariable.addWidget(&Tavg);
        layoutVariable.addWidget(&Prec);
        layoutVariable.addWidget(&RHavg);
        layoutVariable.addWidget(&Rad);
        layoutVariable.addWidget(&Wind);
        layoutVariable.addWidget(&DewT);
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
        return noMeteoVar;

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
       else if (Wind.isChecked())
           return (dailyWindIntensityAvg);
   }
   else if (myProject->getCurrentFrequency() == hourly)
   {
       if (Tavg.isChecked())
           return (airTemperature);
       else if (RHavg.isChecked())
           return (airRelHumidity);
       else if (Prec.isChecked())
           return (precipitation);
       else if (Rad.isChecked())
           return (globalIrradiance);
       else if (Wind.isChecked())
           return (windIntensity);
       else if (DewT.isChecked())
           return (airDewTemperature);
   }
   else
       return noMeteoVar;

}


#ifdef NETCDF
    bool chooseNetCDFVariable(NetCDFHandler* netCDF, int* varId, QDateTime* firstDateTime, QDateTime* lastDateTime)
    {
        // check
        if (! netCDF->isLoaded)
        {
            QMessageBox::information(nullptr, "No data", "Load NetCDF before");
            return false;
        }
        if (! netCDF->isStandardTime && ! netCDF->isHourly)
        {
            QMessageBox::information(nullptr, "Wrong time", "Reads only POSIX standard (seconds since 1970-01-01)");
            return false;
        }

        QDialog myDialog;
        QVBoxLayout mainLayout;
        QVBoxLayout layoutVariable;
        QHBoxLayout layoutDate;
        QHBoxLayout layoutOk;

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

        //void space
        layoutVariable.addWidget(new QLabel());

        //Date widgets
        *firstDateTime = getQDateTime(netCDF->getFirstTime());
        *lastDateTime = getQDateTime(netCDF->getLastTime());

        QDateTimeEdit *firstYearEdit = new QDateTimeEdit;
        firstYearEdit->setDateTimeRange(*firstDateTime, *lastDateTime);
        firstYearEdit->setTimeSpec(Qt::UTC);
        firstYearEdit->setDateTime(*lastDateTime);

        QLabel *firstDateLabel = new QLabel("<b>First Date:</b>");
        firstDateLabel->setBuddy(firstYearEdit);

        QDateTimeEdit *lastYearEdit = new QDateTimeEdit;
        lastYearEdit->setDateTimeRange(*firstDateTime, *lastDateTime);
        lastYearEdit->setTimeSpec(Qt::UTC);
        lastYearEdit->setDateTime(*lastDateTime);

        QLabel *lastDateLabel = new QLabel("<b>Last Date:</b>");
        lastDateLabel->setBuddy(lastYearEdit);

        layoutDate.addWidget(firstDateLabel);
        layoutDate.addWidget(firstYearEdit);

        layoutDate.addWidget(lastDateLabel);
        layoutDate.addWidget(lastYearEdit);

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
        *firstDateTime = firstYearEdit->dateTime();
        *lastDateTime = lastYearEdit->dateTime();

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




