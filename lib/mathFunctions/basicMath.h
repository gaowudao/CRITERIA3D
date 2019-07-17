#ifndef BASICMATH_H
#define BASICMATH_H

    #ifndef _VECTOR_
        #include <vector>
    #endif

    bool sameSignNoZero(float a, float b);
    bool sameSign(float a, float b);
    bool greaterThan(float a, float b);
    bool greaterThan(double a, double b);
    bool compareValue(float a, float b, bool isPositive);
    int integralPart(double number);
    double fractionalPart(double number);
    double inputSwitch (double x, double y1, double y2);
    double stepFunction (double x, double change, double y1, double y2);
    double boundedValue (double x, double lowerBound, double upperBound);
    void directRotation(float *point, float angle);
    void inverseRotation(float *point, float angle);
    float distance(float* x,float* y, int vectorLength);
    float distance2D(float x1,float y1, float x2, float y2);
    float norm(float* x, int vectorLength);
    bool findLinesIntersection(float q1, float m1, float q2, float m2, float* x, float* y);
    bool findLinesIntersectionAboveThreshold(float q1, float m1, float q2, float m2, float myThreshold, float* x, float* y);
    int sgn(float v);
    bool isNODATA(double value);
    char* decimal_to_binary(unsigned int n, int nrBits);

    namespace sorting
    {
        void quicksortAscendingInteger(int *x,int first, int last);
        void quicksortDescendingInteger(int *x, int first,int last);
        void quicksortAscendingDouble(double *x, int first,int last);
        void quicksortAscendingFloat(std::vector<float> &values, int first,int last);
        double percentile(double* list, int *nList, double perc, bool sortValues);
        float percentile(std::vector<float> &list, int* nList, float perc, bool sortValues);
    }


#endif // BASICMATH_H
