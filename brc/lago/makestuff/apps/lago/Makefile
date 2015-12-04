#
## nothing should be modified here
#
PLATFORM := $(shell uname | tr [A-Z] [a-z])
MACHINE := $(shell uname -m)
MAKESTUFFFLAGS=-I $(LAGO_DAQ)/makestuff/libs/libfpgalink/ -I $(LAGO_DAQ)/makestuff/common/ -L $(LAGO_DAQ)/makestuff/libs/libfpgalink/$(PLATFORM).$(MACHINE)/rel/ -lbuffer -lerror -lfpgalink -lfx2loader -Wl,-rpath $(LAGO_DAQ)/makestuff/libs/libfpgalink/$(PLATFORM).$(MACHINE)/rel/
LIBFPGA=$(LAGO_DAQ)/makestuff/libs/libfpgalink/$(PLATFORM).$(MACHINE)/rel/libfpgalink.so
CC=g++
FLAGS=-Wall -O3 -std=c++11

####
PROG=lago
EXTRA=extras.cpp gps.cpp pressure.cpp

all: $(PROG)

$(PROG): $(PROG).cc $(EXTRA) $(LIBFPGA)
	  $(CC) $(FLAGS) -o $(PROG) $(PROG).cc $(EXTRA) $(MAKESTUFFFLAGS)

clean:
	   rm -f $(PROG)
