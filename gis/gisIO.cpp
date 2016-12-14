/*!
    \copyright 2010-2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by A.R.P.A. Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/


#include <algorithm>
#include <fstream>
#include <sstream>

#include "commonConstants.h"
#include "gis.h"

using namespace std;


    bool splitKeyValue(std::string myLine, std::string *myKey, std::string *myValue)
    {
        *myKey = ""; *myValue = "";
        istringstream myStream(myLine);

        myStream >> *myKey;
        myStream >> *myValue;
        if ((*myKey == "") || (*myValue == "")) return (false);
        else return(true);
    }

    string upperCase(string myStr)
    {
        string upperCaseStr = myStr;
        transform(myStr.begin(), myStr.end(), upperCaseStr.begin(), ::toupper);
        return(upperCaseStr);

    }


namespace gis
    {


/*!
     * \brief Read a ESRI grid header file (.hdr). llCorner is lower-left corner
     * \param myFileName string
     * \param myHeader Crit3DGridHeader pointer
     * \param myError string pointer
     * \return true on success, false otherwise
     */
    bool readEsriGridHeader(string myFileName, gis::Crit3DGridHeader *myHeader, string* myError)
    {
        string myLine, myKey, upKey, myValue;
        int nrKeys = 0;

        myFileName += ".hdr";
        ifstream  myFile (myFileName.c_str());

        if (!myFile.is_open())
        {
            *myError = "File .hdr error";
            return false;
        }

        while (myFile.good())
        {
            getline (myFile, myLine);
            if (splitKeyValue(myLine, &myKey, &myValue))
            {
                upKey = upperCase(myKey);
                if ((upKey == "NCOLS") || (upKey == "NROWS") || (upKey == "CELLSIZE")
                    ||    (upKey == "XLLCORNER") || (upKey == "YLLCORNER")
                    ||    (upKey == "NODATA_VALUE") || (upKey == "NODATA"))
                    nrKeys++;

                if (upKey == "NCOLS")
                    myHeader->nrCols = ::atoi(myValue.c_str());

                else if (upKey == "NROWS")
                    myHeader->nrRows = ::atoi(myValue.c_str());

                else if (upKey == "XLLCORNER")
                    myHeader->llCorner->x = ::atof(myValue.c_str());

                else if (upKey == "YLLCORNER")
                    myHeader->llCorner->y = ::atof(myValue.c_str());

                else if (upKey == "CELLSIZE")
                    myHeader->cellSize = ::atof(myValue.c_str());

                else if ((upKey == "NODATA_VALUE") || (upKey == "NODATA"))
                    myHeader->flag = (float)(::atof(myValue.c_str()));
            }
        }
        myFile.close();

        if (nrKeys < 6)
        {
            *myError = "Key missing in .hdr file.";
            return(false);
        }
        return(true);
    }


    /*!
     * \brief Read a ESRI grid data file (.flt)
     * \param myFileName string name file
     * \param myGrid Crit3DRasterGrid pointer
     * \param myError string pointer
     * \return true on success, false otherwise
     */
    bool readEsriGridFlt(string myFileName, gis::Crit3DRasterGrid *myGrid, string *myError)
    {
        myFileName += ".flt";

        FILE* filePointer;

        /*! alloc memory */
        myGrid->value = (float **) calloc(myGrid->header->nrRows, sizeof(float *));
        for (long myRow = 0; myRow < myGrid->header->nrRows; myRow++)
        {
            myGrid->value[myRow] = (float *) calloc(myGrid->header->nrCols, sizeof(float));
            if (myGrid->value[myRow] == NULL)
            {
                *myError = "Memory error: file too big.";
                myGrid->freeGrid();
                return(false);
            }
        }

        filePointer = fopen (myFileName.c_str(), "rb" );
        if (filePointer == NULL)
        {
            *myError = "File .flt error.";
            return(false);
        }

        for (long myRow = 0; myRow < myGrid->header->nrRows; myRow++)
            fread (myGrid->value[myRow], sizeof(float), myGrid->header->nrCols, filePointer);

        fclose (filePointer);

        return (true);
    }


    /*!
     * \brief Write a ESRI grid header file (.hdr)
     * \param myFileName string name file
     * \param myHeader Crit3DGridHeader pointer
     * \param myError string pointer
     * \return true on success, false otherwise
     */
    bool writeEsriGridHeader(string myFileName, gis::Crit3DGridHeader *myHeader, string* myError)
    {
        myFileName += ".hdr";
        ofstream myFile (myFileName.c_str());

        if (!myFile.is_open())
        {
            *myError = "File .hdr error.";
            return(false);
        }

        myFile << "ncols         " << myHeader->nrCols << "\n";
        myFile << "nrows         " << myHeader->nrRows << "\n";

        char* xllcorner = new char[20];
        char* yllcorner = new char[20];
        sprintf(xllcorner, "%.03f", myHeader->llCorner->x);
        sprintf(yllcorner, "%.03f", myHeader->llCorner->y);

        myFile << "xllcorner     " << xllcorner << "\n";

        myFile << "yllcorner     " << yllcorner << "\n";

        myFile << "cellsize      " << myHeader->cellSize << "\n";

        /*! different version of NODATA */
        myFile << "NODATA_value  " << myHeader->flag << "\n";
        myFile << "NODATA        " << myHeader->flag << "\n";

        /*! crucial information */
        myFile << "byteorder     LSBFIRST" << "\n";

        myFile.close();

        return(true);
    }


    /*!
     * \brief write a ESRI grid data file (.flt)
     * \param myFileName string name file
     * \param myHeader Crit3DGridHeader pointer
     * \param myError string pointer
     * \return true on success, false otherwise
     */
    bool writeEsriGridFlt(string myFileName, gis::Crit3DRasterGrid *myGrid, string *myError)
    {
        myFileName += ".flt";

        FILE* filePointer;

        filePointer = fopen (myFileName.c_str(), "wb" );
        if (filePointer == NULL)
        {
            *myError = "File .flt error.";
            return(false);
        }

        for (long myRow = 0; myRow < myGrid->header->nrRows; myRow++)
            fwrite (myGrid->value[myRow], sizeof(float), myGrid->header->nrCols, filePointer);

        fclose (filePointer);
        return (true);
    }

    bool readEsriGrid(string myFileName, Crit3DRasterGrid* myGrid, string* myError)
    {
        if (myGrid == NULL) return(false);
        myGrid->isLoaded = false;

        Crit3DGridHeader *myHeader;
        myHeader = new Crit3DGridHeader;

        if(gis::readEsriGridHeader(myFileName, myHeader, myError))
        {
            myGrid->freeGrid();
            *(myGrid->header) = *myHeader;

            if (gis::readEsriGridFlt(myFileName, myGrid, myError))
            {
                myGrid->isLoaded = true;
                updateMinMaxRasterGrid(myGrid);
            }
        }

        return(myGrid->isLoaded);
    }

    bool writeEsriGrid(string myFileName, Crit3DRasterGrid *myGrid, string *myError)
    {
        if (gis::writeEsriGridHeader(myFileName, myGrid->header, myError))
            if (gis::writeEsriGridFlt(myFileName, myGrid, myError))
                return(true);

        return(false);
    }

    bool getGeoExtentsFromUTMHeader(const Crit3DGisSettings& mySettings, const Crit3DGridHeader& utmHeader, Crit3DGridHeader *latLonHeader)
    {
        Crit3DGeoPoint v[4];

        Crit3DUtmPoint myVertex = *(utmHeader.llCorner);
        gis::getLatLonFromUtm(mySettings, myVertex, &(v[0]));

        myVertex.x += utmHeader.nrCols *utmHeader.cellSize;
        gis::getLatLonFromUtm(mySettings, myVertex, &(v[1]));

        myVertex.y += utmHeader.nrRows *utmHeader.cellSize;
        gis::getLatLonFromUtm(mySettings, myVertex, &(v[2]));

        myVertex.x = utmHeader.llCorner->x;
        gis::getLatLonFromUtm(mySettings, myVertex, &(v[3]));

        return true;

        /*
        Dim myX As Double, myY As Double
        Dim xLL As Double, yLL As Double, xUR As Double, yUR As Double
        Dim i As Long

        getExtentsFromLatLonHeader = False

        With myLatLonHeader.latLonInfo
            GIS_UTM_util.LatLonToUTM_ForceZone GIS.WGS84, myUTMZone, .Lat0, .Lon0, xLL, yLL
            GIS_UTM_util.LatLonToUTM_ForceZone GIS.WGS84, myUTMZone, .Lat0 + .Ly * myLatLonHeader.nrRows, .Lon0 + .Lx * myLatLonHeader.nrCols, xUR, yUR

            For i = 0 To myLatLonHeader.nrCols
                GIS_UTM_util.LatLonToUTM_ForceZone GIS.WGS84, myUTMZone, .Lat0, .Lon0 + .Lx * i, myX, myY
                If myY < yLL Then
                    yLL = myY
                End If
                GIS_UTM_util.LatLonToUTM_ForceZone GIS.WGS84, myUTMZone, .Lat0 + .Ly * myLatLonHeader.nrRows, .Lon0 + .Lx * i, myX, myY
                If myY > yUR Then
                    yUR = myY
                End If
            Next i

            For i = 0 To myLatLonHeader.nrRows
                GIS_UTM_util.LatLonToUTM_ForceZone GIS.WGS84, myUTMZone, .Lat0 + .Ly * i, .Lon0, myX, myY
                If myX < xLL Then
                    xLL = myX
                End If
                GIS_UTM_util.LatLonToUTM_ForceZone GIS.WGS84, myUTMZone, .Lat0 + .Ly * i, .Lon0 + .Lx * myLatLonHeader.nrCols, myX, myY
                If myX > xUR Then
                    xUR = myX
                End If
            Next i
        End With

        With myUtmHeader
            .cellSize = myCellSize
            .xllCorner = Fix(xLL)
            .yllCorner = Fix(yLL)
            .nrRows = Fix((yUR - yLL) / .cellSize) + 1
            .nrCols = Fix((xUR - xLL) / .cellSize) + 1
        End With

        getExtentsFromLatLonHeader = True
                */

    }
}

