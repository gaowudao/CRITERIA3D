#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QDateEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRadioButton>

#include "dialogDownloadMeteoData.h"
#include "pragaProject.h"


DialogDownloadMeteoData::DialogDownloadMeteoData()
{

    QVBoxLayout mainLayout;
    QHBoxLayout titleLayout;
    QHBoxLayout timeVarLayout;
    QHBoxLayout dateLayout;
    QHBoxLayout buttonLayout;

    setWindowTitle("Download Data");


    QLabel dailyTitle;
    dailyTitle.setText("Daily variables:");
    QLabel hourlyTitle;
    hourlyTitle.setText("Hourly variables:");
    titleLayout.addWidget(&dailyTitle);
    titleLayout.addWidget(&hourlyTitle);
    dailyVar.setSelectionMode(QAbstractItemView::MultiSelection);

    daily_item1.setText("DAILY_TMIN");
    daily_item2.setText("DAILY_TMAX");
    daily_item3.setText("DAILY_TAVG");
    daily_item4.setText("DAILY_PREC");
    daily_item5.setText("DAILY_RHMIN");
    daily_item6.setText("DAILY_RHMAX");
    daily_item7.setText("DAILY_RHAVG");
    daily_item8.setText("DAILY_RAD");
    daily_item9.setText("DAILY_W_SCAL_INT_AVG");
    daily_item10.setText("DAILY_W_VEC_DIR_PREV");
    daily_item11.setText("DAILY_W_SCAL_INT_MAX");
    daily_item12.setText("All variables");
    QFont font("Helvetica", 10, QFont::Bold);
    daily_item12.setFont(font);

    dailyVar.addItem(&daily_item1);
    dailyVar.addItem(&daily_item2);
    dailyVar.addItem(&daily_item3);
    dailyVar.addItem(&daily_item4);
    dailyVar.addItem(&daily_item5);
    dailyVar.addItem(&daily_item6);
    dailyVar.addItem(&daily_item7);
    dailyVar.addItem(&daily_item8);
    dailyVar.addItem(&daily_item9);
    dailyVar.addItem(&daily_item10);
    dailyVar.addItem(&daily_item11);
    dailyVar.addItem(&daily_item12);

    timeVarLayout.addWidget(&dailyVar);
    connect(&dailyVar, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->dailyVarClicked(item); });

    hourlyVar.setSelectionMode(QAbstractItemView::MultiSelection);

    hourly_item1.setText("TAVG");
    hourly_item2.setText("PREC");
    hourly_item3.setText("RHAVG");
    hourly_item4.setText("RAD");
    hourly_item5.setText("W_SCAL_INT");
    hourly_item6.setText("W_VEC_DIR");
    hourly_item7.setText("All variables");
    hourly_item7.setFont(font);

    hourlyVar.addItem(&hourly_item1);
    hourlyVar.addItem(&hourly_item2);
    hourlyVar.addItem(&hourly_item3);
    hourlyVar.addItem(&hourly_item4);
    hourlyVar.addItem(&hourly_item5);
    hourlyVar.addItem(&hourly_item6);
    hourlyVar.addItem(&hourly_item7);

    connect(&hourlyVar, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->hourlyVarClicked(item); });

    timeVarLayout.addWidget(&hourlyVar);
    firstYearEdit.setDate(QDate::currentDate());
    QLabel *FirstDateLabel = new QLabel("   Start Date:");
    FirstDateLabel->setBuddy(&firstYearEdit);

    lastYearEdit.setDate(QDate::currentDate());
    QLabel *LastDateLabel = new QLabel("    End Date:");
    LastDateLabel->setBuddy(&lastYearEdit);

    dateLayout.addWidget(FirstDateLabel);
    dateLayout.addWidget(&firstYearEdit);

    dateLayout.addWidget(LastDateLabel);
    dateLayout.addWidget(&lastYearEdit);

    QDialogButtonBox buttonBox;
    QPushButton downloadButton("Download");
    downloadButton.setCheckable(true);
    downloadButton.setAutoDefault(false);

    QPushButton cancelButton("Cancel");
    cancelButton.setCheckable(true);
    cancelButton.setAutoDefault(false);

    buttonBox.addButton(&downloadButton, QDialogButtonBox::AcceptRole);
    buttonBox.addButton(&cancelButton, QDialogButtonBox::RejectRole);

    connect(&buttonBox, &QDialogButtonBox::accepted, [=](){ this->done(true); });
    connect(&buttonBox, &QDialogButtonBox::rejected, [=](){ this->done(false); });

    buttonLayout.addWidget(&buttonBox);
    mainLayout.addLayout(&titleLayout);
    mainLayout.addLayout(&timeVarLayout);
    mainLayout.addLayout(&dateLayout);
    mainLayout.addLayout(&buttonLayout);
    setLayout(&mainLayout);

    show();
    exec();

}

void DialogDownloadMeteoData::dailyVarClicked(QListWidgetItem* item)
{
    if (item->text() == "All variables")
    {
        for (int i=0; i < dailyVar.count()-1; i++)
        {
            dailyVar.item(i)->setSelected(item->isSelected());
        }
    }
}

void DialogDownloadMeteoData::hourlyVarClicked(QListWidgetItem* item)
{
    if (item->text() == "All variables")
    {
        for (int i=0; i < hourlyVar.count()-1; i++)
        {
            hourlyVar.item(i)->setSelected(item->isSelected());
        }
    }
}

void DialogDownloadMeteoData::done(bool res)
{

    if(res)  // ok was pressed
    {
        QDate firstDate = firstYearEdit.date();
        QDate lastDate = lastYearEdit.date();

        if ((! firstDate.isValid()) || (! lastDate.isValid()))
        {
            QMessageBox::information(nullptr, "Missing parameter", "Select download period");
            return;
        }
        else
        {
             QListWidgetItem* item = nullptr;
             for (int i = 0; i < dailyVar.count()-1; ++i)
             {
                    item = dailyVar.item(i);
                    if (daily_item12.isSelected() || item->isSelected())
                        varD.append(item->text());

             }
             for (int i = 0; i < hourlyVar.count()-1; ++i)
             {
                    item = hourlyVar.item(i);
                    if (hourly_item7.isSelected() || item->isSelected())
                        varH.append(item->text());

             }
             if (varD.isEmpty() && varH.isEmpty())
             {
                 QMessageBox::information(nullptr, "Missing parameter", "Select variable");
                 return;
             }

             prec0024 = true;
             if ( daily_item4.isSelected() || daily_item12.isSelected() )
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
         }
    }
    else    // cancel, close or exc was pressed
    {
        QDialog::done(QDialog::Rejected);
        return;
    }

}


QStringList DialogDownloadMeteoData::getVarD() const
{
    return varD;
}

QStringList DialogDownloadMeteoData::getVarH() const
{
    return varH;
}

QDate DialogDownloadMeteoData::getFirstDate()
{
    return firstYearEdit.date();
}

QDate DialogDownloadMeteoData::getLastDate()
{
    return lastYearEdit.date();
}

bool DialogDownloadMeteoData::getPrec0024() const
{
    return prec0024;
}
