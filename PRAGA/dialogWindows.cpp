#include <QFileDialog>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListWidget>
#include <QRadioButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>

#include "commonConstants.h"
#include "dialogWindows.h"
#include "project.h"
#include "utilities.h"

extern Project myProject;


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

    QRadioButton DTM("Digital Terrain map m");
    QRadioButton Temp("Temperature °C");
    QRadioButton Prec("Precipitation mm");
    QRadioButton RH("Relative humidity %");
    QRadioButton Rad("Solar radiation MJ m-2");
    QRadioButton Wind("Wind intensity m s-1");

    layoutVariable.addWidget(&DTM);
    layoutVariable.addWidget(&Temp);
    layoutVariable.addWidget(&Prec);
    layoutVariable.addWidget(&RH);
    layoutVariable.addWidget(&Rad);
    layoutVariable.addWidget(&Wind);

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

    if (DTM.isChecked())
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


bool chooseMeteoVariable()
{
    if (myProject.getFrequency() == noFrequency)
    {
        QMessageBox::information(NULL, "No frequency", "Choose frequency before");
        return false;
    }

    QDialog myDialog;
    QVBoxLayout mainLayout;
    QVBoxLayout layoutVariable;
    QHBoxLayout layoutOk;

    myDialog.setWindowTitle("Choose variable");
    myDialog.setFixedWidth(300);

    QRadioButton Tavg("Average temperature °C");
    QRadioButton Tmin("Minimum temperature °C");
    QRadioButton Tmax("Maximum temperature °C");
    QRadioButton Prec("Precipitation mm");
    QRadioButton RHavg("Average relative humidity %");
    QRadioButton RHmin("Minimum relative humidity %");
    QRadioButton RHmax("Maximum relative humidity %");
    QRadioButton Rad("Solar radiation MJ m-2");

    if (myProject.getFrequency() == daily)
    {
        layoutVariable.addWidget(&Tmin);
        layoutVariable.addWidget(&Tavg);
        layoutVariable.addWidget(&Tmax);
        layoutVariable.addWidget(&Prec);
        layoutVariable.addWidget(&RHmin);
        layoutVariable.addWidget(&RHavg);
        layoutVariable.addWidget(&RHmax);
        layoutVariable.addWidget(&Rad);
    }
    else if (myProject.getFrequency() == hourly)
    {
        Tavg.setText("Average temperature °C");
        Prec.setText("Precipitation mm");
        RHavg.setText("Average relative humidity %");
        Rad.setText("Solar irradiance W m-2");

        layoutVariable.addWidget(&Tavg);
        layoutVariable.addWidget(&Prec);
        layoutVariable.addWidget(&RHavg);
        layoutVariable.addWidget(&Rad);
    }
    else return false;

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    myDialog.connect(&buttonBox, SIGNAL(accepted()), &myDialog, SLOT(accept()));
    myDialog.connect(&buttonBox, SIGNAL(rejected()), &myDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&layoutVariable);
    mainLayout.addLayout(&layoutOk);
    myDialog.setLayout(&mainLayout);
    myDialog.exec();

    if (myDialog.result() != QDialog::Accepted)
        return false;

   if (myProject.getFrequency() == daily)
   {
       if (Tmin.isChecked())
           myProject.currentVariable = dailyAirTemperatureMin;
       else if (Tmax.isChecked())
           myProject.currentVariable = dailyAirTemperatureMax;
       else if (Tavg.isChecked())
           myProject.currentVariable = dailyAirTemperatureAvg;
       else if (Prec.isChecked())
           myProject.currentVariable = dailyPrecipitation;
       else if (Rad.isChecked())
           myProject.currentVariable = dailyGlobalRadiation;
       else if (RHmin.isChecked())
           myProject.currentVariable = dailyAirRelHumidityMin;
       else if (RHmax.isChecked())
           myProject.currentVariable = dailyAirRelHumidityMax;
       else if (RHavg.isChecked())
           myProject.currentVariable = dailyAirRelHumidityAvg;
   }
   else if (myProject.getFrequency() == hourly)
   {
       if (Tavg.isChecked())
           myProject.currentVariable = airTemperature;
       else if (RHavg.isChecked())
           myProject.currentVariable = airRelHumidity;
       else if (Prec.isChecked())
           myProject.currentVariable = precipitation;
       else if (Rad.isChecked())
           myProject.currentVariable = globalIrradiance;
   }
   else
       return false;

   return true;
}


bool chooseNetCDFVariable(int* varId, QDateTime* firstDate, QDateTime* lastDate)
{
    // check
    if (! myProject.netCDF.isLoaded)
    {
        QMessageBox::information(NULL, "No data", "Load NetCDF before");
        return false;
    }
    if (! myProject.netCDF.isStandardTime)
    {
        QMessageBox::information(NULL, "Wrong time", "Praga reads only POSIX standard (seconds since 1970-01-01)");
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

    int nrVariables = myProject.netCDF.getNrVariables();
    std::vector<QRadioButton*> buttonVars;

    for (int i = 0; i < nrVariables; i++)
    {
        QString varName = QString::fromStdString(myProject.netCDF.variables[i].getVarName());
        buttonVars.push_back(new QRadioButton(varName));

        layoutVariable.addWidget(buttonVars[i]);
    }

    //void space
    layoutVariable.addWidget(new QLabel());

    //Date widgets
    *firstDate = QDateTime::fromTime_t(myProject.netCDF.getFirstTime(), Qt::UTC);
    *lastDate = QDateTime::fromTime_t(myProject.netCDF.getLastTime(), Qt::UTC);

    QDateTimeEdit *firstDateEdit = new QDateTimeEdit;
    firstDateEdit->setDateTimeRange(*firstDate, *lastDate);
    firstDateEdit->setTimeSpec(Qt::UTC);
    firstDateEdit->setDateTime(*firstDate);

    QLabel *firstDateLabel = new QLabel("<b>First Date:</b>");
    firstDateLabel->setBuddy(firstDateEdit);

    QDateTimeEdit *lastDateEdit = new QDateTimeEdit;
    lastDateEdit->setDateTimeRange(*firstDate, *lastDate);
    lastDateEdit->setTimeSpec(Qt::UTC);
    lastDateEdit->setDateTime(*lastDate);

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
    *firstDate = firstDateEdit->dateTime();
    *lastDate = lastDateEdit->dateTime();

    bool isVarSelected = false;
    int i = 0;
    while (i < nrVariables && ! isVarSelected)
    {
        if (buttonVars[i]->isChecked())
        {
           *varId = myProject.netCDF.variables[i].id;
            isVarSelected = true;
        }
        i++;
    }

    return isVarSelected;
}


bool downloadMeteoData()
{
    if(myProject.nrMeteoPoints == 0)
    {
         QMessageBox::information(NULL, "DB not existing", "Create or Open a meteo points database before download");
         return false;
    }

    QDialog downloadDialog;
    QVBoxLayout mainLayout;
    QHBoxLayout timeVarLayout;
    QHBoxLayout dateLayout;
    QHBoxLayout buttonLayout;

    downloadDialog.setWindowTitle("Download Data");

    QCheckBox hourly("Hourly");
    QCheckBox daily("Daily");

    QListWidget variable;
    variable.setSelectionMode(QAbstractItemView::MultiSelection);

    QListWidgetItem item1("Air Temperature");
    QListWidgetItem item2("Precipitation");
    QListWidgetItem item3("Air Humidity");
    QListWidgetItem item4("Radiation");
    QListWidgetItem item5("Wind");
    QListWidgetItem item6("All variables");
    QFont font("Helvetica", 10, QFont::Bold);
    item6.setFont(font);

    variable.addItem(&item1);
    variable.addItem(&item2);
    variable.addItem(&item3);
    variable.addItem(&item4);
    variable.addItem(&item5);
    variable.addItem(&item6);

    timeVarLayout.addWidget(&daily);
    timeVarLayout.addWidget(&hourly);
    timeVarLayout.addWidget(&variable);

    QDateEdit *FirstDateEdit = new QDateEdit;
    FirstDateEdit->setDate(QDate::currentDate());
    QLabel *FirstDateLabel = new QLabel("   Start Date:");
    FirstDateLabel->setBuddy(FirstDateEdit);

    QDateEdit *LastDateEdit = new QDateEdit;
    LastDateEdit->setDate(QDate::currentDate());
    QLabel *LastDateLabel = new QLabel("    End Date:");
    LastDateLabel->setBuddy(LastDateEdit);

    dateLayout.addWidget(FirstDateLabel);
    dateLayout.addWidget(FirstDateEdit);

    dateLayout.addWidget(LastDateLabel);
    dateLayout.addWidget(LastDateEdit);

    QDialogButtonBox buttonBox;
    QPushButton downloadButton("Download");
    downloadButton.setCheckable(true);
    downloadButton.setAutoDefault(false);

    QPushButton cancelButton("Cancel");
    cancelButton.setCheckable(true);
    cancelButton.setAutoDefault(false);

    buttonBox.addButton(&downloadButton, QDialogButtonBox::AcceptRole);
    buttonBox.addButton(&cancelButton, QDialogButtonBox::RejectRole);

    downloadDialog.connect(&buttonBox, SIGNAL(accepted()), &downloadDialog, SLOT(accept()));
    downloadDialog.connect(&buttonBox, SIGNAL(rejected()), &downloadDialog, SLOT(reject()));

    buttonLayout.addWidget(&buttonBox);
    mainLayout.addLayout(&timeVarLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&buttonLayout);
    downloadDialog.setLayout(&mainLayout);

    downloadDialog.exec();

    if (downloadDialog.result() != QDialog::Accepted)
        return false;

   QDate firstDate = FirstDateEdit->date();
   QDate lastDate = LastDateEdit->date();

   if (!daily.isChecked() && !hourly.isChecked())
   {
       QMessageBox::information(NULL, "Missing parameter", "Select hourly or daily");
       return downloadMeteoData();
   }
   else if ((! firstDate.isValid()) || (! lastDate.isValid()))
   {
       QMessageBox::information(NULL, "Missing parameter", "Select download period");
       return downloadMeteoData();
   }
   else if (!item1.isSelected() && !item2.isSelected() && !item3.isSelected() && !item4.isSelected() && !item5.isSelected() && !item6.isSelected())
   {
       QMessageBox::information(NULL, "Missing parameter", "Select variable");
       return downloadMeteoData();
   }
   else
   {
        QListWidgetItem* item = 0;
        QStringList var;
        for (int i = 0; i < variable.count()-1; ++i)
        {
               item = variable.item(i);
               if (item6.isSelected() || item->isSelected())
                   var.append(item->text());

        }
        if (daily.isChecked())
        {
            bool prec0024 = true;
            if ( item2.isSelected() || item6.isSelected() )
            {
                QDialog precDialog;
                precDialog.setFixedWidth(350);
                precDialog.setWindowTitle("Choose daily precipitation time");
                QVBoxLayout precLayout;
                QRadioButton first("0-24");
                QRadioButton second("08-08");

                QDialogButtonBox confirm(QDialogButtonBox::Ok);

                precDialog.connect(&confirm, SIGNAL(accepted()), &precDialog, SLOT(accept()));

                precLayout.addWidget(&first);
                precLayout.addWidget(&second);
                precLayout.addWidget(&confirm);
                precDialog.setLayout(&precLayout);
                precDialog.exec();

                if (second.isChecked())
                    prec0024 = false;
            }

            if (! myProject.downloadDailyDataArkimet(var, prec0024, firstDate, lastDate, true))
            {
                QMessageBox::information(NULL, "Error!", "Error in daily download");
                return false;
            }
        }

        if (hourly.isChecked())
        {
            if (! myProject.downloadHourlyDataArkimet(var, firstDate, lastDate, true))
            {
                QMessageBox::information(NULL, "Error!", "Error in hourly download");
                return false;
            }
        }

        return true;
    }
}

bool computation(QString title)
{
    QDialog computationDialog;

    QVBoxLayout mainLayout;
    QHBoxLayout varLayout;
    QHBoxLayout dateLayout;
    QHBoxLayout elaborationLayout;
    QHBoxLayout layoutOk;

    computationDialog.setWindowTitle(title);
    QComboBox variableList;
    std::map<std::string, meteoVariable>::const_iterator it;
    for (it = MapDailyMeteoVar.begin(); it != MapDailyMeteoVar.end(); ++it)
    {
        variableList.addItem( QString::fromStdString(it->first));
    }

    varLayout.addWidget(new QLabel("Variable: "));
    varLayout.addWidget(&variableList);
    QDateEdit *FirstDateEdit = new QDateEdit;
    FirstDateEdit->setDate(myProject.getCurrentDate());
    QLabel *FirstDateLabel = new QLabel("Start Date:");
    FirstDateLabel->setBuddy(FirstDateEdit);

    QDateEdit *LastDateEdit = new QDateEdit;
    LastDateEdit->setDate(myProject.getCurrentDate());
    QLabel *LastDateLabel = new QLabel("End Date:");
    LastDateLabel->setBuddy(LastDateEdit);

    dateLayout.addWidget(FirstDateLabel);
    dateLayout.addWidget(FirstDateEdit);

    dateLayout.addWidget(LastDateLabel);
    dateLayout.addWidget(LastDateEdit);

    QComboBox periodTypeSelection;
    periodTypeSelection.addItem("Daily");
    periodTypeSelection.addItem("Decadal");
    periodTypeSelection.addItem("Monthly");
    periodTypeSelection.addItem("Seasonal");
    periodTypeSelection.addItem("Annual");
    periodTypeSelection.addItem("Generic");


    elaborationLayout.addWidget(new QLabel("Period Type: "));
    elaborationLayout.addWidget(&periodTypeSelection);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    computationDialog.connect(&buttonBox, SIGNAL(accepted()), &computationDialog, SLOT(accept()));
    computationDialog.connect(&buttonBox, SIGNAL(rejected()), &computationDialog, SLOT(reject()));



    computationDialog.connect(&buttonBox, SIGNAL(accepted()), &computationDialog, SLOT(accept()));
    computationDialog.connect(&buttonBox, SIGNAL(rejected()), &computationDialog, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&varLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&elaborationLayout);
    mainLayout.addLayout(&layoutOk);

    computationDialog.setLayout(&mainLayout);

    computationDialog.exec();


}
