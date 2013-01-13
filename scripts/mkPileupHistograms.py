#!/usr/bin/env python

import logging
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

log = logging.getLogger( 'makePileupHistograms' )

usage = '%prog [options]'
desc  = 'For data pile-up distributions (histograms), please use: pileupCalc.py.'
desc += 'This script generates histograms for the different MC truth pile-up distributions.'

parser = optparse.OptionParser( description = desc, usage = usage )
parser.add_option( '-o', '--outdir', metavar='OUTDIR', default=None,
                   help='Define the output directory for the rootfiles (histograms). [default = $MUSIC_BASE/ConfigFiles]' )
parser.add_option( '-O', '--offset', metavar='OFFSET', type=float, default=0.0,
                   help='Set the offset for the bin edges. [default = %default]' )
parser.add_option( '-c', '--campaign', action='store', default = 'SU12_S10',
                   help = "Specify the MC campaign you are using. So far only 'FA11', 'SU12_S10' and 'SU12_S7' are supported. [default = %default]" )
parser.add_option(       '--debug', metavar = 'LEVEL', default = 'INFO',
                   help = 'Set the debug level. Allowed values: ERROR, WARNING, INFO, DEBUG. [default: %default]' )
( options, args ) = parser.parse_args()

if not options.outdir:
    options.outdir = os.path.join( music_base, 'ConfigFiles' )

if not os.path.isdir( options.outdir ):
    os.makedirs( options.outdir )

format = '%(levelname)s at %(asctime)s: %(message)s'
logging.basicConfig( level = logging._levelNames[ options.debug ], format = format, datefmt = '%F %H:%M:%S' )

from ROOT import *

# See also:
# https://twiki.cern.ch/twiki/bin/view/CMS/Pileup_2011_Reweighting
# https://twiki.cern.ch/twiki/bin/view/CMS/Pileup_MC_Gen_Scenarios
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

# Distribution used for S10 Summer2012 MC.
#
probdist_Summer12_S10 = [
    2.560E-06,
    5.239E-06,
    1.420E-05,
    5.005E-05,
    1.001E-04,
    2.705E-04,
    1.999E-03,
    6.097E-03,
    1.046E-02,
    1.383E-02,
    1.685E-02,
    2.055E-02,
    2.572E-02,
    3.262E-02,
    4.121E-02,
    4.977E-02,
    5.539E-02,
    5.725E-02,
    5.607E-02,
    5.312E-02,
    5.008E-02,
    4.763E-02,
    4.558E-02,
    4.363E-02,
    4.159E-02,
    3.933E-02,
    3.681E-02,
    3.406E-02,
    3.116E-02,
    2.818E-02,
    2.519E-02,
    2.226E-02,
    1.946E-02,
    1.682E-02,
    1.437E-02,
    1.215E-02,
    1.016E-02,
    8.400E-03,
    6.873E-03,
    5.564E-03,
    4.457E-03,
    3.533E-03,
    2.772E-03,
    2.154E-03,
    1.656E-03,
    1.261E-03,
    9.513E-04,
    7.107E-04,
    5.259E-04,
    3.856E-04,
    2.801E-04,
    2.017E-04,
    1.439E-04,
    1.017E-04,
    7.126E-05,
    4.948E-05,
    3.405E-05,
    2.322E-05,
    1.570E-05,
    5.005E-06
    ]

# Distribution used for S7 Summer2012 MC.
#
probdist_Summer12_S7 = [
    2.344E-05,
    2.344E-05,
    2.344E-05,
    2.344E-05,
    4.687E-04,
    4.687E-04,
    7.032E-04,
    9.414E-04,
    1.234E-03,
    1.603E-03,
    2.464E-03,
    3.250E-03,
    5.021E-03,
    6.644E-03,
    8.502E-03,
    1.121E-02,
    1.518E-02,
    2.033E-02,
    2.608E-02,
    3.171E-02,
    3.667E-02,
    4.060E-02,
    4.338E-02,
    4.520E-02,
    4.641E-02,
    4.735E-02,
    4.816E-02,
    4.881E-02,
    4.917E-02,
    4.909E-02,
    4.842E-02,
    4.707E-02,
    4.501E-02,
    4.228E-02,
    3.896E-02,
    3.521E-02,
    3.118E-02,
    2.702E-02,
    2.287E-02,
    1.885E-02,
    1.508E-02,
    1.166E-02,
    8.673E-03,
    6.190E-03,
    4.222E-03,
    2.746E-03,
    1.698E-03,
    9.971E-04,
    5.549E-04,
    2.924E-04,
    1.457E-04,
    6.864E-05,
    3.054E-05,
    1.282E-05,
    5.081E-06,
    1.898E-06,
    6.688E-07,
    2.221E-07,
    6.947E-08,
    2.047E-08
    ]

if not options.campaign:
    given_probdist = probdist_Summer12_S10
else:
    if options.campaign == 'FA11':
        given_probdist = probdist_Fall11
    elif options.campaign == 'SU12_S10':
        given_probdist = probdist_Summer12_S10
    elif options.campaign == 'SU12_S7':
        given_probdist = probdist_Summer12_S7
    else:
        raise RuntimeError, 'Unknown MC campaign %s' %options.campaign

log.info( 'Using pileup distribution from MC campaign %s.' %options.campaign )
probdist = given_probdist

numBin = len( probdist )
xMin   = - options.offset
xMax   = numBin - options.offset

mcHist = TH1D( 'mcHist_%s' %options.campaign, 'Pile-Up MC', numBin, xMin, xMax )
log.info( 'Created histogram mcHist_%s with numPileupBins = %s and maxPileupBin = %s.' %( options.campaign, numBin, xMax ) )

for i in range( 0, numBin ):
    mcHist.SetBinContent( i + 1, probdist[i] )

rootfile = TFile( os.path.join( options.outdir, 'MC_PileUp_%s.root' %options.campaign ), 'RECREATE' )
rootfile.cd()

mcHist.Write( 'pileup' )

rootfile.Close()
del rootfile


if options.campaign == 'FA11':
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

    mcHist = TH1D( 'mcHist_%s_InTime' %options.campaign, 'Pile-Up MC', numBin, xMin, xMax )
    log.info( 'Created histogram mcHist_%s_InTime with numPileupBins = %s and maxPileupBin = %s.' %( options.campaign, numBin, xMax ) )

    for i in range( 0, numBin ):
        mcHist.SetBinContent( i + 1, probdist_Fall11_InTime[i] )

    rootfile = TFile( os.path.join( options.outdir, 'MCPileUp_%s_InTime.root' %options.campaign ), 'RECREATE' )
    rootfile.cd()

    mcHist.Write( 'pileup' )

    rootfile.Close()
    del rootfile
