#-----Flags--------------------------------------------------------------------

ifdef DEBUG
	DEBUG_FLAG:=-O0 -g3 -ggdb -fstack-protector-all
else
	DEBUG_FLAG:=-O2
endif

ifdef CMSSW_RELEASE_BASE
DCAP_BASE:=$(shell cat $(CMSSW_RELEASE_BASE)/config/toolbox/$(SCRAM_ARCH)/tools/selected/dcap.xml | grep 'name="DCAP_BASE"' | sed -e 's/.*default="//' | sed -e 's/"\/>//')
BOOST_BASE:=$(shell cat $(CMSSW_RELEASE_BASE)/config/toolbox/$(SCRAM_ARCH)/tools/selected/boost.xml | grep 'name="BOOST_BASE"' | sed -e 's/.*default="//' | sed -e 's/"\/>//')
GSL_BASE:=$(shell cat $(CMSSW_RELEASE_BASE)/config/toolbox/$(SCRAM_ARCH)/tools/selected/gsl.xml | grep 'name="GSL_BASE"' | sed -e 's/.*default="//' | sed -e 's/"\/>//')
else
$(error Error: CMSSW libraries not found!)
endif

# if you're using a patched CMSSW release, some of the libs are still in the base release, so you also have to look there
CMSSW_RELEASE_BASE_NOPATCH:=$(shell echo $(CMSSW_RELEASE_BASE) | sed -e 's/-patch//' -e 's/_patch.//')

# for the headers there are symlinks
CMSSW_INC:=-I$(CMSSW_BASE)/src -I$(CMSSW_RELEASE_BASE)/src
CMSSW_LIBS:=-L$(CMSSW_BASE)/lib/$(SCRAM_ARCH) -L$(CMSSW_RELEASE_BASE)/lib/$(SCRAM_ARCH) -L$(CMSSW_RELEASE_BASE_NOPATCH)/lib/$(SCRAM_ARCH)

ROOT_CFLAGS:=$(shell root-config --cflags)
ROOT_LDFLAGS:=$(shell root-config --ldflags)
ROOT_GLIBS:=$(shell root-config --libs)
EXTRA_CFLAGS:=-ffloat-store -I$(DCAP_BASE)/include/ -I$(GSL_BASE)/include/ -I$(BOOST_BASE)/include $(CMSSW_INC)
EXTRA_LDFLAGS:=-L$(DCAP_BASE)/lib -ldcap -L$(GSL_BASE)/lib -lgsl -lgslcblas -lz -L$(BOOST_BASE)/lib -lboost_filesystem $(CMSSW_LIBS) -lCondFormatsJetMETObjects -lPhysicsToolsUtilities
CXXFLAGS:=$(DEBUG_FLAG) --ansi -Wall -fpic -c $(ROOT_CFLAGS) $(EXTRA_CFLAGS) -I.
LDFLAGS:= $(ROOT_LDFLAGS) $(ROOT_GLIBS) $(SYSLIBS) -L. $(EXTRA_LDFLAGS)

#Where to store generated libraries
LIBDIR=lib

#Where to store the generate dependency files
DEPDIR=dep

#Where to copy binaries
BINDIR=bin

#-----Default and cleaning rules-----------------------------------------------

.PHONY: all clean install-python

all: $(BINDIR)/music $(BINDIR)/ECMerger $(BINDIR)/ECFileUtil $(BINDIR)/FakeClass $(BINDIR)/ROI_analysis $(BINDIR)/ECCrossSectionRescaler $(BINDIR)/GlobalStuff $(BINDIR)/TECResultMerger $(BINDIR)/dicePseudoData $(BINDIR)/printClass $(BINDIR)/printData $(BINDIR)/scanClass EventClassFactory/TEventClass.so MISalgo/TECResult.so install-python

clean: 
	rm -f music EventClassFactory/ECMerger EventClassFactory/ECFileUtil EventClassFactory/FakeClass EventClassFactory/TEventClass.so MISalgo/ROI_analysis MISalgo/TECResult.so EventClassFactory/ECCrossSectionRescaler MISalgo/GlobalStuff MISalgo/TECResultMerger MISv2/dicePseudoData MISv2/printClass MISv2/printData MISv2/scanClass
	rm -f *.o */*.o */*/*.o
	rm -f */*Dict* */*/*Dict*
	rm -rf $(LIBDIR) $(DEPDIR) $(BINDIR)

install-python: | $(BINDIR)
	ln -sf ../MISv2/MISMaster/MISMaster.py $(BINDIR)/MISMaster
	ln -sf ../EventClassFactory/rebinEventClasses $(BINDIR)/rebinEventClasses
	ln -sf ../EventClassFactory/renameProcess $(BINDIR)/renameProcess
	ln -sf ../Tools/listFiles.py $(BINDIR)/listFiles
	ln -sf ../Tools/musicEnv.py $(BINDIR)/musicEnv
	ln -sf ../Tools/radio.py $(BINDIR)/radio
	ln -sf ../Tools/jukebox $(BINDIR)/jukebox
	ln -sf ../Tools/Condor/submit_music.py $(BINDIR)/submit_music
	ln -sf ../Tools/makePlot.py $(BINDIR)/makePlot
	ln -sf ../Tools/ECMerger.py $(BINDIR)/ECMerger2


#-----Rules for executables----------------------------------------------------

#Create the bin directory if needed.
$(BINDIR):
	mkdir $(BINDIR)

$(BINDIR)/music:		music.o Tools/PXL/PXL.o Tools/AnyOption.o Tools/RunLumiRanges.o Tools/ReWeighter.o EventClassFactory/CcEventClass.o DuplicateObjects/DuplicateObjects.o $(LIBDIR)/Tools.a Tools/dCache/dCacheBuf.o Tools/SignalHandler.o $(LIBDIR)/EventClass.a $(LIBDIR)/ParticleMatcher.a  $(LIBDIR)/ControlPlotFactory.a $(LIBDIR)/ControlPlots2.a | $(BINDIR)/ECMerger $(BINDIR)
		$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/ECMerger:	EventClassFactory/ECMerger.o Tools/PXL/PXL.o Tools/AnyOption.o $(LIBDIR)/EventClass.a | $(BINDIR)
				$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/ECFileUtil:	EventClassFactory/ECFileUtil.o Tools/PXL/PXL.o Tools/AnyOption.o $(LIBDIR)/EventClass.a | $(BINDIR)
				$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/FakeClass:	EventClassFactory/FakeClass.o Tools/PXL/PXL.o Tools/AnyOption.o $(LIBDIR)/Tools.a $(LIBDIR)/EventClass.a | $(BINDIR)
				$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/ROI_analysis:	MISalgo/ROI_analysis.o Tools/PXL/PXL.o ControlPlotFactory/HistoPolisher.o Tools/AnyOption.o $(LIBDIR)/EventClass.a $(LIBDIR)/MISalgo.a $(LIBDIR)/Tools.a | $(BINDIR)
			$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/GlobalStuff:	MISalgo/GlobalStuff.o MISalgo/GlobalPicture.o Tools/PXL/PXL.o ControlPlotFactory/HistoPolisher.o Tools/AnyOption.o $(LIBDIR)/Tools.a $(LIBDIR)/MISalgo.a $(LIBDIR)/EventClass.a | $(BINDIR)
			$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/ECCrossSectionRescaler:	EventClassFactory/ECCrossSectionRescaler.o Tools/PXL/PXL.o Tools/AnyOption.o $(LIBDIR)/EventClass.a | $(BINDIR)
						$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/TECResultMerger:	MISalgo/TECResultMerger.o Tools/PXL/PXL.o MISalgo/TECResultDict.o MISalgo/TECResult.o ControlPlotFactory/HistoPolisher.o Tools/AnyOption.o $(LIBDIR)/Tools.a $(LIBDIR)/EventClass.a | $(BINDIR)
				$(CXX) -o $@ $(LDFLAGS) $^



$(BINDIR)/dicePseudoData: 	MISv2/dicePseudoData.o $(LIBDIR)/Tools.a $(LIBDIR)/MISv2.a $(LIBDIR)/EventClass.a | $(BINDIR)
			$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/printClass: 	MISv2/printClass.o $(LIBDIR)/Tools.a $(LIBDIR)/EventClass.a  $(LIBDIR)/MISv2.a | $(BINDIR)
			$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/printData: 	MISv2/printData.o $(LIBDIR)/Tools.a $(LIBDIR)/EventClass.a $(LIBDIR)/MISv2.a | $(BINDIR)
			$(CXX) -o $@ $(LDFLAGS) $^

$(BINDIR)/scanClass:        MISv2/scanClass.o $(LIBDIR)/Tools.a $(LIBDIR)/EventClass.a $(LIBDIR)/MISv2.a | $(BINDIR)
			$(CXX) -o $@ $(LDFLAGS) $^


#-----Rules for shared libraries for interactive root--------------------------

EventClassFactory/TEventClass.so: 	$(LIBDIR)/EventClass.a EventClassFactory/Resolutions.o EventClassFactory/ECFileUtil.o Tools/PXL/PXL.o Tools/AnyOption.o  EventClassFactory/TEventClassDict.o EventClassFactory/TEventClass.o $(LIBDIR)/Tools.a
					$(CXX) -o $@ -shared $(LDFLAGS) -O $^

MISalgo/TECResult.so:	$(LIBDIR)/MISalgo.a Tools/PXL/PXL.o ControlPlotFactory/HistoPolisher.o Tools/AnyOption.o $(LIBDIR)/EventClass.a  MISalgo/TECResult.o MISalgo/TECResultDict.o EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o $(LIBDIR)/Tools.a
			$(CXX) -o $@ -shared $(LDFLAGS) -O $^



#-----Library rules------------------------------------------------------------

$(LIBDIR):
	mkdir $(LIBDIR)

$(LIBDIR)/ControlPlotFactory.a: 	ControlPlotFactory/CcControl.o ControlPlotFactory/PlotBase.o ControlPlotFactory/DiffPlotBase.o ControlPlotFactory/MuonPlots.o ControlPlotFactory/MuonDiffPlots.o ControlPlotFactory/ElePlots.o ControlPlotFactory/EleDiffPlots.o  ControlPlotFactory/GammaPlots.o ControlPlotFactory/GammaDiffPlots.o ControlPlotFactory/METPlots.o ControlPlotFactory/METDiffPlots.o ControlPlotFactory/JetPlots.o ControlPlotFactory/JetDiffPlots.o ControlPlotFactory/bTagDiffPlotsAllJets.o ControlPlotFactory/bTagDiffPlotsTaggedJets.o ControlPlotFactory/VertexDiffPlots.o ControlPlotFactory/EventPlots.o ControlPlotFactory/HistoPolisher.o | $(LIBDIR)
				ar rcs $@ $^

$(LIBDIR)/ControlPlots2.a: 	ControlPlotFactory/HistoPolisher.o ControlPlots2/PlotBase.o ControlPlots2/MultiParticlePlots.o ControlPlots2/ParticlePlots.o ControlPlots2/RecControl.o ControlPlots2/RecGammaPlots.o ControlPlots2/RecJetPlots.o ControlPlots2/RecHltPlots.o ControlPlots2/RecL1Plots.o ControlPlots2/RecElePlots.o ControlPlots2/RecECALPlots.o | $(LIBDIR)
			ar rcs $@ $^

$(LIBDIR)/ParticleMatcher.a: ParticleMatcher/ParticleMatcher.o ParticleMatcher/EventSelector.o $(LIBDIR)/Tools.a | $(LIBDIR)
			ar rcs $@ $^


$(LIBDIR)/EventClass.a:	EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o Tools/PXL/PXL.o EventClassFactory/Resolutions.o | $(LIBDIR)
			ar rcs $@ $^

$(LIBDIR)/MISalgo.a:	MISalgo/TECResult.o MISalgo/TECResultDict.o MISalgo/RegionScanner.o MISalgo/PoissonCalculator.o MISalgo/ErrorComputer.o MISalgo/ConvolutionComputer.o MISalgo/ECDicer.o MISalgo/ErrorService.o MISalgo/ECDicer_add.o MISalgo/ErrorService_add.o MISalgo/ECDicer_multiply.o MISalgo/ErrorService_multiply.o MISalgo/ECUpDownError.o MISalgo/ECResultTable.o $(LIBDIR)/Tools.a | $(LIBDIR)
		ar rcs $@ $^


$(LIBDIR)/MISv2.a: MISv2/ErrorContainer.o MISv2/ErrorService.o MISv2/ErrorService_add.o MISv2/ErrorService_multiply.o MISv2/ECUpDownError.o MISv2/MCBin.o MISv2/ProcessList.o MISv2/ECReader.o MISv2/ECDicer_add.o MISv2/ECDicer_multiply.o MISv2/ErrorComputer_add.o MISv2/ECPrinter.o MISv2/DataPrinter.o MISv2/PoissonCalculator.o MISv2/ConvolutionComputer_add.o MISv2/ECScanner.o | $(LIBDIR)
		ar rcs $@ $^

$(LIBDIR)/Tools.a: Tools/Tools.o Tools/MConfig.o | $(LIBDIR)
		ar rcs $@ $^

#-----Misc rules---------------------------------------------------------------

EventClassFactory/TEventClassDict.cc: EventClassFactory/TEventClass.hh EventClassFactory/TEventClassLinkDef.h
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


df = $(DEPDIR)/$(subst /,_,$(subst $(suffix $@),,$@))

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
