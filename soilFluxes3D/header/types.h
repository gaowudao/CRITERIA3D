/*!

    CRITERIA 3D
    \copyright (C) 2011 Fausto Tomei, Gabriele Antolini, Alberto Pistocchi,
    Antonio Volta, Giulia Villani, Marco Bittelli

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
    ftomei@arpa.emr.it
    fausto.tomei@gmail.com
    gantolini@arpa.emr.it
    alberto.pistocchi@gecosistema.it
    marco.bittelli@unibo.it
*/

#ifndef SOILFLUXES3DTYPES
#define SOILFLUXES3DTYPES


    #include "parameters.h"
    #include "extra.h"

    struct Tboundary{
        short type;
        float slope;
        double waterFlow;                   /*!< [m3 s-1] */
        double sumBoundaryWaterFlow;        /*!< [m3] sum of boundary water flow */
        double prescribedTotalPotential;	   /*!< [m] imposed total soil-water potential (H) */

        TboundaryHeat *Heat;
    } ;

    struct TCrit3DStructure{
        long nrLayers;
        long nrNodes;
        int nrLateralLinks;
        int maxNrColumns;

        bool computeHeat;
        bool computeSolutes;

        void initialize()
            {
                nrLayers = 0;
                nrNodes = 0;
                nrLateralLinks = 0;
                maxNrColumns = 0;
                computeHeat = false;
                computeSolutes = false;
            }
        } ;


    struct TlinkedNode{
        long index;                 /*!< index of linked elements */
        float area;                 /*!< interface area [m^2] */
        float sumFlow;              /*!< [m^3] sum of flow(i,j) */
        } ;


    struct Tsoil{
        double VG_alpha;            /*!< [m^-1] Van Genutchen alpha parameter */
        double VG_n;                /*!< [-] Van Genutchen n parameter */
        double VG_m;                /*!< [-] Van Genutchen m parameter  ]0. , 1.[ */
        double VG_he;               /*!< [m] air-entry potential for modified VG formulation [0 , 1] */
        double VG_Sc;               /*!< [-] reduction factor for modified VG formulation */
        double Theta_s;             /*!< [m^3/m^3] saturated water content */
        double Theta_r;             /*!< [m^3/m^3] residual water content */
        double K_sat;               /*!< [m/sec] saturated hydraulic conductivity */
        double Mualem_L;            /*!< [-] Mualem tortuosity parameter */

        double Roughness;           /*!< [s/m^0.33] surface: Manning roughness */
        double Pond;                /*!< [m] surface: height of immobilized water */
        } ;


     struct TCrit3Dnode{

         double Se;					/*!< [-] degree of saturation */
         double k;                  /*!< [m s^-1] soil water conductivity */
         double H;                  /*!< [m] pressure head */
         double oldH;				/*!< [m] previous pressure head */
         double bestH;				/*!< [m] pressure head of best iteration */
         double waterSinkSource;    /*!< [m^3 s^-1] water sink source */
         double Qw;                 /*!< [m^3 s^-1] water flow */

         double volume_area;		/*!< [m^3] volume of sub-surface elements : [m^2] area of surface nodes */
        float x, y, z;              /*!< [m] coordinates of the center of the element */

        Tsoil *Soil;                /*!< soil pointer */
        Tboundary *boundary;        /*!< boundary pointer */
        TlinkedNode up;				/*!< upper link */
        TlinkedNode down;			/*!< lower link */
        TlinkedNode *lateral;       /*!< lateral link */

        TCrit3DnodeExtra* extra;    /*!< extra variables for heat and solutes */

        bool isSurface;
        } ;


     struct TmatrixElement{
        long index;
        double val;
        } ;


     struct Tbalance{
        double storageWater;
        double sinkSourceWater;
        double waterMBE, waterMBR;

        double storageHeat;
        double sinkSourceHeat;
        double heatMBE, heatMBR;
        } ;

     extern TCrit3DStructure myStructure;
     extern TParameters myParameters;
     extern TCrit3Dnode *myNode;
     extern TmatrixElement **A;
     extern double *b, *C0, *C, *X;
     extern double Courant;

     extern Tbalance balanceCurrentTimeStep, balancePreviousTimeStep, balanceCurrentPeriod, balanceWholePeriod;

#endif
