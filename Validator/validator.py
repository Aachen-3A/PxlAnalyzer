#!/bin/env python

import optparse, time, os, subprocess
import resource
from datetime import datetime
import numpy as np
import logging
import multiprocessing

from ROOT import TCanvas, TGraph, TF1, TLegend, kBlue, gStyle, gPad, TPad

log = logging.getLogger( 'Validator' )

def opt_parser():
    date_time = datetime.now()
    usage = '%prog [options] CONFIG_FILE'
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( '-u', '--user', default = os.getenv( 'LOGNAME' ),
                            help = 'which user on dcache [default = %s]'%(os.getenv( 'LOGNAME' )))
    parser.add_option( '-o', '--Output', default = '%s'%(os.getlogin())+"/TAG", metavar = 'DIRECTORY',
                            help = 'Define the output directory. [default = %default]')
    parser.add_option( '--debug', metavar = 'LEVEL', default = 'INFO',
                       help= 'Set the debug level. Allowed values: ERROR, WARNING, INFO, DEBUG. [default = %default]' )

    ( options, args ) = parser.parse_args()
    if len( args ) != 0:
        parser.error( 'Exactly zero CONFIG_FILE required!' )

    format = '%(levelname)s from %(name)s at %(asctime)s: %(message)s'
    date = '%F %H:%M:%S'
    logging.basicConfig( level = logging._levelNames[ options.debug ], format = format, datefmt = date )

    return args,options

def final_user_decision():
    raw_input("waiting for the final user decision")

def comparison_norm():
    print("comparing the normalization of distributions")

def comparison_shape():
    print("comparing the shape of distributions")

def comparison_events():
    print("comparing events")

def do_comparison():
    print("doing the comparison")

    comparison_norm()

    comparison_shape()

    comparison_events()

def get_reference_output():
    print("getting the reference output")

def get_analysis_output():
    print("getting the analysis output")

def run_analysis():
    print("running the analysis")
    
    music_prog = "music"
    music_opt  = "--SpecialAna"
    music_cfg  = "/home/home1/institut_3a/erdweg/Desktop/Software/MUSiC/ConfigFiles/MC.cfg"
    music_path = "/disk1/erdweg/validation/val_files/"
    
    files = ["DrellYan/DYJetsToLL_madgraph_102_1_uNZ.pxlio",
             "DrellYan/DYJetsToLL_madgraph_219_1_ZuZ.pxlio",
             "DrellYan/DYJetsToLL_madgraph_280_1_yeF.pxlio",
             "DrellYan/DYJetsToLL_madgraph_35_1_y0a.pxlio",
             "DrellYan/DYJetsToLL_madgraph_55_1_wyr.pxlio",
             "DrellYan/DYJetsToLL_madgraph_142_1_ylE.pxlio",
             "DrellYan/DYJetsToLL_madgraph_224_1_Y5b.pxlio",
             "DrellYan/DYJetsToLL_madgraph_307_1_kSG.pxlio",
             "DrellYan/DYJetsToLL_madgraph_48_1_5oK.pxlio",
             "DrellYan/DYJetsToLL_madgraph_94_1_Cqr.pxlio"
    ]
    
    item_list = []
    for item in files:
        item_list.append([music_prog,"-o %s"%(item[item.find("/")+1:-6]),music_opt,music_cfg,music_path+item])

    pool = multiprocessing.Pool()
    pool.map_async(run_analysis_task, item_list)
    while True:
        time.sleep(1)
        if not pool._cache: break
    pool.close()
    pool.join()

    #for item in item_list:
        #run_analysis_task(item)

    #run_analysis_task(item_list[0])

def run_analysis_task(item):
    usage_start = resource.getrusage(resource.RUSAGE_CHILDREN)
    rssList = []
    virtual = []
    other = []
    cmd = [item[0], item[1], item[2], item[3], item[4]]
    #print(" ".join(cmd))
    p = subprocess.Popen(" ".join(cmd), shell=True,stdout=subprocess.PIPE)
    pid = p.pid
    while True:
        if p.poll() != None:
            break
        p2 = subprocess.Popen("top -b -n 1 -p %s | grep %s"%(pid,pid),shell=True,stdout=subprocess.PIPE)
        output = p2.communicate()[0]
        #print("top -b -n 1 -p %s"%pid,"  -  ",output)
        if output != '':
            if "m" in output.split()[5]:
                rssList.append(output.split()[5].split("m")[0]) 
            else:
                rssList.append(output.split()[5]) 
            if "m" in output.split()[4]:
                virtual.append(output.split()[4].split("m")[0]) 
            else:
                virtual.append(output.split()[4])
        time.sleep(1)

    output = p.communicate()[0]
    exitCode = p.returncode
    print(output,exitCode)
    usage_end = resource.getrusage(resource.RUSAGE_CHILDREN)
    cpu_time = usage_end.ru_utime - usage_start.ru_utime

    rssArray = np.array(rssList,"d")
    virtualArray = np.array(virtual,"d")
    xAxis = []
    for i in range(0,len(rssArray)):
        xAxis.append(i*(cpu_time/len(rssArray)))

    xAxisArray = np.array(xAxis,"d")
    graphRSS = TGraph(len(rssArray),xAxisArray,rssArray)
    graphVirtual = TGraph(len(virtualArray),xAxisArray,virtualArray)

    graphRSS.SetMarkerColor(kBlue)
    graphRSS.SetMarkerStyle(21)

    graphRSS.SaveAs(item[1][3:]+"_rss.root")
    graphVirtual.SaveAs(item[1][3:]+"_vir.root")

    #c1 = TCanvas("c1","c1",800,800)

    #legend = TLegend(0.65, 0.65, 0.98, 0.90)
    #legend.SetFillStyle(0)
    #legend.SetBorderSize(0)

    #legend.AddEntry(graphRSS,"resident memory consumption","p")
    #legend.AddEntry(graphVirtual,"virtual memory consumption","p")

    #c1.DrawFrame(0,0,cpu_time+0.15*cpu_time,2000,";run time [s]; memory consumption [MB]")

    #graphRSS.Draw("samep")
    #graphVirtual.Draw("samep")
    #legend.Draw("same")

    #raw_input("waiting for input")

    #print(exitCode,usage_end,cpu_time)

def main():
    print("doing the validation")

    args,options = opt_parser()

    run_analysis()

    get_analysis_output()

    get_reference_output()

    do_comparison()

    final_user_decision()

if __name__ == '__main__':
    main()
