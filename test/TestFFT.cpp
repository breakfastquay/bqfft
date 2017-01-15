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

#ifdef TEST_FFT_IS_SINGLE_PRECISION_ONLY
static const double eps = 1e-7;
static const float epsf = 1e-6f;
#else
static const double eps = 1e-14;
static const float epsf = 1e-7f;
#endif

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

BOOST_AUTO_TEST_CASE(dc)
{
    // DC-only signal. The DC bin is purely real
    double in[] = { 1, 1, 1, 1 };
    double re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE(re[0], 4.0);
    COMPARE_ZERO(re[1]);
    COMPARE_ZERO(re[2]);
    COMPARE_ALL(im, 0.0);
    double back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}

BOOST_AUTO_TEST_CASE(showImplementation)
{
    FFT f(4); // to ensure an implementation has been picked
    (void)f;  // avoid compiler warning
    std::cerr << "Using implementation: " << FFT::getDefaultImplementation()
              << std::endl;
}

BOOST_AUTO_TEST_CASE(sine)
{
    // Sine. Output is purely imaginary
    double in[] = { 0, 1, 0, -1 };
    double re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_ALL(re, 0.0);
    COMPARE_ZERO(im[0]);
    COMPARE(im[1], -2.0);
    COMPARE_ZERO(im[2]);
    double back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}

BOOST_AUTO_TEST_CASE(cosine)
{
    // Cosine. Output is purely real
    double in[] = { 1, 0, -1, 0 };
    double re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_ZERO(re[0]);
    COMPARE(re[1], 2.0);
    COMPARE_ZERO(re[2]);
    COMPARE_ALL(im, 0.0);
    double back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}
	
BOOST_AUTO_TEST_CASE(sineCosine)
{
    // Sine and cosine mixed
    double in[] = { 0.5, 1, -0.5, -1 };
    double re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_ZERO(re[0]);
    COMPARE(re[1], 1.0);
    COMPARE_ZERO(re[2]);
    COMPARE_ZERO(im[0]);
    COMPARE(im[1], -2.0);
    COMPARE_ZERO(im[2]);
    double back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}

BOOST_AUTO_TEST_CASE(nyquist)
{
    double in[] = { 1, -1, 1, -1 };
    double re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_ZERO(re[0]);
    COMPARE_ZERO(re[1]);
    COMPARE(re[2], 4.0);
    COMPARE_ALL(im, 0.0);
    double back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}
	
BOOST_AUTO_TEST_CASE(interleaved)
{
    // Sine and cosine mixed, test output format
    double in[] = { 0.5, 1, -0.5, -1 };
    double out[6];
    FFT(4).forwardInterleaved(in, out);
    COMPARE_ZERO(out[0]);
    COMPARE_ZERO(out[1]);
    COMPARE(out[2], 1.0);
    COMPARE(out[3], -2.0);
    COMPARE_ZERO(out[4]);
    COMPARE_ZERO(out[5]);
    double back[4];
    FFT(4).inverseInterleaved(out, back);
    COMPARE_SCALED(back, in, 4);
}

BOOST_AUTO_TEST_CASE(cosinePolar)
{
    double in[] = { 1, 0, -1, 0 };
    double mag[3], phase[3];
    FFT(4).forwardPolar(in, mag, phase);
    COMPARE_ZERO(mag[0]);
    COMPARE(mag[1], 2.0);
    COMPARE_ZERO(mag[2]);
    // No meaningful tests for phase[i] where mag[i]==0 (phase
    // could legitimately be anything)
    COMPARE_ZERO(phase[1]);
    double back[4];
    FFT(4).inversePolar(mag, phase, back);
    COMPARE_SCALED(back, in, 4);
}

BOOST_AUTO_TEST_CASE(sinePolar)
{
    double in[] = { 0, 1, 0, -1 };
    double mag[3], phase[3];
    FFT(4).forwardPolar(in, mag, phase);
    COMPARE_ZERO(mag[0]);
    COMPARE(mag[1], 2.0);
    COMPARE_ZERO(mag[2]);
    // No meaningful tests for phase[i] where mag[i]==0 (phase
    // could legitimately be anything)
    COMPARE(phase[1], -M_PI/2.0);
    double back[4];
    FFT(4).inversePolar(mag, phase, back);
    COMPARE_SCALED(back, in, 4);
}

BOOST_AUTO_TEST_CASE(magnitude)
{
    // Sine and cosine mixed
    double in[] = { 0.5, 1, -0.5, -1 };
    double out[3];
    FFT(4).forwardMagnitude(in, out);
    COMPARE_ZERO(out[0]);
    COMPARE_F(float(out[1]), sqrtf(5.0));
    COMPARE_ZERO(out[2]);
}

BOOST_AUTO_TEST_CASE(dirac)
{
    double in[] = { 1, 0, 0, 0 };
    double re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE(re[0], 1.0);
    COMPARE(re[1], 1.0);
    COMPARE(re[2], 1.0);
    COMPARE_ALL(im, 0.0);
    double back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED(back, in, 4);
}

BOOST_AUTO_TEST_CASE(cepstrum)
{
    double in[] = { 1, 0, 0, 0, 1, 0, 0, 0 };
    double mag[5];
    FFT(8).forwardMagnitude(in, mag);
    double cep[8];
    FFT(8).inverseCepstral(mag, cep);
    COMPARE_ZERO(cep[1]);
    COMPARE_ZERO(cep[2]);
    COMPARE_ZERO(cep[3]);
    COMPARE_ZERO(cep[5]);
    COMPARE_ZERO(cep[6]);
    COMPARE_ZERO(cep[7]);
    BOOST_CHECK_SMALL(-6.561181 - cep[0]/8, 0.000001);
    BOOST_CHECK_SMALL( 7.254329 - cep[4]/8, 0.000001);
}

BOOST_AUTO_TEST_CASE(forwardArrayBounds)
{
    // initialise bins to something recognisable, so we can tell
    // if they haven't been written
    double in[] = { 1, 1, -1, -1 };
    double re[] = { 999, 999, 999, 999, 999 };
    double im[] = { 999, 999, 999, 999, 999 };
    FFT(4).forward(in, re+1, im+1);
    // And check we haven't overrun the arrays
    COMPARE(re[0], 999.0);
    COMPARE(im[0], 999.0);
    COMPARE(re[4], 999.0);
    COMPARE(im[4], 999.0);
}

BOOST_AUTO_TEST_CASE(inverseArrayBounds)
{
    // initialise bins to something recognisable, so we can tell
    // if they haven't been written
    double re[] = { 0, 1, 0 };
    double im[] = { 0, -2, 0 };
    double out[] = { 999, 999, 999, 999, 999, 999 };
    FFT(4).inverse(re, im, out+1);
    // And check we haven't overrun the arrays
    COMPARE(out[0], 999.0);
    COMPARE(out[5], 999.0);
}

BOOST_AUTO_TEST_CASE(dcF)
{
    // DC-only signal. The DC bin is purely real
    float in[] = { 1, 1, 1, 1 };
    float re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_F(re[0], 4.0f);
    COMPARE_ZERO_F(re[1]);
    COMPARE_ZERO_F(re[2]);
    COMPARE_ALL_F(im, 0.0f);
    float back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}

BOOST_AUTO_TEST_CASE(sineF)
{
    // Sine. Output is purely imaginary
    float in[] = { 0, 1, 0, -1 };
    float re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_ALL_F(re, 0.0f);
    COMPARE_ZERO_F(im[0]);
    COMPARE_F(im[1], -2.0f);
    COMPARE_ZERO_F(im[2]);
    float back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}

BOOST_AUTO_TEST_CASE(cosineF)
{
    // Cosine. Output is purely real
    float in[] = { 1, 0, -1, 0 };
    float re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_ZERO_F(re[0]);
    COMPARE_F(re[1], 2.0f);
    COMPARE_ZERO_F(re[2]);
    COMPARE_ALL_F(im, 0.0f);
    float back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}
	
BOOST_AUTO_TEST_CASE(sineCosineF)
{
    // Sine and cosine mixed
    float in[] = { 0.5, 1, -0.5, -1 };
    float re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_ZERO_F(re[0]);
    COMPARE_F(re[1], 1.0f);
    COMPARE_ZERO_F(re[2]);
    COMPARE_ZERO_F(im[0]);
    COMPARE_F(im[1], -2.0f);
    COMPARE_ZERO_F(im[2]);
    float back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}

BOOST_AUTO_TEST_CASE(nyquistF)
{
    float in[] = { 1, -1, 1, -1 };
    float re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_ZERO_F(re[0]);
    COMPARE_ZERO_F(re[1]);
    COMPARE_F(re[2], 4.0f);
    COMPARE_ALL_F(im, 0.0f);
    float back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}
	
BOOST_AUTO_TEST_CASE(interleavedF)
{
    // Sine and cosine mixed, test output format
    float in[] = { 0.5, 1, -0.5, -1 };
    float out[6];
    FFT(4).forwardInterleaved(in, out);
    COMPARE_ZERO_F(out[0]);
    COMPARE_ZERO_F(out[1]);
    COMPARE_F(out[2], 1.0f);
    COMPARE_F(out[3], -2.0f);
    COMPARE_ZERO_F(out[4]);
    COMPARE_ZERO_F(out[5]);
    float back[4];
    FFT(4).inverseInterleaved(out, back);
    COMPARE_SCALED_F(back, in, 4);
}

BOOST_AUTO_TEST_CASE(cosinePolarF)
{
    float in[] = { 1, 0, -1, 0 };
    float mag[3], phase[3];
    FFT(4).forwardPolar(in, mag, phase);
    COMPARE_ZERO_F(mag[0]);
    COMPARE_F(mag[1], 2.0f);
    COMPARE_ZERO_F(mag[2]);
    // No meaningful tests for phase[i] where mag[i]==0 (phase
    // could legitimately be anything)
    COMPARE_ZERO_F(phase[1]);
    float back[4];
    FFT(4).inversePolar(mag, phase, back);
    COMPARE_SCALED_F(back, in, 4);
}

BOOST_AUTO_TEST_CASE(sinePolarF)
{
    float in[] = { 0, 1, 0, -1 };
    float mag[3], phase[3];
    FFT(4).forwardPolar(in, mag, phase);
    COMPARE_ZERO_F(mag[0]);
    COMPARE_F(mag[1], 2.0f);
    COMPARE_ZERO_F(mag[2]);
    // No meaningful tests for phase[i] where mag[i]==0 (phase
    // could legitimately be anything)
    COMPARE_F(phase[1], -float(M_PI)/2.0f);
    float back[4];
    FFT(4).inversePolar(mag, phase, back);
    COMPARE_SCALED_F(back, in, 4);
}

BOOST_AUTO_TEST_CASE(magnitudeF)
{
    // Sine and cosine mixed
    float in[] = { 0.5, 1, -0.5, -1 };
    float out[3];
    FFT(4).forwardMagnitude(in, out);
    COMPARE_ZERO_F(out[0]);
    COMPARE_F(float(out[1]), sqrtf(5.0f));
    COMPARE_ZERO_F(out[2]);
}

BOOST_AUTO_TEST_CASE(diracF)
{
    float in[] = { 1, 0, 0, 0 };
    float re[3], im[3];
    FFT(4).forward(in, re, im);
    COMPARE_F(re[0], 1.0f);
    COMPARE_F(re[1], 1.0f);
    COMPARE_F(re[2], 1.0f);
    COMPARE_ALL_F(im, 0.0f);
    float back[4];
    FFT(4).inverse(re, im, back);
    COMPARE_SCALED_F(back, in, 4);
}

BOOST_AUTO_TEST_CASE(cepstrumF)
{
    float in[] = { 1, 0, 0, 0, 1, 0, 0, 0 };
    float mag[5];
    FFT(8).forwardMagnitude(in, mag);
    float cep[8];
    FFT(8).inverseCepstral(mag, cep);
    COMPARE_ZERO_F(cep[1]);
    COMPARE_ZERO_F(cep[2]);
    COMPARE_ZERO_F(cep[3]);
    COMPARE_ZERO_F(cep[5]);
    COMPARE_ZERO_F(cep[6]);
    COMPARE_ZERO_F(cep[7]);
    BOOST_CHECK_SMALL(-6.561181 - cep[0]/8, 0.000001);
    BOOST_CHECK_SMALL( 7.254329 - cep[4]/8, 0.000001);
}

BOOST_AUTO_TEST_CASE(forwardArrayBoundsF)
{
    // initialise bins to something recognisable, so we can tell
    // if they haven't been written
    float in[] = { 1, 1, -1, -1 };
    float re[] = { 999, 999, 999, 999, 999 };
    float im[] = { 999, 999, 999, 999, 999 };
    FFT(4).forward(in, re+1, im+1);
    // And check we haven't overrun the arrays
    COMPARE_F(re[0], 999.0f);
    COMPARE_F(im[0], 999.0f);
    COMPARE_F(re[4], 999.0f);
    COMPARE_F(im[4], 999.0f);
}

BOOST_AUTO_TEST_CASE(inverseArrayBoundsF)
{
    // initialise bins to something recognisable, so we can tell
    // if they haven't been written
    float re[] = { 0, 1, 0 };
    float im[] = { 0, -2, 0 };
    float out[] = { 999, 999, 999, 999, 999, 999 };
    FFT(4).inverse(re, im, out+1);
    // And check we haven't overrun the arrays
    COMPARE_F(out[0], 999.0f);
    COMPARE_F(out[5], 999.0f);
}

BOOST_AUTO_TEST_SUITE_END()
