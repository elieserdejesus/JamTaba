#ifndef UTILS_H
#define UTILS_H

#include <cmath>

class Utils // TODO - use a namespace
{

public:

    static float linearGainToPower(float linearGain)
    {
        return std::pow(linearGain, 3);
    }

    static float poweredGainToLinear(float poweredGain)
    {
        return std::pow(poweredGain, 1.0/3);
    }

    static float dbToLinear(float dbValue)
    {
        // db-to-linear(x) = 10^(x / 20)
        return std::pow(10, dbValue/20);
    }

    static float linearToDb(float linearValue)
    {
        // linear-to-db(x) = log(x) * 20
        return std::log10(linearValue) * 20.0;
    }
};

#endif // UTILS_H
