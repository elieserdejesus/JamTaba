#include "Filters.h"
#include <cfloat>
#include <cmath>

using namespace Audio;

#define SQUARE(x) ((x) * (x))

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Filter::Filter (FilterType type, double samplerate, double frequency, double Q, double gain) :
    sampleRate(samplerate),
    type(type),
    z1(0.0),
    z2(0.0),
    a1(0.0),
    a2(0.0),
    b0(1.0),
    b1(0.0),
    b2(0.0)
{
    initialize(type, frequency, Q, gain);
}

void Filter::process(float *data, const quint32 samples)
{
    for (uint32_t i = 0; i < samples; ++i) {
        const float xn = data[i];
        const float z = b0 * xn + z1;
        z1 = b1 * xn - a1 * z + z2;
        z2 = b2 * xn - a2 * z;
        data[i] = z;
    }

    if (!std::isfinite(z1))
        z1 = 0;
    if (!std::isfinite(z2))
        z2 = 0;
}

void Filter::setFrequency(double newFrequency)
{
    initialize(this->type, newFrequency, 1.0, 1.0); // recompute filter coeficients
}

void Filter::initialize(FilterType type, double freq, double Q, double gain)
{
    if (Q <= .001)
        Q = 0.001;

    if (freq <= 1.)
        freq = 1.;

    if (freq >= sampleRate)
        freq = sampleRate;

    /* Compute biquad filter settings.
     * Based on 'Cookbook formulae for audio EQ biquad filter coefficents'
     * by Robert Bristow-Johnson
     */
    const double A = pow(10.0, (gain / 40.0));
    const double W0 = (2.0 * M_PI * freq) / sampleRate;
    const double sinW0 = sin(W0);
    const double cosW0 = cos(W0);
    const double alpha = sinW0 / (2.0 * Q);
    const double beta = sqrt(A) / Q;

    double a0 = 0;

    switch (type) {
    case LowPass:
        b0 = (1.0 - cosW0) / 2.0;
        b1 = 1.0 - cosW0;
        b2 = (1.0 - cosW0) / 2.0;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cosW0;
        a2 = 1.0 - alpha;
        break;

    case HighPass:
        b0 = (1.0 + cosW0) / 2.0;
        b1 = -(1.0 + cosW0);
        b2 = (1.0 + cosW0) / 2.0;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cosW0;
        a2 = 1.0 - alpha;
        break;

    case BandPassSkirt:     /* Constant skirt gain, peak gain = Q */
        b0 = sinW0 / 2.0;
        b1 = 0.0;
        b2 = -sinW0 / 2.0;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cosW0;
        a2 = 1.0 - alpha;
        break;

    case BandPass0dB:     /* Constant 0 dB peak gain */
        b0 = alpha;
        b1 = 0.0;
        b2 = -alpha;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cosW0;
        a2 = 1.0 - alpha;
        break;

    case Notch:
        b0 = 1.0;
        b1 = -2.0 * cosW0;
        b2 = 1.0;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cosW0;
        a2 = 1.0 - alpha;
        break;

    case AllPass:
        b0 = 1.0 - alpha;
        b1 = -2.0 * cosW0;
        b2 = 1.0 + alpha;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cosW0;
        a2 = 1.0 - alpha;
        break;

    case Peaking:
        b0 = 1.0 + (alpha * A);
        b1 = -2.0 * cosW0;
        b2 = 1.0 - (alpha * A);
        a0 = 1.0 + (alpha / A);
        a1 = -2.0 * cosW0;
        a2 = 1.0 - (alpha / A);
        break;

    case LowShelf:
        b0 = A * ((A + 1) - ((A - 1) * cosW0) + (beta * sinW0));
        b1 = (2.0 * A) * ((A - 1) - ((A + 1) * cosW0));
        b2 = A * ((A + 1) - ((A - 1) * cosW0) - (beta * sinW0));
        a0 = (A + 1) + ((A - 1) * cosW0) + (beta * sinW0);
        a1 = -2.0 * ((A - 1) + ((A + 1) * cosW0));
        a2 = (A + 1) + ((A - 1) * cosW0) - (beta * sinW0);
        break;

    case HighShelf:
        b0 = A * ((A + 1) + ((A - 1) * cosW0) + (beta * sinW0));
        b1 = -(2.0 * A) * ((A - 1) + ((A + 1) * cosW0));
        b2 = A * ((A + 1) + ((A - 1) * cosW0) - (beta * sinW0));
        a0 = (A + 1) - ((A - 1) * cosW0) + (beta * sinW0);
        a1 = 2.0 * ((A - 1) - ((A + 1) * cosW0));
        a2 = (A + 1) - ((A - 1) * cosW0) - (beta * sinW0);
        break;
    }

    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
}

float Filter::dBAtFrequency(float freq) const
{
    const double W0 = (2.0 * M_PI * freq) / sampleRate;
    const float c1 = cosf(W0);
    const float s1 = sinf(W0);

    const float A = b0 + b2;
    const float B = b0 - b2;
    const float C = 1.0 + a2;
    const float D = 1.0 - a2;

    const float a = A * c1 + b1;
    const float b = B * s1;
    const float c = C * c1 + a1;
    const float d = D * s1;

    float rv = 20.f * log10f(sqrtf((SQUARE(a) + SQUARE(b)) * (SQUARE(c) + SQUARE(d)))
                        / (SQUARE(c) + SQUARE(d)));

    if (!std::isfinite(rv))
        rv = 0;

    return std::min(120.f, std::max(-120.f, rv));
}
