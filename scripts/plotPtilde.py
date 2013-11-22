#!/usr/bin/env python

import array
import code
import cPickle
import math
import optparse
import os
import sys


def main():
    parser = optparse.OptionParser()

    distribution_choices = [ '0', '1', '2' ]
    parser.add_option( '-d', '--dist', metavar='DISTRIBUTION', type='choice', choices=distribution_choices, default=distribution_choices[0],
                       help='Set distribution (0: Sumpt, 1: Minv, 2: MET). [default = %default]' )
    parser.add_option( '-r', '--rootfile', metavar='ROOTFILENAME', default='p-tilde-new.root',
                       help='Set the name of the output rootfile. [default = %default]' )
    parser.add_option( '-p', '--pdf', metavar='PDFFILENAME', default=None,
                       help='Set the name of the output pdffile. [default = p-tilde-I.pdf, where I is the distribution.]' )
    parser.add_option( '-n', '--num-bins', metavar='NUMBINS', type='int', default=10,
                       help='Set number of bins for p-tilde histogram. [default = %default]' )
    parser.add_option( '-i', '--input', metavar='INPUTPICKLE', default='./p-tilde.pkl',
                       help='Set the input python-pickle file. [default = %default]' )
    parser.add_option( '-R', '--results', metavar='RESULTFILE', default='./results.pkl',
                       help='Set the actual results-pickle file or p-tilde rootfile (only used if INPUTPICKLE is not found). [default = %default]' )
    parser.add_option( '-s', '--square', action = 'store_true', default=False,
                       help='Set to make a square p-tilde plot. [default = %default]' )
    parser.add_option( '-V', '--vec-sumpt', action = 'store_true', default = False,
                       help = "Replace 'Sum p_t' by 'Sum |vec(p_t)|'. [default = %default]" )
    parser.add_option( '-g', '--grid', action='store_true', default=False,
                       help='Draw grid lines along the x and y-axis. [default = %default]' )
    parser.add_option(       '--no-interact', action = 'store_true', default = False,
                       help = "Do not use the interactive mode. [default = %default]" )

    ( options, args ) = parser.parse_args()
    del parser

    sys.argv = []
    import ROOT
    global ROOT

    ROOT.gROOT.SetBatch( options.no_interact )

    ROOT.gStyle.SetOptStat(0)

    if options.dist.isdigit():
        options.dist_plain = options.dist
        if int( options.dist ) == 0:
            if options.vec_sumpt:
                options.dist = '#scale[1.3]{#Sigma} |#vec{p}_{T}|'
            else:
                options.dist = '#scale[1.3]{#Sigma} p_{T}'
        elif int( options.dist ) == 1:
            options.dist = 'M_{inv}^{(T)}'
        elif int( options.dist ) == 2:
            options.dist = 'MET'

    if options.results.endswith( '.root' ):
        rootfile = ROOT.TFile.Open( options.results )
        c_p_tilde = rootfile.Get( 'c_p-tilde' )
        rootfile.Close()

        if options.grid:
            ROOT.gStyle.SetPadGridX( ROOT.kTRUE )
            ROOT.gStyle.SetPadGridY( ROOT.kTRUE )

        lines = []
        for prim in c_p_tilde.GetListOfPrimitives():
            if prim.ClassName() == 'TLegend':
                legend = prim
                legend.SetX1NDC( 0.76 )
                legend.SetY1NDC( 0.74 )
                legend.SetX2NDC( 1.0 )
                legend.SetY2NDC( 1.0 )
                legend.SetTextSize( 0.04 )
                for legEntry in legend.GetListOfPrimitives():
                    if legEntry.GetLabel() == 'MC':
                        legEntry.SetLabel( 'MC vs. MC' )
                    if legEntry.GetLabel() == 'Data':
                        legEntry.SetLabel( 'Data vs. MC' )
            if prim.ClassName() == 'TLine':
                lines.append( prim )

        p_tilde_stack = c_p_tilde.GetPrimitive( 'p_tilde_stack' )
        p_tilde_stack.GetXaxis().SetTitleOffset( 0.95 )

        hists = p_tilde_stack.GetHists()

        p_tilde          = hists.FindObject( 'p_tilde' )
        bg_p_tilde       = hists.FindObject( 'bg_p_tilde' )
        p_tilde_overflow = hists.FindObject( 'p_tilde_overflow' )

        p_tilde.SetLineWidth( 2 )

        if p_tilde_overflow:
            p_tilde_overflow.SetLineWidth( 2 )

        bg_p_tilde.SetFillColor( ROOT.kCyan - 10 )

        max_value = p_tilde.GetXaxis().GetXmax()

        if options.square:
            canvas = ROOT.TCanvas( 'c_p-tilde', 'canvas for p_tilde', 700, 650 )
        else:
            canvas = ROOT.TCanvas( 'c_p-tilde', 'canvas for p_tilde', 1000, 750 )

        canvas.SetLogy( True )
        canvas.cd()

        canvas.SetTopMargin( 0.06 )
        canvas.SetBottomMargin( 0.12 )
        canvas.SetLeftMargin( 0.1 )
        canvas.SetRightMargin( 0.03 )

        p_tilde_stack.Draw( 'axis nostack' )
        bg_p_tilde.Draw( 'hist same' )
        p_tilde.Draw( 'e same' )

        if p_tilde_overflow:
            p_tilde_overflow.Draw( 'e1 same' )
        else:
            legend.SetX1NDC( 0.79 )
            legend.SetY1NDC( 0.80 )

        if options.grid:
            p_tilde.Draw( 'axis axig same' )
        else:
            p_tilde.Draw( 'axis same' )

        y_min = lines[0].GetY1()
        y_max = lines[0].GetY2()

        line = ROOT.TLine()
        line.SetLineColor(4)
        line.SetLineWidth(3)
        line.SetLineStyle(2)

        text = ROOT.TLatex()
        text.SetTextColor(4)
        for sigma in xrange( 1, 38 ):
            prob = ROOT.TMath.Prob( sigma**2, 1 )
            logprob = -math.log10( prob )
            if logprob > max_value:
                break
            line.DrawLine( logprob, y_min, logprob, y_max )
            text.DrawLatex( logprob - 0.15, 1.3 * y_max, '%s#sigma' % sigma )

        legend.Draw()

        label = ROOT.TPaveLabel( 0.17, 0.18, 0.37, 0.32, options.dist, 'brNDC' )
        label.SetFillColor( 0 )
        label.SetTextSize( 0.5 )
        label.Draw()

        canvas.Update()
        canvas.RedrawAxis()
        canvas.Update()

        if not options.no_interact:
            code.interact()

        if options.pdf:
            canvas.SaveAs( options.pdf + '.pdf' )
        else:
            canvas.SaveAs( 'p-tilde-' + options.dist_plain + '.pdf' )

    else:
        all_p_tilde    = []  # p-tilde for data or pseudo-data
        all_p_tilde_bg = []  # p-tilde for background

        if os.path.exists( options.input ):
            print 'Found existing, opening...'
            pickle_data = cPickle.load( open( options.input ) )
            print '...done.'

            all_p_tilde    = pickle_data[ 'p_tilde' ]
            all_p_tilde_bg = pickle_data[ 'p_tilde_bg' ]
            config         = pickle_data[ 'config' ]

        else:
            print 'Loading pickle...'
            results = cPickle.load( open( options.results ) )
            print '...done.'

            scanned = results[ 'scanned' ]
            config  = results[ 'config' ]

            for result in scanned:
                all_p_tilde.append( result.p_tilde )
                if result.bg_scanned:
                    num_dice = len( result.bg_scanned )
                    for bg in result.bg_scanned:
                        all_p_tilde_bg.append( [ bg.p, bg.p_tilde, num_dice ] )

            # As the actual results.pkl can grow very large (O(GB)), which can make the
            # access very very slow, we store only the p-tilde values in order to access
            # them later, if we wish to.
            all_results = dict()
            all_results[ 'p_tilde' ]    = all_p_tilde
            all_results[ 'p_tilde_bg' ] = all_p_tilde_bg
            all_results[ 'config' ]     = config
            cPickle.dump( all_results, open( 'p-tilde.pkl', 'w' ) )

        p_tilde_hist, bg_p_tilde_hist, bg_p_tilde_hist_weight = fillHistograms( options, config, all_p_tilde, all_p_tilde_bg )

        printHistos( options, p_tilde_hist, bg_p_tilde_hist, bg_p_tilde_hist_weight )


def printHistos( options, p_tilde_hist, bg_p_tilde_hist, bg_p_tilde_hist_weight ):
    ROOT.gStyle.SetOptStat( 'neo' )

    num_bins  = p_tilde_hist.GetNbinsX()
    max_value = p_tilde_hist.GetXaxis().GetXmax()

    # Normalize the bg histogram.
    integral_bg   = bg_p_tilde_hist.Integral()
    integral_data = p_tilde_hist.Integral()
    if integral_bg != 0 and integral_data != 0:
        bg_p_tilde_hist.Scale( integral_data / integral_bg )

    integral_bg_weight = bg_p_tilde_hist_weight.Integral()
    if integral_bg_weight != 0 and integral_data != 0:
        bg_p_tilde_hist_weight.Scale( integral_data / integral_bg_weight )

    # Do we have overflow?
    overflow = False
    if p_tilde_hist.GetBinContent( p_tilde_hist.GetNbinsX() + 1 ) > 0:
        overflow = True
        num_bins_overflow = p_tilde_hist.GetNbinsX() + 1

        all_bins = []
        # In case we have dynamic binning.
        for bin in xrange( num_bins_overflow ):
            # Don't count the underflow bin!
            all_bins.append( p_tilde_hist.GetBinLowEdge( bin + 1 ) )
        # Get the bin width of the overflow bin.
        last_bin = all_bins[-1] + p_tilde_hist.GetBinWidth( num_bins_overflow )
        all_bins.append( last_bin )

        bin_array = array.array( 'd', all_bins )
        p_tilde_hist_base = ROOT.TH1D( p_tilde_hist.GetName(),
                                       p_tilde_hist.GetTitle(),
                                       num_bins_overflow,
                                       bin_array
                                       )
        p_tilde_hist_base.SetStats( 0 )
        p_tilde_hist_base.SetLineWidth( 2 )

        p_tilde_overflow_hist = p_tilde_hist_base.Clone( p_tilde_hist_base.GetName() + '_overflow' )
        p_tilde_overflow_hist.SetStats( 0 )
        p_tilde_overflow_hist.SetMarkerColor( ROOT.kRed )
        p_tilde_overflow_hist.SetLineColor( ROOT.kRed )

        bg_p_tilde_hist_overflow = ROOT.TH1D( bg_p_tilde_hist.GetName(),
                                              bg_p_tilde_hist.GetTitle(),
                                              num_bins_overflow,
                                              bin_array
                                              )
        bg_p_tilde_hist_overflow.SetStats( 0 )
        bg_p_tilde_hist_overflow.SetFillColor( bg_p_tilde_hist.GetFillColor() )

        # Loop over all bins *except* the overflow bin!
        for bin in xrange( num_bins_overflow ):
            p_tilde_hist_base.SetBinContent( bin, p_tilde_hist.GetBinContent( bin ) )
            bg_p_tilde_hist_overflow.SetBinContent( bin, bg_p_tilde_hist.GetBinContent( bin ) )

        # And now the overflow bin!
        p_tilde_overflow_hist.SetBinContent( num_bins_overflow, p_tilde_hist.GetBinContent( num_bins_overflow ) )

        # Normalize the background histogram.
        if bg_p_tilde_hist_overflow.Integral( 1, num_bins_overflow ) != 0 \
            and p_tilde_hist.Integral( 1, num_bins_overflow ) != 0:
                bg_p_tilde_hist_overflow.Scale( p_tilde_hist.Integral( 1, num_bins_overflow ) /
                                                bg_p_tilde_hist_overflow.Integral( 1, num_bins_overflow ) )

        max_value = p_tilde_hist_base.GetXaxis().GetXmax()

    # Draw and store in .ps
    if options.square:
        canvas = ROOT.TCanvas( 'c_p-tilde', 'canvas for p_tilde', 700, 650 )
    else:
        canvas = ROOT.TCanvas( 'c_p-tilde', 'canvas for p_tilde', 1128, 780 )

    canvas.SetLogy( True )
    canvas.cd()

    legend = ROOT.TLegend( 0.54, 0.73, 0.79, 0.92 )
    legend.SetFillColor( 0 )

    p_tilde_stack = ROOT.THStack( 'p_tilde_stack', '' )
    if overflow:
        # To get the axes right.
        p_tilde_stack.Add( bg_p_tilde_hist_overflow )
        p_tilde_stack.Add( p_tilde_hist_base )
        p_tilde_stack.Add( p_tilde_overflow_hist )

        legend.AddEntry( bg_p_tilde_hist_overflow, 'MC', 'f' )
        legend.AddEntry( p_tilde_hist_base, 'Data', 'lpe' )
        legend.AddEntry( p_tilde_overflow_hist, 'Data overflow', 'lpe' )

        p_tilde_stack.Draw( 'axis nostack' )
        p_tilde_stack.GetXaxis().SetTitle( '-log_{10}(#tilde{p})' )
        p_tilde_stack.GetYaxis().SetTitle( '# distributions' )
        p_tilde_stack.GetYaxis().SetTitleOffset( 1.3 )

        bg_p_tilde_hist_overflow.Draw( 'hist same' )
        p_tilde_hist_base.Draw( 'e same' )

        # Redraw axes.
        p_tilde_stack.Draw( 'axis nostack same' )
        p_tilde_overflow_hist.Draw( 'e1 same' )
    else:
        p_tilde_stack = ROOT.THStack( 'p_tilde_stack', '' )
        p_tilde_stack.Add( bg_p_tilde_hist )
        p_tilde_stack.Add( p_tilde_hist )

        legend.AddEntry( bg_p_tilde_hist, 'MC', 'f' )
        legend.AddEntry( p_tilde_hist, 'Data', 'lpe' )

        p_tilde_stack.Draw( 'nostack' )
        p_tilde_stack.GetXaxis().SetTitle( '-log_{10}(#tilde{p})' )
        p_tilde_stack.GetYaxis().SetTitle( '# distributions' )
        p_tilde_stack.GetYaxis().SetTitleOffset( 1.3 )
        p_tilde_stack.Draw( 'nostack' )

    y_min = p_tilde_stack.GetHistogram().GetMinimum()
    y_max = p_tilde_stack.GetHistogram().GetMaximum()

    #draw lines for sigmas
    line = ROOT.TLine()
    line.SetLineColor(4)
    line.SetLineWidth(3)
    line.SetLineStyle(2)
    text = ROOT.TLatex()
    text.SetTextColor(4)
    for sigma in xrange( 1, 38 ):
        prob = ROOT.TMath.Prob( sigma**2, 1 )
        logprob = -math.log10( prob )
        if logprob > max_value:
            break
        line.DrawLine( logprob, y_min, logprob, y_max )
        text.DrawLatex( logprob - 0.15, 1.1 * y_max, '%s#sigma' % sigma )

    label = ROOT.TPaveLabel( 0.17, 0.18, 0.37, 0.32, options.dist, 'brNDC' )
    label.SetFillColor( 0 )
    label.SetTextSize( 0.5 )
    label.Draw()

    canvas.Update()

    # Draw legend last, to overlap everything else.
    legend.Draw()

    canvas.Update()

    if options.pdf:
        canvas.SaveAs( options.pdf + '.pdf' )
    else:
        canvas.SaveAs( 'p-tilde-' + options.dist_plain + '.pdf' )

    c2 = ROOT.TCanvas( 'p-tilde-weighted', 'weight' )
    c2.cd()
    bg_p_tilde_hist_weight.Draw()
    p_tilde_hist.Draw( 'same e1' )
    c2.Update()

    # write to file
    file = ROOT.TFile( options.rootfile, 'recreate' )
    file.cd()
    canvas.Write()
    file.Close()

    code.interact()


def fillHistograms( options, config, all_p_tilde, all_p_tilde_bg ):
    num_bins = options.num_bins
    max_dice = config.bg_num_dice

    # Calculate the upper limit on log10( p-tilde ).
    if max_dice:
        max_value = 1.1 * math.ceil( math.log10( config.bg_num_dice ) )
    else:
        max_value = 1

    p_tilde_hist = ROOT.TH1D( 'p_tilde',
                              'Distribution of -log_{10}(#tilde{p}) for data',
                              num_bins,
                              0,
                              max_value
                              )
    p_tilde_hist.SetLineWidth( 2 )
    p_tilde_hist.Sumw2()

    bg_p_tilde_hist = ROOT.TH1D( 'bg_p_tilde',
                                 'Distribution of -log_{10}(#tilde{p}) for background',
                                 num_bins,
                                 0,
                                 max_value
                                 )
    #bg_p_tilde_hist.SetLineWidth( 2 )
    bg_p_tilde_hist.SetFillColor( ROOT.kCyan - 10 )
    bg_p_tilde_hist.SetFillStyle( 3003 )

    bg_p_tilde_hist_weight = ROOT.TH1D( 'bg_p_tilde_weighted', '', num_bins*100, 0, max_value )
    bg_p_tilde_hist_weight.Sumw2()

    for p_tilde in all_p_tilde:
        if p_tilde > 0:
            p_tilde_hist.Fill( -math.log10( p_tilde ) )
        else:
            # We got a badass overhere. Put it into the overflow bin.
            p_tilde_hist.AddBinContent( p_tilde_hist.GetNbinsX() + 1 )

    for bg_p, bg_p_tilde, num_dice in all_p_tilde_bg:
        if bg_p < config.p_threshold and bg_p_tilde > 0:
            log_p_tilde = -math.log10( bg_p_tilde )
            bg_p_tilde_hist.Fill( log_p_tilde )

            if max_dice:
                weight = 1.0 / float( num_dice )
                bg_p_tilde_hist_weight.Fill( log_p_tilde, weight )

    return p_tilde_hist, bg_p_tilde_hist, bg_p_tilde_hist_weight


if __name__ == '__main__':
    main()
