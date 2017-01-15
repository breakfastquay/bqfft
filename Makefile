
# Add to FFT_DEFINES the relevant options for your desired third-party
# library support.
#
# Available options are
#
#  -DHAVE_IPP         Intel's Integrated Performance Primitives are available
#  -DHAVE_VDSP        Apple's Accelerate framework is available
#  -DHAVE_FFTW3       The FFTW library is available
#  -DHAVE_KISSFFT     The KissFFT library is available
#  -DHAVE_MEDIALIB    The Medialib library (from Sun) is available
#  -DHAVE_OPENMAX     The OpenMAX signal processing library is available
#  -DUSE_BUILTIN_FFT  Compile the built-in FFT code (which is very slow)
#
# You may define more than one of these. If you define
# USE_BUILTIN_FFT, the code will be compiled in but will only be used
# if no other option is available. The default, if no flags are
# supplied, is for the code to refuse to compile.
#
# Note that you must supply the same flags when including bqfft
# headers later as you are using now when compiling the library. (You
# may find it simplest to just add the bqfft source files to your
# application's build system and not build a bqfft library at all.)

# WARNING! The default option here is VERY SLOW! Read above for better
# alternatives!
FFT_DEFINES		:= -DUSE_BUILTIN_FFT


# Add to VECTOR_DEFINES and ALLOCATOR_DEFINES any options desired for
# the bqvec library (that are not already defined in FFT_DEFINES).
# See the bqvec build documentation for more details.
#
VECTOR_DEFINES := 
ALLOCATOR_DEFINES := 


# Add any related includes and libraries here
#
THIRD_PARTY_INCLUDES	:=
THIRD_PARTY_LIBS	:=


include build/Makefile.inc
