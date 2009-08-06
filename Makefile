#-----Flags--------------------------------------------------------------------

ifdef DEBUG
	DEBUG_FLAG:=-g
else
	DEBUG_FLAG:=-O2
endif

ifdef CMSSW_RELEASE_BASE
DCAP_BASE:=$(shell cat $(CMSSW_RELEASE_BASE)/config/toolbox/$(SCRAM_ARCH)/tools/selected/dcap.xml | grep 'name="DCAP_BASE"' | sed -e 's/.*default="//' | sed -e 's/"\/>//')
else
$(error Error: We need dcap.h, so ready a CMSSW release!)
endif

ROOT_CFLAGS:=$(shell root-config --cflags)
ROOT_LDFLAGS:=$(shell root-config --ldflags)
ROOT_GLIBS:=$(shell root-config --libs)
CXXFLAGS:=$(DEBUG_FLAG) --ansi -Wall -fpic -c $(ROOT_CFLAGS) -I$(DCAP_BASE)/include/ -I.
LDFLAGS:= $(ROOT_LDFLAGS) $(ROOT_GLIBS) $(SYSLIBS) -L. -L$(DCAP_BASE)/lib -ldcap -lz

#Where to store generated libraries
LIBDIR=lib

#Where to store the generate dependency files
DEPDIR=dep


#-----Default and cleaning rules-----------------------------------------------

.PHONY: all clean

all: music EventClassFactory/ECMerger EventClassFactory/ECFileUtil EventClassFactory/FakeClass MISalgo/ROI_analysis EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger MISv2/dicePseudoData EventClassFactory/TEventClass.so MISalgo/TECResult.so

clean: 
	rm -f music EventClassFactory/ECMerger EventClassFactory/ECFileUtil EventClassFactory/FakeClass EventClassFactory/TEventClass.so MISalgo/ROI_analysis MISalgo/TECResult.so EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger MISv2/dicePseudoData
	rm -f *.o */*.o */*/*.o
	rm -f */*Dict* */*/*Dict*
	rm -rf $(LIBDIR) $(DEPDIR)


#-----Rules for executables----------------------------------------------------

music:		music.o Tools/PXL/PXL.o Tools/AnyOption.o Tools/RunLumiRanges.o EventClassFactory/CcEventClass.o DuplicateObjects/DuplicateObjects.o Tools/Tools.o Tools/dCache/dCacheBuf.o $(LIBDIR)/EventClass.a $(LIBDIR)/ParticleMatcher.a $(LIBDIR)/TConfig.a $(LIBDIR)/ControlPlotFactory.a $(LIBDIR)/ControlPlots2.a | EventClassFactory/ECMerger
		$(CXX) -o $@ $(LDFLAGS) $^

EventClassFactory/ECMerger:	EventClassFactory/ECMerger.o Tools/PXL/PXL.o Tools/AnyOption.o $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a
				$(CXX) -o $@ $(LDFLAGS) $^

EventClassFactory/ECFileUtil:	EventClassFactory/ECFileUtil.o Tools/PXL/PXL.o Tools/AnyOption.o $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a
				$(CXX) -o $@ $(LDFLAGS) $^

EventClassFactory/FakeClass:	EventClassFactory/FakeClass.o Tools/PXL/PXL.o Tools/AnyOption.o Tools/Tools.o $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a
				$(CXX) -o $@ $(LDFLAGS) $^

MISalgo/ROI_analysis:	MISalgo/ROI_analysis.o Tools/PXL/PXL.o ControlPlotFactory/HistoPolisher.o Tools/AnyOption.o $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a $(LIBDIR)/MISalgo.a
			$(CXX) -o $@ $(LDFLAGS) $^

MISalgo/GlobalStuff:	MISalgo/GlobalStuff.o MISalgo/GlobalPicture.o Tools/PXL/PXL.o ControlPlotFactory/HistoPolisher.o Tools/AnyOption.o $(LIBDIR)/MISalgo.a $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a
			$(CXX) -o $@ $(LDFLAGS) $^

EventClassFactory/ECCrossSectionRescaler:	EventClassFactory/ECCrossSectionRescaler.o Tools/PXL/PXL.o Tools/AnyOption.o $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a
						$(CXX) -o $@ $(LDFLAGS) $^

MISalgo/TECResultMerger:	MISalgo/TECResultMerger.o Tools/PXL/PXL.o MISalgo/TECResultDict.o MISalgo/TECResult.o ControlPlotFactory/HistoPolisher.o Tools/AnyOption.o Tools/Tools.o $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a
				$(CXX) -o $@ $(LDFLAGS) $^



MISv2/dicePseudoData: 	MISv2/dicePseudoData.o Tools/Tools.o $(LIBDIR)/MISv2.a $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a
			$(CXX) -o MISv2/dicePseudoData $(LDFLAGS) $^


#-----Rules for shared libraries for interactive root--------------------------

EventClassFactory/TEventClass.so: 	$(LIBDIR)/EventClass.a EventClassFactory/ECFileUtil.o Tools/PXL/PXL.o Tools/AnyOption.o $(LIBDIR)/TConfig.a EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o TConfig/TConfigDict.o TConfig/TConfig.o Tools/Tools.o
					$(CXX) -o $@ -shared $(LDFLAGS) -O $^

MISalgo/TECResult.so:	$(LIBDIR)/MISalgo.a Tools/PXL/PXL.o ControlPlotFactory/HistoPolisher.o Tools/AnyOption.o $(LIBDIR)/EventClass.a $(LIBDIR)/TConfig.a MISalgo/TECResult.o MISalgo/TECResultDict.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o Tools/Tools.o TConfig/TConfigDict.o TConfig/TConfig.o
			$(CXX) -o $@ -shared $(LDFLAGS) -O $^



#-----Library rules------------------------------------------------------------

$(LIBDIR):
	mkdir $(LIBDIR)

$(LIBDIR)/ControlPlotFactory.a: 	ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o  ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/VertexDiffPlots.o ControlPlotFactory/TriggerDiffPlots.o ControlPlotFactory/EventPlots.o ControlPlotFactory/HistoPolisher.o | $(LIBDIR)
				ar rcs $@ $^

$(LIBDIR)/ControlPlots2.a: 	ControlPlotFactory/HistoPolisher.o ControlPlots2/PlotBase.o ControlPlots2/MultiParticlePlots.o ControlPlots2/ParticlePlots.o ControlPlots2/RecControl.o ControlPlots2/RecGammaPlots.o ControlPlots2/RecJetPlots.o | $(LIBDIR)
			ar rcs $@ $^

$(LIBDIR)/ParticleMatcher.a:	ParticleMatcher/ParticleMatcher.o ParticleMatcher/EventSelector.o ElectronID/LikelihoodEstimator.o ElectronID/ElectronLikelihood.o ElectronID/GammaLikelihood.o Tools/Tools.o | $(LIBDIR)
			ar rcs $@ $^

$(LIBDIR)/TConfig.a:	TConfig/TConfigDict.o TConfig/TConfig.o | $(LIBDIR)
		ar rcs $@ $^

$(LIBDIR)/EventClass.a:	EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o Tools/PXL/PXL.o | $(LIBDIR)
			ar rcs $@ $^

$(LIBDIR)/MISalgo.a:	MISalgo/TECResult.o MISalgo/TECResultDict.o MISalgo/RegionScanner.o MISalgo/PoissonCalculator.o MISalgo/ErrorComputer.o MISalgo/ConvolutionComputer.o MISalgo/ECDicer.o MISalgo/ErrorService.o MISalgo/ECDicer_add.o MISalgo/ErrorService_add.o MISalgo/ECDicer_multiply.o MISalgo/ErrorService_multiply.o MISalgo/ECUpDownError.o MISalgo/ECResultTable.o Tools/Tools.o | $(LIBDIR)
		ar rcs $@ $^


$(LIBDIR)/MISv2.a: MISv2/ErrorService.o MISv2/ErrorService_add.o MISv2/ErrorService_multiply.o | $(LIBDIR)
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



#-----Automatic rules----------------------------------------------------------
#For details on what's happening below, please see: http://make.paulandlesley.org/autodep.html
#Advantage: Should automatically keep track of all and every dependency.
#Disadvantage: Makes make slower, because the automatically generated dependency files tend to be huge.

#All .P dependency files are collected in one directory, to avoid cluttering up the source directories.
#Drawback: We can't have two source files with the same name, even if they are in different directories.
$(DEPDIR):
	mkdir $(DEPDIR)


df = $(DEPDIR)/$(*F)

%.o: %.cc | $(DEPDIR)
	g++ -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $(df).P;
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(df).P;
	@rm -f $*.d

#Notable difference to the system described above: All .P files in the DEPDIR will be included.
#Even if there is no .cc file for it anymore.
#Advantage: No need to keep track of all .cc files
#Disadvantage: Removing a .cc will leave a stale .P file, that might or might not cause problems
#If that happens (probably causing a make target error):
# - Delete the offending .P file in dep/
# - OR do a make clean
# - Do NOT delete the whole dep/ directory without doing make clean
-include $(DEPDIR)/*.P
