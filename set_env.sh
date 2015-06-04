#!/usr/bin/sh
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
export MUSIC_BASE=$DIR
export PXLANA=$DIR
export PATH=$PATH:$MUSIC_BASE/Progs;
export PYTHONPATH=$MUSIC_BASE/python:$MUSIC_BASE/MISv2/MISMaster:$PYTHONPATH

export LHAPDF=/cvmfs/cms.cern.ch/slc6_amd64_gcc481/external/lhapdf6/6.1.4/
export LD_LIBRARY_PATH=$LHAPDF/lib:$LD_LIBRARY_PATH
export LHAPATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current/:/cvmfs/cms.cern.ch/slc6_amd64_gcc481/external/lhapdf6/6.1.4/share/LHAPDF/PDFsets
