#ifndef _FILTERS_
#define _FILTERS_

#include <QtGlobal>

namespace Audio {

/** Biquad Filter - Adapted from Ardour code: http://ardour.org/ */

class Filter
{
public:
    enum FilterType {
        LowPass,
        HighPass,
        BandPassSkirt,
        BandPass0dB,
        Notch,
        AllPass,
        Peaking,
        LowShelf,
        HighShelf
    };

    Filter (FilterType type, double samplerate, double frequency, double Q, double gain);

    void process(float *data, const quint32 samples);


    /** filter transfer function (filter response for spectrum visualization)
     * @param freq frequency
     * @return gain at given frequency in dB (clamped to -120..+120)
     */
    float dBAtFrequency(float freq) const;

    inline void reset()
    {
        z1 = z2 = 0.0;
    }

private:

    /** setup filter, compute coefficients
     *
     * @param t filter type (LowPass, HighPass, etc)
     * @param freq filter frequency
     * @param Q filter quality
     * @param gain filter gain
     */
    void initialize(FilterType t, double freq, double Q, double gain);

    double sampleRate;
    float z1, z2;      // history samples
    double a1, a2;
    double b0, b1, b2;
    FilterType type;
};
} // namespace

#endif
