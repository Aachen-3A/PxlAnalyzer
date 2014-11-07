#!/bin/env python

import optparse, time, os, subprocess, sys
import resource
from datetime import datetime
import numpy as np
import logging
import multiprocessing
sys.path.append("lib/")
from configobj import ConfigObj

from ROOT import TCanvas, TGraph, TF1, TLegend, kBlue, gStyle, gPad, TPad, TFile

log = logging.getLogger( 'Validator' )

def opt_parser():
    date_time = datetime.now()
    usage = '%prog [options] CONFIG_FILE'
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( '-u', '--user', default = os.getenv( 'LOGNAME' ),
                            help = 'which user on dcache [default = %s]'%(os.getenv( 'LOGNAME' )))
    parser.add_option( '-o', '--Output', default = "./output", metavar = 'DIRECTORY',
                            help = 'Define the output directory. [default = %default]')
    parser.add_option( '--debug', metavar = 'LEVEL', default = 'INFO',
                       help= 'Set the debug level. Allowed values: ERROR, WARNING, INFO, DEBUG. [default = %default]' )

    parser.add_option( '--executable', metavar = 'EXECUTABLE' , default = 'music',
                            help = 'Name of the executable. [default = %default]')
    parser.add_option( '--exeoption', metavar = 'EXEOPTION' , default = '--SpecialAna',
                            help = 'Options that should be passed to the executable. [default = %default]' )
    parser.add_option( '--execonfig', metavar = 'EXECONFIG' , default = '$MUSIC_BASE/ConfigFiles/MC.cfg',
                            help = 'Configuration file that should be passed to the executable. [default = %default]')
    parser.add_option( '--cfgfile', metavar = 'CFGFILE' , default = './config.cfg',
                            help = 'Name of the configuration file for the used files. [default = %default]' )
 
    ( options, args ) = parser.parse_args()
    if len( args ) != 0:
        parser.error( 'Exactly zero CONFIG_FILE required!' )

    format = '%(levelname)s from %(name)s at %(asctime)s: %(message)s'
    date = '%F %H:%M:%S'
    logging.basicConfig( level = logging._levelNames[ options.debug ], format = format, datefmt = date )

    try: 
        cfg_file= ConfigObj(options.cfgfile)
    except IOError as e:
        print("There was a error reading the File "+ options.cfgfile)
        print e
        exit()


    return args,options,cfg_file

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

def run_analysis(options,cfg_file):
    print("running the analysis")
    
    music_prog = options.executable
    music_opt  = options.exeoption
    music_cfg  = options.execonfig
    music_path = cfg_file["basic"]["path"]
    
    item_list = []
    sample_list = []
    for item in cfg_file["samples"]:
        item_list.append([music_prog,"-o %s"%(item[item.find("/")+1:-6]),music_opt,music_cfg,music_path+item])
        if cfg_file["samples"][item]["label"] not in sample_list:
            sample_list.append(cfg_file["samples"][item]["label"])
    pool = multiprocessing.Pool()
    pool.map_async(run_analysis_task, item_list)
    while True:
        time.sleep(1)
        if not pool._cache: break
    pool.close()
    pool.join()

    os.mkdir(options.Output)

    p = subprocess.Popen("hadd -f9 %s/%s *_mem_log.root"%(options.Output,"log.root"),shell=True,stdout=subprocess.PIPE)
    output = p.communicate()[0]

    for item in sample_list:
        sample_files = []
        sample_folders = []
        for item2 in cfg_file["samples"]:
            if cfg_file["samples"][item2]["label"] == item:
                sample_files.append(item2[item2.find("/")+1:-6]+"/SpecialHistos.root")
                sample_folders.append(item2[item2.find("/")+1:-6])
        p3 = subprocess.Popen("hadd -f9 %s/%s.root "%(options.Output,item)+" ".join(sample_files),shell=True,stdout=subprocess.PIPE)
        output = p3.communicate()[0]
        for item2 in sample_folders:
            p2 = subprocess.Popen("rm -r %s"%item2,shell=True,stdout=subprocess.PIPE)
            output = p2.communicate()[0]

    p4 = subprocess.Popen("rm *.root",shell=True,stdout=subprocess.PIPE)
    output = p4.communicate()[0]

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

    dummy_file = TFile(item[1][3:]+"_mem_log.root","RECREATE")

    graphRSS.Write(item[1][3:]+"_rss.root")
    graphVirtual.Write(item[1][3:]+"_vir.root")

    dummy_file.Close()

def main():
    print("doing the validation")

    args,options,cfg_file = opt_parser()

    run_analysis(options,cfg_file)

    get_analysis_output()

    get_reference_output()

    do_comparison()

    final_user_decision()

if __name__ == '__main__':
    main()
