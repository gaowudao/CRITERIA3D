#include <stdio.h>
#include "dbMeteoGrid.h"
#include "project.h"
#include "commonConstants.h"

#include <QtSql>


//Crit3DMeteoGridDbHandler meteoGridDbHandler;
static Project* projectEraclitoWG2D ;

int main ()
{

    QString pathXML;
    QString myError;
    pathXML = "../../PRAGA/DATA/METEOGRID/DBGridXML_Eraclito4.xml";
    myError = "errorDefault";
    projectEraclitoWG2D->loadMeteoGridDB(pathXML);
    return 0;
}
