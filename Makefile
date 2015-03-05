# If you want to learn more about makefiles read this:
# https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile
# In general we try to stick to the recipes shown there as far as possible.

# If you want to compile with debug flags call:
#    DEBUG=1 make
#
# If you want verbose output call:
#    VERBOSE=1 make

# Just for fun:
NO_COLOR  = \e[0m
GREEN     = \e[0;32m
VIOLET    = \e[0;35m
BOLDGREEN = \e[1;32m
RED       = \e[0;31m
YELLOW    = \e[0;33m

# Prepare the compiler/linker flags.

SHELL = /bin/bash

# Debug flags?
ifdef DEBUG
   CXXFLAGS += -O0 -fstack-protector-all -Wfatal-errors
   # NOTE: For some ominous reasons, these DEBUG flags cause the linker to need
   # 20 min to link everything when linking on NFS. Perhaps this will be better
   # with a newer version?!
   CXXFLAGS += -g3 -ggdb
else
   CXXFLAGS := -O3
endif

# ROOT libraries:
ROOT_CFLAGS  := $(shell root-config --cflags)
ROOT_LDFLAGS := $(shell root-config --ldflags)
ROOT_GLIBS   := $(shell root-config --libs)

ifndef CMSSW_RELEASE_BASE
   $(error Error: CMSSW libraries not found!)
endif

# Paths for CMSSW libraries:
CMSSW_DCAP_BASE  := $(shell cat $(CMSSW_RELEASE_BASE)/config/toolbox/$(SCRAM_ARCH)/tools/selected/dcap.xml | grep 'name="DCAP_BASE"' | sed -e 's/.*default="//' | sed -e 's/"\/>//')
CMSSW_BOOST_BASE := $(shell cat $(CMSSW_RELEASE_BASE)/config/toolbox/$(SCRAM_ARCH)/tools/selected/boost.xml | grep 'name="BOOST_BASE"' | sed -e 's/.*default="//' | sed -e 's/"\/>//')
CMSSW_GSL_BASE   := $(shell cat $(CMSSW_RELEASE_BASE)/config/toolbox/$(SCRAM_ARCH)/tools/selected/gsl.xml | grep 'name="GSL_BASE"' | sed -e 's/.*default="//' | sed -e 's/"\/>//')

# If you're using a patched CMSSW release, some of the libs are still in the base release, so you also have to look there.
CMSSW_RELEASE_BASE_NOPATCH := $(shell echo $(CMSSW_RELEASE_BASE) | sed -e 's/-patch//' -e 's/_patch.//')

CMSSW_LIB_PATHS := -L$(CMSSW_BASE)/lib/$(SCRAM_ARCH)
CMSSW_LIB_PATHS += -L$(CMSSW_RELEASE_BASE)/lib/$(SCRAM_ARCH)
CMSSW_LIB_PATHS += -L$(CMSSW_RELEASE_BASE_NOPATCH)/lib/$(SCRAM_ARCH)
CMSSW_LIB_PATHS += -L$(CMSSW_DCAP_BASE)/lib
CMSSW_LIB_PATHS += -L$(CMSSW_GSL_BASE)/lib
CMSSW_LIB_PATHS += -L$(CMSSW_BOOST_BASE)/lib

CMSSW_LIBS += -lCondFormatsJetMETObjects
CMSSW_LIBS += -lPhysicsToolsUtilities
CMSSW_LIBS += -ldcap
CMSSW_LIBS += -lgsl
CMSSW_LIBS += -lgslcblas
CMSSW_LIBS += -lboost_filesystem
CMSSW_LIBS += -lboost_date_time
CMSSW_LIBS += -lboost_serialization
CMSSW_LIBS += -lz

# For the headers there are symlinks.
CMSSW_INC_PATHS := -isystem$(CMSSW_BASE)/src
CMSSW_INC_PATHS += -isystem$(CMSSW_RELEASE_BASE)/src
CMSSW_INC_PATHS += -isystem$(CMSSW_DCAP_BASE)/include
CMSSW_INC_PATHS += -isystem$(CMSSW_BOOST_BASE)/include
CMSSW_INC_PATHS += -isystem$(CMSSW_GSL_BASE)/include
CMSSW_INC_PATHS += -isystem$(LIBS3A)/include

#LHAPDF_LIB_PATH := -L$(MUSIC_BASE)/local/lib
#LHAPDF_INC_PATH := -I$(MUSIC_BASE)/local/include
#LHAPDF_LIB := -lLHAPDF
#no nice way to get this automated
LHAPDF_BASE := /cvmfs/cms.cern.ch/slc6_amd64_gcc481/external/lhapdf6/6.1.4/
LHAPDF_LIB_PATH := -L$(LHAPDF_BASE)/lib
LHAPDF_INC_PATH := -I$(LHAPDF_BASE)/include
LHAPDF_LIB := -lLHAPDF

EXTRA_CFLAGS  := -ffloat-store $(CMSSW_INC_PATHS) $(LHAPDF_INC_PATH) -DPXL_ENABLE_DCAP
EXTRA_LDFLAGS := $(CMSSW_LIB_PATHS) $(CMSSW_LIBS) $(LHAPDF_LIB_PATH) $(LHAPDF_LIB)

CXXFLAGS += --ansi -Wall -fpic -Wno-sign-compare -c -I. $(ROOT_CFLAGS) $(EXTRA_CFLAGS)
LDFLAGS  += $(ROOT_LDFLAGS) $(ROOT_GLIBS) $(SYSLIBS) -L. $(EXTRA_LDFLAGS)

all: CXXFLAGS += --ansi -w -Wall -fpic -c -I. $(ROOT_CFLAGS) $(EXTRA_CFLAGS)
validation: CXXFLAGS += --ansi -Wall -fpic -c -I. $(ROOT_CFLAGS) $(EXTRA_CFLAGS) -Dvalidation

ECHO = @echo -e "$(BOLDGREEN)Building$(NO_COLOR) $@ ..."

BUILDBIN    = $(CXX) -o $@ $(LDFLAGS) $^
BUILDSHARED = $(CXX) -o $@ -shared $(LDFLAGS) -O $^

# Set directories:
# Where to store generated libraries?
LIBDIR = lib
# Where to store the generate dependency files?
DEPDIR = dep
# Where to copy binaries?
BINDIR = bin
# Where are the .cc files for the executables?
PROGSDIR = Progs
#pxl needs extra
PXLDIR = Pxl/Pxl

ifneq ("$(wildcard specialAna/)","")
$(shell   echo " ")
else
$(shell   mkdir -p "specialAna")
$(shell   cp Validator/specialAna.* specialAna/)
endif

# Define all targets:
# List all executables and .so's here.
TARGETS := $(BINDIR)/music
TARGETS += $(BINDIR)/ECMerger
TARGETS += $(BINDIR)/ECFileUtil
TARGETS += $(BINDIR)/FakeClass
TARGETS += $(BINDIR)/ECCrossSectionRescaler
TARGETS += $(BINDIR)/dicePseudoData
TARGETS += $(BINDIR)/printClass
TARGETS += $(BINDIR)/printData
TARGETS += $(BINDIR)/scanClass
TARGETS += $(LIBDIR)/TEventClass.so

TARGETS2 := $(BINDIR)/music2
TARGETS2 += $(BINDIR)/ECMerger
TARGETS2 += $(BINDIR)/ECFileUtil
TARGETS2 += $(BINDIR)/FakeClass
TARGETS2 += $(BINDIR)/ECCrossSectionRescaler
TARGETS2 += $(BINDIR)/dicePseudoData
TARGETS2 += $(BINDIR)/printClass
TARGETS2 += $(BINDIR)/printData
TARGETS2 += $(BINDIR)/scanClass
TARGETS2 += $(LIBDIR)/TEventClass.so

# Sources and objects for ControlPlotFactory:
SRCS := $(wildcard ControlPlotFactory/*.cc)
CPF  := $(patsubst %.cc,%.o,$(SRCS))

# Sources and objects for ControlPlots2:
SRCS := $(wildcard ControlPlots2/*.cc)
CP2  := $(patsubst %.cc,%.o,$(SRCS))

# Sources and objects for the main program:
SRCS := $(wildcard Main/*.cc)
MAIN := $(patsubst %.cc,%.o,$(SRCS))

# Sources and objects for specialAna:
SRCS  := $(wildcard specialAna/*.cc)
SANA := $(patsubst %.cc,%.o,$(SRCS))

# Sources and objects for Validator:
SRCS  := $(wildcard Validator/*.cc)
SVAL := $(patsubst %.cc,%.o,$(SRCS))

# Sources and objects for MISv2:
SRCS  := $(wildcard MISv2/*.cc)
MISV2 := $(patsubst %.cc,%.o,$(SRCS))

# Tools obejcts:
TOOLS := Tools/Tools.o Tools/MConfig.o Tools/SignalHandler.o Tools/dCache/dCacheBuf.o

# pxl lib:
#PXLTARGET	:=Tools/pxl.o
SRCS 		:= $(wildcard $(PXLDIR)/src/*cc)
#HEADERS = $(wildcard $(PXLDIR)/interface/pxl*.hh)
PXL := $(patsubst %.cc,%.o,$(SRCS))
#TARGETS += $(PXLTARGET)

# Objects for EventClass(Factory):
SRCS := $(wildcard EventClassFactory/*.cc)
TEC  := $(patsubst %.cc,%.o,$(SRCS))
TEVENTCLASS        := EventClassFactory/TEventClass.o EventClassFactory/TEventClassDict.o EventClassFactory/Resolutions.o $(TOOLS) $(PXL)
TEVENTCLASSFACTORY := $(TEVENTCLASS) $(TEC)

# Define rules:

.PHONY: all clean install-python validation

all: $(TARGETS) install-python
	@echo $(TEST)

clean:
ifndef VERBOSE
	@echo -e "$(RED)Removing all targets...$(NO_COLOR)"
	@rm -f $(TARGETS)
	@echo -e "$(RED)Removing all objects...$(NO_COLOR)"
	@find . -name '*.o' -exec rm {} \;
	@echo -e "$(RED)Removing all ROOT dictionaries...$(NO_COLOR)"
	@find . -name '*Dict.h' -exec rm {} \;
	@find . -name '*Dict.cc' -exec rm {} \;
	@echo -e "$(RED)Removing $(LIBDIR) $(DEPDIR) $(BINDIR)...$(NO_COLOR)"
	@rm -rf $(LIBDIR) $(DEPDIR) $(BINDIR)
	@echo -e "$(RED)Removing python relics...$(NO_COLOR)"
	@find ./ -name '*.pyc' -exec rm {} \;
else
	@echo -e "$(RED)Removing all targets...$(NO_COLOR)"
	rm -f $(TARGETS)
	@echo -e "$(RED)Removing all objects...$(NO_COLOR)"
	find . -name '*.o' -exec rm {} \;
	@echo -e "$(RED)Removing all ROOT dictionaries...$(NO_COLOR)"
	find . -name '*Dict.h' -exec rm {} \;
	find . -name '*Dict.cc' -exec rm {} \;
	@echo -e "$(RED)Removing $(LIBDIR) $(DEPDIR) $(BINDIR)...$(NO_COLOR)"
	rm -rf $(LIBDIR) $(DEPDIR) $(BINDIR)
	@echo -e "$(RED)Removing python relics...$(NO_COLOR)"
	find ./ -name '*.pyc' -exec rm {} \;
endif

install-python: | $(BINDIR)
	@ln -sf ../MISv2/MISMaster/MISMaster.py $(BINDIR)/MISMaster
	@ln -sf ../MISv2/MISMaster/MISPrinter.py $(BINDIR)/MISPrinter
	@ln -sf ../scripts/aix3adb2music.py $(BINDIR)/aix3adb2music
	@ln -sf ../scripts/ECMerger.py $(BINDIR)/ECMerger2
	@ln -sf ../scripts/ECMerger.py $(BINDIR)/ECMerger.py
	@ln -sf ../scripts/jukebox.py $(BINDIR)/jukebox
	@ln -sf ../scripts/mkPileupHistograms.py $(BINDIR)/mkPileupHistograms
	@ln -sf ../scripts/mkSubmitFile.py $(BINDIR)/mkSubmitFile
	@ln -sf ../scripts/mkPlot.py $(BINDIR)/mkPlot
	@ln -sf ../scripts/remix.py $(BINDIR)/remix
	@ln -sf ../scripts/rebinEventClasses.py $(BINDIR)/rebinEventClasses
	@ln -sf ../scripts/renameProcess.py $(BINDIR)/renameProcess
	@ln -sf ../scripts/submitClassification.py $(BINDIR)/submitClassification
	@ln -sf ../scripts/handleGridOutput.py $(BINDIR)/handleGridOutput
	@ln -sf ../scripts/plotPileUp.py $(BINDIR)/plotPileUp
	@ln -sf ../scripts/installLHAPDF.py $(BINDIR)/installLHAPDF
	@ln -sf ../python/listFiles.py $(BINDIR)/listFiles
	@ln -sf ../python/radio.py $(BINDIR)/radio
	@ln -sf ../Tools/plotPtilde.py $(BINDIR)/plotPtilde

validation: $(TARGETS2) install-python

# Directory rules:
$(BINDIR):
	@mkdir $(BINDIR)

$(LIBDIR):
	@mkdir $(LIBDIR)

$(DEPDIR):
	@mkdir $(DEPDIR)

# Main music build:
$(BINDIR)/music: $(PROGSDIR)/music.o $(MAIN) $(SANA) $(TEVENTCLASSFACTORY) $(TOOLS) $(CPF) $(CP2) | $(BINDIR)/ECMerger $(BINDIR) $(PXL)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

$(BINDIR)/music2: $(PROGSDIR)/music.o $(MAIN) $(SVAL) $(TEVENTCLASSFACTORY) $(TOOLS) $(CPF) $(CP2) | $(BINDIR)/ECMerger $(BINDIR) $(PXL)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
	@mv $(BINDIR)/music2 $(BINDIR)/music
else
	$(BUILDBIN)
	@mv $(BINDIR)/music2 $(BINDIR)/music
endif

# Rules to build the standalone helper programs:
$(BINDIR)/ECMerger: $(PROGSDIR)/ECMerger.o Tools/AnyOption.o $(TEVENTCLASS) | $(BINDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

$(BINDIR)/ECFileUtil: $(PROGSDIR)/ECFileUtil.o Tools/AnyOption.o $(TEVENTCLASS) | $(BINDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

$(BINDIR)/FakeClass: $(PROGSDIR)/FakeClass.o $(TOOLS) $(TEVENTCLASS) | $(BINDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

$(BINDIR)/ECCrossSectionRescaler: $(PROGSDIR)/ECCrossSectionRescaler.o $(TOOLS) Tools/AnyOption.o $(TEVENTCLASS) | $(BINDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

# Rules to build the MISv2 programs:
$(BINDIR)/dicePseudoData: $(PROGSDIR)/dicePseudoData.o $(MISV2) $(TEVENTCLASS) $(TOOLS) | $(BINDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

$(BINDIR)/printClass: $(PROGSDIR)/printClass.o $(MISV2) $(TEVENTCLASS) $(TOOLS) | $(BINDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

$(BINDIR)/printData: $(PROGSDIR)/printData.o $(MISV2) $(TEVENTCLASS) $(TOOLS) | $(BINDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

$(BINDIR)/scanClass: $(PROGSDIR)/scanClass.o $(MISV2) $(TEVENTCLASS) $(TOOLS) | $(BINDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDBIN)
else
	$(BUILDBIN)
endif

# Rules for shared libraries for interactive (py)ROOT:
$(LIBDIR)/TEventClass.so: $(TEVENTCLASS) | $(LIBDIR)
ifndef VERBOSE
	$(ECHO)
	@$(BUILDSHARED)
else
	$(BUILDSHARED)
endif

EventClassFactory/TEventClassDict.cc: EventClassFactory/TEventClass.hh EventClassFactory/TEventClassLinkDef.h
	@echo "Generating TEventClass dictionary ..."
	@rootcint -v2 -f $@ -c $^ # -v2: Display error and warning messages.
	@sed -e "s@#include \"EventClassFactory/TEventClass.hh\"@#include \"TEventClass.hh\"@" EventClassFactory/TEventClassDict.h > EventClassFactory/TEventClassDict.tmp
	@mv EventClassFactory/TEventClassDict.tmp EventClassFactory/TEventClassDict.h


#-----Automatic rules----------------------------------------------------------

#For details on what's happening below, please see: http://make.paulandlesley.org/autodep.html
#Advantage: Should automatically keep track of all and every dependency.
#Disadvantage: Makes make slower, because the automatically generated dependency files tend to be huge.

#All .P dependency files are collected in one directory, to avoid cluttering up the source directories.

# Take the relative path to the file and replace '/' by '_'.
# Thus we will have two different dependency files even if the files have the
# same name.
df = $(DEPDIR)/$(subst /,_,$(subst $(suffix $@),,$@))

%.o: %.cc | $(DEPDIR)
ifndef VERBOSE
	@echo -e "$(VIOLET)Compiling$(NO_COLOR) $< ..."
	@$(CXX) -MD $(CXXFLAGS) -o $@ $<
else
	$(CXX) -MD $(CXXFLAGS) -o $@ $<
endif
	@cp $*.d $(df).P;
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(df).P;
	@$(RM) $*.d

#Notable difference to the system described above: All .P files in the DEPDIR will be included.
#Even if there is no .cc file for it anymore.
#Advantage: No need to keep track of all .cc files
#Disadvantage: Removing a .cc will leave a stale .P file, that might or might not cause problems
#If that happens (probably causing a make target error):
# - Delete the offending .P file in dep/
# - OR do a make clean
# - Do NOT delete the whole dep/ directory without doing make clean
-include $(DEPDIR)/*.P
