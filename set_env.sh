#!/usr/bin/sh
export MUSIC_BASE=$PWD
export PATH=$PATH:$MUSIC_BASE/bin;
export PYTHONPATH=$MUSIC_BASE/python:$MUSIC_BASE/MISv2/MISMaster:$PYTHONPATH

export LHAPDF=/cvmfs/cms.cern.ch/slc6_amd64_gcc481/external/lhapdf6/6.1.4/
export LD_LIBRARY_PATH=$LHAPDF/lib:$LD_LIBRARY_PATH
export LHAPATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current/:/cvmfs/cms.cern.ch/slc6_amd64_gcc481/external/lhapdf6/6.1.4/share/LHAPDF/PDFsets
