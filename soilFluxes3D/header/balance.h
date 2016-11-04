
void halveTimeStep();

bool getForcedHalvedTime();

void setForcedHalvedTime(bool isForced);

double computeTotalWaterContent();

double getMatrixValue(long i, TlinkedNode *link);

void InitializeBalanceWater();

bool waterBalance(double deltaT, int approxNr);

void updateBalanceWaterWholePeriod();
