#ifndef READXMLGRAPEVINEPARAMETERS_H
#define READXMLGRAPEVINEPARAMETERS_H

    #ifndef QSTRING_H
        #include <QString>
    #endif

    //struct Tvine;
    class Vine3DProject;

    bool readXmlProject(Vine3DProject* myProject, QString xmlFileName);

#endif // READXMLGRAPEVINEPARAMETERS_H
