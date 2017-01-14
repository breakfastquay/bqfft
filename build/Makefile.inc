
SRC_DIR	:= src
HEADER_DIR := bqfft

SOURCES	:= $(wildcard $(SRC_DIR)/*.cpp)
HEADERS	:= $(wildcard $(HEADER_DIR)/*.h) $(wildcard $(SRC_DIR)/*.h)

OBJECTS	:= $(SOURCES:.cpp=.o)
OBJECTS	:= $(OBJECTS:.c=.o)

OPTFLAGS ?= -O3 -ffast-math

CXXFLAGS ?= -std=c++98 -Wall -Wextra -Werror $(FFT_DEFINES) $(VECTOR_DEFINES) $(ALLOCATOR_DEFINES) $(OPTFLAGS) -I. $(THIRD_PARTY_INCLUDES) -I../bqvec -fpic

LIBRARY	:= libbqfft.a

all:	$(LIBRARY)

test:	$(LIBRARY) test-fft
	./test-fft

valgrind:	$(LIBRARY) test-fft
	valgrind ./test-fft

$(LIBRARY):	$(OBJECTS)
	$(AR) rc $@ $^

test-fft:	test/TestFFT.o $(LIBRARY)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_unit_test_framework -L. -lbqfft -L../bqvec -lbqvec $(THIRD_PARTY_LIBS)

clean:		
	rm -f $(OBJECTS) test/TestFFT.o

distclean:	clean
	rm -f $(LIBRARY) test-fft

depend:
	makedepend -Y -fbuild/Makefile.inc $(SOURCES) $(HEADERS)


# DO NOT DELETE

src/FFT.o: bqfft/FFT.h
