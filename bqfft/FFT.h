/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    bqfft

    A small library wrapping various FFT implementations for some
    common audio processing use cases.

    Copyright 2007-2015 Particular Programs Ltd.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the names of Chris Cannam and
    Particular Programs Ltd shall not be used in advertising or
    otherwise to promote the sale, use or other dealings in this
    Software without prior written authorization.
*/

#ifndef BQFFT_FFT_H
#define BQFFT_FFT_H

#include "system/sysutils.h"

#include <string>
#include <set>

namespace breakfastquay {

class FFTImpl;

/**
 * Provide the basic FFT computations we need, using one of a set of
 * candidate FFT implementations (depending on compile flags).
 *
 * Implements real->complex FFTs of power-of-two sizes only.  Note
 * that only the first half of the output signal is returned (the
 * complex conjugates half is omitted), so the "complex" arrays need
 * room for size/2+1 elements.
 *
 * The "interleaved" functions use the format sometimes called CCS --
 * size/2+1 real+imaginary pairs.  So, the array elements at indices 1
 * and size+1 will always be zero (since the signal is real).
 * 
 * All pointer arguments must point to valid data. A NullArgument
 * exception is thrown if any argument is NULL.
 *
 * Neither forward nor inverse transform is scaled.
 *
 * This class is reentrant but not thread safe: use a separate
 * instance per thread, or use a mutex.
 */
class FFT
{
public:
    enum Exception {
        NullArgument, InvalidSize, InvalidImplementation, InternalError
    };

    FFT(int size, int debugLevel = 0); // may throw InvalidSize
    ~FFT();

    void forward(const double *R__ realIn, double *R__ realOut, double *R__ imagOut);
    void forwardInterleaved(const double *R__ realIn, double *R__ complexOut);
    void forwardPolar(const double *R__ realIn, double *R__ magOut, double *R__ phaseOut);
    void forwardMagnitude(const double *R__ realIn, double *R__ magOut);

    void forward(const float *R__ realIn, float *R__ realOut, float *R__ imagOut);
    void forwardInterleaved(const float *R__ realIn, float *R__ complexOut);
    void forwardPolar(const float *R__ realIn, float *R__ magOut, float *R__ phaseOut);
    void forwardMagnitude(const float *R__ realIn, float *R__ magOut);

    void inverse(const double *R__ realIn, const double *R__ imagIn, double *R__ realOut);
    void inverseInterleaved(const double *R__ complexIn, double *R__ realOut);
    void inversePolar(const double *R__ magIn, const double *R__ phaseIn, double *R__ realOut);
    void inverseCepstral(const double *R__ magIn, double *R__ cepOut);

    void inverse(const float *R__ realIn, const float *R__ imagIn, float *R__ realOut);
    void inverseInterleaved(const float *R__ complexIn, float *R__ realOut);
    void inversePolar(const float *R__ magIn, const float *R__ phaseIn, float *R__ realOut);
    void inverseCepstral(const float *R__ magIn, float *R__ cepOut);

    // Calling one or both of these is optional -- if neither is
    // called, the first call to a forward or inverse method will call
    // init().  You only need call these if you don't want to risk
    // expensive allocations etc happening in forward or inverse.
    void initFloat();
    void initDouble();

    enum Precision {
        SinglePrecision = 0x1,
        DoublePrecision = 0x2
    };
    typedef int Precisions;

    /**
     * Return the OR of all precisions supported by this
     * implementation. All of the functions (float and double) are
     * available regardless of the supported implementations, but they
     * will be calculated at the proper precision only if it is
     * available. (So float functions will be calculated using doubles
     * and then truncated if single-precision is unavailable, and
     * double functions will use single-precision arithmetic if double
     * is unavailable.)
     */
    Precisions getSupportedPrecisions() const;

    static std::set<std::string> getImplementations();
    static std::string getDefaultImplementation();
    static void setDefaultImplementation(std::string);

#ifdef FFT_MEASUREMENT
    static std::string tune();
#endif

protected:
    FFTImpl *d;
    static std::string m_implementation;
    static void pickDefaultImplementation();
};

}

#endif
