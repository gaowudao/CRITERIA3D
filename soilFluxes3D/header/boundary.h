void updateBoundary();
void updateBoundaryHeat();
void updateBoundaryWater(double deltaT);
void initializeBoundary(Tboundary *myBoundary, int myType, float slope);
double getSurfaceWaterFraction(int i);

