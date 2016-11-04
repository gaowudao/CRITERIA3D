
bool computationLoop(double deltaT);

double getWaterFlux(long index, TlinkedNode *link);

double getWaterExchange(long index, TlinkedNode *link, double deltaT);

bool computeWater(double maxTime, double *acceptedTime);

void restoreWater();
