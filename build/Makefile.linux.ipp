
FFT_DEFINES		:= -DHAVE_IPP

VECTOR_DEFINES		:= 

ALLOCATOR_DEFINES 	:= -DHAVE_POSIX_MEMALIGN

THIRD_PARTY_INCLUDES	:= -I/opt/intel/ipp/include
THIRD_PARTY_LIBS	:= -L/opt/intel/ipp/lib/intel64_lin -Wl,-Bstatic -lipps -lippvm -lippcore -Wl,-Bdynamic

include build/Makefile.inc
