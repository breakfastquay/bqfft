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

#include "bqfft/FFT.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <iostream>

#include <cstdio>
#include <cmath>

using namespace breakfastquay;

BOOST_AUTO_TEST_SUITE(TestFFT)

#define DEFINE_EPS(fft) \
    float epsf = 1e-6f; \
    double eps; \
    if (fft.getSupportedPrecisions() & FFT::DoublePrecision) { \
        eps = 1e-14; \
    } else { \
        eps = epsf; \
    } \
    (void)epsf; (void)eps;

#define USING_FFT(n) \
    FFT fft(n); \
    DEFINE_EPS(fft);

#define COMPARE(a, b) BOOST_CHECK_SMALL(a-b, eps) 
#define COMPARE_F(a, b) BOOST_CHECK_SMALL(a-b, epsf) 

#define COMPARE_ZERO(a) BOOST_CHECK_SMALL(a, eps)
#define COMPARE_ZERO_F(a) BOOST_CHECK_SMALL(a, epsf)

#define COMPARE_ALL(a, x) \
    for (int cmp_i = 0; cmp_i < (int)(sizeof(a)/sizeof(a[0])); ++cmp_i) { \
        BOOST_CHECK_SMALL(a[cmp_i] - x, eps); \
    }
#define COMPARE_ALL_F(a, x) \
    for (int cmp_i = 0; cmp_i < (int)(sizeof(a)/sizeof(a[0])); ++cmp_i) { \
        BOOST_CHECK_SMALL(a[cmp_i] - x, epsf); \
    }

#define COMPARE_SCALED(a, b, s)						\
    for (int cmp_i = 0; cmp_i < (int)(sizeof(a)/sizeof(a[0])); ++cmp_i) { \
        BOOST_CHECK_SMALL(a[cmp_i]/s - b[cmp_i], eps); \
    }
#define COMPARE_SCALED_F(a, b, s)						\
    for (int cmp_i = 0; cmp_i < (int)(sizeof(a)/sizeof(a[0])); ++cmp_i) { \
        BOOST_CHECK_SMALL(a[cmp_i]/s - b[cmp_i], epsf); \
    }

#define ONE_IMPL_AUTO_TEST_CASE(name, impl) \
    BOOST_AUTO_TEST_CASE(name##_##impl) \
    { \
        std::set<std::string> impls = FFT::getImplementations(); \
        if (impls.find(#impl) == impls.end()) return; \
        FFT::setDefaultImplementation(#impl); \
        performTest_##name(); \
        FFT::setDefaultImplementation(""); \
    }

// If you add an implementation in FFT.cpp, add it also to
// ALL_IMPL_AUTO_TEST_CASE and all_implementations[] below

#define ALL_IMPL_AUTO_TEST_CASE(name) \
    void performTest_##name (); \
    ONE_IMPL_AUTO_TEST_CASE(name, ipp); \
    ONE_IMPL_AUTO_TEST_CASE(name, fftw); \
    ONE_IMPL_AUTO_TEST_CASE(name, kissfft); \
    ONE_IMPL_AUTO_TEST_CASE(name, vdsp); \
    ONE_IMPL_AUTO_TEST_CASE(name, medialib); \
    ONE_IMPL_AUTO_TEST_CASE(name, openmax); \
    ONE_IMPL_AUTO_TEST_CASE(name, sfft); \
    ONE_IMPL_AUTO_TEST_CASE(name, cross); \
    ONE_IMPL_AUTO_TEST_CASE(name, dft); \
    void performTest_##name ()

std::string all_implementations[] = {
    "ipp", "vdsp", "fftw", "sfft", "openmax",
    "medialib", "kissfft", "cross", "dft"
};

BOOST_AUTO_TEST_CASE(showImplementations)
{
    std::set<std::string> impls = FFT::getImplementations();
    std::cerr << "\nThe following implementations are compiled in and will be tested:" << std::endl;
    for (int i = 0; i < int(sizeof(all_implementations)/sizeof(all_implementations[0])); ++i) {
        if (impls.find(all_implementations[i]) != impls.end()) {
            std::cerr << " +" << all_implementations[i];
        }
    }
    std::cerr << std::endl << std::endl;
    std::cerr << "The following implementations are NOT compiled in and will not be tested:" << std::endl;
    for (int i = 0; i < int(sizeof(all_implementations)/sizeof(all_implementations[0])); ++i) {
        if (impls.find(all_implementations[i]) == impls.end()) {
            std::cerr << " -" << all_implementations[i];
        }
    }
    std::cerr << std::endl << std::endl;
}


/*
 * 1a. Simple synthetic signals, transforms to separate real/imag arrays,
 *     double-precision
 */

ALL_IMPL_AUTO_TEST_CASE(dc)
{
    // DC-only signal. The DC bin is purely real
    double in[] = { 1, 1, 1, 1 };
    double re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE(re[0], 4.0);
    COMPARE_ZERO(re[1]);
    COMPARE_ZERO(re[2]);
    COMPARE_ALL(im, 0.0);
    double back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(sine)
{
    // Sine. Output is purely imaginary
    double in[] = { 0, 1, 0, -1 };
    double re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_ALL(re, 0.0);
    COMPARE_ZERO(im[0]);
    COMPARE(im[1], -2.0);
    COMPARE_ZERO(im[2]);
    double back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(cosine)
{
    // Cosine. Output is purely real
    double in[] = { 1, 0, -1, 0 };
    double re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_ZERO(re[0]);
    COMPARE(re[1], 2.0);
    COMPARE_ZERO(re[2]);
    COMPARE_ALL(im, 0.0);
    double back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}
	
ALL_IMPL_AUTO_TEST_CASE(sineCosine)
{
    // Sine and cosine mixed
    double in[] = { 0.5, 1, -0.5, -1 };
    double re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_ZERO(re[0]);
    COMPARE(re[1], 1.0);
    COMPARE_ZERO(re[2]);
    COMPARE_ZERO(im[0]);
    COMPARE(im[1], -2.0);
    COMPARE_ZERO(im[2]);
    double back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(nyquist)
{
    double in[] = { 1, -1, 1, -1 };
    double re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_ZERO(re[0]);
    COMPARE_ZERO(re[1]);
    COMPARE(re[2], 4.0);
    COMPARE_ALL(im, 0.0);
    double back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(dirac)
{
    double in[] = { 1, 0, 0, 0 };
    double re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE(re[0], 1.0);
    COMPARE(re[1], 1.0);
    COMPARE(re[2], 1.0);
    COMPARE_ALL(im, 0.0);
    double back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}


/*
 * 1b. Simple synthetic signals, transforms to separate real/imag arrays,
 *     single-precision (i.e. single-precision version of 1a)
 */

ALL_IMPL_AUTO_TEST_CASE(dcF)
{
    // DC-only signal. The DC bin is purely real
    float in[] = { 1, 1, 1, 1 };
    float re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_F(re[0], 4.0f);
    COMPARE_ZERO_F(re[1]);
    COMPARE_ZERO_F(re[2]);
    COMPARE_ALL_F(im, 0.0f);
    float back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(sineF)
{
    // Sine. Output is purely imaginary
    float in[] = { 0, 1, 0, -1 };
    float re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_ALL_F(re, 0.0f);
    COMPARE_ZERO_F(im[0]);
    COMPARE_F(im[1], -2.0f);
    COMPARE_ZERO_F(im[2]);
    float back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(cosineF)
{
    // Cosine. Output is purely real
    float in[] = { 1, 0, -1, 0 };
    float re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_ZERO_F(re[0]);
    COMPARE_F(re[1], 2.0f);
    COMPARE_ZERO_F(re[2]);
    COMPARE_ALL_F(im, 0.0f);
    float back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}
	
ALL_IMPL_AUTO_TEST_CASE(sineCosineF)
{
    // Sine and cosine mixed
    float in[] = { 0.5, 1, -0.5, -1 };
    float re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_ZERO_F(re[0]);
    COMPARE_F(re[1], 1.0f);
    COMPARE_ZERO_F(re[2]);
    COMPARE_ZERO_F(im[0]);
    COMPARE_F(im[1], -2.0f);
    COMPARE_ZERO_F(im[2]);
    float back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(nyquistF)
{
    float in[] = { 1, -1, 1, -1 };
    float re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_ZERO_F(re[0]);
    COMPARE_ZERO_F(re[1]);
    COMPARE_F(re[2], 4.0f);
    COMPARE_ALL_F(im, 0.0f);
    float back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(diracF)
{
    float in[] = { 1, 0, 0, 0 };
    float re[3], im[3];
    USING_FFT(4);
    fft.forward(in, re, im);
    COMPARE_F(re[0], 1.0f);
    COMPARE_F(re[1], 1.0f);
    COMPARE_F(re[2], 1.0f);
    COMPARE_ALL_F(im, 0.0f);
    float back[4];
    fft.inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}


/*
 * 2a. Subset of synthetic signals, testing different output formats
 *     (interleaved complex, polar, magnitude-only, and our weird
 *     cepstral thing), double-precision
 */ 

ALL_IMPL_AUTO_TEST_CASE(interleaved)
{
    // Sine and cosine mixed, test output format
    double in[] = { 0.5, 1, -0.5, -1 };
    double out[6];
    USING_FFT(4);
    fft.forwardInterleaved(in, out);
    COMPARE_ZERO(out[0]);
    COMPARE_ZERO(out[1]);
    COMPARE(out[2], 1.0);
    COMPARE(out[3], -2.0);
    COMPARE_ZERO(out[4]);
    COMPARE_ZERO(out[5]);
    double back[4];
    fft.inverseInterleaved(out, back);
    COMPARE_SCALED(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(sinePolar)
{
    double in[] = { 0, 1, 0, -1 };
    double mag[3], phase[3];
    USING_FFT(4);
    fft.forwardPolar(in, mag, phase);
    COMPARE_ZERO(mag[0]);
    COMPARE(mag[1], 2.0);
    COMPARE_ZERO(mag[2]);
    // No meaningful tests for phase[i] where mag[i]==0 (phase
    // could legitimately be anything)
    COMPARE(phase[1], -M_PI/2.0);
    double back[4];
    fft.inversePolar(mag, phase, back);
    COMPARE_SCALED(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(cosinePolar)
{
    double in[] = { 1, 0, -1, 0 };
    double mag[3], phase[3];
    USING_FFT(4);
    fft.forwardPolar(in, mag, phase);
    COMPARE_ZERO(mag[0]);
    COMPARE(mag[1], 2.0);
    COMPARE_ZERO(mag[2]);
    // No meaningful tests for phase[i] where mag[i]==0 (phase
    // could legitimately be anything)
    COMPARE_ZERO(phase[1]);
    double back[4];
    fft.inversePolar(mag, phase, back);
    COMPARE_SCALED(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(magnitude)
{
    // Sine and cosine mixed
    double in[] = { 0.5, 1, -0.5, -1 };
    double out[3];
    USING_FFT(4);
    fft.forwardMagnitude(in, out);
    COMPARE_ZERO(out[0]);
    COMPARE_F(float(out[1]), sqrtf(5.0));
    COMPARE_ZERO(out[2]);
}

ALL_IMPL_AUTO_TEST_CASE(cepstrum)
{
    double in[] = { 1, 0, 0, 0, 1, 0, 0, 0 };
    double mag[5];
    USING_FFT(8);
    fft.forwardMagnitude(in, mag);
    double cep[8];
    fft.inverseCepstral(mag, cep);
    BOOST_CHECK_SMALL(cep[1], 1e-9);
    BOOST_CHECK_SMALL(cep[2], 1e-9);
    BOOST_CHECK_SMALL(cep[3], 1e-9);
    BOOST_CHECK_SMALL(cep[5], 1e-9);
    BOOST_CHECK_SMALL(cep[6], 1e-9);
    BOOST_CHECK_SMALL(cep[7], 1e-9);
    BOOST_CHECK_SMALL(-6.561181 - cep[0]/8, 0.000001);
    BOOST_CHECK_SMALL( 7.254329 - cep[4]/8, 0.000001);
}


/*
 * 2b. Subset of synthetic signals, testing different output formats
 *     (interleaved complex, polar, magnitude-only, and our weird
 *     cepstral thing), single-precision (i.e. single-precision
 *     version of 2a)
 */ 
	
ALL_IMPL_AUTO_TEST_CASE(interleavedF)
{
    // Sine and cosine mixed, test output format
    float in[] = { 0.5, 1, -0.5, -1 };
    float out[6];
    USING_FFT(4);
    fft.forwardInterleaved(in, out);
    COMPARE_ZERO_F(out[0]);
    COMPARE_ZERO_F(out[1]);
    COMPARE_F(out[2], 1.0f);
    COMPARE_F(out[3], -2.0f);
    COMPARE_ZERO_F(out[4]);
    COMPARE_ZERO_F(out[5]);
    float back[4];
    fft.inverseInterleaved(out, back);
    COMPARE_SCALED_F(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(cosinePolarF)
{
    float in[] = { 1, 0, -1, 0 };
    float mag[3], phase[3];
    USING_FFT(4);
    fft.forwardPolar(in, mag, phase);
    COMPARE_ZERO_F(mag[0]);
    COMPARE_F(mag[1], 2.0f);
    COMPARE_ZERO_F(mag[2]);
    // No meaningful tests for phase[i] where mag[i]==0 (phase
    // could legitimately be anything)
    COMPARE_ZERO_F(phase[1]);
    float back[4];
    fft.inversePolar(mag, phase, back);
    COMPARE_SCALED_F(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(sinePolarF)
{
    float in[] = { 0, 1, 0, -1 };
    float mag[3], phase[3];
    USING_FFT(4);
    fft.forwardPolar(in, mag, phase);
    COMPARE_ZERO_F(mag[0]);
    COMPARE_F(mag[1], 2.0f);
    COMPARE_ZERO_F(mag[2]);
    // No meaningful tests for phase[i] where mag[i]==0 (phase
    // could legitimately be anything)
    COMPARE_F(phase[1], -float(M_PI)/2.0f);
    float back[4];
    fft.inversePolar(mag, phase, back);
    COMPARE_SCALED_F(back, in, 4);
}

ALL_IMPL_AUTO_TEST_CASE(magnitudeF)
{
    // Sine and cosine mixed
    float in[] = { 0.5, 1, -0.5, -1 };
    float out[3];
    USING_FFT(4);
    fft.forwardMagnitude(in, out);
    COMPARE_ZERO_F(out[0]);
    COMPARE_F(float(out[1]), sqrtf(5.0f));
    COMPARE_ZERO_F(out[2]);
}

ALL_IMPL_AUTO_TEST_CASE(cepstrumF)
{
    float in[] = { 1, 0, 0, 0, 1, 0, 0, 0 };
    float mag[5];
    USING_FFT(8);
    fft.forwardMagnitude(in, mag);
    float cep[8];
    fft.inverseCepstral(mag, cep);
    COMPARE_ZERO_F(cep[1]);
    COMPARE_ZERO_F(cep[2]);
    COMPARE_ZERO_F(cep[3]);
    COMPARE_ZERO_F(cep[5]);
    COMPARE_ZERO_F(cep[6]);
    COMPARE_ZERO_F(cep[7]);
    BOOST_CHECK_SMALL(-6.561181 - cep[0]/8, 0.000001);
    BOOST_CHECK_SMALL( 7.254329 - cep[4]/8, 0.000001);
}


/*
 * 4. Bounds checking, double-precision and single-precision
 */

ALL_IMPL_AUTO_TEST_CASE(forwardArrayBounds)
{
    double in[] = { 1, 1, -1, -1 };

    // Initialise output bins to something recognisable, so we can
    // tell if they haven't been written
    double re[] = { 999, 999, 999, 999, 999 };
    double im[] = { 999, 999, 999, 999, 999 };
    
    USING_FFT(4);
    fft.forward(in, re+1, im+1);
    
    // Check we haven't overrun the output arrays
    COMPARE(re[0], 999.0);
    COMPARE(im[0], 999.0);
    COMPARE(re[4], 999.0);
    COMPARE(im[4], 999.0);
}

ALL_IMPL_AUTO_TEST_CASE(inverseArrayBounds)
{
    // The inverse transform is only supposed to refer to the first
    // N/2+1 bins and synthesise the rest rather than read them - so
    // initialise the next one to some value that would mess up the
    // results if it were used
    double re[] = { 0, 1, 0, 456 };
    double im[] = { 0, -2, 0, 456 };

    // Initialise output bins to something recognisable, so we can
    // tell if they haven't been written
    double out[] = { 999, 999, 999, 999, 999, 999 };
    
    USING_FFT(4);
    fft.inverse(re, im, out+1);

    // Check we haven't overrun the output arrays
    COMPARE(out[0], 999.0);
    COMPARE(out[5], 999.0);

    // And check the results are as we expect, i.e. that we haven't
    // used the bogus final bin
    COMPARE(out[1] / 4, 0.5);
    COMPARE(out[2] / 4, 1.0);
    COMPARE(out[3] / 4, -0.5);
    COMPARE(out[4] / 4, -1.0);
}

ALL_IMPL_AUTO_TEST_CASE(forwardArrayBoundsF)
{
    float in[] = { 1, 1, -1, -1 };
    
    // Initialise output bins to something recognisable, so we can
    // tell if they haven't been written
    float re[] = { 999, 999, 999, 999, 999 };
    float im[] = { 999, 999, 999, 999, 999 };
    
    USING_FFT(4);
    fft.forward(in, re+1, im+1);
    
    // Check we haven't overrun the output arrays
    COMPARE_F(re[0], 999.0f);
    COMPARE_F(im[0], 999.0f);
    COMPARE_F(re[4], 999.0f);
    COMPARE_F(im[4], 999.0f);
}

ALL_IMPL_AUTO_TEST_CASE(inverseArrayBoundsF)
{
    // The inverse transform is only supposed to refer to the first
    // N/2+1 bins and synthesise the rest rather than read them - so
    // initialise the next one to some value that would mess up the
    // results if it were used
    float re[] = { 0, 1, 0, 456 };
    float im[] = { 0, -2, 0, 456 };

    // Initialise output bins to something recognisable, so we can
    // tell if they haven't been written
    float out[] = { 999, 999, 999, 999, 999, 999 };
    
    USING_FFT(4);
    fft.inverse(re, im, out+1);
    
    // Check we haven't overrun the output arrays
    COMPARE_F(out[0], 999.0f);
    COMPARE_F(out[5], 999.0f);

    // And check the results are as we expect, i.e. that we haven't
    // used the bogus final bin
    COMPARE_F(out[1] / 4.0f, 0.5f);
    COMPARE_F(out[2] / 4.0f, 1.0f);
    COMPARE_F(out[3] / 4.0f, -0.5f);
    COMPARE_F(out[4] / 4.0f, -1.0f);
}


/*
 * 5. Less common transform lengths - we should always fall back on
 *    some implementation that can handle these, even if the requested
 *    one doesn't. Note that the dirac tests we do first are "easier"
 *    in that they don't vary with length
 */

ALL_IMPL_AUTO_TEST_CASE(dirac_1)
{
    double in[] = { 1 };
    double re[1], im[1];
    USING_FFT(1);
    fft.forward(in, re, im);
    COMPARE(re[0], 1.0);
    COMPARE_ALL(im, 0.0);
    double back[1];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 1);
}

ALL_IMPL_AUTO_TEST_CASE(dirac_6)
{
    double in[] = { 1, 0, 0, 0, 0, 0 };
    double re[4], im[4];
    USING_FFT(6);
    fft.forward(in, re, im);
    COMPARE(re[0], 1.0);
    COMPARE(re[1], 1.0);
    COMPARE(re[2], 1.0);
    COMPARE(re[3], 1.0);
    COMPARE_ALL(im, 0.0);
    double back[6];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 6);
}

ALL_IMPL_AUTO_TEST_CASE(dirac_7)
{
    double in[] = { 1, 0, 0, 0, 0, 0, 0 };
    double re[4], im[4];
    USING_FFT(7);
    fft.forward(in, re, im);
    COMPARE(re[0], 1.0);
    COMPARE(re[1], 1.0);
    COMPARE(re[2], 1.0);
    COMPARE(re[3], 1.0);
    COMPARE_ALL(im, 0.0);
    double back[7];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 7);
}

ALL_IMPL_AUTO_TEST_CASE(sineCosine_6)
{
    // Sine and cosine mixed, i.e. f(x) = 0.5 * cos(2*x*pi/6) + sin(2*x*pi/6)
    double r = sqrt(3.0)/2.0;
    double in[] = { 0.5, r + 0.25, r - 0.25, -0.5, -r - 0.25, -r + 0.25 };
    double re[4], im[4];
    USING_FFT(6);
    fft.forward(in, re, im);
    COMPARE(re[0], 0.0);
    COMPARE(re[1], 1.5);
    COMPARE(re[2], 0.0);
    COMPARE(re[3], 0.0);
    COMPARE(im[0], 0.0);
    COMPARE(im[1], -3.0);
    COMPARE(im[2], 0.0);
    COMPARE(im[3], 0.0);
    double back[6];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 6);
}

ALL_IMPL_AUTO_TEST_CASE(sineCosine_7)
{
    // Sine and cosine mixed, i.e. f(x) = 0.5 * cos(2*x*pi/6) + sin(2*x*pi/6)
    double in[] = {
        0.5,
        1.0935763833973966,
        0.8636674452036665,
        -0.016600694833651286,
        -0.8843681730687676,
        -1.086188379159981,
        -0.47008658153866323
    };
    double re[4], im[4];
    USING_FFT(7);
    fft.forward(in, re, im);
    COMPARE(re[0], 0.0);
    COMPARE(re[1], 1.75);
    COMPARE(re[2], 0.0);
    COMPARE(re[3], 0.0);
    COMPARE(im[0], 0.0);
    COMPARE(im[1], -3.5);
    COMPARE(im[2], 0.0);
    COMPARE(im[3], 0.0);
    double back[7];
    fft.inverse(re, im, back);
    COMPARE_SCALED(back, in, 7);
}


BOOST_AUTO_TEST_SUITE_END()
