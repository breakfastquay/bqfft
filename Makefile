
# Add to FFT_DEFINES the relevant options for your desired third-party
# library support.
#
# Available library options are
#
#  -DHAVE_IPP         Intel's Integrated Performance Primitives are available
#  -DHAVE_VDSP        Apple's Accelerate framework is available
#  -DHAVE_FFTW3       The FFTW library is available
#  -DHAVE_KISSFFT     The KissFFT library is available
#  -DUSE_BUILTIN_FFT  Compile the built-in FFT code (which is not bad)
#
# You may define more than one of these. If you do so, the decision
# about which implementation to use when an FFT object is constructed
# will depend on the FFT length (some libraries only support certain
# lengths) and on some hardcoded expectations about performance. If no
# flags are supplied, the code will refuse to compile.
#
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
