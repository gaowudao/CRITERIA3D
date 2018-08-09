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
#include <map>

#include "commonConstants.h"
#include "dialogWindows.h"
#include "project.h"
#include "utilities.h"
#include "climate.h"

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

    QRadioButton DTM("Elevation m");
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

    QDateTimeEdit *firstYearEdit = new QDateTimeEdit;
    firstYearEdit->setDateTimeRange(*firstDate, *lastDate);
    firstYearEdit->setTimeSpec(Qt::UTC);
    firstYearEdit->setDateTime(*firstDate);

    QLabel *firstDateLabel = new QLabel("<b>First Date:</b>");
    firstDateLabel->setBuddy(firstYearEdit);

    QDateTimeEdit *lastYearEdit = new QDateTimeEdit;
    lastYearEdit->setDateTimeRange(*firstDate, *lastDate);
    lastYearEdit->setTimeSpec(Qt::UTC);
    lastYearEdit->setDateTime(*lastDate);

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
    *firstDate = firstYearEdit->dateTime();
    *lastDate = lastYearEdit->dateTime();

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

    QDateEdit *firstYearEdit = new QDateEdit;
    firstYearEdit->setDate(QDate::currentDate());
    QLabel *FirstDateLabel = new QLabel("   Start Date:");
    FirstDateLabel->setBuddy(firstYearEdit);

    QDateEdit *lastYearEdit = new QDateEdit;
    lastYearEdit->setDate(QDate::currentDate());
    QLabel *LastDateLabel = new QLabel("    End Date:");
    LastDateLabel->setBuddy(lastYearEdit);

    dateLayout.addWidget(FirstDateLabel);
    dateLayout.addWidget(firstYearEdit);

    dateLayout.addWidget(LastDateLabel);
    dateLayout.addWidget(lastYearEdit);

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

   QDate firstDate = firstYearEdit->date();
   QDate lastDate = lastYearEdit->date();

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

ComputationDialog::ComputationDialog(QWidget *parent)
    : QDialog(parent)
{

}

bool ComputationDialog::computation(bool isAnomaly)
{
    QVBoxLayout mainLayout;
    QHBoxLayout varLayout;
    QHBoxLayout dateLayout;
    QHBoxLayout periodLayout;
    QHBoxLayout displayLayout;
    QHBoxLayout genericPeriodLayout;
    QHBoxLayout layoutOk;

    QHBoxLayout elaborationLayout;
    QHBoxLayout secondElabLayout;

    setWindowTitle(title);
    QComboBox variableList;
    meteoVariable var;

    if (!isAnomaly)
    {
        Q_FOREACH (QString group, settings->childGroups())
        {
            if (!group.endsWith("_VarToElab1"))
                continue;
            std::string item;
            std::string variable = group.left(group.size()-11).toStdString(); // remove "_VarToElab1"
            try {
              var = MapDailyMeteoVar.at(variable);
              item = MapDailyMeteoVarToString.at(var);
            }
            catch (const std::out_of_range& oor) {
              return false;
            }
            variableList.addItem(QString::fromStdString(item));
        }
    }
    else
    {
        var = myProject.clima->variable();
        std::string item = MapDailyMeteoVarToString.at(var);
        variableList.addItem(QString::fromStdString(item));
    }

    QLabel variableLabel("Variable: ");
    varLayout.addWidget(&variableLabel);
    varLayout.addWidget(&variableList);

    QCheckBox readReference("Read reference climate from db");
    varLayout.addWidget(&readReference);
    if (!isAnomaly)
    {
        readReference.setVisible(false);
    }

    currentDayLabel.setText("Day/Month:");
    currentDay.setDate(myProject.getCurrentDate());
    currentDay.setDisplayFormat("dd/MM");
    currentDayLabel.setBuddy(&currentDay);
    currentDayLabel.setVisible(true);
    currentDay.setVisible(true);

    int currentYear = myProject.getCurrentDate().year();
    QLabel firstDateLabel("Start Year:");
    //firstYearEdit.setPlaceholderText("yyyy");
    firstYearEdit.setText(QString::number(currentYear));
    firstYearEdit.setFixedWidth(110);
    firstYearEdit.setValidator(new QIntValidator(1800, 3000));
    firstDateLabel.setBuddy(&firstYearEdit);

    QLabel lastDateLabel("End Year:");
    //lastYearEdit.setPlaceholderText("yyyy");
    lastYearEdit.setText(QString::number(currentYear));
    lastYearEdit.setFixedWidth(110);
    lastYearEdit.setValidator(new QIntValidator(1800, 3000));
    lastDateLabel.setBuddy(&lastYearEdit);

    dateLayout.addWidget(&currentDayLabel);
    dateLayout.addWidget(&currentDay);

    dateLayout.addWidget(&firstDateLabel);
    dateLayout.addWidget(&firstYearEdit);

    dateLayout.addWidget(&lastDateLabel);
    dateLayout.addWidget(&lastYearEdit);

    periodTypeList.addItem("Daily");
    periodTypeList.addItem("Decadal");
    periodTypeList.addItem("Monthly");
    periodTypeList.addItem("Seasonal");
    periodTypeList.addItem("Annual");
    periodTypeList.addItem("Generic");

    QLabel periodTypeLabel("Period Type: ");
    periodLayout.addWidget(&periodTypeLabel);
    periodLayout.addWidget(&periodTypeList);

    QString periodSelected = periodTypeList.currentText();
    int dayOfYear = currentDay.date().dayOfYear();
    periodDisplay.setText("Day Of Year: " + QString::number(dayOfYear));
    periodDisplay.setReadOnly(true);

    displayLayout.addWidget(&periodDisplay);

    genericStartLabel.setText("Start Date:");
    genericPeriodStart.setDisplayFormat("dd/MM");
    genericStartLabel.setBuddy(&genericPeriodStart);
    genericEndLabel.setText("End Date:");
    genericPeriodEnd.setDisplayFormat("dd/MM");
    genericEndLabel.setBuddy(&genericPeriodEnd);
    nrYearLabel.setText("Nr Years");
    nrYear.setValidator(new QIntValidator(-500, 500));
    nrYear.setText("0");
    nrYearLabel.setBuddy(&nrYear);

    genericStartLabel.setVisible(false);
    genericEndLabel.setVisible(false);
    genericPeriodStart.setVisible(false);
    genericPeriodEnd.setVisible(false);
    nrYearLabel.setVisible(false);
    nrYear.setVisible(false);

    genericPeriodLayout.addWidget(&genericStartLabel);
    genericPeriodLayout.addWidget(&genericPeriodStart);
    genericPeriodLayout.addWidget(&genericEndLabel);
    genericPeriodLayout.addWidget(&genericPeriodEnd);
    genericPeriodLayout.addWidget(&nrYearLabel);
    genericPeriodLayout.addWidget(&nrYear);

    elaborationLayout.addWidget(new QLabel("Elaboration: "));
    QString value = variableList.currentText();
    meteoVariable key = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, value.toStdString());
    std::string keyString = getKeyStringMeteoMap(MapDailyMeteoVar, key);
    QString group = QString::fromStdString(keyString)+"_VarToElab1";
    settings->beginGroup(group);
    int size = settings->beginReadArray(QString::fromStdString(keyString));
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QString elab = settings->value("elab").toString();
        elaborationList.addItem( elab );
    }
    settings->endArray();
    settings->endGroup();
    elaborationLayout.addWidget(&elaborationList);

    elab1Parameter.setPlaceholderText("Parameter");
    elab1Parameter.setFixedWidth(90);
    elab1Parameter.setValidator(new QDoubleValidator(-9999.0, 9999.0, 2)); //LC accetta double con 2 cifre decimali da -9999 a 9999
    QCheckBox readParam("Read param from db Climate");


    QString elab1Field = elaborationList.currentText();
    if ( MapElabWithParam.find(elab1Field.toStdString()) == MapElabWithParam.end())
    {
        elab1Parameter.clear();
        elab1Parameter.setReadOnly(true);
    }
    else
    {
        if (!readParam.isChecked())
        {
            elab1Parameter.setReadOnly(false);
        }
    }

    elaborationLayout.addWidget(&elab1Parameter);
    elaborationLayout.addWidget(&readParam);
    secondElabLayout.addWidget(new QLabel("Secondary Elaboration: "));

    group = elab1Field +"_Elab1Elab2";
    settings->beginGroup(group);
    secondElabList.addItem("None");
    size = settings->beginReadArray(elab1Field);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QString elab2 = settings->value("elab2").toString();
        secondElabList.addItem( elab2 );
    }
    settings->endArray();
    settings->endGroup();
    secondElabLayout.addWidget(&secondElabList);

    elab2Parameter.setPlaceholderText("Parameter");
    elab2Parameter.setFixedWidth(90);
    elab2Parameter.setValidator(new QDoubleValidator(-9999.0, 9999.0, 2)); //LC accetta double con 2 cifre decimali da -9999 a 9999

    QString elab2Field = secondElabList.currentText();
    if ( MapElabWithParam.find(elab2Field.toStdString()) == MapElabWithParam.end())
    {
        elab2Parameter.clear();
        elab2Parameter.setReadOnly(true);
    }
    else
    {
        elab2Parameter.setReadOnly(false);
    }

    secondElabLayout.addWidget(&elab2Parameter);

    connect(&firstYearEdit, &QLineEdit::editingFinished, [=](){ this->checkYears(); });
    connect(&lastYearEdit, &QLineEdit::editingFinished, [=](){ this->checkYears(); });

    connect(&variableList, &QComboBox::currentTextChanged, [=](const QString &newVar){ this->listElaboration(newVar); });
    connect(&currentDay, &QDateTimeEdit::dateChanged, [=](const QDate &newDate){ this->changeDate(newDate); });
    connect(&periodTypeList, &QComboBox::currentTextChanged, [=](const QString &newVar){ this->displayPeriod(newVar); });
    connect(&elaborationList, &QComboBox::currentTextChanged, [=](const QString &newElab){ this->listSecondElab(newElab); });
    connect(&secondElabList, &QComboBox::currentTextChanged, [=](const QString &newSecElab){ this->activeSecondParameter(newSecElab); });
    connect(&readParam, &QCheckBox::stateChanged, [=](int state){ this->readParameter(state); });

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(&buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&varLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&periodLayout);
    mainLayout.addLayout(&displayLayout);
    mainLayout.addLayout(&genericPeriodLayout);
    mainLayout.addLayout(&elaborationLayout);
    mainLayout.addLayout(&secondElabLayout);
    mainLayout.addLayout(&layoutOk);

    setLayout(&mainLayout);

    exec();

    if (myProject.clima == NULL)
    {
        QMessageBox::information(NULL, "Error!", "clima is null...");
        return false;
    }

    if (this->result() != QDialog::Accepted)
    {
        qInfo() << "return false";
        return false;
    }
    else
    {

        periodSelected = periodTypeList.currentText();
        value = variableList.currentText();
        var = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, value.toStdString());


        myProject.clima->setVariable(var);
        myProject.clima->setYearStart(firstYearEdit.text().toInt());
        myProject.clima->setYearEnd(lastYearEdit.text().toInt());
        myProject.clima->setPeriodStr(periodSelected);
        if (periodSelected == "Generic")
        {
            myProject.clima->setGenericPeriodDateStart(genericPeriodStart.date());
            myProject.clima->setGenericPeriodDateEnd(genericPeriodEnd.date());
            myProject.clima->setNYears(nrYear.text().toInt());
        }
        else
        {
            myProject.clima->setNYears(0);
            QDate start;
            QDate end;
            getPeriodDates(periodSelected, firstYearEdit.text().toInt(), currentDay.date(), &start, &end);
            myProject.clima->setNYears(start.year() - firstYearEdit.text().toInt());
            myProject.clima->setGenericPeriodDateStart(start);
            myProject.clima->setGenericPeriodDateEnd(end);
        }

        myProject.clima->setElab1(elaborationList.currentText());

        if (!readParam.isChecked())
        {
            myProject.clima->setParam1IsClimate(false);
            if (elab1Parameter.text() != "")
            {
                myProject.clima->setParam1(elab1Parameter.text().toFloat());
            }
            else
            {
                myProject.clima->setParam1(NODATA);
            }
        }
        else
        {
            myProject.clima->setParam1IsClimate(true);
            // TO DO LC

        }
        if (secondElabList.currentText() == "None" || secondElabList.currentText() == "No elaboration available")
        {
            myProject.clima->setElab2("");
            myProject.clima->setParam2(NODATA);
        }
        else
        {
            myProject.clima->setElab2(secondElabList.currentText());
            if (elab2Parameter.text() != "")
            {
                myProject.clima->setParam2(elab2Parameter.text().toFloat());
            }
            else
            {
                myProject.clima->setParam2(NODATA);
            }
        }

        return true;
    }
}


void ComputationDialog::done(int r)
{

    if(QDialog::Accepted == r)  // ok was pressed
    {
        if(firstYearEdit.text().size() == 4 && lastYearEdit.text().size() == 4)   // validate the data
        {
            if (firstYearEdit.text().toInt() > lastYearEdit.text().toInt())
            {
                QMessageBox::information(NULL, "Invalid year", "first year greater than last year");
                return;
            }
            if (elaborationList.currentText().toStdString() == "huglin" || elaborationList.currentText().toStdString() == "winkler" || elaborationList.currentText().toStdString() == "fregoni")
            {
                if (secondElabList.currentText().toStdString() == "None")
                {
                    QMessageBox::information(NULL, "Second Elaboration missing", elaborationList.currentText() + " requires second elaboration");
                    return;
                }

            }
            if ( MapElabWithParam.find(elaborationList.currentText().toStdString()) != MapElabWithParam.end())
            {
                if (elab1Parameter.text().isEmpty())
                {
                    QMessageBox::information(NULL, "Missing Parameter", "insert parameter");
                    return;
                }
            }
            if ( MapElabWithParam.find(secondElabList.currentText().toStdString()) != MapElabWithParam.end())
            {
                if (elab2Parameter.text().isEmpty())
                {
                    QMessageBox::information(NULL, "Missing Parameter", "insert second elaboration parameter");
                    return;
                }
            }
            if (periodTypeList.currentText() == "Generic")
            {
                if (nrYear.text().isEmpty())
                {
                    QMessageBox::information(NULL, "Missing Parameter", "insert Nr Years");
                    return;
                }
            }
            QDialog::done(r);
            return;
        }
        else if (firstYearEdit.text().size() != 4)
        {
            QMessageBox::information(NULL, "Missing year", "Insert first year");
            return;
        }
        else
        {
            QMessageBox::information(NULL, "Missing year", "Insert last year");
            return;
        }

    }
    else    // cancel, close or exc was pressed
    {
        QDialog::done(r);
        return;
    }

}

void ComputationDialog::checkYears()
{
    if (firstYearEdit.text().toInt() == lastYearEdit.text().toInt())
    {
        secondElabList.clear();
        secondElabList.addItem("No elaboration available");
    }
    else
    {
        listSecondElab(elaborationList.currentText());
    }
}

void ComputationDialog::changeDate(const QDate newDate)
{
    displayPeriod(periodTypeList.currentText());
}


void ComputationDialog::displayPeriod(const QString value)
{

    if (value == "Daily")
    {
        periodDisplay.setVisible(true);
        currentDayLabel.setVisible(true);
        currentDay.setVisible(true);
        genericStartLabel.setVisible(false);
        genericEndLabel.setVisible(false);
        genericPeriodStart.setVisible(false);
        genericPeriodEnd.setVisible(false);
        nrYearLabel.setVisible(false);
        nrYear.setVisible(false);
        int dayOfYear = currentDay.date().dayOfYear();
        periodDisplay.setText("Day Of Year: " + QString::number(dayOfYear));
    }
    else if (value == "Decadal")
    {
        periodDisplay.setVisible(true);
        currentDayLabel.setVisible(true);
        currentDay.setVisible(true);
        genericStartLabel.setVisible(false);
        genericEndLabel.setVisible(false);
        genericPeriodStart.setVisible(false);
        genericPeriodEnd.setVisible(false);
        nrYearLabel.setVisible(false);
        nrYear.setVisible(false);
        int decade = decadeFromDate(currentDay.date());
        periodDisplay.setText("Decade: " + QString::number(decade));
    }
    else if (value == "Monthly")
    {
        periodDisplay.setVisible(true);
        currentDayLabel.setVisible(true);
        currentDay.setVisible(true);
        genericStartLabel.setVisible(false);
        genericEndLabel.setVisible(false);
        genericPeriodStart.setVisible(false);
        genericPeriodEnd.setVisible(false);
        nrYearLabel.setVisible(false);
        nrYear.setVisible(false);
        periodDisplay.setText("Month: " + QString::number(currentDay.date().month()));
    }
    else if (value == "Seasonal")
    {
        periodDisplay.setVisible(true);
        currentDayLabel.setVisible(true);
        currentDay.setVisible(true);
        genericStartLabel.setVisible(false);
        genericEndLabel.setVisible(false);
        genericPeriodStart.setVisible(false);
        genericPeriodEnd.setVisible(false);
        nrYearLabel.setVisible(false);
        nrYear.setVisible(false);
        QString season = getStringSeasonFromDate(currentDay.date());
        periodDisplay.setText("Season: " + season);
    }
    else if (value == "Annual")
    {
        periodDisplay.setVisible(false);
        currentDayLabel.setVisible(true);
        currentDay.setVisible(true);
        genericStartLabel.setVisible(false);
        genericEndLabel.setVisible(false);
        genericPeriodStart.setVisible(false);
        genericPeriodEnd.setVisible(false);
        nrYearLabel.setVisible(false);
        nrYear.setVisible(false);
    }
    else if (value == "Generic")
    {
        periodDisplay.setVisible(false);
        currentDayLabel.setVisible(false);
        currentDay.setVisible(false);

        genericStartLabel.setVisible(true);
        genericEndLabel.setVisible(true);
        genericPeriodStart.setVisible(true);
        genericPeriodEnd.setVisible(true);

        nrYearLabel.setVisible(true);
        nrYear.setVisible(true);
        nrYear.setText("0");
        nrYear.setEnabled(true);

        if (elaborationList.currentText().toStdString() == "huglin" || elaborationList.currentText().toStdString() == "fregoni")
        {
            QDate fixStart(firstYearEdit.text().toInt(),4,1);
            QDate fixEnd(lastYearEdit.text().toInt(),9,30);
            genericPeriodStart.setDate(fixStart);
            genericPeriodStart.setDisplayFormat("dd/MM");
            genericPeriodEnd.setDisplayFormat("dd/MM");
            genericPeriodEnd.setDate(fixEnd);
            nrYear.setText("0");
            nrYear.setEnabled(false);
        }
        else if (elaborationList.currentText().toStdString() == "winkler")
        {
            QDate fixStart(firstYearEdit.text().toInt(),4,1);
            QDate fixEnd(lastYearEdit.text().toInt(),10,31);
            genericPeriodStart.setDate(fixStart);
            genericPeriodStart.setDisplayFormat("dd/MM");
            genericPeriodEnd.setDisplayFormat("dd/MM");
            genericPeriodEnd.setDate(fixEnd);
            nrYear.setText("0");
            nrYear.setEnabled(false);
        }
        else
        {
            QDate defaultStart(firstYearEdit.text().toInt(),1,1);
            QDate defaultEnd(lastYearEdit.text().toInt(),1,1);
            genericPeriodStart.setDate(defaultStart);
            genericPeriodStart.setDisplayFormat("dd/MM");
            genericPeriodEnd.setDisplayFormat("dd/MM");
            genericPeriodEnd.setDate(defaultEnd);
            nrYear.setText("0");
            nrYear.setEnabled(true);
        }

    }

}

void ComputationDialog::listElaboration(const QString value)
{

    meteoVariable key = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, value.toStdString());
    std::string keyString = getKeyStringMeteoMap(MapDailyMeteoVar, key);
    QString group = QString::fromStdString(keyString)+"_VarToElab1";
    settings->beginGroup(group);
    int size = settings->beginReadArray(QString::fromStdString(keyString));
    elaborationList.clear();

    for (int i = 0; i < size; ++i)
    {
        settings->setArrayIndex(i);
        QString elab = settings->value("elab").toString();
        elaborationList.addItem( elab );

    }
    settings->endArray();
    settings->endGroup();

    listSecondElab(elaborationList.currentText());

}

void ComputationDialog::listSecondElab(const QString value)
{

    if ( MapElabWithParam.find(value.toStdString()) == MapElabWithParam.end())
    {
        elab1Parameter.clear();
        elab1Parameter.setReadOnly(true);
    }
    else
    {
        elab1Parameter.setReadOnly(false);
    }

    if (elaborationList.currentText().toStdString() == "huglin" || elaborationList.currentText().toStdString() == "winkler" || elaborationList.currentText().toStdString() == "fregoni")
    {
        periodTypeList.setCurrentText("Generic");
        periodTypeList.setEnabled(false);
    }
    else
    {
        periodTypeList.setEnabled(true);
        nrYear.setEnabled(true);
    }

    displayPeriod(periodTypeList.currentText());

    QString group = value + "_Elab1Elab2";
    settings->beginGroup(group);
    int size = settings->beginReadArray(value);

    if (size == 0 || firstYearEdit.text().toInt() == lastYearEdit.text().toInt())
    {
        secondElabList.clear();
        secondElabList.addItem("No elaboration available");
        settings->endArray();
        settings->endGroup();
        return;
    }
    secondElabList.clear();
    secondElabList.addItem("None");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QString elab2 = settings->value("elab2").toString();
        secondElabList.addItem( elab2 );
    }
    settings->endArray();
    settings->endGroup();

}

void ComputationDialog::activeSecondParameter(const QString value)
{

        if ( MapElabWithParam.find(value.toStdString()) == MapElabWithParam.end())
        {
            elab2Parameter.clear();
            elab2Parameter.setReadOnly(true);
        }
        else
        {
            elab2Parameter.setReadOnly(false);
        }
}

void ComputationDialog::readParameter(int state)
{
    if (state)
    {
        elab1Parameter.clear();
        elab1Parameter.setReadOnly(true);
    }
    else
    {
        elab1Parameter.setReadOnly(false);
    }
}


QSettings *ComputationDialog::getSettings() const
{
    return settings;
}

void ComputationDialog::setSettings(QSettings *value)
{
    settings = value;
}

QString ComputationDialog::getTitle() const
{
    return title;
}

void ComputationDialog::setTitle(const QString &value)
{
    title = value;
}
