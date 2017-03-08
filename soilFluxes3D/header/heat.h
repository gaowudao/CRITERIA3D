double IsothermalVaporConductivity(long i, double myT);
double getSoilRelativeHumidity(double myPsi, double myT);
double soilHeatCapacity(long i, double H);
double soilHeatConductivity(long i);
double VaporFromPsiTemp(double Psi, double T);
double ThermalVaporConductivity(long i, double myTMean, double psi);
void restoreHeat();
void initializeBalanceHeat();
void updateBalanceHeatWholePeriod();
bool HeatComputation(double myTime);

