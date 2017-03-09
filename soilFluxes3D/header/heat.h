double IsothermalVaporConductivity(long i, double Psi, double myT);
double SoilRelativeHumidity(double myPsi, double myT);
double SoilHeatCapacity(long i, double theta);
double SoilHeatConductivity(long i);
double VaporFromPsiTemp(double Psi, double T);
double ThermalVaporConductivity(long i, double myTMean, double psi);
void restoreHeat();
void initializeBalanceHeat();
void updateBalanceHeatWholePeriod();
bool HeatComputation(double myTime);

