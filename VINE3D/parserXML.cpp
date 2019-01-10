#include <QDomElement>
#include <QFile>

#include "meteo.h"
#include "vine3DProject.h"
#include "plant.h"
#include "dataHandler.h"
#include "parserXML.h"
#include "utilities.h"


bool stopParsing(Vine3DProject* myProject, QString myWarning, bool isIgnored)
{
    myProject->logError(myWarning);
    return(!isIgnored);
}

bool parseXMLFile(Vine3DProject* myProject, QString xmlFileName, QDomDocument* myDocument)
{
    if (xmlFileName == "")
    {
        myProject->projectError = "Please select a xmlFile";
        return(false);
    }

    QFile myFile(xmlFileName);
    if (!myFile.open(QIODevice::ReadOnly))
    {
       myProject->projectError = "Open xml failed:" + xmlFileName + "\n" + myFile.errorString();
        return (false);
    }

    QString myError;
    int myErrLine, myErrColumn;
    if (!myDocument->setContent(&myFile, &myError, &myErrLine, &myErrColumn))
    {
        myProject->projectError = "Parse xml failed:" + xmlFileName
                            + " Row: " + QString::number(myErrLine)
                            + " - Column: " + QString::number(myErrColumn)
                            + "\n" + myError;
        myFile.close();
        return(false);
    }

    myFile.close();
    return true;
}

bool readXmlProject(Vine3DProject* myProject, QString xmlFileName)
{
   myProject->logInfo("Read xml " + xmlFileName);

   QString myPath = getPath(xmlFileName);
   QDomDocument myDocument("Project");
   if (!parseXMLFile(myProject, xmlFileName, &myDocument))
   {
       myProject->logError();
       return false;
   }

   QDomNode child;
   QDomNode ancestor = myDocument.documentElement().firstChild();
   QString myTag;
   int nrTokens = 0;
   const int nrRequiredToken = 11;

   while(!ancestor.isNull())
   {
       if (ancestor.toElement().tagName().toUpper() == "DATABASE")
       {
           child = ancestor.firstChild();
           while( !child.isNull())
           {
               myTag = child.toElement().tagName().toUpper();
               if ((myTag == "SQLDRIVER") || (myTag == "DRIVER"))
                    {myProject->sqlDriver = child.toElement().text(); nrTokens++;}
               else if ((myTag == "HOSTNAME") || (myTag == "HOST"))
                    {myProject->hostName = child.toElement().text(); nrTokens++;}
               else if (myTag == "PORT")
                    {myProject->connectionPort = child.toElement().text().toInt(); nrTokens++;}
               else if ((myTag == "DATABASENAME") || (myTag == "DATABASE"))
                    {myProject->databaseName = child.toElement().text(); nrTokens++;}
               else if ((myTag == "USERNAME") || (myTag == "USER"))
                    {myProject->userName = child.toElement().text(); nrTokens++;}
               else if (myTag == "PASSWORD")
                    {myProject->password = child.toElement().text(); nrTokens++;}

               child = child.nextSibling();
           }
       }
       else if (ancestor.toElement().tagName().toUpper() == "PROJECT")
       {
           child = ancestor.firstChild();
           while( !child.isNull())
           {
               myTag = child.toElement().tagName().toUpper();
               if ((myTag == "ID") || (myTag == "IDAREA"))
                   {myProject->idArea = child.toElement().text(); nrTokens++;}
               else if ((myTag == "PATH") || (myTag == "PROJECTPATH"))
                    {myProject->path = myPath + child.toElement().text(); nrTokens++;}
               else if ((myTag == "DTM") || (myTag == "DTMFILE") || (myTag == "DTMFILENAME"))
                    {myProject->dtmFileName = child.toElement().text(); nrTokens++;}
               else if (myTag == "FIELDMAP")
                    {myProject->fieldMapName = child.toElement().text();}
               child = child.nextSibling();
           }
       }
       else if (ancestor.toElement().tagName().toUpper() == "SETTINGS")
       {
           child = ancestor.firstChild();
           while( !child.isNull())
           {
               myTag = child.toElement().tagName().toUpper();
               if ((myTag == "HOURLYFRACTION") || (myTag == "HOURLYINTERVALS"))
                    {myProject->hourlyIntervals = child.toElement().text().toInt();}
               else if ((myTag == "TIMEREFERENCE") || (myTag == "TIMETYPE"))
                    {myProject->gisSettings.isUTC = (child.toElement().text().toUpper() == "UTC"); nrTokens++;}
               else if ((myTag == "TOTALDEPTH") || (myTag == "SOILDEPTH"))
                    myProject->soilDepth = child.toElement().text().toFloat();
               else if ((myTag == "WINDINTENSITYDEFAULT") || (myTag == "WINDDEFAULT"))
                    myProject->windIntensityDefault = child.toElement().text().toFloat();

               child = child.nextSibling();
           }
       }
       else if (ancestor.toElement().tagName().toUpper() == "GEO")
       {
           child = ancestor.firstChild();
           while( !child.isNull())
           {
               myTag = child.toElement().tagName().toUpper();
               // TO DO: controllo
               if ((myTag == "UTMZONE") || (myTag == "UTM"))
                    {myProject->gisSettings.utmZone = child.toElement().text().toInt(); nrTokens++;}
               else if ((myTag == "TIMEZONE") || (myTag == "TIME"))
                    {myProject->gisSettings.timeZone = child.toElement().text().toInt();}
               child = child.nextSibling();
           }
       }
       ancestor = ancestor.nextSibling();
   }
   myDocument.clear();

   if (nrTokens < nrRequiredToken)
   {
       int missingTokens = nrRequiredToken - nrTokens;
       myProject->projectError = "Missing " + QString::number(missingTokens) + " key informations.";
       return false;
   }
   return true;
}

