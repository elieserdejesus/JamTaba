#ifndef UTILS_H
#define UTILS_H

#include <cmath>

class Utils
{
public:
    static float linearGainToPower(float linearGain){
        return std::pow( linearGain, 3);
    }

    static float poweredGainToLinear(float poweredGain){
        return std::pow( poweredGain, 1.0/3);
    }
};

#endif // UTILS_H
