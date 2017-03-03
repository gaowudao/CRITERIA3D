double IsothermalVaporConductivity(long i, double myT);
double getSoilRelativeHumidity(double myPsi, double myT);
double soilHeatCapacity(long i, double H);
double soilHeatConductivity(long i);
double VaporFromPsiTemp(double Psi, double T);
double getNonIsothermalAirVaporConductivity(long i, double myTMean);
void restoreHeat();
void initializeBalanceHeat();
void updateBalanceHeatWholePeriod();
bool HeatComputation(double myTime);

