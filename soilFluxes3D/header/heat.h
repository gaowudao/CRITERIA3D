double getSoilRelativeHumidity(double myPsi, double myT);
double getMeanIsothermalVaporConductivity(long myIndex, long myLinkIndex);
double getSpecificHeat(long i, double myH);

double getNonIsothermalAirVaporConductivity(long i, double myTMean);
void restoreHeat();
void initializeBalanceHeat();
void updateBalanceHeatWholePeriod();
bool computeHeat(float myTime);

