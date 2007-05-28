ROOT_LIBS:=$(shell root-config --libs)
ROOT_GLIBS:=$(shell root-config --glibs)
ROOT_INCLUDE:=$(shell root-config --incdir)
CXXFLAGS:=-O3 --ansi -Wall -g $(ROOT_GLIBS) $(SYSLIBS) -I$(ROOT_INCLUDE) -I/opt/d-cache/dcap/include/ -fomit-frame-pointer -I.
LDFLAGS:=-L. $(ROOT_GLIBS) $(SYSLIBS) -L/opt/d-cache/dcap/lib -ldcap -I$(ROOT_INCLUDE) -I/opt/d-cache/dcap/include/

all: chfactory
        
clean: 
	rm -f chfactory
	rm -f *.o */*.o
#-------------------------------------------------------------------------------

chfactory:	PXL.o PXLdCache.o chfactory.o TConfig/TConfigDict.o TConfig/TConfig.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o EventClassFactory/CcEventClass.o  ParticleMatcher/ParticleMatcher.o ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o  ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/HistoPolisher.o
		$(CXX) -o chfactory PXL.o PXLdCache.o chfactory.o TConfig/TConfigDict.o TConfig/TConfig.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o EventClassFactory/CcEventClass.o  ParticleMatcher/ParticleMatcher.o ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/HistoPolisher.o $(LDFLAGS) -lz

#		rm -f *.o */*.o
#-------------------------------------------------------------------------------
%.o: %.cc
	g++ -g -s -c $(LDFLAGS) -o $@ $<
