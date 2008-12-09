#-----Flags--------------------------------------------------------------------

ROOT_CFLAGS:=$(shell root-config --cflags)
ROOT_LDFLAGS:=$(shell root-config --ldflags)
ROOT_GLIBS:=$(shell root-config --glibs)
LHAPDFDIR:=/home/home1/institut_3a/chof/Generators/LHAPDFLIB/lhapdf-5.3.1/
CXXFLAGS:=-O2 --ansi -Wall -fpic -c $(ROOT_CFLAGS) -I/opt/d-cache/dcap/include/ -I.
LDFLAGS:= $(ROOT_LDFLAGS) $(ROOT_GLIBS) $(SYSLIBS) -L. -L/opt/d-cache/dcap/lib -ldcap -lLHAPDF -L$(LHAPDFDIR)../lhapdf/lib/ -lz



#-----Default and cleaning rules-----------------------------------------------

all: lib music EventClassFactory/ECMerger EventClassFactory/ECFileUtil MISalgo/ROI_analysis EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger

clean: 
	rm -f music EventClassFactory/ECMerger EventClassFactory/ECFileUtil EventClassFactory/TEventClass.so MISalgo/ROI_analysis MISalgo/TECResult.so EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger
	rm -f *.o */*.o */*/*.o
	rm -f */*Dict*
	rm -f */*/*Dict*
	rm -rf lib




#-----Rules for executables----------------------------------------------------

music:		music.o PXL.o PXLdCache.o $(LHAPDFDIR)/ccwrap/LHAPDFWrap.o $(LHAPDFDIR)/ccwrap/LHAPDFfw.o MISalgo/AnyOption.o EventClassFactory/CcEventClass.o DuplicateObjects/DuplicateObjects.o lib/EventClass.a lib/ParticleMatcher.a lib/TConfig.a lib/ControlPlotFactory.a
		$(CXX) -o music $(LDFLAGS) $^

EventClassFactory/ECMerger:	EventClassFactory/ECMerger.o PXL.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
				$(CXX) -o EventClassFactory/ECMerger $(LDFLAGS) $^

EventClassFactory/ECFileUtil:	EventClassFactory/ECFileUtil.o PXL.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
				$(CXX) -o EventClassFactory/ECFileUtil $(LDFLAGS) $^

MISalgo/ROI_analysis:	MISalgo/ROI_analysis.o PXL.o ControlPlotFactory/HistoPolisher.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a lib/MISalgo.a
			$(CXX) -o MISalgo/ROI_analysis $(LDFLAGS) $^

MISalgo/GlobalStuff:	MISalgo/GlobalStuff.o MISalgo/GlobalPicture.o PXL.o ControlPlotFactory/HistoPolisher.o MISalgo/AnyOption.o lib/MISalgo.a lib/EventClass.a lib/TConfig.a
			$(CXX) -o MISalgo/GlobalStuff $(LDFLAGS) $^

EventClassFactory/ECCrossSectionRescaler:	EventClassFactory/ECCrossSectionRescaler.o PXL.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
						$(CXX) -o EventClassFactory/ECCrossSectionRescaler $(LDFLAGS) $^

MISalgo/TECResultMerger:	MISalgo/TECResultMerger.o PXL.o MISalgo/TECResultDict.o MISalgo/TECResult.o ControlPlotFactory/HistoPolisher.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
				$(CXX) -o MISalgo/TECResultMerger $(LDFLAGS) $^



#-----Automatic rules----------------------------------------------------------
%.o: %.cc
	g++ $(CXXFLAGS) -o $@ $<



#-----Library rules------------------------------------------------------------

lib:
	mkdir lib

lib/ControlPlotFactory.a: 	ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o  ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/VertexDiffPlots.o ControlPlotFactory/TriggerDiffPlots.o ControlPlotFactory/HistoPolisher.o
				ar rcs $@ $^

lib/ParticleMatcher.a:	ParticleMatcher/ParticleMatcher.o ParticleMatcher/EventSelector.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o ElectronID/GammaLikelihood.o
			ar rcs $@ $^

lib/TConfig.a:	TConfig/TConfigDict.o TConfig/TConfig.o
		ar rcs $@ $^

lib/EventClass.a:	EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o
			ar rcs $@ $^

lib/MISalgo.a:	MISalgo/TECResult.o MISalgo/TECResultDict.o MISalgo/RegionScanner.o MISalgo/PoissonCalculator.o MISalgo/ErrorComputer.o MISalgo/ConvolutionComputer.o MISalgo/ECDicer.o MISalgo/ErrorService.o MISalgo/ECUpDownError.o MISalgo/ECResultTable.o
		ar rcs $@ $^




#-----Misc rules---------------------------------------------------------------

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
