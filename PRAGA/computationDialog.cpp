#include "computationDialog.h"
#include "climate.h"
#include "dbClimate.h"
#include "utilities.h"

extern PragaProject myProject;

ComputationDialog::ComputationDialog(QSettings *settings, bool isAnomaly, bool saveClima)
    : settings(settings), isAnomaly(isAnomaly), saveClima(saveClima)
{

    if (saveClima)
    {
        setWindowTitle("Climate Elaboration");
    }
    else if (!isAnomaly)
    {
        setWindowTitle("Elaboration");
    }
    else
    {
        setWindowTitle("Anomaly");
    }

    QVBoxLayout mainLayout;
    QHBoxLayout varLayout;
    QHBoxLayout dateLayout;
    QHBoxLayout periodLayout;
    QHBoxLayout displayLayout;
    QHBoxLayout genericPeriodLayout;
    QHBoxLayout layoutOk;

    QHBoxLayout elaborationLayout;
    QHBoxLayout readParamLayout;
    QHBoxLayout secondElabLayout;

    QVBoxLayout anomalyMainLayout;
    QHBoxLayout buttonLayout;
    QVBoxLayout saveClimaMainLayout;
    QFrame anomalyLine;

    anomalyLine.setFrameShape(QFrame::HLine);
    anomalyLine.setFrameShadow(QFrame::Sunken);
    QLabel anomalyLabel("<font color='red'>Reference Data:</font>");
    copyData.setText("Copy data above");
    copyData.setMaximumWidth(this->width()/3);

    meteoVariable var;

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
        catch (const std::out_of_range& ) {
           myProject.logError("variable " + QString::fromStdString(variable) + " missing in MapDailyMeteoVar");
           continue;
        }
        variableList.addItem(QString::fromStdString(item));
    }

    QLabel variableLabel("Variable: ");
    varLayout.addWidget(&variableLabel);
    varLayout.addWidget(&variableList);

    readReference.setText("Read reference climate from db");
    readReference.setTristate(false);
    varLayout.addWidget(&readReference);
    if (!isAnomaly)
    {
        readReference.setVisible(false);
    }

    if (myProject.clima->genericPeriodDateStart() == QDate(1800,1,1))
    {
        currentDay.setDate(myProject.getCurrentDate());
    }
    else
    {
        currentDay.setDate(myProject.clima->genericPeriodDateStart());
    }

    currentDay.setDisplayFormat("dd/MM");
    currentDayLabel.setBuddy(&currentDay);

    if(saveClima)
    {
        currentDayLabel.setVisible(false);
        currentDay.setVisible(false);
    }
    else
    {
        currentDayLabel.setText("Day/Month:");
        currentDayLabel.setVisible(true);
        currentDay.setVisible(true);
    }


    int currentYear = myProject.getCurrentDate().year();

    QLabel firstDateLabel("Start Year:");
    if (myProject.clima->yearStart() == NODATA)
    {
        firstYearEdit.setText(QString::number(currentYear));
    }
    else
    {
        firstYearEdit.setText(QString::number(myProject.clima->yearStart()));
    }

    firstYearEdit.setFixedWidth(110);
    firstYearEdit.setValidator(new QIntValidator(1800, 3000));
    firstDateLabel.setBuddy(&firstYearEdit);

    QLabel lastDateLabel("End Year:");
    if (myProject.clima->yearEnd() == NODATA)
    {
        lastYearEdit.setText(QString::number(currentYear));
    }
    else
    {
        lastYearEdit.setText(QString::number(myProject.clima->yearEnd()));
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

    QLabel periodTypeLabel("Period Type: ");
    periodLayout.addWidget(&periodTypeLabel);
    periodLayout.addWidget(&periodTypeList);

    QString periodSelected = periodTypeList.currentText();
    int dayOfYear = currentDay.date().dayOfYear();
    periodDisplay.setText("Day Of Year: " + QString::number(dayOfYear));
    periodDisplay.setReadOnly(true);

    if (saveClima)
    {
        periodDisplay.setVisible(false);
    }

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

    QLocale local(QLocale::system().language());
    local.setNumberOptions(QLocale::RejectGroupSeparator);
    elab1Parameter.setPlaceholderText("Parameter");
    elab1Parameter.setFixedWidth(90);
    QDoubleValidator* validator = new QDoubleValidator(-9999.0, 9999.0, 2); //LC accetta double con 2 cifre decimali da -9999 a 9999
    validator->setLocale(local);
    elab1Parameter.setValidator(validator);
    readParam.setText("Read param from db Climate");
    readParam.setChecked(myProject.clima->param1IsClimate());
    climateDbElabList.setVisible(false);


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
    }
    secondElabLayout.addWidget(&secondElabList);

    elab2Parameter.setPlaceholderText("Parameter");
    elab2Parameter.setFixedWidth(90);
    elab2Parameter.setValidator(validator);

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


    if (isAnomaly)
    {
        anomaly.AnomalySetVariableElab(variableList.currentText());
        anomaly.build(settings);

        anomalyLabel.setAlignment(Qt::AlignCenter);
        anomalyMainLayout.addWidget(&anomalyLine);
        anomalyMainLayout.setSpacing(5);
        anomalyMainLayout.addWidget(&anomalyLabel);
        anomalyMainLayout.addWidget(&copyData);
        anomalyMainLayout.setSpacing(15);
        anomalyMainLayout.addWidget(&anomaly);
    }
    else if(saveClima)
    {
        saveClimaMainLayout.addWidget(&saveClimaLayout);
    }


    connect(&firstYearEdit, &QLineEdit::editingFinished, [=](){ this->checkYears(); });
    connect(&lastYearEdit, &QLineEdit::editingFinished, [=](){ this->checkYears(); });
    connect(&currentDay, &QDateEdit::dateChanged, [=](){ this->displayPeriod(periodTypeList.currentText()); });
    connect(&periodTypeList, &QComboBox::currentTextChanged, [=](const QString &newVar){ this->displayPeriod(newVar); });

    connect(&variableList, &QComboBox::currentTextChanged, [=](const QString &newVar){ this->listElaboration(newVar); });
    connect(&elaborationList, &QComboBox::currentTextChanged, [=](const QString &newElab){ this->listSecondElab(newElab); });
    connect(&secondElabList, &QComboBox::currentTextChanged, [=](const QString &newSecElab){ this->activeSecondParameter(newSecElab); });
    connect(&readParam, &QCheckBox::stateChanged, [=](int state){ this->readParameter(state); });
    connect(&copyData, &QPushButton::clicked, [=](){ this->copyDataToAnomaly(); });


    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(&buttonBox, &QDialogButtonBox::accepted, [=](){ this->done(true); });
    connect(&buttonBox, &QDialogButtonBox::rejected, [=](){ this->done(false); });

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&varLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&periodLayout);
    mainLayout.addLayout(&displayLayout);
    mainLayout.addLayout(&genericPeriodLayout);
    mainLayout.addLayout(&elaborationLayout);
    mainLayout.addLayout(&readParamLayout);
    mainLayout.addLayout(&secondElabLayout);

    if (isAnomaly)
    {
        mainLayout.addLayout(&anomalyMainLayout);
    }
    else if(saveClima)
    {
        add.setText("Add");
        del.setText("Delete");
        delAll.setText("Delete all");
        buttonLayout.addWidget(&add);
        buttonLayout.addWidget(&del);
        buttonLayout.addWidget(&delAll);

        connect(&add, &QPushButton::clicked, [=](){ this->copyDataToSaveLayout(); });
        connect(&del, &QPushButton::clicked, [=](){ saveClimaLayout.deleteRaw(); });
        connect(&delAll, &QPushButton::clicked, [=](){ saveClimaLayout.deleteAll(); });
        mainLayout.addLayout(&buttonLayout);
        mainLayout.addLayout(&saveClimaMainLayout);
    }

    mainLayout.addLayout(&layoutOk);

    setLayout(&mainLayout);

    // show stored values
    if (myProject.clima->variable() != noMeteoVar)
    {
        std::string storedVar = MapDailyMeteoVarToString.at(myProject.clima->variable());
        variableList.setCurrentText(QString::fromStdString(storedVar));
    }
    if (myProject.clima->elab1() != "")
    {
        elaborationList.setCurrentText(myProject.clima->elab1());
        if (myProject.clima->param1() != NODATA)
        {
            elab1Parameter.setText(QString::number(myProject.clima->param1()));
        }
    }
    if (myProject.clima->elab2() != "")
    {
        secondElabList.setCurrentText(myProject.clima->elab2());
        if (myProject.clima->param2() != NODATA)
        {
            elab2Parameter.setText(QString::number(myProject.clima->param2()));
        }
    }
    if (myProject.clima->periodStr() != "")
    {
            periodTypeList.setCurrentText(myProject.clima->periodStr());
            if (myProject.clima->periodStr() == "Generic")
            {
                genericPeriodStart.setDate(myProject.clima->genericPeriodDateStart());
                genericPeriodEnd.setDate(myProject.clima->genericPeriodDateEnd());
                nrYear.setText(QString::number(myProject.clima->nYears()));
            }
    }


    show();
    exec();

}


void ComputationDialog::done(bool res)
{

    if(res)  // ok was pressed
    {
        if (!checkValidData())
        {
            return;
        }
        else  // validate the data
        {
            // store elaboration values

            if (myProject.clima == NULL)
            {
                QMessageBox::information(NULL, "Error!", "clima is null...");
                return;
            }
            else
            {
                if (isAnomaly && myProject.referenceClima == NULL)
                {
                    QMessageBox::information(NULL, "Error!", "reference clima is null...");
                    return;
                }
            }

            QString periodSelected = periodTypeList.currentText();
            QString value = variableList.currentText();
            meteoVariable var = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, value.toStdString());


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

            // store reference data
            if (isAnomaly)
            {
                myProject.referenceClima->setVariable(myProject.clima->variable());
                QString AnomalyPeriodSelected = anomaly.AnomalyGetPeriodTypeList();

                myProject.referenceClima->setYearStart(anomaly.AnomalyGetYearStart());
                myProject.referenceClima->setYearEnd(anomaly.AnomalyGetYearLast());
                myProject.referenceClima->setPeriodStr(AnomalyPeriodSelected);

                if (AnomalyPeriodSelected == "Generic")
                {
                    myProject.referenceClima->setGenericPeriodDateStart(anomaly.AnomalyGetGenericPeriodStart());
                    myProject.referenceClima->setGenericPeriodDateEnd(anomaly.AnomalyGetGenericPeriodEnd());
                    myProject.referenceClima->setNYears(anomaly.AnomalyGetNyears());
                }
                else
                {
                    myProject.referenceClima->setNYears(0);
                    QDate start;
                    QDate end;
                    getPeriodDates(AnomalyPeriodSelected, anomaly.AnomalyGetYearStart(), anomaly.AnomalyGetCurrentDay(), &start, &end);
                    myProject.referenceClima->setNYears(start.year() - anomaly.AnomalyGetYearStart());
                    myProject.referenceClima->setGenericPeriodDateStart(start);
                    myProject.referenceClima->setGenericPeriodDateEnd(end);
                }
                myProject.referenceClima->setElab1(anomaly.AnomalyGetElaboration());

                if (!anomaly.AnomalyReadParamIsChecked())
                {
                    myProject.referenceClima->setParam1IsClimate(false);
                    if (anomaly.AnomalyGetParam1() != "")
                    {
                        myProject.referenceClima->setParam1(anomaly.AnomalyGetParam1().toFloat());
                    }
                    else
                    {
                        myProject.referenceClima->setParam1(NODATA);
                    }
                }
                else
                {
                    myProject.referenceClima->setParam1IsClimate(true);
                    // TO DO LC

                }
                if (anomaly.AnomalyGetSecondElaboration() == "None" || anomaly.AnomalyGetSecondElaboration() == "No elaboration available")
                {
                    myProject.referenceClima->setElab2("");
                    myProject.referenceClima->setParam2(NODATA);
                }
                else
                {
                    myProject.referenceClima->setElab2(anomaly.AnomalyGetSecondElaboration());
                    if (anomaly.AnomalyGetParam2() != "")
                    {
                        myProject.referenceClima->setParam2(anomaly.AnomalyGetParam2().toFloat());
                    }
                    else
                    {
                        myProject.referenceClima->setParam2(NODATA);
                    }
                }
            }

            QDialog::done(QDialog::Accepted);
            return;
        }

    }
    else    // cancel, close or exc was pressed
    {
        QDialog::done(QDialog::Rejected);
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


void ComputationDialog::displayPeriod(const QString value)
{

    if (value == "Daily")
    {
        if (saveClima)
        {
            periodDisplay.setVisible(false);
            currentDayLabel.setVisible(false);
            currentDay.setVisible(false);
            genericStartLabel.setVisible(false);
            genericEndLabel.setVisible(false);
            genericPeriodStart.setVisible(false);
            genericPeriodEnd.setVisible(false);
            nrYearLabel.setVisible(false);
            nrYear.setVisible(false);
            return;
        }
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
        if (saveClima)
        {
            periodDisplay.setVisible(false);
            currentDayLabel.setVisible(false);
            currentDay.setVisible(false);
            genericStartLabel.setVisible(false);
            genericEndLabel.setVisible(false);
            genericPeriodStart.setVisible(false);
            genericPeriodEnd.setVisible(false);
            nrYearLabel.setVisible(false);
            nrYear.setVisible(false);
            return;
        }
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
        if (saveClima)
        {
            periodDisplay.setVisible(false);
            currentDayLabel.setVisible(false);
            currentDay.setVisible(false);
            genericStartLabel.setVisible(false);
            genericEndLabel.setVisible(false);
            genericPeriodStart.setVisible(false);
            genericPeriodEnd.setVisible(false);
            nrYearLabel.setVisible(false);
            nrYear.setVisible(false);
            return;
        }
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
        if (saveClima)
        {
            periodDisplay.setVisible(false);
            currentDayLabel.setVisible(false);
            currentDay.setVisible(false);
            genericStartLabel.setVisible(false);
            genericEndLabel.setVisible(false);
            genericPeriodStart.setVisible(false);
            genericPeriodEnd.setVisible(false);
            nrYearLabel.setVisible(false);
            nrYear.setVisible(false);
            return;
        }
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
        if (saveClima)
        {
            periodDisplay.setVisible(false);
            currentDayLabel.setVisible(false);
            currentDay.setVisible(false);
            genericStartLabel.setVisible(false);
            genericEndLabel.setVisible(false);
            genericPeriodStart.setVisible(false);
            genericPeriodEnd.setVisible(false);
            nrYearLabel.setVisible(false);
            nrYear.setVisible(false);
            return;
        }
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
    if (readParam.isChecked())
    {
        readParameter(Qt::Checked);
    }
    if(isAnomaly)
    {
        anomaly.AnomalySetVariableElab(variableList.currentText());
    }

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
        displayPeriod(periodTypeList.currentText());
    }
    else
    {
        periodTypeList.setEnabled(true);
        nrYear.setEnabled(true);
    }

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

    climateDbElabList.clear();
    climateDbElab.clear();

    std::string myError = myProject.errorString;

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
                selectVarElab(myProject.clima->db(), &myError, climateTables.at(i), variableList.currentText(), &climateDbElab);
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

void ComputationDialog::copyDataToAnomaly()
{

    anomaly.AnomalySetReadReference(readReference.isChecked());
    if (firstYearEdit.text().size() == 4 && lastYearEdit.text().size() == 4 && firstYearEdit.text().toInt() < lastYearEdit.text().toInt())
    {
        anomaly.AnomalySetYearStart(firstYearEdit.text());
        anomaly.AnomalySetYearLast(lastYearEdit.text());
    }

    anomaly.AnomalySetPeriodTypeList(periodTypeList.currentText());
    if (periodTypeList.currentText() == "Generic")
    {
        anomaly.AnomalySetGenericPeriodStart(genericPeriodStart.date());
        anomaly.AnomalySetGenericPeriodEnd(genericPeriodEnd.date());
        anomaly.AnomalySetNyears(nrYear.text());
    }
    else
    {
        anomaly.AnomalySetCurrentDay(currentDay.date());
    }
    anomaly.AnomalySetElaboration(elaborationList.currentText());
    anomaly.AnomalySetReadParamIsChecked(readParam.isChecked());
    if (readParam.isChecked() == false)
    {
        anomaly.AnomalySetParam1(elab1Parameter.text());
    }

    anomaly.AnomalySetSecondElaboration(secondElabList.currentText());
    anomaly.AnomalySetParam2(elab2Parameter.text());

}

void ComputationDialog::copyDataToSaveLayout()
{

    if (!checkValidData())
    {
        return;
    }
    saveClimaLayout.setFirstYear(firstYearEdit.text());
    saveClimaLayout.setLastYear(lastYearEdit.text());
    saveClimaLayout.setVariable(variableList.currentText());
    saveClimaLayout.setPeriod(periodTypeList.currentText());
    if (periodTypeList.currentText() == "Generic")
    {
        if (genericPeriodStart.date().day() < 10)
        {
            saveClimaLayout.setGenericPeriodStartDay("0"+QString::number(genericPeriodStart.date().day()));
        }
        else
        {
            saveClimaLayout.setGenericPeriodStartDay(QString::number(genericPeriodStart.date().day()));
        }
        if (genericPeriodStart.date().month() < 10)
        {
            saveClimaLayout.setGenericPeriodStartMonth("0"+QString::number(genericPeriodStart.date().month()));
        }
        else
        {
            saveClimaLayout.setGenericPeriodStartMonth(QString::number(genericPeriodStart.date().month()));
        }

        if (genericPeriodEnd.date().day() < 10)
        {
            saveClimaLayout.setGenericPeriodEndDay("0"+QString::number(genericPeriodEnd.date().day()));
        }
        else
        {
            saveClimaLayout.setGenericPeriodEndDay(QString::number(genericPeriodEnd.date().day()));
        }
        if (genericPeriodEnd.date().month() < 10)
        {
            saveClimaLayout.setGenericPeriodEndMonth("0"+QString::number(genericPeriodEnd.date().month()));
        }
        else
        {
            saveClimaLayout.setGenericPeriodEndMonth(QString::number(genericPeriodEnd.date().month()));
        }

        if (nrYear.text().toInt() < 10)
        {
            saveClimaLayout.setGenericNYear("0"+nrYear.text());
        }
        else
        {
            saveClimaLayout.setGenericNYear(nrYear.text());
        }

    }
    saveClimaLayout.setSecondElab(secondElabList.currentText());
    saveClimaLayout.setElab2Param(elab2Parameter.text());
    saveClimaLayout.setElab(elaborationList.currentText());
    saveClimaLayout.setElab1Param(elab1Parameter.text());

    saveClimaLayout.addElab();

}

bool ComputationDialog::checkValidData()
{

    if (firstYearEdit.text().size() != 4)
    {
        QMessageBox::information(NULL, "Missing year", "Insert first year");
        return false;
    }
    if (lastYearEdit.text().size() != 4)
    {
        QMessageBox::information(NULL, "Missing year", "Insert last year");
        return false;
    }

    if (firstYearEdit.text().toInt() > lastYearEdit.text().toInt())
    {
        QMessageBox::information(NULL, "Invalid year", "first year greater than last year");
        return false;
    }
    if (elaborationList.currentText().toStdString() == "huglin" || elaborationList.currentText().toStdString() == "winkler" || elaborationList.currentText().toStdString() == "fregoni")
    {
        if (secondElabList.currentText().toStdString() == "None")
        {
            QMessageBox::information(NULL, "Second Elaboration missing", elaborationList.currentText() + " requires second elaboration");
            return false;
        }

    }
    if ( MapElabWithParam.find(elaborationList.currentText().toStdString()) != MapElabWithParam.end())
    {
        if ( (!readParam.isChecked() && elab1Parameter.text().isEmpty()) || (readParam.isChecked() && climateDbElabList.currentText() == "No saved elaborations found" ))
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert parameter");
            return false;
        }
    }
    if ( MapElabWithParam.find(secondElabList.currentText().toStdString()) != MapElabWithParam.end())
    {
        if (elab2Parameter.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert second elaboration parameter");
            return false;
        }
    }
    if (periodTypeList.currentText() == "Generic")
    {
        if (nrYear.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert Nr Years");
            return false;
        }
    }
    return true;

}

QStringList ComputationDialog::getElabSaveList()
{
    return saveClimaLayout.getList();
}

