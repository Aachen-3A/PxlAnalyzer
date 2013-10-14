#!/usr/bin/env python

import optparse
import sys

def main():
    options, filePaths = parseCommandLine()

    if len( filePaths ) == 1:
        rootfile = r.TFile.Open( filePaths[0] )
        pileup = rootfile.Get( options.histo[0] )

        if options.normalize:
            integral = pileup.Integral()
            pileup.Scale( 100.0 / integral )

        minVal = pileup.GetMinimum() / 5.0
        maxVal = pileup.GetMaximum() * 5.0

        if options.normalize:
            plot = plotPileUpHisto( options, pileup, minVal, 100, 'rate / %' )
        else:
            plot = plotPileUpHisto( options,
                                    pileup,
                                    minVal,
                                    maxVal,
                                    'absolute contribution'
                                    )

        raw_input( 'Press ENTER to quit!' )

        rootfile.Close()

    else:
        rootFileData = r.TFile.Open( filePaths[0] )
        pileupData = rootFileData.Get( options.histo[0] )
        pileupData.Scale( 1.0 / pileupData.Integral() )

        rootFileMC = r.TFile.Open( filePaths[1] )
        try:
            pileupMC = rootFileMC.Get( options.histo[1] )
        except IndexError:
            # Perhabs they have the same name.
            pileupMC = rootFileMC.Get( options.histo[0] )

        pileupMC.Scale( 1.0 / pileupMC.Integral() )

        # Scale it:
        pileupData.Divide( pileupMC )

        minVal = pileupData.GetMinimum() / 5.0
        maxVal = pileupData.GetMaximum() * 10.0

        plot = plotPileUpHisto( options,
                                pileupData,
                                minVal,
                                maxVal,
                                'w_{PU}',
                                optStat=1100,
                                )
        raw_input( 'Press ENTER to quit!' )

        rootFileData.Close()
        rootFileMC.Close()



def plotPileUpHisto( options, histo, yMin, yMax, yTitle, optStat=1110 ):
    histo.SetTitle( '' )
    histo.SetLineWidth( 2 )
    histo.GetYaxis().SetRangeUser( yMin, yMax )

    histo.GetXaxis().SetTitle( '<N> events' )
    histo.GetXaxis().SetTitleSize( 0.05 )
    histo.GetXaxis().SetTitleFont( 62 )
    histo.GetXaxis().SetLabelSize( 0.05 )
    histo.GetXaxis().SetLabelFont( 62 )

    histo.GetYaxis().SetTitle( yTitle )
    histo.GetYaxis().SetTitleOffset( 1.10 )
    histo.GetYaxis().SetTitleSize( 0.05 )
    histo.GetYaxis().SetTitleFont( 62 )
    histo.GetYaxis().SetLabelSize( 0.05 )
    histo.GetYaxis().SetLabelFont( 62 )

    if options.square:
        canvas = r.TCanvas( 'Pile-up', 'Pile-up', 700, 650 )
    else:
        canvas = r.TCanvas( 'Pile-up', 'Pile-up', 1100, 700 )

    canvas.SetLeftMargin( 0.11 )
    canvas.SetRightMargin( 0.07 )
    canvas.SetLogy( r.kTRUE )
    canvas.cd()

    if options.no_stats:
        r.gStyle.SetOptStat( 0 )
        histo.SetStats( r.kFALSE )
    else:
        r.gStyle.SetOptStat( optStat )
        histo.SetStats( r.kTRUE )

    histo.Draw()
    canvas.Update()

    # Must be done AFTER drawing!
    if not options.no_stats:
        stats = histo.FindObject( 'stats' )
        r.SetOwnership( stats, 0 )
        stats.SetX1NDC( 0.68 )
        stats.SetY1NDC( 0.71 )
        stats.SetX2NDC( 0.88 )
        stats.SetY2NDC( 0.92 )

    if options.sqrt_s and not options.mc:
        sqrts = getSqrtS( options.sqrt_s )
        sqrts.Draw()

    if options.mc:
        print "HERE"
        mc = getMC( options.mc )
        mc.Draw()

    if options.run_range:
        rr = getRunRange( options.run_range )
        rr.Draw()

    if options.output:
        canvas.Print( options.output )

    canvas.Update()
    return canvas


def getSqrtS( sqrt_s=7 ):
    text = '#sqrt{s} = %.0f TeV' % sqrt_s
    tex = r.TLatex( 0.37, 0.88, text )
    r.SetOwnership( tex, 0 )
    tex.SetNDC()
    tex.SetLineWidth( 2 )

    return tex


def getMC( mc ):
    text = mc
    tex = r.TLatex( 0.37, 0.88, text )
    r.SetOwnership( tex, 0 )
    tex.SetNDC()
    tex.SetLineWidth( 2 )

    return tex


def getRunRange( runRange ):
    text = 'Run range: %s' % runRange
    tex = r.TLatex( 0.37, 0.84, text )
    r.SetOwnership( tex, 0 )
    tex.SetNDC()
    tex.SetLineWidth( 2 )
    tex.SetTextSize( 0.03 )

    return tex


def parseCommandLine():
    usage = '%prog [options] ROOTFILE(S)'
    desc  = 'Plot the pile-up histogram nicely. ' \
            'Take PILEUPHISTO from ROOTFILE and plot it with some ' \
            'meaningful labels and information. ' \
            'If FILENAME is specified, save the resulting histogram there. ' \
            'If two ROOTFILES are specified, plot the ratio of both ' \
            'PILEUPHISTOS!'

    parser = optparse.OptionParser( description=desc, usage=usage )
    parser.add_option( '-H', '--histo',
                       metavar='PILEUPHISTO(S)',
                       default='pileup',
                       help='Set the name(s) of the pile-up histogram(s) in ' \
                            'the ROOTFILE(S) (comma separated). ' \
                            '[default = %default]'
                       )
    parser.add_option( '-n', '--normalize',
                       action='store_true',
                       default=False,
                       help='Normalize the histogram to unit area. ' \
                            '[default = %default]'
                       )
    parser.add_option( '-s', '--square',
                       action='store_true',
                       default=False,
                       help='Make a square plot. [default = %default]'
                       )
    parser.add_option( '-S', '--sqrt_s',
                       metavar='SRQT(S)',
                       type='float',
                       default=None,
                       help='Set the value for sqrt(s) (in TeV) for these ' \
                            'plot. [default = %default]'
                       )
    parser.add_option( '-r', '--run-range',
                       metavar='RUNRANGE',
                       default=None,
                       help='Specify the used RUNRANGE for this plots in the ' \
                            'form firstRun-lastRun. [default = %default]'
                       )
    parser.add_option( '-m', '--mc',
                       metavar='MC',
                       default=None,
                       help="Specify the used MC production campaign for " \
                            "this plot. When used, '--sqrt_s' is ignored. " \
                            "[default = %default]"
                       )
    parser.add_option( '--no-stats',
                       action='store_true',
                       default=False,
                       help='Do not draw the statistics box. ' \
                            '[default = Draw it!].'
                       )
    parser.add_option( '-o', '--output',
                       metavar='FILENAME',
                       default=None,
                       help='Specify the output file name. ' \
                            '[default = %default]'
                       )

    ( options, args ) = parser.parse_args()

    if 0 > len( args ) > 3:
        parser.error( 'Exactly one or two input ROOTFILE(S) needed!' )

    if options.histo:
        options.histo = options.histo.split( ',' )

    del parser

    # Trick to not interfer with ROOT comand line parsing.
    sys.argv = []
    import ROOT as r
    global r

    return ( options, args )


if __name__ == '__main__':
    main()
