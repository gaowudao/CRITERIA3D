#ifndef HYDROLOGY_H
#define HYDROLOGY_H

#include <vector>

bool searchRainfallEvent(unsigned long *firstIndex, float *sumPrec, float *maxIntensity,
                         int *lenght, int *peakHour, std::vector<float> *prec);

void refineRainfallEvent(unsigned long firstIndex, int Lenght, float whc, float *sumPrec,
                         float *maxIntensity, int *peakHour, std::vector<float> *prec);
#endif // HYDROLOGY_H
