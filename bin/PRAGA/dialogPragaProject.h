#ifndef DIALOGPRAGAPROJECT_H
#define DIALOGPRAGAPROJECT_H

#include <QWidget>

#ifndef PRAGAPROJECT_H
    #include "pragaProject.h"
#endif

#ifndef DIALOGPROJECT_H
    #include "dialogProject.h"
#endif


class DialogPragaProject : public DialogProject
{
    Q_OBJECT

    public:
        explicit DialogPragaProject(PragaProject* myProject);

        bool acceptPraga();
        void accept();

    protected:
        PragaProject* project_;

    private:

};

#endif // DIALOGPRAGAPROJECT_H
