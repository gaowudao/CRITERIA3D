#include "seriesOnZonesDialog.h"

#include "pragaProject.h"

extern PragaProject myProject;

meteoVariable SeriesOnZonesDialog::getVariable() const
{
    return variable;
}

QDate SeriesOnZonesDialog::getStartDate() const
{
    return startDate;
}

QDate SeriesOnZonesDialog::getEndDate() const
{
    return endDate;
}

aggregationMethod SeriesOnZonesDialog::getSpatialElaboration() const
{
    return spatialElaboration;
}

SeriesOnZonesDialog::SeriesOnZonesDialog(QSettings *settings)
    : settings(settings)
{

    setWindowTitle("Spatial average series on zones");


    QVBoxLayout mainLayout;
    QHBoxLayout varLayout;
    QHBoxLayout dateLayout;
    QHBoxLayout spatialElabLayout;

    QHBoxLayout layoutOk;

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


    genericStartLabel.setText("Start Date:");
    genericPeriodStart.setDate(myProject.getCurrentDate());
    genericStartLabel.setBuddy(&genericPeriodStart);
    genericEndLabel.setText("End Date:");
    genericPeriodEnd.setDate(myProject.getCurrentDate());
    genericEndLabel.setBuddy(&genericPeriodEnd);


    dateLayout.addWidget(&genericStartLabel);
    dateLayout.addWidget(&genericPeriodStart);
    dateLayout.addWidget(&genericEndLabel);
    dateLayout.addWidget(&genericPeriodEnd);

    QLabel spatialElabLabel("Spatial Elaboration: ");
    spatialElab.addItem("average");
    spatialElab.addItem("median");
    spatialElab.addItem("stddev");


    spatialElabLayout.addWidget(&spatialElabLabel);
    spatialElabLayout.addWidget(&spatialElab);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(&buttonBox, &QDialogButtonBox::accepted, [=](){ this->done(true); });
    connect(&buttonBox, &QDialogButtonBox::rejected, [=](){ this->done(false); });

    layoutOk.addWidget(&buttonBox);

    mainLayout.addLayout(&varLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&spatialElabLayout);

    mainLayout.addLayout(&layoutOk);

    setLayout(&mainLayout);


    show();
    exec();

}


void SeriesOnZonesDialog::done(bool res)
{

    if(res)  // ok was pressed
    {
        if (!checkValidData())
        {
            QDialog::done(QDialog::Rejected);
            return;
        }
        else  // validate the data
        {
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

bool SeriesOnZonesDialog::checkValidData()
{

    startDate = genericPeriodStart.date();
    endDate = genericPeriodEnd.date();

    if (startDate > endDate)
    {
        QMessageBox::information(nullptr, "Invalid date", "first date greater than last date");
        return false;
    }

    QString var = variableList.currentText();
    variable = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, var.toStdString());
    spatialElaboration = getKeyGridAggregationMethod(spatialElab.currentText().toStdString());

    return true;

}



