CC = g++
CFLAGS  = -c -O3 -Wall -std=c++11
LDFLAGS =

CFLAGS  += `root-config --cflags --libs` -lRooFitCore -lRooFit
LDFLAGS += `root-config --cflags --libs` -lRooFitCore -lRooFit

SOURCES = $(wildcard sources/*.cc)

OBJECTS  = $(SOURCES:.cc=.o)
EXECUTABLE=a.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf sources/*.o $(EXECUTABLE)
#all:
#	g++ `root-config --cflags --glibs` -lRGL -lGeom main.cc
