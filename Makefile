
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
# if no other option is available. If no flags are supplied, the code
# will refuse to compile.

# WARNING! The default option here is VERY SLOW! Read above for better
# alternatives!
FFT_DEFINES		:= -DUSE_BUILTIN_FFT


# Add to VECTOR_DEFINES and ALLOCATOR_DEFINES any options desired for
# the bqvec library (that are not already defined in FFT_DEFINES).
# See the bqvec build documentation for more details.
#
VECTOR_DEFINES 		:= 
ALLOCATOR_DEFINES 	:= 


# Add any related includes and libraries here
#
THIRD_PARTY_INCLUDES	:=
THIRD_PARTY_LIBS	:=


# If you are including a set of bq libraries into a project, you can
# override variables for all of them (including all of the above) in
# the following file, which all bq* Makefiles will include if found

-include ../Makefile.inc-bq


# This project-local Makefile describes the source files and contains
# no routinely user-modifiable parts

include build/Makefile.inc
