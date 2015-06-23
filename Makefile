### definition of the compiler options ###
#       -I location of directory containing include files
#       -L location of directory containing libraries
#       -lname include the library from -L location called libname.a
#          -lg2c is the library containing info on converting fortran to C
#          -lf   is the library containing the intrinsic for HPUX only.
#       -shared make a shared library as output
#       -fPIC produce position independent code
#        necessary on some platforms (including HPUX) for -shared
#       -fpic ^^ same(?)
#       -O optimizes
#       -g produces output for the debugger
#       -pg produces output for gprof profiler
#       note: if you want to see all warnings and ensure ansi standard
#             compatibility, use:
#             -pipe -ansi -pedantic -fnonnull-objects \
#             -W -Wall -Wwrite-strings -Wpointer-arith -Wnested-externs \
#             -Woverloaded-virtual -Wbad-function-cast -fnonnull-objects
#       The proper order for cernlib libraries is:
#       -lpawlib -lgraflib -lgrafX11 -lmathlib -lkernlib -lpacklib -ljetset74
#
# makefile syntax:
#        for target thedir/target.suf from source anotherdir/source.suf2
#        ${*D}  = thedir
#        ${*F}  = target
#        $^     = name of all prerequisites
#        $+     = like $^ with duplicated prerequisites if mentioned more than once
#        $*     = thedir/target
#        $@     = thedir/target.suf
#        $<     = anotherdir/source.suf2
#

PROGRAM:=Progs/music

########################################
# directories
# by default, the validator is compiled.
# usually one will change this to their personal analysis.

ifndef MYPXLANA
	MYPXLANA:=Validator
endif

#define all directories with source code
DIRS	:= $(MYPXLANA)
DIRS	+= Main
DIRS	+= Tools
DIRS	+= Progs

PXLDIR	= Pxl/Pxl
DIRS	+=$(PXLDIR)/src
DIRS	+=$(PXLDIR)/interface
# define source files
SOURCES	:= $(wildcard *.cc)
SOURCES	+= $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cc))

# define header, object and dependency files based on source files
HEADERS	:= $(SOURCES:.cc=.h)
OBJECTS	:= $(SOURCES:.cc=.o)
DEPENDS	:= $(SOURCES:.cc=.d)


########################################
# compiler and flags

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
CMSSW_LIBS += -lboost_program_options

# For the headers there are symlinks.
CMSSW_INC_PATHS := -isystem$(CMSSW_BASE)/src
CMSSW_INC_PATHS += -isystem$(CMSSW_RELEASE_BASE)/src
CMSSW_INC_PATHS += -isystem$(CMSSW_DCAP_BASE)/include
CMSSW_INC_PATHS += -isystem$(CMSSW_BOOST_BASE)/include
CMSSW_INC_PATHS += -isystem$(CMSSW_GSL_BASE)/include
CMSSW_INC_PATHS += -isystem$(LIBS3A)/include

#no nice way to get this automated
LHAPDF_BASE := /cvmfs/cms.cern.ch/slc6_amd64_gcc481/external/lhapdf6/6.1.4/
LHAPDF_LIB_PATH := -L$(LHAPDF_BASE)/lib
LHAPDF_INC_PATH := -I$(LHAPDF_BASE)/include
LHAPDF_LIB := -lLHAPDF

EXTRA_CFLAGS  := -ffloat-store $(CMSSW_INC_PATHS) $(LHAPDF_INC_PATH) -DPXL_ENABLE_DCAP
EXTRA_LDFLAGS := $(CMSSW_LIB_PATHS) $(CMSSW_LIBS) $(LHAPDF_LIB_PATH) $(LHAPDF_LIB)

CC	:= g++
CFLAGS	:= -O3 -Wall -fPIC -fsignaling-nans -funsafe-math-optimizations -fno-rounding-math -fno-signaling-nans -fcx-limited-range -fno-associative-math # -DNDEBUG # -pg for gprof

LD	:= g++
LDFLAGS	:= -O3fast -lz -fno-associative-math # -pg for gprof

# Debug flags?
ifdef DEBUG
   CFLAGS = -O0 -Wall -fPIC -g -pg -fprofile-generate
   LDFLAGS = -O0 -g -pg -lz -fprofile-generate
endif

CFLAGS	+= -DMYPXLANA=$(MYPXLANA)/AnalysisComposer.hh
CFLAGS	+= -I. $(ROOT_CFLAGS) $(EXTRA_CFLAGS)

LDFLAGS  += $(ROOT_LDFLAGS) $(ROOT_GLIBS) $(SYSLIBS) -L. $(EXTRA_LDFLAGS)


########################################
# define colors for better overview

RED=$(shell tput setaf 1)
YELLOW=$(shell tput setaf 2)
GREEN=$(shell tput setaf 3)
BOLD=$(shell tput bold)
NORMAL=$(shell tput sgr0)


########################################
# define all targets

TARGETS	:= $(PROGRAM)


########################################
# additional includes
# if you have additional includes, you can put them in the includes.mk file in
# your analysis directory
ifneq (,$(wildcard $(MYPXLANA)/includes.mk))
$(info Makefile fragment $(GREEN)include.mk$(NORMAL) found. Including ...)
-include $(MYPXLANA)/includes.mk
endif


########################################
# targets

# target: dependency
#	@shellcommand
#	command

all: $(TARGETS)

clean:
	@rm -f $(PROGRAM) $(OBJECTS) $(DEPENDS)

$(PROGRAM): $(OBJECTS)
	@echo "Building $@ ..."
	$(LD) $(LDFLAGS) $^ -o $@
	@echo "$@ done"


########################################
# additional targets
# if you have additional targets, you can put them in the includes.mk file in
# your analysis directory

ifneq (,$(wildcard $(MYPXLANA)/targets.mk))
$(info Makefile fragment $(GREEN)targets.mk$(NORMAL) found. Including ...)
-include $(MYPXLANA)/targets.mk
endif


########################################
# rules

%.o : %.cc
	$(CC) -MD -MP $(CFLAGS) -c -o $@ $<

-include $(DEPENDS)
