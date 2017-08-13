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

    #define PROXY_VAR_NR 6
    #define PEARSONSTANDARDTHRESHOLD 0.21
    #define PREC_BINARY_THRESHOLD 0.5
    #define PREC_THRESHOLD 0.2

#endif // INTERPOLATIONCONSTS_H
