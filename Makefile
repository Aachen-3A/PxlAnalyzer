ROOT_LIBS:=$(shell root-config --libs)
ROOT_GLIBS:=$(shell root-config --glibs)
ROOT_INCLUDE:=$(shell root-config --incdir)
CXXFLAGS:=-O3 --ansi -Wall -g $(ROOT_GLIBS) $(SYSLIBS) -I$(ROOT_INCLUDE) -fomit-frame-pointer -I.
LDFLAGS:=-L. $(ROOT_GLIBS) $(SYSLIBS) -I$(ROOT_INCLUDE)

all: chfactory
	
clean: 
	rm -f chfactory
	rm -f *.o */*.o
#-------------------------------------------------------------------------------
chfactory:	ePAX.o chfactory.o ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonDiffPlots.o
	 	$(CXX) -o chfactory ePAX.o chfactory.o ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonDiffPlots.o $(LDFLAGS) -lz
		rm -f *.o */*.o
#-------------------------------------------------------------------------------
%.o: %.cc
	g++ -g -s -c $(LDFLAGS) -o $@ $<
