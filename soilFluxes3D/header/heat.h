
double getSoilRelativeHumidity(double myPsi, double myT);
double getMeanIsothermalVaporConductivity(long myIndex, long myLinkIndex);
double soilHeatCapacity(long i, double H);
double soilHeatConductivity(long i);

double getNonIsothermalAirVaporConductivity(long i, double myTMean);
void restoreHeat();
void initializeBalanceHeat();
void updateBalanceHeatWholePeriod();
bool HeatComputation(double myTime);

