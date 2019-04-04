
SRC_DIR	:= src
TEST_DIR := test
HEADER_DIR := bqfft

SOURCES	:= $(wildcard $(SRC_DIR)/*.cpp)
HEADERS	:= $(wildcard $(HEADER_DIR)/*.h) $(wildcard $(SRC_DIR)/*.h)

OBJECTS	:= $(SOURCES:.cpp=.o)
OBJECTS	:= $(OBJECTS:.c=.o)

TEST_SOURCES	:= $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS	:= $(TEST_SOURCES:.cpp=.o)

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

timings:       test/timings.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:		
	rm -f $(OBJECTS) $(TEST_OBJECTS)

distclean:	clean
	rm -f $(LIBRARY) test-fft

depend:
	makedepend -Y -fbuild/Makefile.inc $(SOURCES) $(HEADERS) $(TEST_SOURCES)


# DO NOT DELETE

src/FFT.o: bqfft/FFT.h
test/TestFFT.o: bqfft/FFT.h
test/timings.o: src/FFT.cpp bqfft/FFT.h
