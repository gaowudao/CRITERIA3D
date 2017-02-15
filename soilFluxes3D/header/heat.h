
double getSoilRelativeHumidity(double myPsi, double myT);
double getMeanIsothermalVaporConductivity(long myIndex, long myLinkIndex);
double getHeatCapacity(long i, double myH);
double getHeatConductivity(long i);

double getNonIsothermalAirVaporConductivity(long i, double myTMean);
void restoreHeat();
void initializeBalanceHeat();
void updateBalanceHeatWholePeriod();
bool HeatComputation(double myTime);

