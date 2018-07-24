#ifndef INTERPOLATIONCONSTS_H
#define INTERPOLATIONCONSTS_H

#define MIN_REGRESSION_POINTS 3
#define PEARSONSTANDARDTHRESHOLD 0.2
#define PREC_JRC_THRESHOLD 0.5
#define PREC_THRESHOLD 0.2

namespace interpolationMethod
{
    enum { idw, kriging, shepard };
}

namespace proxyVars
{
    enum TProxyVar { height, urbanFraction, orogIndex, seaDistance, aspect, generic, noProxy };
}

enum TkrigingMode {KRIGING_SPHERICAL = 1,
                   KRIGING_EXPONENTIAL=2,
                   KRIGING_GAUSSIAN=3,
                   KRIGING_LINEAR=4
                  };



#endif // INTERPOLATIONCONSTS_H
