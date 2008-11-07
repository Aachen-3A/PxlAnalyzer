ROOT_LIBS:=$(shell root-config --libs)
ROOT_GLIBS:=$(shell root-config --glibs)
ROOT_INCLUDE:=$(shell root-config --incdir)
LHAPDFDIR:=/home/home1/institut_3a/chof/Generators/LHAPDFLIB/lhapdf-5.3.1/
CXXFLAGS:=-O2 --ansi -Wall -fpic -g -c -I$(ROOT_INCLUDE) -I/opt/d-cache/dcap/include/ -I.
LDFLAGS:= -L. $(ROOT_GLIBS) $(SYSLIBS) -L/opt/d-cache/dcap/lib -ldcap -lLHAPDF -L$(LHAPDFDIR)../lhapdf/lib/ -lz

all: music EventClassFactory/ECMerger EventClassFactory/ECFileUtil EventClassFactory/TEventClass.so MISalgo/ROI_analysis MISalgo/TECResult.so EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger

clean: 
	rm -f music EventClassFactory/ECMerger EventClassFactory/ECFileUtil EventClassFactory/TEventClass.so MISalgo/ROI_analysis MISalgo/TECResult.so EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger
	rm -f *.o */*.o */*/*.o
	rm -f */*Dict*
	rm -f */*/*Dict*


#-------------------------------------------------------------------------------

TConfig/TConfigDict.cpp:	TConfig/TConfig.h TConfig/TConfigLinkDef.h
			@echo "Generating TConfig dictionary ..."
			rootcint -v -f $@ -c $^
			@sed -e "s@#include \"TConfig/TConfig.h\"@#include \"TConfig.h\"@" TConfig/TConfigDict.h > TConfig/TConfig.tmp
			@mv TConfig/TConfig.tmp TConfig/TConfigDict.h

EventClassFactory/TEventClassDict.cc:	EventClassFactory/TEventClass.hh
			@echo "Generating TEventClass dictionary ..."
			rootcint -v -f $@ -c $^
			@sed -e "s@#include \"EventClassFactory/TEventClass.hh\"@#include \"TEventClass.hh\"@" EventClassFactory/TEventClassDict.h > EventClassFactory/TEventClassDict.tmp
			@mv EventClassFactory/TEventClassDict.tmp EventClassFactory/TEventClassDict.h

MISalgo/TECResultDict.cc:	MISalgo/TECResult.hh MISalgo/TECResultLinkDef.h
			@echo "Generating TECResult dictionary ..."
			rootcint -v -f $@ -c $^
			@sed -e "s@#include \"MISalgo/TECResult.hh\"@#include \"TECResult.hh\"@" MISalgo/TECResultDict.h > MISalgo/TECResultDict.tmp
			@mv MISalgo/TECResultDict.tmp MISalgo/TECResultDict.h			

music:		PXL.o music.o PXLdCache.o $(LHAPDFDIR)/ccwrap/LHAPDFWrap.o $(LHAPDFDIR)/ccwrap/LHAPDFfw.o MISalgo/AnyOption.o TConfig/TConfigDict.o TConfig/TConfig.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o ElectronID/GammaLikelihood.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o EventClassFactory/CcEventClass.o  ParticleMatcher/ParticleMatcher.o ParticleMatcher/EventSelector.o ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o  ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/VertexDiffPlots.o ControlPlotFactory/TriggerDiffPlots.o ControlPlotFactory/HistoPolisher.o DuplicateObjects/DuplicateObjects.o
		$(CXX) -o music $^ $(LDFLAGS)

music_pdfonly:		PXL.o PXLdCache.o music_pdfonly.o $(LHAPDFDIR)/ccwrap/LHAPDFWrap.o $(LHAPDFDIR)/ccwrap/LHAPDFfw.o MISalgo/AnyOption.o TConfig/TConfigDict.o TConfig/TConfig.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o ElectronID/GammaLikelihood.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o EventClassFactory/CcEventClass.o  ParticleMatcher/ParticleMatcher.o ParticleMatcher/EventSelector.o ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o  ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/VertexDiffPlots.o ControlPlotFactory/TriggerDiffPlots.o ControlPlotFactory/HistoPolisher.o DuplicateObjects/DuplicateObjects.o
			$(CXX) -o music_pdfonly $^ $(LDFLAGS)

EventClassFactory/ECMerger:	PXL.o EventClassFactory/ECMerger.o EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o TConfig/TConfigDict.o TConfig/TConfig.o MISalgo/AnyOption.o
				$(CXX) -o EventClassFactory/ECMerger $^ $(LDFLAGS)
EventClassFactory/ECFileUtil:	PXL.o EventClassFactory/ECFileUtil.o EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o TConfig/TConfigDict.o TConfig/TConfig.o MISalgo/AnyOption.o
				$(CXX) -o EventClassFactory/ECFileUtil $^ $(LDFLAGS)
EventClassFactory/TEventClass.so: EventClassFactory/TEventClass.o EventClassFactory/ECFileUtil.o PXL.o EventClassFactory/TEventClassDict.o TConfig/TConfigDict.o TConfig/TConfig.o MISalgo/AnyOption.o
				$(CXX) -shared $(LDFLAGS) -O $^ -o $@

MISalgo/ROI_analysis:	MISalgo/ROI_analysis.o PXL.o EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o MISalgo/TECResult.o MISalgo/TECResultDict.o TConfig/TConfigDict.o TConfig/TConfig.o MISalgo/RegionScanner.o MISalgo/PoissonCalculator.o MISalgo/ErrorComputer.o MISalgo/ConvolutionComputer.o MISalgo/ECDicer.o MISalgo/ErrorService.o MISalgo/ECUpDownError.o ControlPlotFactory/HistoPolisher.o MISalgo/ECResultTable.o MISalgo/AnyOption.o
			$(CXX) $(LDFLAGS) -o MISalgo/ROI_analysis $^ $(LDFLAGS)

MISalgo/GlobalStuff:	MISalgo/GlobalStuff.o MISalgo/GlobalPicture.o PXL.o EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o MISalgo/TECResult.o MISalgo/TECResultDict.o TConfig/TConfigDict.o TConfig/TConfig.o MISalgo/RegionScanner.o MISalgo/PoissonCalculator.o MISalgo/ErrorComputer.o MISalgo/ConvolutionComputer.o MISalgo/ECDicer.o MISalgo/ErrorService.o MISalgo/ECUpDownError.o ControlPlotFactory/HistoPolisher.o MISalgo/ECResultTable.o MISalgo/AnyOption.o
			$(CXX) -o MISalgo/GlobalStuff $^ $(LDFLAGS)

MISalgo/TECResult.so:	PXL.o EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o MISalgo/TECResult.o MISalgo/TECResultDict.o TConfig/TConfigDict.o TConfig/TConfig.o MISalgo/RegionScanner.o MISalgo/PoissonCalculator.o MISalgo/ErrorComputer.o MISalgo/ConvolutionComputer.o MISalgo/ECDicer.o MISalgo/ErrorService.o MISalgo/ECUpDownError.o ControlPlotFactory/HistoPolisher.o MISalgo/ECResultTable.o MISalgo/AnyOption.o 
			$(CXX) -shared -O $^ -o $@

EventClassFactory/ECCrossSectionRescaler:	PXL.o EventClassFactory/ECCrossSectionRescaler.o EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o TConfig/TConfigDict.o TConfig/TConfig.o MISalgo/AnyOption.o 
						$(CXX) -o EventClassFactory/ECCrossSectionRescaler $^ $(LDFLAGS)

MISalgo/TECResultMerger:	PXL.o MISalgo/TECResultMerger.o EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o MISalgo/TECResultDict.o MISalgo/TECResult.o TConfig/TConfigDict.o TConfig/TConfig.o ControlPlotFactory/HistoPolisher.o MISalgo/AnyOption.o
				$(CXX) -o MISalgo/TECResultMerger $^ $(LDFLAGS)

#		rm -f *.o */*.o
#-------------------------------------------------------------------------------
%.o: %.cc
	g++ $(CXXFLAGS) -o $@ $<
