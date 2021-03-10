
bqfft
=====

A small library wrapping various FFT implementations for some common
audio processing use cases. Contains a built-in implementation and
wrappers for FFTW3, KissFFT, Intel IPP, and Apple vDSP. Suitable for
Windows, Mac, Linux, and mobile platforms.

Note this is not a general FFT interface, as it handles only real
signals on the time-domain side.

Transforms of any length are supported, but if you request a length
that bqfft does not know how to calculate using any of the libraries
that have been compiled in, a simple slow DFT will be used instead. A
warning will be printed to stderr if this happens.

Of the available libraries, vDSP, IPP, and the built-in implementation
support power-of-two FFT lengths only, KissFFT supports any multiple
of two, and FFTW supports any length. You can compile in more than one
library, so for example if you compile in Accelerate and KissFFT, the
former will be used for powers of two and the latter for other even
lengths.

Here are some other pros and cons of the supported libraries:

 * Intel IPP - By far the fastest on actual Intel hardware. Of
   uncertain benefit with other manufacturers. Not available beyond
   x86/amd64, not open source.

 * Apple vDSP - Faster than the open source libraries on all Apple
   hardware, and provided with the OS. There is seldom any good reason
   not to use this on Apple platforms.
 
 * FFTW3 - Fastest open source library and portable, but its bulk and
   GPL licence may be an issue.
 
 * KissFFT - As used here it is single-precision throughout, so it may
   be a good choice for platforms on which double-precision arithmetic
   is slow. Disadvantage is that it does not preserve the full float
   range of precision (i.e. forward-inverse transform pairs on float
   data do not produce identical results to the input). Not especially
   fast on desktop or modern mobile hardware.

 * Built-in implementation - Double precision, so more precise than
   KissFFT, and faster on typical 64-bit desktop and modern mobile
   hardware. Slower than IPP, vDSP, and FFTW3.

Requires the bqvec library.

This code originated as part of the Rubber Band Library written by the
same authors (see https://hg.sr.ht/~breakfastquay/rubberband/).
It has been pulled out into a separate library and relicensed under a
more permissive licence.

C++ standard required: C++98 (does not use C++11 or newer features)

 * To compile on Linux: Edit Makefile to select implementation, then make.
   Do read the notes in the Makefile, and don't attempt to use the default
   implementation, which is very slow
   
 * To compile on macOS: make -f build/Makefile.osx

 * To build and run tests: as above, but add the "test" target -
   requires Boost test headers installed

 * Depends on: [bqvec](https://hg.sr.ht/~breakfastquay/bqvec)

 * See also: [bqresample](https://hg.sr.ht/~breakfastquay/bqresample) [bqaudioio](https://hg.sr.ht/~breakfastquay/bqaudioio) [bqthingfactory](https://hg.sr.ht/~breakfastquay/bqthingfactory) [bqaudiostream](https://hg.sr.ht/~breakfastquay/bqaudiostream)

[![Build status](https://builds.sr.ht/~breakfastquay/bqfft.svg)](https://builds.sr.ht/~breakfastquay/bqfft?)

Copyright 2007-2021 Particular Programs Ltd. See the file COPYING for
(BSD/MIT-style) licence terms.
