#ifndef SOIL_H
#define SOIL_H

    namespace soil {

        enum units {METER, KPA, CM};

        /*!
         * \brief The Crit3DTexture class
         */
        class Crit3DTexture
        {
        public:
            float sand;
            float silt;
            float clay;
            int classUSDA;
            int classNL;

            Crit3DTexture();
            /*!
             * \brief Crit3DTexture class constructor
             * \param sand fraction of sand [-]
             * \param silt fraction of silt [-]
             * \param clay fraction of clay [-]
             */
            Crit3DTexture (float sand, float silt, float clay);
        };

        class Crit3DVanGenuchten
        {
        public:
            double alpha;                   /*!<  [kPa^-1] Van Genuchten parameter */
            double n;                       /*!<  [-] Van Genuchten parameter */
            double m;                       /*!<  [-] Van Genuchten parameter (restricted: 1-1/n) */
            double he;                      /*!<  [kPa] air potential (modified VG - Ippisch, 2006) */
            double sc;                      /*!<  [-] reduction factor (modified VG - Ippisch, 2006) */
            double thetaR;                  /*!<  [m^3 m^-3] */
            double thetaS;                  /*!<  [m^3 m^-3] volumetric water content at saturation */
            double refThetaS;               /*!<  [m^3 m^-3] reference volumetric water content at saturation */

            Crit3DVanGenuchten();
        };

        /*! Driessen parameters for empirical infiltration model
         * van Keulen, Wolf, 1986 */
        class Crit3DDriessen
        {
        public:
            double k0;                          /*!<   [cm day^-1] saturated hydraulic conductivity */
            double maxSorptivity;               /*!<   [cm day^-1/2] maximum sorptivity (sorptivity of a completely dry matrix) */
            double gravConductivity;            /*!<   [cm day^-1] infiltration due to gravitational force */

            Crit3DDriessen();
        };

        class Crit3DWaterConductivity
        {
        public:
            double kSat;                        /*!<   [cm day^-1] saturated conductivity  */
            double l;                           /*!<   [-] tortuosity parameter (Van Genuchten - Mualem)  */

            Crit3DWaterConductivity();
        };

        class Crit3DSoilClass
        {
        public:
            Crit3DVanGenuchten vanGenuchten;
            Crit3DWaterConductivity waterConductivity;
            Crit3DDriessen Driessen;
        };

        class Crit3DHorizon
        {
        public:
            double upperDepth, lowerDepth;      /*!<   [m]   */
            double coarseFragments;             /*!<   [-] 0-1  */
            double organicMatter;               /*!<   [-] 0-1  */
            double bulkDensity;                 /*!<   [g/cm^3]  */
            double fieldCapacity;               /*!<   [kPa]  */
            double wiltingPoint;                /*!<   [kPa]  */
            double waterContentFC;              /*!<   [m^3 m^-3]  */
            double waterContentWP;              /*!<   [m^3 m^-3]  */
            double PH;                          /*!<   [-]  */
            double CEC;                         /*!<   [meq/100g]  */

            Crit3DTexture texture;
            Crit3DVanGenuchten vanGenuchten;
            Crit3DWaterConductivity waterConductivity;
            Crit3DDriessen Driessen;

            Crit3DHorizon();
        };

        class Crit3DSoil
        {
        public:
            int id;
            int nrHorizons;
            double totalDepth;          /*!<   [m] */
            Crit3DHorizon* horizon;

            Crit3DSoil();
            Crit3DSoil(int idSoil, int nrHorizons);
            void initialize(int idSoil, int nrHorizons);
            void cleanSoil();
        };

        class Crit3DLayer
        {
        public:
            double depth;               /*!<   [m] */
            double thickness;           /*!<   [m] */
            double waterContent;        /*!<   [mm] */
            double soilFraction;        /*!<   [-]fraction of soil (1 - coarse fragment fraction) */
            double SAT;                 /*!<   [mm] water content at saturation  */
            double FC;                  /*!<   [mm] water content at field capacity */
            double WP;                  /*!<   [mm] water content at wilting point  */
            double HH;                  /*!<   [mm] water content at hygroscopic humidity */
            double critical;            /*!<   [mm] water content at critical point for water movement (typical FC)  */
            double maxInfiltration;     /*!<   [mm]  */
            double flux;                /*!<   [mm]  */
            double runoff;              /*!<   [mm]  */

            Crit3DHorizon *horizon;

            Crit3DLayer();
        };


     int getUSDATextureClass(float sand, float silt, float clay);
     int getNLTextureClass(float sand, float silt, float clay);
     int getHorizonIndex(Crit3DSoil* soil, float depth);
     double getFieldCapacity(Crit3DHorizon* horizon, soil::units unit);
     double getWiltingPoint(soil::units unit);
     double getThetaFC(Crit3DHorizon* horizon);
     double getThetaWP(Crit3DHorizon* horizon);

     double kPaToMeters(double value);
     double metersTokPa(double value);

     double kPaToCm(double value);
     double cmTokPa(double value);

     double SeFromTheta(double theta, Crit3DHorizon* horizon);
     double psiFromTheta(double theta, Crit3DHorizon* horizon);
     double thetaFromSignPsi(double signPsi, Crit3DHorizon* horizon);

     double waterConductivity(double Se, Crit3DHorizon* horizon);

     double estimateBulkDensity(Crit3DHorizon* mySoil, double totalPorosity);
     double estimateSaturatedConductivity(Crit3DHorizon* mySoil, double bulkDensity);
     double estimateTotalPorosity(Crit3DHorizon* mySoil, double bulkDensity);

     double getVolumetricWaterContent(Crit3DLayer* layer);
     double getWaterContent(double signPsi, Crit3DLayer* layer);
     double getWaterPotential(Crit3DLayer* layer);
     double getWaterConductivity(Crit3DLayer* layer);
    }

#endif // SOIL_H
