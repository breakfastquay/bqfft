
bqfft
=====

A small library wrapping various FFT implementations for some common
audio processing use cases. Note this is not a general FFT interface,
as it handles only real inputs.

Transforms of any length are supported, but if you request a length
not supported by any of the library implementations provided here, or
by any of the libraries compiled in, a simple slow DFT will be used
instead. Of the most commonly used libraries, Accelerate and IPP
support power-of-two lengths only, KissFFT supports any multiple of
two (because we only use the real-input interface), and FFTW supports
any length. You can compile in more than one library, so as to use
e.g. VDSP for powers of two and KissFFT for other even lengths.

Requires the bqvec library.

This code originated as part of the Rubber Band Library written by the
same authors (see https://bitbucket.org/breakfastquay/rubberband/).
It has been pulled out into a separate library and relicensed under a
more permissive licence.

C++ standard required: C++98 (does not use C++11 or newer features)

 * To compile on Linux: Edit Makefile to select implementation, then make test.
   Do read the notes in the Makefile, and don't attempt to use the default
   implementation, which is very slow
   
 * To compile on macOS: make -f build/Makefile.osx test

[![Build Status](https://travis-ci.org/breakfastquay/bqfft.svg?branch=master)](https://travis-ci.org/breakfastquay/bqfft)

Copyright 2007-2019 Particular Programs Ltd. See the file COPYING for
(BSD/MIT-style) licence terms.
