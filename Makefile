ROOT_LIBS:=$(shell root-config --libs)
ROOT_GLIBS:=$(shell root-config --glibs)
ROOT_INCLUDE:=$(shell root-config --incdir)
CXXFLAGS:=-O3 --ansi -Wall -g  $(ROOT_GLIBS) $(SYSLIBS) -I$(ROOT_INCLUDE) -I/opt/d-cache/dcap/include/ -fomit-frame-pointer -I.
LDFLAGS:=-L. $(ROOT_GLIBS) $(SYSLIBS) -L/opt/d-cache/dcap/lib -ldcap -I$(ROOT_INCLUDE) -I/opt/d-cache/dcap/include/

all: music
        
clean: 
	rm -f music
	rm -f *.o */*.o
#-------------------------------------------------------------------------------

music:		PXL.o PXLdCache.o music.o MISalgo/AnyOption.o TConfig/TConfigDict.o TConfig/TConfig.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o ElectronID/GammaLikelihood.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o EventClassFactory/CcEventClass.o  ParticleMatcher/ParticleMatcher.o ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o  ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/VertexDiffPlots.o ControlPlotFactory/TriggerDiffPlots.o ControlPlotFactory/HistoPolisher.o DuplicateObjects/DuplicateObjects.o
		$(CXX) -o music PXL.o PXLdCache.o music.o MISalgo/AnyOption.o TConfig/TConfigDict.o TConfig/TConfig.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o ElectronID/GammaLikelihood.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o EventClassFactory/CcEventClass.o  ParticleMatcher/ParticleMatcher.o ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/VertexDiffPlots.o ControlPlotFactory/TriggerDiffPlots.o ControlPlotFactory/HistoPolisher.o DuplicateObjects/DuplicateObjects.o $(LDFLAGS) -lz

#		rm -f *.o */*.o
#-------------------------------------------------------------------------------
%.o: %.cc
	g++ -g -s -c $(LDFLAGS) -o $@ $<
