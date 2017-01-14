
bqfft
=====

A small library wrapping various FFT implementations for some common
audio processing use cases. Note this is not a general FFT interface,
as it handles only power-of-two FFT sizes and real inputs.

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

Copyright 2007-2017 Particular Programs Ltd. See the file COPYING for
(BSD/MIT-style) licence terms.
