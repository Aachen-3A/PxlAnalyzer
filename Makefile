#-----Flags--------------------------------------------------------------------

ifdef DEBUG
	DEBUG_FLAG:=-g
else
	DEBUG_FLAG:=-O2
endif

ROOT_CFLAGS:=$(shell root-config --cflags)
ROOT_LDFLAGS:=$(shell root-config --ldflags)
ROOT_GLIBS:=$(shell root-config --libs)
CXXFLAGS:=$(DEBUG_FLAG) --ansi -Wall -fpic -c $(ROOT_CFLAGS) -I/opt/d-cache/dcap/include/ -I.
LDFLAGS:= $(ROOT_LDFLAGS) $(ROOT_GLIBS) $(SYSLIBS) -L. -L/opt/d-cache/dcap/lib -ldcap -lz



#-----Default and cleaning rules-----------------------------------------------

.PHONY: all clean

all: music EventClassFactory/ECMerger EventClassFactory/ECFileUtil EventClassFactory/FakeClass MISalgo/ROI_analysis EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger EventClassFactory/TEventClass.so MISalgo/TECResult.so

clean: 
	rm -f music EventClassFactory/ECMerger EventClassFactory/ECFileUtil EventClassFactory/FakeClass EventClassFactory/TEventClass.so MISalgo/ROI_analysis MISalgo/TECResult.so EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger
	rm -f *.o */*.o */*/*.o
	rm -f */*Dict*
	rm -f */*/*Dict*
	rm -rf lib




#-----Rules for executables----------------------------------------------------

music:		music.o PXL.o PXLdCache.o MISalgo/AnyOption.o EventClassFactory/CcEventClass.o DuplicateObjects/DuplicateObjects.o lib/EventClass.a lib/ParticleMatcher.a lib/TConfig.a lib/ControlPlotFactory.a
		$(CXX) -o music $(LDFLAGS) $^

EventClassFactory/ECMerger:	EventClassFactory/ECMerger.o PXL.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
				$(CXX) -o EventClassFactory/ECMerger $(LDFLAGS) $^

EventClassFactory/ECFileUtil:	EventClassFactory/ECFileUtil.o PXL.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
				$(CXX) -o EventClassFactory/ECFileUtil $(LDFLAGS) $^

EventClassFactory/FakeClass:	EventClassFactory/FakeClass.o PXL.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
				$(CXX) -o EventClassFactory/FakeClass $(LDFLAGS) $^

MISalgo/ROI_analysis:	MISalgo/ROI_analysis.o PXL.o ControlPlotFactory/HistoPolisher.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a lib/MISalgo.a
			$(CXX) -o MISalgo/ROI_analysis $(LDFLAGS) $^

MISalgo/GlobalStuff:	MISalgo/GlobalStuff.o MISalgo/GlobalPicture.o PXL.o ControlPlotFactory/HistoPolisher.o MISalgo/AnyOption.o lib/MISalgo.a lib/EventClass.a lib/TConfig.a
			$(CXX) -o MISalgo/GlobalStuff $(LDFLAGS) $^

EventClassFactory/ECCrossSectionRescaler:	EventClassFactory/ECCrossSectionRescaler.o PXL.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
						$(CXX) -o EventClassFactory/ECCrossSectionRescaler $(LDFLAGS) $^

MISalgo/TECResultMerger:	MISalgo/TECResultMerger.o PXL.o MISalgo/TECResultDict.o MISalgo/TECResult.o ControlPlotFactory/HistoPolisher.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a
				$(CXX) -o MISalgo/TECResultMerger $(LDFLAGS) $^


#-----Rules for shared libraries for interactive root--------------------------

EventClassFactory/TEventClass.so: 	lib/EventClass.a EventClassFactory/ECFileUtil.o PXL.o MISalgo/AnyOption.o lib/TConfig.a EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o
					$(CXX) -o $@ -shared $(LDFLAGS) -O $^

MISalgo/TECResult.so:	lib/MISalgo.a PXL.o ControlPlotFactory/HistoPolisher.o MISalgo/AnyOption.o lib/EventClass.a lib/TConfig.a MISalgo/TECResult.o MISalgo/TECResultDict.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o
			$(CXX) -o $@ -shared $(LDFLAGS) -O $^



#-----Automatic rules----------------------------------------------------------
%.o: %.cc %.hh
	g++ $(CXXFLAGS) -o $@ $<



#-----Library rules------------------------------------------------------------

lib:
	mkdir lib

lib/ControlPlotFactory.a: 	ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o  ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/VertexDiffPlots.o ControlPlotFactory/TriggerDiffPlots.o ControlPlotFactory/HistoPolisher.o | lib
				ar rcs $@ $^

lib/ParticleMatcher.a:	ParticleMatcher/ParticleMatcher.o ParticleMatcher/EventSelector.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o ElectronID/GammaLikelihood.o | lib
			ar rcs $@ $^

lib/TConfig.a:	TConfig/TConfigDict.o TConfig/TConfig.o | lib
		ar rcs $@ $^

lib/EventClass.a:	EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o | lib
			ar rcs $@ $^

lib/MISalgo.a:	MISalgo/TECResult.o MISalgo/TECResultDict.o MISalgo/RegionScanner.o MISalgo/PoissonCalculator.o MISalgo/ErrorComputer.o MISalgo/ConvolutionComputer.o MISalgo/ECDicer.o MISalgo/ErrorService.o MISalgo/ECUpDownError.o MISalgo/ECResultTable.o | lib
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
