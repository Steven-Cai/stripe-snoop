CXX=c++
CXXFLAGS = -O -Wall
CC=cc

OBJECTS=main.o capture.o parser.o database.o
APPLICATIONS=ss bitgen mod10 rdetect

all: ss bitgen mod10 rdetect

ss: $(OBJECTS)
	@echo Linking ss
	$(CXX) main.o capture.o parser.o database.o -o ss -O -Wall


%.o: %.cpp %.h
	@echo Compling $*
	@rm -f $*.o
	$(CXX) $(CXXFLAGS) -c $*.cpp

bitgen: bitgen.c
	@echo Building bitgen
	$(CC) $(CXXFLAGS) -o bitgen bitgen.c

mod10: mod10.c
	@echo Building mod10
	$(CC) $(CXXFLAGS) -o mod10 mod10.c

rdetect: rdetect.o capture.o parser.o 
	@echo Linking rdetect
	$(CXX) $(CXXFLAGS) rdetect.o capture.o parser.o -o rdetect

rdetect.o: rdetect.cpp
	@rm -f $*.o
	$(CXX) $(CXXFLAGS) -c rdetect.cpp

clean:
	@rm -f $(OBJECTS) $(APPLICATIONS)
	@echo All Clean!

