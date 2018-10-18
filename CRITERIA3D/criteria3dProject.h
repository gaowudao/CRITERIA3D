#ifndef CRITERIA3DPROJECT_H
#define CRITERIA3DPROJECT_H

    #define CRITERIA3D

    #ifndef CRIT3DPROJECT_H
        #include "crit3dProject.h"
    #endif

    #ifndef PROJECT_H
        #include "project.h"
    #endif

    #include "elaborationSettings.h"

    #include <QString>

    class Criteria3DProject : public Project
    {
    private:

    public:
        Crit3DElaborationSettings* elaborationSettings;
        Crit3DProject myProject;

        Criteria3DProject();

        bool readCriteria3DSettings();

        bool loadModelParameters(QString dbName);
        bool loadSoilData(QString dbName);
        bool loadSoilMap(QString fileName);
        bool initializeCriteria3D();
    };


#endif // CRITERIA3DPROJECT_H
