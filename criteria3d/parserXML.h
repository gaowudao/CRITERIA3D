#ifndef READXML_H
#define READXML_H

    #ifndef QSTRING_H
        #include <QString>
    #endif

    class Crit3DProject;

    bool readXmlProject(Crit3DProject* myProject, QString xmlFileName);

#endif // READXML_H
