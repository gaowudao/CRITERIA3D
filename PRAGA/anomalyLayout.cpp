#include "anomalyLayout.h"
#include "climate.h"
#include "dbClimate.h"
#include "utilities.h"

extern PragaProject myProject;

AnomalyLayout::AnomalyLayout()
{ }

void AnomalyLayout::build(QSettings *AnomalySettings)
{

    this->AnomalySettings = AnomalySettings;

    readReference.setText("Read reference climate from db");
    readReference.setTristate(false);
    varLayout.addWidget(&readReference);

    currentDayLabel.setText("Day/Month:");


    if (myProject.referenceClima->genericPeriodDateStart() == QDate(1800,1,1))
    {
        currentDay.setDate(myProject.getCurrentDate());
    }
    else
    {
        currentDay.setDate(myProject.referenceClima->genericPeriodDateStart());
    }

    currentDay.setDisplayFormat("dd/MM");
    currentDayLabel.setBuddy(&currentDay);
    currentDayLabel.setVisible(true);
    currentDay.setVisible(true);

    int currentYear = myProject.getCurrentDate().year();

    firstDateLabel.setText("Start Year:");
    if (myProject.referenceClima->yearStart() == NODATA)
    {
        firstYearEdit.setText(QString::number(currentYear));
    }
    else
    {
        firstYearEdit.setText(QString::number(myProject.referenceClima->yearStart()));
    }

    firstYearEdit.setFixedWidth(110);
    firstYearEdit.setValidator(new QIntValidator(1800, 3000));
    firstDateLabel.setBuddy(&firstYearEdit);

    lastDateLabel.setText("End Year:");
    if (myProject.referenceClima->yearEnd() == NODATA)
    {
        lastYearEdit.setText(QString::number(currentYear));
    }
    else
    {
        lastYearEdit.setText(QString::number(myProject.referenceClima->yearEnd()));
    }

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

    periodTypeLabel.setText("Period Type: ");
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
    nrYearLabel.setText("Delta Years:");
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

    meteoVariable key = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, variableElab.text().toStdString());
    std::string keyString = getKeyStringMeteoMap(MapDailyMeteoVar, key);
    QString group = QString::fromStdString(keyString)+"_VarToElab1";
    AnomalySettings->beginGroup(group);
    int size = AnomalySettings->beginReadArray(QString::fromStdString(keyString));
    for (int i = 0; i < size; ++i) {
        AnomalySettings->setArrayIndex(i);
        QString elab = AnomalySettings->value("elab").toString();
        elaborationList.addItem( elab );
    }
    AnomalySettings->endArray();
    AnomalySettings->endGroup();
    elaborationLayout.addWidget(&elaborationList);

    elab1Parameter.setPlaceholderText("Parameter");
    elab1Parameter.setFixedWidth(90);
    elab1Parameter.setValidator(new QDoubleValidator(-9999.0, 9999.0, 2)); //LC accetta double con 2 cifre decimali da -9999 a 9999
    readParam.setText("Read param from db Climate");
    readParam.setChecked(myProject.referenceClima->param1IsClimate());
    readParam.setTristate(false);


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
    readParamLayout.addWidget(&readParam);
    readParamLayout.addWidget(&climateDbElabList);
    secondElabLayout.addWidget(new QLabel("Secondary Elaboration: "));

    if (firstYearEdit.text().toInt() == lastYearEdit.text().toInt())
    {
        secondElabList.addItem("No elaboration available");
    }
    else
    {
        group = elab1Field +"_Elab1Elab2";
        AnomalySettings->beginGroup(group);
        secondElabList.addItem("None");
        size = AnomalySettings->beginReadArray(elab1Field);
        for (int i = 0; i < size; ++i) {
            AnomalySettings->setArrayIndex(i);
            QString elab2 = AnomalySettings->value("elab2").toString();
            secondElabList.addItem( elab2 );
        }
        AnomalySettings->endArray();
        AnomalySettings->endGroup();
    }
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

    connect(&firstYearEdit, &QLineEdit::editingFinished, [=](){ this->AnomalyCheckYears(); });
    connect(&lastYearEdit, &QLineEdit::editingFinished, [=](){ this->AnomalyCheckYears(); });
    connect(&currentDay, &QDateEdit::dateChanged, [=](){ this->AnomalyDisplayPeriod(periodTypeList.currentText()); });
    connect(&variableElab, &QLineEdit::textChanged, [=](const QString &newVar){ this->AnomalyListElaboration(newVar); });

    connect(&periodTypeList, &QComboBox::currentTextChanged, [=](const QString &newVar){ this->AnomalyDisplayPeriod(newVar); });
    connect(&elaborationList, &QComboBox::currentTextChanged, [=](const QString &newElab){ this->AnomalyListSecondElab(newElab); });
    connect(&secondElabList, &QComboBox::currentTextChanged, [=](const QString &newSecElab){ this->AnomalyActiveSecondParameter(newSecElab); });
    connect(&readParam, &QCheckBox::stateChanged, [=](int state){ this->AnomalyReadParameter(state); });


    mainLayout.addLayout(&varLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&periodLayout);
    mainLayout.addLayout(&displayLayout);
    mainLayout.addLayout(&genericPeriodLayout);
    mainLayout.addLayout(&elaborationLayout);
    mainLayout.addLayout(&readParamLayout);
    mainLayout.addLayout(&secondElabLayout);

    setLayout(&mainLayout);

    // show stored values

    if (myProject.referenceClima->elab1() != "")
    {
        elaborationList.setCurrentText(myProject.referenceClima->elab1());
        if (myProject.referenceClima->param1() != NODATA)
        {
            elab1Parameter.setText(QString::number(myProject.referenceClima->param1()));
        }
    }
    if (myProject.referenceClima->elab2() != "")
    {
        secondElabList.setCurrentText(myProject.referenceClima->elab2());
        if (myProject.referenceClima->param2() != NODATA)
        {
            elab2Parameter.setText(QString::number(myProject.referenceClima->param2()));
        }
    }
    if (myProject.referenceClima->periodStr() != "")
    {
            periodTypeList.setCurrentText(myProject.referenceClima->periodStr());
            if (myProject.referenceClima->periodStr() == "Generic")
            {
                genericPeriodStart.setDate(myProject.referenceClima->genericPeriodDateStart());
                genericPeriodEnd.setDate(myProject.referenceClima->genericPeriodDateEnd());
                nrYear.setText(QString::number(myProject.referenceClima->nYears()));
            }
    }

}

void AnomalyLayout::AnomalySetVariableElab(const QString &value)
{
    variableElab.setText(value);
}

void AnomalyLayout::AnomalyListElaboration(const QString value)
{

    meteoVariable key = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, value.toStdString());
    std::string keyString = getKeyStringMeteoMap(MapDailyMeteoVar, key);
    QString group = QString::fromStdString(keyString)+"_VarToElab1";
    AnomalySettings->beginGroup(group);
    int size = AnomalySettings->beginReadArray(QString::fromStdString(keyString));
    elaborationList.clear();

    for (int i = 0; i < size; ++i)
    {
        AnomalySettings->setArrayIndex(i);
        QString elab = AnomalySettings->value("elab").toString();
        elaborationList.addItem( elab );

    }
    AnomalySettings->endArray();
    AnomalySettings->endGroup();

    if (myProject.referenceClima->variable() == key && myProject.referenceClima->elab1() != "")
    {
        elaborationList.setCurrentText(myProject.referenceClima->elab1());
        if (myProject.referenceClima->param1() != NODATA)
        {
            elab1Parameter.setText(QString::number(myProject.referenceClima->param1()));
        }
    }

    if (readParam.isChecked())
    {
        AnomalyReadParameter(Qt::Checked);
    }

    AnomalyListSecondElab(elaborationList.currentText());
}


void AnomalyLayout::AnomalyCheckYears()
{
    if (firstYearEdit.text().toInt() == lastYearEdit.text().toInt())
    {
        secondElabList.clear();
        secondElabList.addItem("No elaboration available");
    }
    else
    {
        AnomalyListSecondElab(elaborationList.currentText());
    }
}


void AnomalyLayout::AnomalyDisplayPeriod(const QString value)
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

void AnomalyLayout::AnomalyListSecondElab(const QString value)
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
        AnomalyDisplayPeriod(periodTypeList.currentText());
    }
    else
    {
        periodTypeList.setEnabled(true);
        nrYear.setEnabled(true);
    }

    QString group = value + "_Elab1Elab2";
    AnomalySettings->beginGroup(group);
    int size = AnomalySettings->beginReadArray(value);

    if (size == 0 || firstYearEdit.text().toInt() == lastYearEdit.text().toInt())
    {
        secondElabList.clear();
        secondElabList.addItem("No elaboration available");
        AnomalySettings->endArray();
        AnomalySettings->endGroup();
        return;
    }
    secondElabList.clear();
    secondElabList.addItem("None");
    for (int i = 0; i < size; ++i) {
        AnomalySettings->setArrayIndex(i);
        QString elab2 = AnomalySettings->value("elab2").toString();
        secondElabList.addItem( elab2 );
    }
    AnomalySettings->endArray();
    AnomalySettings->endGroup();

    if (myProject.referenceClima->elab1() == value && myProject.referenceClima->elab2() != "")
    {
        secondElabList.setCurrentText(myProject.referenceClima->elab2());
        if (myProject.referenceClima->param2() != NODATA)
        {
            elab2Parameter.setText(QString::number(myProject.referenceClima->param2()));
        }
    }

}

void AnomalyLayout::AnomalyActiveSecondParameter(const QString value)
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

void AnomalyLayout::AnomalyReadParameter(int state)
{

    std::string myError  = myProject.errorString;
    climateDbElabList.clear();
    climateDbElab.clear();

    if (state!= 0)
    {
        climateDbElabList.setVisible(true);
        QStringList climateTables;
        if ( !showClimateTables(myProject.clima->db(), &myError, &climateTables) )
        {
            climateDbElabList.addItem("No saved elaborations found");
        }
        else
        {
            for (unsigned int i=0; i < climateTables.size(); i++)
            {
                selectVarElab(myProject.clima->db(), &myError, climateTables.at(i), variableElab.text(), &climateDbElab);
            }
            if (climateDbElab.isEmpty())
            {
                climateDbElabList.addItem("No saved elaborations found");
            }
            else
            {
                for (unsigned int i=0; i < climateDbElab.size(); i++)
                {
                    climateDbElabList.addItem(climateDbElab.at(i));
                }
            }

        }


        elab1Parameter.clear();
        elab1Parameter.setReadOnly(true);
    }
    else
    {
        climateDbElabList.setVisible(false);
        elab1Parameter.setReadOnly(false);
    }

}

void AnomalyLayout::AnomalySetReadReference(bool set)
{
    readReference.setChecked(set);
}

QString AnomalyLayout::AnomalyGetPeriodTypeList() const
{
    return periodTypeList.currentText();
}

void AnomalyLayout::AnomalySetPeriodTypeList(QString period)
{
    periodTypeList.setCurrentText(period);
}

int AnomalyLayout::AnomalyGetYearStart() const
{
    return firstYearEdit.text().toInt();
}

void AnomalyLayout::AnomalySetYearStart(QString year)
{
    firstYearEdit.setText(year);
}

int AnomalyLayout::AnomalyGetYearLast() const
{
    return lastYearEdit.text().toInt();
}

void AnomalyLayout::AnomalySetYearLast(QString year)
{
    lastYearEdit.setText(year);
}

QDate AnomalyLayout::AnomalyGetGenericPeriodStart() const
{
    return genericPeriodStart.date();
}

void AnomalyLayout::AnomalySetGenericPeriodStart(QDate genericStart)
{
    genericPeriodStart.setDate(genericStart);
}

QDate AnomalyLayout::AnomalyGetGenericPeriodEnd() const
{
    return genericPeriodEnd.date();
}

void AnomalyLayout::AnomalySetGenericPeriodEnd(QDate genericEnd)
{
    genericPeriodEnd.setDate(genericEnd);
}

QDate AnomalyLayout::AnomalyGetCurrentDay() const
{
    return currentDay.date();
}

void AnomalyLayout::AnomalySetCurrentDay(QDate date)
{
    currentDay.setDate(date);
}

int AnomalyLayout::AnomalyGetNyears() const
{
    return nrYear.text().toInt();
}

void AnomalyLayout::AnomalySetNyears(QString nYears)
{
    nrYear.setText(nYears);
}

QString AnomalyLayout::AnomalyGetElaboration() const
{
    return elaborationList.currentText();
}

void AnomalyLayout::AnomalySetElaboration(QString elab)
{
    elaborationList.setCurrentText(elab);
}

QString AnomalyLayout::AnomalyGetSecondElaboration() const
{
    return secondElabList.currentText();
}

void AnomalyLayout::AnomalySetSecondElaboration(QString elab)
{
    secondElabList.setCurrentText(elab);
}

QString AnomalyLayout::AnomalyGetParam1() const
{
    return elab1Parameter.text();
}

void AnomalyLayout::AnomalySetParam1(QString param)
{
    elab1Parameter.setText(param);
}

QString AnomalyLayout::AnomalyGetParam2() const
{
    return elab2Parameter.text();
}

void AnomalyLayout::AnomalySetParam2(QString param)
{
    elab2Parameter.setText(param);
}

bool AnomalyLayout::AnomalyReadParamIsChecked() const
{
    return readParam.isChecked();
}

void AnomalyLayout::AnomalySetReadParamIsChecked(bool set)
{
    readParam.setChecked(set);
}

