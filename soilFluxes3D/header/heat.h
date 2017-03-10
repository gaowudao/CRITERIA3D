double IsothermalVaporConductivity(long i, double h, double myT);
double SoilRelativeHumidity(double h, double myT);
double SoilHeatCapacity(long i, double theta);
double SoilHeatConductivity(long i);
double VaporFromPsiTemp(double h, double T);
double ThermalVaporConductivity(long i, double myTMean, double h);
void restoreHeat();
void initializeBalanceHeat();
void updateBalanceHeatWholePeriod();
bool HeatComputation(double myTime);

