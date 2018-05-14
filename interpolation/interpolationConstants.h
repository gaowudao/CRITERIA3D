#ifndef INTERPOLATIONCONSTS_H
#define INTERPOLATIONCONSTS_H

    namespace geostatisticsMethods
    {
        enum { idw, kriging, shepard };
    }

    namespace proxyVars
    {
        enum TProxyVar { height, urbanFraction, orogIndex, seaDistance, aspect, generic, noProxy };
    }

    namespace elaborationMethods
    {
        enum { mean, median, stdDeviation};
    }

    #define PROXY_VAR_NR 6
    #define PEARSONSTANDARDTHRESHOLD 0.21
    #define PREC_BINARY_THRESHOLD 0.5
    #define PREC_THRESHOLD 0.2

    #define GRID_MIN_COVERAGE 10 //LC settata una percentuale casuale, TBC

#endif // INTERPOLATIONCONSTS_H
