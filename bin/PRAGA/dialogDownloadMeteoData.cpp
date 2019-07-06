#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QCheckBox>
#include <QDateEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRadioButton>

#include "dialogDownloadMeteoData.h"
#include "pragaProject.h"

bool downloadMeteoData(PragaProject* project_)
{
    if(project_->nrMeteoPoints == 0)
    {
         QMessageBox::information(nullptr, "DB not existing", "Create or Open a meteo points database before download");
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
       QMessageBox::information(nullptr, "Missing parameter", "Select hourly or daily");
       return downloadMeteoData(project_);
   }
   else if ((! firstDate.isValid()) || (! lastDate.isValid()))
   {
       QMessageBox::information(nullptr, "Missing parameter", "Select download period");
       return downloadMeteoData(project_);
   }
   else if (!item1.isSelected() && !item2.isSelected() && !item3.isSelected() && !item4.isSelected() && !item5.isSelected() && !item6.isSelected())
   {
       QMessageBox::information(nullptr, "Missing parameter", "Select variable");
       return downloadMeteoData(project_);
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

            if (! project_->downloadDailyDataArkimet(var, prec0024, firstDate, lastDate, true))
            {
                QMessageBox::information(nullptr, "Error!", "Error in daily download");
                return false;
            }
        }

        if (hourly.isChecked())
        {
            if (! project_->downloadHourlyDataArkimet(var, firstDate, lastDate, true))
            {
                QMessageBox::information(nullptr, "Error!", "Error in hourly download");
                return false;
            }
        }

        return true;
    }
}
