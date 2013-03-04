#!/usr/bin/env python

import code
import json
import logging
import math
import optparse
import os
import sys

from collections import defaultdict
log = logging.getLogger( 'makePlot' )

import ROOT as r

r.gROOT.SetBatch( False )


def main():
    ( options, Class ) = commandLineParsing()

    canvas = options.root_file.Get( Class )

    log.debug( 'Looking for EventClass: ' + Class + '.' )
    if canvas:
        log.debug( 'Found EventClass: ' + Class + '.' )
    else:
        log.error( 'Could not open EventClass: ' + Class + '.' )
        log.info( 'Exiting...' )
        sys.exit( 1 )

    num_pads = 0
    for thing in canvas.GetListOfPrimitives():
        if thing.ClassName() == 'TPad':
            num_pads += 1

    if num_pads == 0:
        canvas = oldPlot( options, Class, canvas )
    elif num_pads == 2:
        canvas = plot( options, Class, canvas )

    canvas.Draw()

    if options.square:
        # Plot the canvas square.
        canvas.SetWindowSize( 680, 680 )
        canvas.SetCanvasSize( 650, 650 )

        canvas.Update()

    canvas.SaveAs( Class + options.postfix + '.pdf'  )
    canvas.SaveAs( Class + options.postfix + '.eps'  )
    canvas.SaveAs( Class + options.postfix + '.svg'  )
    canvas.SaveAs( Class + options.postfix + '.root' )

    code.interact()


def plot( options, Class, canvas ):
    canvas.cd()
    canvas.Draw()
    for pad in canvas.GetListOfPrimitives():
        pad.cd()
        primitives = pad.GetListOfPrimitives()

        loopOverPrimitives( options, primitives )

        if 'ratio' in pad.GetName():
            # Update colors and fill style for errors.
            bg_error_ratio = primitives.FindObject( 'bg_error_ratio' )
            bg_error_ratio.SetFillStyle( options.fill_style )
            bg_error_ratio.SetFillColor( options.fill_color )

            if options.logy_ratio != 'auto':
                pad.SetLogy( bool( options.logy_ratio ) )

            axes = primitives.FindObject( Class + '_axis_ratio' )
            axes.GetYaxis().SetTitleOffset( 0.45 )

            if options.x_min != None and options.x_max != None:
                axes.GetXaxis().SetRangeUser( options.x_min, options.x_max )

            if options.y_min_ratio != None and options.y_max_ratio != None:
                axes.GetYaxis().SetRangeUser( options.y_min_ratio, options.y_max_ratio )
            pad.Update()

            updateLineLengths( options, primitives, ratio=True )
        else:
            # Update colors and fill style for errors.
            bg_error = primitives.FindObject( 'bg_error' )
            bg_error.SetFillStyle( options.fill_style )
            bg_error.SetFillColor( options.fill_color )

            bg_histo_CL = primitives.FindObject( 'bg_histo_CL' )
            bg_histo_CL.SetFillStyle( options.fill_style )
            bg_histo_CL.SetFillColor( options.fill_color )

            axes = primitives.FindObject( Class + '_axis' )

            # First need to scale the axes!
            if options.x_min != None and options.x_max != None:
                axes.GetXaxis().SetRangeUser( options.x_min, options.x_max )

            if options.y_min != None and options.y_max != None:
                axes.GetYaxis().SetRangeUser( options.y_min, options.y_max )
            pad.Update()

            correctErrorPlotting( primitives )

            updateLegend( options, primitives, ratio=True )

            tex = getWIP( ratio=True )
            tex.Draw()

            sqrt_s = getSqrtS( options.sqrt_s )
            sqrt_s.Draw()

            xy_lines = updateLineLengths( options, primitives )

            if options.roi and not options.no_roi:
                roi = getROI( options, pad, xy_lines )
                roi.Draw()

        canvas.cd()
        canvas.Update()

        pad.Update()

    canvas.cd()
    canvas.Update()

    # Return a clone because ROOT.
    return canvas.Clone( canvas.GetName() )


# For backward compatibility.
def oldPlot( options, Class, canvas ):
    if options.nology:
        canvas.SetLogy( False )

    if options.logx:
        canvas.SetLogx( True )

    canvas.Draw()

    # Update the canvas size
    #
    canvas.SetWindowSize( 1050, 750 )
    canvas.SetCanvasSize( 1000, 700 )

    canvas.Update()

    canvas.SetRightMargin( 0.05 )
    canvas.SetLeftMargin( 0.11 )

    canvas.Update()
    canvas.Draw()

    primitives = canvas.GetListOfPrimitives()
    stack_axis = primitives.FindObject( Class + '_axis' )

    if options.x_min != None and options.x_max != None:
        stack_axis.GetXaxis().SetLimits( options.x_min, options.x_max );
        stack_axis.GetXaxis().SetRangeUser( options.x_min, options.x_max )

    stack_axis.GetYaxis().SetTitleOffset( 1.15 )

    if options.y_min != None:
        stack_axis.SetMinimum( options.y_min )

    if options.y_max != None:
        stack_axis.SetMaximum( options.y_max )

    loopOverPrimitives( options, primitives )

    updateLegend( options, primitives )

    correctErrorPlotting( primitives )

    canvas.Update()

    xy_lines = updateLineLengths( options, primitives )

    if options.roi:
        roi = getROI( options, canvas, xy_lines )
        roi.Draw()

    if options.pseudo:
        pl = r.TPaveLabel( 0.33, 0.75, 0.55, 0.86, 'MC study', 'brNDC' )
        pl.SetFillColor( 0 )
        pl.Draw()

    tex = getWIP()
    tex.Draw()

    return canvas.Clone( canvas.GetName() )


def determineDist( dist ):
    dist = str( dist )
    isSumpt = False
    isMinv  = False
    isMet   = False

    if dist.isdigit():
        num = int( dist )
        if num == 0: isSumpt = True
        if num == 1: isMinv  = True
        if num == 2: isMet   = True

    return isSumpt, isMinv, isMet


def updatePseudoData( options, leg ):
    leglist = leg.GetListOfPrimitives()
    if options.pseudo:
        next = r.TIter( leglist )
        while True:
            obj = next()
            if not obj:
                break

            # Here ClassName is the C++ class name!
            #
            ClassName = obj.ClassName()
            log.debug( 'In updatePseudoData(...): ClassName = %s.' % ClassName )

            if ClassName == 'TLegendEntry':
                label = obj.GetLabel()
                if label ==  'Data':
                    obj.SetLabel( 'Pseudo Data' )


def updateLegend( options, primitives, ratio=False ):
    leg = primitives.FindObject( 'TPave' )

    if leg:
        if ratio:
            leg.SetX1NDC( 0.71 )
            leg.SetY1NDC( 0.32 )
            leg.SetX2NDC( 0.93 )
            leg.SetY2NDC( 0.91 )
        else:
            leg.SetX1NDC( 0.70 )
            leg.SetY1NDC( 0.55 )
            leg.SetX2NDC( 0.93 )
            leg.SetY2NDC( 0.91 )

        updatePseudoData( options, leg )


def loopOverPrimitives( options, primitives):
    next = r.TIter( primitives )
    while True:
        obj = next()
        if not obj:
            break

        # Here ClassName is the C++ class name!
        #
        ClassName = obj.ClassName()
        log.debug( 'In loopOverPrimitives(...): ClassName = %s.' % ClassName )

        if ClassName == 'TLatex':
            title = obj.GetTitle()
            if 'p_{data}' in title:
                obj.SetY( 0.955 )
                if options.no_pvalue:
                    obj.Delete()

            if '#tilde{p}' in title:
                obj.SetY( 0.955 )
                if title.endswith( '=0}' ):
                    obj.SetTitle( '#tilde{p} < 10^{-5}' )
                if '=1e-05' in title:
                    obj.SetTitle( '#tilde{p} = 10^{-5}' )
                if options.ptildeNA:
                    obj.SetTitle( '#tilde{p} = N/A' )

            if 'Class:' in title:
                obj.SetY( 0.955 )

            if '{#int}L' in title:
                lumi = title.split( '=' )[1]
                ( value, unit ) = lumi.split( '/' )
                value = float( value.strip() )
                unit  = unit.strip().strip( '{}' )
                if unit == 'pb' and not options.pb:
                    value /= 1000.0
                    unit = 'fb'
                Lint = '#scale[0.9]{#scale[0.6]{#int}L dt = '
                Lint += str( round( value, 1 ) )
                Lint += ' / '
                Lint += unit
                Lint += '}'

                obj.SetTitle( Lint )

        if options.vec_sumpt:
            if ClassName == 'TH1D':
                name = obj.GetName()

                if 'axis_ratio' in name:
                    title = obj.GetXaxis().GetTitle()
                    if '#Sigma' in title:
                        title = '#Sigma |#vec{p}_{T}| / GeV'

                        obj.GetXaxis().SetTitle( title )


def updateLineLengths( options, primitives, ratio=False ):
    # List to store the x and y values of all lines.
    # Just append them in the right ordering.
    #
    list_of_xy_values = []

    next = r.TIter( primitives )
    while True:

        obj = next()
        if not obj:
            break

        # Here ClassName is the C++ class name!
        #
        ClassName = obj.ClassName()
        log.debug( 'In updateLineLengths(...): ClassName = %s.' % ClassName )

        if ClassName == 'TLine':
            if ratio:
                if options.y_min_ratio != None and options.y_max_ratio != None:
                    obj.SetY1( options.y_min_ratio )
                    obj.SetY2( options.y_max_ratio )
            else:
                if options.y_min != None and options.y_max != None:
                    obj.SetY1( options.y_min )
                    obj.SetY2( options.y_max )

            list_of_xy_values += [ obj.GetX1(), obj.GetY1(), obj.GetX2(), obj.GetY2() ]

            if options.no_roi:
                obj.Delete()

    return list_of_xy_values


def correctErrorPlotting( primitives ):
    errorhist = primitives.FindObject( 'bg_error' )

    for i in range( 1, errorhist.GetNbinsX() + 1 ):
        e_x = errorhist.GetBinContent( i )
        e_e = errorhist.GetBinError( i )

        if e_x > 0.0 and e_e > e_x:
            errorhist.SetBinContent( i, ( e_x + e_e ) / 2.0 )
            errorhist.SetBinError( i, ( e_x + e_e ) / 2.0 )


def commandLineParsing():
    usage = '%prog [options] CLASSNAME'

    plot_config_default = './plot.json'
    distribution_choices = [ '0', '1', '2' ]
    log_ratio_choices = [ 'True', 'False', 'auto' ]
    log_ratio_choices_parser = [ 'True', 'true', '1', 'False', 'false', '0', 'auto' ]
    parser = optparse.OptionParser( usage = usage, version = '%prog version 2' )
    parser.add_option( '-f', '--root-file', metavar = 'ROOTFILE', default = './RoI.root',
                       help = 'Set the root file containing scanned distributions. [default = %default]' )
    parser.add_option( '-d', '--distribution', metavar = 'DISTRIBUTION', type = 'choice', choices = distribution_choices,
                       help = 'Set distribution (0: Sumpt, 1: Minv, 2: MET).' )
    parser.add_option( '-a', '--auto', action = 'store_true', default = False,
                       help = "If this option is set, the script tries to automatically determine the distribution. '--distribution' is ignored in this case.  [default = %default]" )
    parser.add_option( '-p', '--pseudo', action = 'store_true', default = False,
                       help = 'Processing pseudo data. [default = %default]' )
    parser.add_option(       '--debug', metavar = 'LEVEL', default = 'INFO',
                       help= 'Set the debug level. Allowed values: ERROR, WARNING, INFO, DEBUG. [default = %default]' )
    parser.add_option( '-c', '--coordinates', metavar = 'COORD',
                       help = "Set the printing coordinates for the canvas in the form: <x_min>--<x_max>x<y_min>--<y_max>x<y_min_ratio>--<y_max_ratio>. This allows negative values and 'e-n' notation. The coordinates are written to CONFIG." )
    parser.add_option( '-C', '--cfg', metavar = 'CONFIG', default = None,
                       help = 'Set the file with the plotting information. [default = %s]' % plot_config_default )
    parser.add_option( '-P', '--no-pvalue', action = 'store_true', default = False,
                       help = 'Set not to show p_data in the plots. [default = %default]' )
    parser.add_option(       '--postfix', default = '', metavar = 'POSTFIX',
                       help = "Set a postfix for the filenames where the plots will be saved. [default = '%default']" )
    parser.add_option(       '--logx', action = 'store_true', default = False,
                       help = "Set to draw the x-axis logarithmically. [default = %default]" )
    parser.add_option(       '--nology', action = 'store_true', default = False,
                       help = "Set NOT to draw the y-axis logarithmically. [default = %default]" )
    parser.add_option(       '--logy-ratio', metavar = 'LOGYRATIO', type = 'choice', choices = log_ratio_choices_parser, default = 'auto',
                       help = 'Set drawing option for y-Axis of the ratio plot (choices: ' + ', '.join( log_ratio_choices ) + '). [default = %default]' )
    parser.add_option( '-r', '--roi', action = 'store_true', default = False,
                       help = "Plot a label in the middle of the 'Region of Interest'. [default = %default]" )
    parser.add_option(       '--pb', action = 'store_true', default = False,
                       help = "Do not convert Lumi from pb^-1 to fb^-1. [default = %default]" )
    parser.add_option(       '--ptildeNA', action = 'store_true', default = False,
                       help = "Set ptilde to 'N/A'. [default = %default]" )
    parser.add_option(       '--fill-style', metavar = 'FILLSTYLE', type = 'int', default = 3244,
                       help = 'Set the FILLSTYLE for the error histograms. [default = %default]' )
    parser.add_option(       '--fill-color', metavar = 'FILLCOLOR', default = 'kGray+1',
                       help = 'Set the FILLCOLOR for the error histograms. [default = %default]' )
    parser.add_option( '-s', '--square', action = 'store_true', default = False,
                       help = 'Plot the canvas (more or less) square. [default = %default]' )
    parser.add_option( '-S', '--sqrt_s', metavar = 'SRQT(S)', type = 'float', default = 7.0,
                       help = 'Set the value for sqrt(s) for these plots. [default = %default TeV]' )
    parser.add_option(       '--no-roi', action = 'store_true', default = False,
                       help = "Do not draw the 'Region of Interest'. [default = %default]" )
    parser.add_option( '-V', '--vec-sumpt', action = 'store_true', default = False,
                       help = "Replace 'Sum p_t' by 'Sum |vec(p_t)|'. [default = %default]" )

    ( options, args ) = parser.parse_args()

    format = '%(levelname)s (%(name)s) [%(asctime)s]: %(message)s'
    date = '%F %H:%M:%S'
    logging.basicConfig( level = logging._levelNames[ options.debug ], format = format, datefmt = date )

    if len( args ) != 1:
        parser.error( 'You must provide exactly one CLASSNAME.' )

    # CLASSNAME can have the form Rec_<something> oder Rec_<something_else>-<number>
    # where <number> is the distribution (0, 1, 2)
    try:
        ( class_name, dist ) = args[0].split( '-' )
    except ValueError:
        class_name = args[0]
        dist = None

    if os.path.exists( options.root_file ):
        options.root_file = r.TFile.Open( options.root_file )
    else:
        parser.error( "Rootfile '%s' not found. Please provide an exiting rootfile!" % options.root_file )

    if dist:
        isSumpt, isMinv, isMet = determineDist( dist )

    elif options.auto:
        # To determine the distribution automatically, get the list of all classes
        # and look at the first entry.
        name = options.root_file.GetListOfKeys().First().GetName()

        dist = name.split( '-' )[-1].strip()

        isSumpt, isMinv, isMet = determineDist( dist )

    elif options.distribution:
        isSumpt, isMinv, isMet = determineDist( options.distribution )

    else:
        parser.error( "Please set DISTRIBUTION or use '--auto'." )

    all_distr = [ d == True for d in [ isSumpt, isMinv, isMet ] ]

    if all( all_distr ) or not any( all_distr ):
        parser.error( 'Failed to determine distribution: isSumpt = %s, isMinv = %s, isMet = %s: ' %( isSumpt, isMinv, isMet ) )

    Class = class_name
    if isSumpt:
        Class += '-0'
    if isMinv:
        Class += '-1'
    if isMet:
        Class += '-2'

    if options.cfg:
        if os.path.exists( options.cfg ):
            log.info( "Found plot config file: '%s'" % options.cfg )
        else:
            log.warning( "Plot config file not found: '%s'" % options.cfg )
    else:
        log.debug( "Using default plot config file: '%s'" % options.cfg )
        options.cfg = plot_config_default

    coords = defaultdict( dict )
    if os.path.exists( options.cfg ):
        coords = json.load( open( options.cfg, 'r' ) )

    x_min = None
    x_max = None
    y_min = None
    y_max = None

    y_min_ratio = None
    y_max_ratio = None

    if options.coordinates:
        if options.coordinates.count( 'x' ) == 1:
            ( X, Y ) = options.coordinates.split( 'x' )
            ( x_min, x_max ) = X.split( '--' )
            ( y_min, y_max ) = Y.split( '--' )

        elif options.coordinates.count( 'x' ) == 2:
            ( X, Y, Y_ratio ) = options.coordinates.split( 'x' )
            ( x_min, x_max ) = X.split( '--' )
            ( y_min, y_max ) = Y.split( '--' )
            ( y_min_ratio, y_max_ratio ) = Y_ratio.split( '--' )
        else:
            parser.error( "Invalid coordinates: '%s'" % options.coordinates )

        coords[ Class ] = dict()
        coords[ Class ][ 'x_min' ] = float( x_min )
        coords[ Class ][ 'x_max' ] = float( x_max )
        coords[ Class ][ 'y_min' ] = float( y_min )
        coords[ Class ][ 'y_max' ] = float( y_max )

        if y_min_ratio:
            coords[ Class ][ 'y_min_ratio' ] = float( y_min_ratio )
        if y_max_ratio:
            coords[ Class ][ 'y_max_ratio' ] = float( y_max_ratio )

        json.dump( coords, open( options.cfg , 'w' ), sort_keys = True, indent = 4 )

    try:
        x_min = coords[ Class ][ 'x_min' ]
        x_max = coords[ Class ][ 'x_max' ]
        y_min = coords[ Class ][ 'y_min' ]
        y_max = coords[ Class ][ 'y_max' ]
    except KeyError:
        log.info( "No plot config found for class: '%s'. Using defaults." % Class )

    try:
        y_min_ratio = coords[ Class ][ 'y_min_ratio' ]
        y_max_ratio = coords[ Class ][ 'y_max_ratio' ]
    except KeyError:
        log.info( "No plot config found for ratio plot for class: '%s'. Using defaults." % Class )

    options.x_min = x_min
    options.x_max = x_max
    options.y_min = y_min
    options.y_max = y_max
    options.y_min_ratio = y_min_ratio
    options.y_max_ratio = y_max_ratio

    if options.logy_ratio != 'auto':
        if options.logy_ratio in [ 'True', 'true', '1' ]:
            options.logy_ratio = True
        elif options.logy_ratio in [ 'False', 'false', '0' ]:
            options.logy_ratio = False

    if options.fill_color.isdigit():
        options.fill_color = int( options.fill_color )
    else:
        split = options.fill_color.split( '+' )
        col = split[0].strip()
        del split[0]
        if not col.isdigit():
            col = getattr( r, col )

        for i in split:
            i.strip()
            if i.isdigit():
                col += int( i )

        options.fill_color = int( col )

    return ( options, Class )


def getWIP( ratio=False ):
    if ratio:
        tex = r.TLatex( 0.15, 0.86, 'CMS work in progress' )
    else:
        tex = r.TLatex( 0.12, 0.88, 'CMS work in progress' )
    tex.SetNDC()
    tex.SetLineWidth( 2 )

    return tex


def getSqrtS( sqrt_s=7 ):
    text = '#sqrt{s} = %.0f TeV' % sqrt_s
    tex = r.TLatex( 0.5, 0.86, text )
    tex.SetNDC()
    tex.SetLineWidth( 2 )

    return tex


def getROI( options, pad, xy_lines ):
    xy_line1 = convertToNDC( pad, xy_lines[0], xy_lines[1], xy_lines[2], xy_lines[3] )
    xy_line2 = convertToNDC( pad, xy_lines[4], xy_lines[5], xy_lines[6], xy_lines[7] )

    x_mean = xy_line2[0] - xy_line1[0]
    x_mean /= 2.0
    x_mean += xy_line1[0]

    roi = r.TPaveLabel( x_mean - 0.04 ,0.75, x_mean + 0.04, 0.80, 'RoI', 'brNDC' )
    roi.SetFillColor( r.kWhite )
    roi.SetLineColor( r.kBlue )
    roi.SetTextColor( r.kBlue )
    roi.SetTextFont( 102 )
    roi.SetTextSize( 1 )

    return roi


# Conversion according to:
# http://root.cern.ch/root/html/TPad.html#TPad:ResizePad
#
# ... THANK YOU ROOT *arghh*
#
def convertToNDC( pad, x1, y1 = 0.0, x2 = 0.0, y2 = 0.0 ):
    isLogx = pad.GetLogx()
    isLogy = pad.GetLogy()

    if isLogx:
        xmin = math.pow( 10, pad.GetX1() )
        xmax = math.pow( 10, pad.GetX2() )

        x1NDC = math.log( ( x1 / xmin ), 10 ) / math.log( ( xmax / xmin ), 10 )
        x2NDC = math.log( ( x2 / xmin ), 10 ) / math.log( ( xmax / xmin ), 10 )

    else:
        xmin = pad.GetX1()
        xmax = pad.GetX2()

        x1NDC = ( x1 - xmin ) / ( xmax - xmin )
        x2NDC = ( x2 - xmin ) / ( xmax - xmin )

    if isLogy:
        ymin = math.pow( 10, pad.GetY1() )
        ymax = math.pow( 10, pad.GetY2() )

        y1NDC = math.log( ( y1 / ymin ), 10 ) / math.log( ( ymax / ymin ), 10 )
        y2NDC = math.log( ( y2 / ymin ), 10 ) / math.log( ( ymax / ymin ), 10 )
    else:
        ymin = pad.GetY1()
        ymax = pad.GetY2()

        y1NDC = ( y1 - ymin ) / ( ymax - ymin )
        y2NDC = ( y2 - ymin ) / ( ymax - ymin )

    return x1NDC, y1NDC, x2NDC, y2NDC


if __name__ == '__main__':
    main()
