#!/usr/bin/env python

import optparse
import os
import platform
import subprocess
import tarfile
import urllib
import urlparse

import sys

import checkEnvironment

def main():
    music_base, \
    cmssw_version, \
    cmssw_base, \
    scram_arch = checkEnvironment.checkEnvironment()

    usage = '%prog [options]'
    desc = ''
    install_path_default = os.path.join( music_base, 'local' )
    external_path_default = os.path.join( music_base, 'external' )
    parser = optparse.OptionParser( description=desc, usage=usage )
    parser.add_option( '-j', '--jobs',
                       metavar='JOBS',
                       type='int',
                       default=1,
                       help="Set number of jobs to be used to build " \
                            "(for 'make'). [default = %default]"
                       )
    parser.add_option( '--get',
                       action='store_true',
                       default=False,
                       help="Force the download of the specified LHAPDF and " \
                            "yaml (if '--force-yaml' is use) versions. " \
                            "[default = only download if not present in " \
                            "LHAPDFPATH (and YAMLPATH)]."
                       )
    parser.add_option( '--install-path',
                       metavar='INSTALLPATH',
                       default=None,
                       help='Set the PATH to install yaml and LHAPDF. ' \
                            '[default = $MUSIC_BASE/local]'
                       )
    parser.add_option( '-l', '--lhapdf-version',
                       metavar='VERSION',
                       default='LHAPDF-6.1.3',
                       help='Set the VERSION of LHAPDF to be used. ' \
                            '[default = %default]'
                       )
    parser.add_option( '--lhapdf-path',
                       metavar='LHAPDFPATH',
                       default=None,
                       help='Set the PATH where LHAPDF should be located. ' \
                            '[default = $MUSIC_BASE/external]'
                       )
    parser.add_option( '--lhapdf-url',
                       metavar='URL',
                       default='http://www.hepforge.org/archive/lhapdf/',
                       help='Set the URL where LHAPDF should be downloaded. ' \
                            '[default = %default]'
                       )
    parser.add_option( '-y', '--force-yaml',
                       action='store_true',
                       default=False,
                       help='From version 6.1, a version of yaml is included ' \
                            'in LHAPDF. Use this switch only if you really ' \
                            'want to download and build your own yaml ' \
                            'version! [default = use internal yaml version]'
                       )
    parser.add_option( '--yaml-version',
                       metavar='VERSION',
                       default='yaml-cpp-0.3.0',
                       help='Set the VERSION of yaml to be used. ' \
                            '[default = %default]'
                       )
    parser.add_option( '--yaml-path',
                       metavar='YAMLPATH',
                       default=None,
                       help='Set the PATH where yaml should be located. ' \
                            '[default = $MUSIC_BASE/external]'
                       )
    parser.add_option( '--yaml-url',
                       metavar='URL',
                       default='http://yaml-cpp.googlecode.com/files/',
                       help='Set the URL where yaml should be downloaded. ' \
                            '[default = %default]'
                       )
    ( options, args ) = parser.parse_args()

    if options.install_path == None:
        options.install_path = install_path_default

    if options.lhapdf_path == None:
        options.lhapdf_path = external_path_default

    if options.force_yaml:
        if options.yaml_path == None:
            options.yaml_path = external_path_default

    try:
        os.mkdir( options.install_path )
    except OSError:
        # If the directory already exists, go on.
        pass

    try:
        os.mkdir( options.lhapdf_path )
    except OSError:
        # If the directory already exists, go on.
        pass

    # From SL6 on the boost libraries should be new enough.
    options.cmssw_boost = None
    ID, release, codename = platform.linux_distribution()
    major, minor = release.split( '.' )
    if int( major ) < 6:
        # Check all possible boost versions in CMSSW.
        boost_versions = [
            '1.51.0-cms5',
            '1.51.0-cms4',
            '1.51.0-cms3',
            '1.51.0-cms2',
            '1.51.0-cms',
            '1.51.0',
            '1.50.0-cms5',
            '1.50.0-cms4',
            '1.50.0-cms3',
            '1.50.0-cms2',
            '1.50.0-cms',
            '1.50.0',
            '1.49.0-cms2',
            '1.49.0-cms',
            '1.49.0',
            '1.47.0-cms4',
            '1.47.0-cms3',
            '1.47.0-cms',
            '1.47.0',
            '1.44.0-cms3',
            '1.44.0-cms2',
            '1.44.0-cms',
            '1.42.0-cms',
            ]

        for ver in boost_versions:
            boost_path = os.path.join( '/cvmfs/cms.cern.ch',
                                       scram_arch,
                                       'external/boost',
                                       ver,
                                       )
            if os.path.exists( boost_path ):
                # Choose the first one found.
                options.cmssw_boost = boost_path
                break
        else:
            print "No suitable boost version found! Please investigate!"
            print "Aborting..."
            sys.exit( 2 )

    if options.force_yaml:
        installYAML( options, music_base )

    lhapdf_file_path = os.path.join( music_base,
                                     'external',
                                     options.lhapdf_version + '.tar.gz'
                                     )

    lhapdf_url_full = urlparse.urljoin( options.lhapdf_url,
                                        options.lhapdf_version + '.tar.gz'
                                        )

    if options.get or not os.path.exists( lhapdf_file_path ):
        urllib.urlretrieve( lhapdf_url_full, lhapdf_file_path )

    lhapdf_path_full = extract( lhapdf_file_path,
                                options.lhapdf_version,
                                options.lhapdf_path
                                )

    # Not completely fail-save.
    conf = open( '/tmp/configureLHAPDF.sh' , 'w' )
    print >> conf, '#!/bin/bash'
    print >> conf, 'LHAPDFDIR=$1'
    print >> conf, 'INSTALLDIR=$2'
    print >> conf, 'BOOSTPATH=$3'
    print >> conf, 'cd $LHAPDFDIR'

    configure  = './configure '
    configure += '--prefix=$INSTALLDIR '
    if options.cmssw_boost:
        configure += '--with-boost=$BOOSTPATH '
    if options.force_yaml:
        configure += '--with-yaml-cpp=$INSTALLDIR '

    print >> conf, 'echo "Running configure script in $LHAPDFDIR..."'
    print >> conf, 'echo "Full call: \'%s\'"' % configure
    print >> conf, configure
    print >> conf, 'exit 0'

    conf.close()
    os.chmod( conf.name, 0744 )

    cmd = [ conf.name,
            lhapdf_path_full,
            options.install_path,
            str( options.cmssw_boost ),
            ]
    proc = subprocess.Popen( cmd )
    proc.communicate()
    del cmd

    build = open( '/tmp/buildLHAPDF.sh' , 'w' )
    print >> build, '#!/bin/bash'
    print >> build, 'LHAPDFDIR=$1'
    print >> build, 'cd $LHAPDFDIR'

    print >> build, 'echo "Cleaning..."'
    print >> build, 'make clean'
    print >> build, 'echo "Building..."'
    print >> build, 'make -j%i && make -j%i install' % \
                    ( options.jobs, options.jobs )
    print >> build, 'exit 0'
    build.close()
    os.chmod( build.name, 0744 )

    cmd = [ build.name,
            lhapdf_path_full,
            options.install_path,
            str( options.cmssw_boost ),
            ]
    proc = subprocess.Popen( cmd )
    proc.communicate()
    del cmd


def installYAML( options, music_base ):
    try:
        os.mkdir( options.yaml_path )
    except OSError:
        # If the directory already exists, go on.
        pass

    yaml_file_path = os.path.join( music_base, 'external',
                                   options.yaml_version + '.tar.gz'
                                   )

    yaml_url_full = urlparse.urljoin( options.yaml_url,
                                      options.yaml_version + '.tar.gz'
                                      )

    if options.get or not os.path.exists( yaml_file_path ):
        urllib.urlretrieve( yaml_url_full, yaml_file_path )

    yaml_path_full = extract( yaml_file_path,
                              options.yaml_version,
                              options.yaml_path
                              )

    # Not completely fail-save.
    yaml_sh = open( '/tmp/installYAML.sh', 'w' )
    print >> yaml_sh, '#!/bin/bash'
    print >> yaml_sh, 'YAMLDIR=$1'
    print >> yaml_sh, 'INSTALLDIR=$2'
    print >> yaml_sh, 'cd $YAMLDIR'
    print >> yaml_sh, 'echo "Building YAML..."'
    print >> yaml_sh, 'cmake -DBUILD_SHARED_LIBS=ON \
        -DCMAKE_INSTALL_PREFIX=$INSTALLDIR'
    print >> yaml_sh, 'make -j4 && make install'
    print >> yaml_sh, 'exit 0'

    yaml_sh.close()
    os.chmod( yaml_sh.name, 0744 )

    cmd = [ yaml_sh.name, yaml_path_full, options.install_path ]
    proc = subprocess.Popen( cmd )
    proc.communicate()


def extract( file_path, version, extract_path ):
    full_path = extract_path
    tar = tarfile.open( file_path )
    if not tar.getmembers()[0].isdir():
        # If (for whatever reason) the content is not tar'ed into an own
        # directory, create one and untar everything there.
        full_path = os.path.join( extract_path, version )
        os.mkdir( full_path )
        tar.extractall( full_path )
    else:
        full_path = os.path.join( extract_path, tar.getmembers()[0].name )
        tar.extractall( extract_path )
    tar.close()
    del tar

    return full_path


if __name__ == '__main__':
    main()
