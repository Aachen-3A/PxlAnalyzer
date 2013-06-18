#!/usr/bin/env python

import optparse
import os

import checkEnvironment

try:
    music_base, cmssw_version, cmssw_base, scram_arch = checkEnvironment.checkEnvironment()
except EnvironmentError, err:
    log.error( err )
    log.info( 'Exiting...' )
    sys.exit( err.errno )


# Make path abolute.
# This means expand all $-variables and ~ and prepend the current working directory if path is relative.
#
def makeAbsPath( path ):
    if path:
        path = os.path.realpath( os.path.expandvars( os.path.expanduser( path ) ) )
    return path


usage = '%prog [options]'
desc  = 'For data pile-up distributions (histograms), please use: pileupCalc.py.'
desc += 'This script generates histograms for the different MC truth pile-up distributions.'

parser = optparse.OptionParser( description = desc, usage = usage )
parser.add_option( '-o', '--outdir', metavar='OUTDIR', default=None,
                   help='Define the output directory for the rootfiles (histograms). [default = $MUSIC_BASE/ConfigFiles]' )
parser.add_option( '-O', '--offset', metavar='OFFSET', type=float, default=0.0,
                   help='Set the offset for the bin edges. [default = %default]' )

( options, args ) = parser.parse_args()

if not options.outdir:
    options.outdir = os.path.join( music_base, 'ConfigFiles' )

if not os.path.isdir( options.outdir ):
    os.makedirs( options.outdir )

from ROOT import *

# See also:
# https://twiki.cern.ch/twiki/bin/view/CMS/Pileup_2011_Reweighting
#
# Distribution used for Fall2011 MC.
#
probdist_Fall11 = [
    0.003388501,
    0.010357558,
    0.024724258,
    0.042348605,
    0.058279812,
    0.068851751,
    0.072914824,
    0.071579609,
    0.066811668,
    0.060672356,
    0.054528356,
    0.04919354,
    0.044886042,
    0.041341896,
    0.0384679,
    0.035871463,
    0.03341952,
    0.030915649,
    0.028395374,
    0.025798107,
    0.023237445,
    0.020602754,
    0.0180688,
    0.015559693,
    0.013211063,
    0.010964293,
    0.008920993,
    0.007080504,
    0.005499239,
    0.004187022,
    0.003096474,
    0.002237361,
    0.001566428,
    0.001074149,
    0.000721755,
    0.000470838,
    0.00030268,
    0.000184665,
    0.000112883,
    6.74043E-05,
    3.82178E-05,
    2.22847E-05,
    1.20933E-05,
    6.96173E-06,
    3.4689E-06,
    1.96172E-06,
    8.49283E-07,
    5.02393E-07,
    2.15311E-07,
    9.56938E-08,
    ]

numBin = len( probdist_Fall11 )
xMin   = - options.offset
xMax   = numBin - options.offset

mcHist = TH1D( 'mcHist_Fall11', 'Pile-Up MC', numBin, xMin, xMax )

for i in range( 0, numBin ):
    mcHist.SetBinContent( i + 1, probdist_Fall11[i] )

rootfile = TFile( os.path.join( options.outdir, 'MCPileUp_Fall11.root' ), 'RECREATE' )
rootfile.cd()

mcHist.Write( 'pileup' )

rootfile.Close()
del rootfile


# In-time (Poisson-smeared) Distribution for Fall 2011
#
probdist_Fall11_InTime = [
    0.014583699,
    0.025682975,
    0.038460562,
    0.049414536,
    0.056931087,
    0.061182816,
    0.062534625,
    0.061476918,
    0.058677499,
    0.055449877,
    0.051549051,
    0.047621024,
    0.043923799,
    0.040569076,
    0.037414654,
    0.034227033,
    0.031437714,
    0.028825596,
    0.026218978,
    0.023727061,
    0.021365645,
    0.01918743,
    0.016972815,
    0.014920601,
    0.013038989,
    0.011293777,
    0.009612465,
    0.008193556,
    0.006888047,
    0.005715239,
    0.004711232,
    0.003869926,
    0.003154521,
    0.002547417,
    0.002024714,
    0.001574411,
    0.001245808,
    0.000955206,
    0.000735305,
    0.000557304,
    0.000412503,
    0.000305502,
    0.000231002,
    0.000165701,
    0.000121201,
    9.30006E-05,
    6.40004E-05,
    4.22003E-05,
    2.85002E-05,
    1.96001E-05,
    1.59001E-05,
    1.01001E-05,
    8.50006E-06,
    6.60004E-06,
    2.70002E-06,
    ]

numBin = len( probdist_Fall11_InTime )
xMin   = - options.offset
xMax   = numBin - options.offset

mcHist = TH1D( 'mcHist_Fall11_InTime', 'Pile-Up MC', numBin, xMin, xMax )

for i in range( 0, numBin ):
    mcHist.SetBinContent( i + 1, probdist_Fall11_InTime[i] )

rootfile = TFile( os.path.join( options.outdir, 'MCPileUp_Fall11_InTime.root' ), 'RECREATE' )
rootfile.cd()

mcHist.Write( 'pileup' )

rootfile.Close()
del rootfile
