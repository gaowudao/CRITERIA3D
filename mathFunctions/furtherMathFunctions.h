#ifndef MATHEMATICALFUNCTIONS_H
#define MATHEMATICALFUNCTIONS_H

#include <vector>
#include <string>

enum estimateFunction {FUNCTION_CODE_SPHERICAL, FUNCTION_CODE_LINEAR, FUNCTION_CODE_PARABOLIC,
                       FUNCTION_CODE_EXPONENTIAL, FUNCTION_CODE_LOGARITMIC, TWOPARAMETERSPOLYNOMIAL};

    float errorFunctionPrimitive(float x);
    char *decimal_to_binary(unsigned int n, int nrBits);

    struct TfunctionInput{
        float x ;
        int nrPar ;
        float *par ;
    };

    struct TfunctionInputMonteCarlo2D{
        float x ;
        float y ;
        int nrPar ;
        float *par ;
    };

    struct TfunctionInputMonteCarlo3D{
        float x ;
        float y ;
        float z ;
        int nrPar ;
        float *par ;
    };

    namespace integration
    {
        float trapzdParametric(float (*func)(TfunctionInput), int nrPar, float *par , float a , float b , int n);
    }

    namespace integration
    {
        float trapzdParametric(float (*func)(TfunctionInput), int nrPar, float *par , float a , float b , int n);
        float qsimpParametric(float (*func)(TfunctionInput), int nrPar, float *par,float a , float b , float EPS);
        float trapzd(float (*func)(float) , float a , float b , int n);
        float trapezoidalRule(float (*func)(float) , float a , float b , int n);
        double trapezoidalRule(double (*func)(double) , double a , double b , int n);
        float simpsonRule(float (*func)(float),float a , float b , float EPS);
        double simpsonRule(double (*func)(double),double a , double b , double EPS);
        float monteCarlo3D(bool (*func)(TfunctionInputMonteCarlo3D),float den,float xLower, float xUpper, float yLower , float yUpper , float zLower , float zUpper,int nrPar, float *par, float requiredPercentageError ,  float *reachedPercentageErrorW);
        float monteCarlo2D(bool (*func)(TfunctionInputMonteCarlo2D),float den,float xLower, float xUpper, float yLower , float yUpper,int nrPar, float *par, float requiredPercentageError ,  float *reachedPercentageErrorW);
    }

    namespace interpolation
    {
        float linearInterpolation (float x, float *xColumn , float *yColumn, int dimTable );
        float linearExtrapolation(double x3,double x1,double y1,double x2 , double y2);
        double normGeneric(double *myParameters, double *myX,int nrMyX, double *myY,int myFunctionCode);
        float estimateFunction(int myFunctionCode, double *myParameters, double *myX);
        void leastSquares(int myFunctionCode,double *myParameters, int nrMyParameters,
                    double *myX,int nrMyX, double *myY,double *myLambda,
                    double *myParametersDelta, double *myParametersChange);
        bool fittingMarquardt(double *myParMin, double *myParMax,
                    double *myPar, int nrMyPar, double *myParDelta, int myMaxIterations,
                    double myEpsilon,int myFunctionCode, double *myX,int nrMyX,double *myY);
    }

    namespace sorting
    {
        void quicksortAscendingInteger(int *x,int first, int last);
        void quicksortDescendingInteger(int *x, int first,int last);
        void quicksortAscendingDouble(double *x, int first,int last);
        void quicksortAscendingFloat(std::vector<float> &values, int first,int last);
        double percentile(double* list, int *nList, double perc, bool sortValues);
        float percentile(std::vector<float> &list, int* nList, float perc, bool sortValues);
    }

    namespace matricial
    {
        int matrixSum(double**a , double**b, int rowA , int rowB, int colA, int colB,double **c);
        int matrixProduct(double **first,double**second,int colFirst, int rowFirst,int colSecond,int rowSecond,double ** multiply);
        void choleskyDecompositionSinglePointer(double *a, int n, double *p);
        void choleskyDecompositionTriangularMatrix(double **a, int n, bool isLowerMatrix);
        void transposedSquareMatrix(double **a, int n);
        void transposedMatrix(double** inputMatrix, int nrRows, int nrColumns,double** outputMatrix);
    }

    namespace distribution
    {
        float normalGauss(TfunctionInput fInput);
    }

    namespace myrandom
    {
        //float ran1(long *idum);
        //float gasdev(long *idum);
        float normalRandom(int *gasDevIset,float *gasDevGset);
        double normalRandom(int *gasDevIset,double *gasDevGset);

    }


#endif
