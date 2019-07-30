#include "dialogPragaProject.h"


DialogPragaProject::DialogPragaProject(PragaProject *myProject) : DialogProject(myProject)
{
    project_ = myProject;
}

void DialogPragaProject::accept()
{
    project_->createProject(lineEditProjectPath->text(), lineEditProjectName->text(), lineEditProjectDescription->text());

    QDialog::done(QDialog::Accepted);
}

