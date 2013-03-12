#include <time.h>

#include "Tools/PXL/PXL.hh"
#include <iostream>
#include <iomanip>
#include "ControlPlotFactory/CcControl.hh"
#include "ControlPlots2/RecControl.hh"
#include "ControlPlots2/GenControl.hh"
#include "EventClassFactory/CcEventClass.hh"
#include "Tools/PXL/PXLdCache.hh"
#include "Tools/argstream.h"
#include "Tools/Tools.hh"

#include <boost/filesystem/path.hpp>

#include "Main/EventSelector.hh"
#include "Main/ReWeighter.hh"
#include "Main/RunLumiRanges.hh"
#include "Main/SkipEvents.hh"

namespace fs = boost::filesystem;

using namespace std;

//----------------------------------------------------------------------

void PrintProcessInfo( ProcInfo_t &info );

int main( int argc, char* argv[] ) {
   if( getenv( "MUSIC_BASE" ) == NULL ) throw runtime_error( "MUSIC_BASE not set!" );

   TDirectory::AddDirectory( kFALSE ); // Force ROOT to give directories in our hand - Yes, we can
   TH1::AddDirectory( kFALSE );        // Force ROOT to give histograms in our hand - Yes, we can

   // Variables for argstream.
   // The values they are initialized with serve as default values.
   // The number of music options should be kept short, so it is easier to use
   // it within condor/grid.
   // All analysis based options/configurations belong into the config file!!!
   //
   string outputDirectory = "./MusicOutDir";
   int numberOfEvents = -1;
   string XSectionsFile( "$MUSIC_BASE/ConfigFiles/XSections.txt" );
   string PlotConfigFile( "$MUSIC_BASE/ConfigFiles/ControlPlots2.cfg" );
   bool useOldECMerger = false;
   bool NoCcControl    = false;
   bool NoCcEventClass = false;
   bool DumpECHistos   = false;
   vector< string > arguments;

   argstream as( argc, argv );

   as >> help()
      >> parameter( 'o', "Output", outputDirectory, "Define the output directory.", false )
      >> parameter( 'N', "Num", numberOfEvents, "Number of events to analyze.", false )
      >> parameter( 'x', "XSections", XSectionsFile, "Path to cross-sections file.", false )
      >> parameter( 'p', "PlotConfig", PlotConfigFile, "Path to the plot-config file.", false )
      >> option(    'M', "oldECMerger", useOldECMerger, "Use ECMerger instead of ECMerger2." )
      >> option(         "NoCcEventClass", NoCcEventClass, "Do NOT create EventClass file. (Not allowed if --NoCcControl specified!)" )
      >> option(         "NoCcControl",  NoCcControl, "Do NOT make ControlPlots. (Not allowed if --NoCcEventClass specified!)" )
      >> option(         "DumpECHistos", DumpECHistos, "Write out all histograms into a separate file (slow!)." )
      >> values< string >( back_inserter( arguments ), "CONFIG_FILE and PXLIO_FILE(S)." );

   as.defaultErrorHandling();

   if( NoCcEventClass and NoCcControl ) {
      cerr << "ERROR: Options: --NoCcControl and --NoCcEventClass not allowed simultaneously!" << endl;
      cout << as.usage() << endl;
      exit( 1 );
   }

   if( arguments.size() < 2 ) {
      cerr << "ERROR: At least CONFIG_FILE and one PXLIO_FILE needed as arguments!" << endl << endl;
      cout << as.usage() << endl;
      exit( 1 );
   }

   // First argument is the config file.
   //
   string FinalCutsFile( arguments.at( 0 ) );

   FinalCutsFile  = Tools::AbsolutePath( FinalCutsFile );
   XSectionsFile  = Tools::AbsolutePath( XSectionsFile );
   PlotConfigFile = Tools::AbsolutePath( PlotConfigFile );

   if( not fs::exists( FinalCutsFile ) ) throw Tools::file_not_found( FinalCutsFile, "Config file" );
   else cout << "INFO: Using Config file: " << FinalCutsFile << endl;

   if( not fs::exists( XSectionsFile ) ) throw Tools::file_not_found( XSectionsFile, "Cross-sections file" );
   else cout << "INFO: Using XSections file: " << XSectionsFile << endl;

   if( not fs::exists( PlotConfigFile ) ) throw Tools::file_not_found( PlotConfigFile, "Plot-config file" );
   else cout << "INFO: Using plot-config file: " << PlotConfigFile << endl;

   const Tools::MConfig config( FinalCutsFile );
   const Tools::MConfig XSections( XSectionsFile );
   const Tools::MConfig PlotConfig( PlotConfigFile );

   // All other arguments must be .pxlio files.
   //
   vector< string > input_files( ++arguments.begin(), arguments.end() );

   const bool runCcEventClass = not NoCcEventClass;
   const bool runCcControl    = not NoCcControl;

   // Get the run config file from config file.
   //
   string RunConfigFile;

   bool runOnData = config.GetItem< bool >( "General.RunOnData" );
   if( runOnData ) {
      RunConfigFile = Tools::AbsolutePath( config.GetItem< string >( "General.RunConfig" ) );
      if( not fs::exists( RunConfigFile ) ) {
         stringstream error;
         error << "RunConfigFile '" << RunConfigFile << "' ";
         error << "in config file: '" << FinalCutsFile << "' not found!";
         throw Tools::config_error( error.str() );
      }
   }
   if( !RunConfigFile.empty() ) cout << "INFO: Using Run config file: " << RunConfigFile << endl;

   const string startDir = getcwd( NULL, 0 );

   // (Re)create outputDirectory dir and cd into it.
   //
   system( ( "rm -rf " + outputDirectory ).c_str() );
   system( ( "mkdir -p " + outputDirectory ).c_str() );
   system( ( "cd " + outputDirectory ).c_str() );
   chdir( outputDirectory.c_str() );

   system( ( "cp " + FinalCutsFile  + " . " ).c_str() );
   system( ( "cp " + XSectionsFile  + " . " ).c_str() );
   system( ( "cp " + PlotConfigFile + " . " ).c_str() );

   if( !RunConfigFile.empty() ) system( ( "cp " + RunConfigFile + " . " ).c_str() );

   if( runOnData ) system( "mkdir -p Event-lists" );

   // Init the run config
   //
   lumi::RunLumiRanges runcfg( RunConfigFile );
   SkipEvents skipEvents( config );

   // Configure fork:
   pxl::AnalysisFork fork;
   fork.setName("MISFork");

   //initialize the EventSelector
   EventSelector Selector( config );

   // configure processes:
   if( runCcControl ){
      cp2::RecControl *plots = new cp2::RecControl( config, PlotConfig, &Selector );
      fork.setObject( plots );
      fork.setIndex( "RecControl", plots );
      if( not runOnData ) {
         cp2::GenControl *gen_plots = new cp2::GenControl();
         fork.setObject( gen_plots );
         fork.setIndex( "GenControl", gen_plots );

         CcControl *old_plots = new CcControl( config, &Selector);
         fork.setObject( old_plots );
         fork.setIndex( "CcControl", old_plots );
      }
   }
   if (runCcEventClass){
      CcEventClass *cc_event = new CcEventClass( config, XSections, 0, &Selector, DumpECHistos );
      fork.setObject( cc_event );
      fork.setIndex( "CcEventClass", cc_event );

      if( not runOnData ){
         CcEventClass *cc_event_up = new CcEventClass( config, XSections, +1, &Selector, DumpECHistos );
         fork.setObject( cc_event_up );
         fork.setIndex( "CcEventClass_JES_UP", cc_event_up );
         CcEventClass *cc_event_down = new CcEventClass( config, XSections, -1, &Selector, DumpECHistos );
         fork.setObject( cc_event_down );
         fork.setIndex( "CcEventClass_JES_DOWN", cc_event_down );
      }
   }

   // begin analysis
   fork.beginJob();
   fork.beginRun();

   // performance monitoring
   double dTime1 = pxl::getCpuTime();    // Start Time
   int    e = 0;                         // Event counter
   unsigned int skipped=0; //number of events skipped from run/LS config

   ReWeighter reweighter = ReWeighter( config );

   unsigned int analyzed_files = 0;
   unsigned int lost_files = 0;

   // loop over all files
   vector<string>::const_iterator file_iter = input_files.begin();

   // initialize process info object
   ProcInfo_t info;
   for( unsigned int f = 0; f < input_files.size() && ( numberOfEvents == -1 || e < numberOfEvents ); f++ ) {
      // open file(s):
      pxl::InputHandler* input;
      string filename = *file_iter;
      if( filename.substr(0,7)  == "dcap://" || filename.substr(0,6) == "/pnfs/") {
         input = new pxl::dCacheInputFile();
      } else {
         if( filename.substr(0,1) != "/" ) {
            filename = startDir + "/" + filename;
         }
         input = new pxl::InputFile();
      }

      cout << "Opening file " << filename << endl;
      unsigned int numTrials = 0;
      //we need to get done in 3 days, so don't wait too long
      unsigned int timeout = 3*24*60*60/input_files.size();
      cout << "Opening file (timeout " << timeout << " seconds) " << filename << endl;
      while( true ) {
         numTrials++;
         try {
            time_t rawtime;
            time ( &rawtime );
            cout << "Opening time: " << ctime ( &rawtime );
            input->open( filename, timeout );
         } catch( dCache_error& d ) {
            if( not runOnData ) {
               //increase lost files counter, but don't try again
               lost_files++;
               cout << "Failed, skipping file." << endl;
            } else if( numTrials >= 3 ) {
               cout <<"Failed three times."<< endl;
               lost_files++;
               throw;
            } else {
               cout <<"Failed to open, trying again!"<< endl;
               //reset the timeout for the second trial
               if( numTrials == 1 ) timeout = 36000;
               //try one hour in all following trials
               else timeout = 3600;
               continue;
            }
         }
         //increase successful files counter
         analyzed_files++;
         break;
      }

      // run event loop:
      while (input->nextEvent()) {

         if( numberOfEvents > -1 and e >= numberOfEvents ) break;

         //pxl::Objects event;
         pxl::Event event;
         // read event from disk
         if ( ! input->readEvent(&event) ){
            cout << "AAAARGH, event not properly read!" << endl;
            if( runOnData ) {
               return 1;
            } else {
               lost_files++;
               break;
            }
         }

         //check if we shall analyze this event
         lumi::ID run      = event.findUserRecord< lumi::ID >( "Run" );
         lumi::ID LS       = event.findUserRecord< lumi::ID >( "LumiSection" );
         lumi::ID eventNum = event.findUserRecord< lumi::ID >( "EventNum" );
         if( ! runcfg.check( run, LS ) ) {
            ++skipped;
            continue;
         }

         if( skipEvents.skip( run, LS, eventNum ) ) {
            ++skipped;
            continue;
         }

         pxl::EventView *RecEvtView = event.getObjectOwner().findObject< pxl::EventView >( "Rec" );

         // Switch to cocktail muons.
         // (Use the four momentum from TeV-optimised reconstructors.)
         //
         Selector.adaptMuons( RecEvtView );

         if( runOnData ){
            //for data we just need to run the selection
            Selector.performSelection(RecEvtView, 0);
         } else {
            reweighter.ReWeightEvent( event );
            pxl::EventView* GenEvtView = event.getObjectOwner().findObject<pxl::EventView>("Gen");

            Selector.preSynchronizeGenRec( GenEvtView, RecEvtView );

            // create Copys of the original Event View and modify the JES
            pxl::EventView *GenEvtView_JES_UP = event.create< pxl::EventView >( GenEvtView );
            event.setIndex( "Gen_JES_UP", GenEvtView_JES_UP );
            pxl::EventView *RecEvtView_JES_UP = event.create< pxl::EventView >( RecEvtView );
            event.setIndex( "Rec_JES_UP", RecEvtView_JES_UP );
            pxl::EventView *GenEvtView_JES_DOWN = event.create< pxl::EventView >( GenEvtView );
            event.setIndex( "Gen_JES_DOWN", GenEvtView_JES_DOWN );
            pxl::EventView *RecEvtView_JES_DOWN = event.create< pxl::EventView >( RecEvtView );
            event.setIndex( "Rec_JES_DOWN", RecEvtView_JES_DOWN );

            // Apply cuts, remove duplicates, recalculate Event Class, perform >= 1 lepton cut, redo matching, set index:
            Selector.performSelection(GenEvtView, 0);
            Selector.performSelection(RecEvtView, 0);

            // Redo Matching
            Selector.redoMatching(GenEvtView, RecEvtView);

            //synchronize some user records
            Selector.synchronizeGenRec( GenEvtView, RecEvtView );

            if( runCcEventClass ) {
               // SAME for JES_UP: modify Jets apply cuts, remove duplicates, recalculate Event Class, perform >= 1 lepton cut, redo matching, set index:
               Selector.performSelection(GenEvtView_JES_UP, +1);
               Selector.performSelection(RecEvtView_JES_UP, +1);
               // SAME for JES_DOWN: modify Jets,  apply cuts, remove duplicates, recalculate Event Class, perform >= 1 lepton cut, redo matching, set index:
               Selector.performSelection(GenEvtView_JES_DOWN, -1);
               Selector.performSelection(RecEvtView_JES_DOWN, -1);
               // redo Matching
               Selector.redoMatching(GenEvtView_JES_UP, RecEvtView_JES_UP);
               Selector.redoMatching(GenEvtView_JES_DOWN, RecEvtView_JES_DOWN);
               //synchronize some user records
               Selector.synchronizeGenRec( GenEvtView_JES_UP, RecEvtView_JES_UP );
               Selector.synchronizeGenRec( GenEvtView_JES_DOWN, RecEvtView_JES_DOWN );
            }
         }
         // run the fork ..
         fork.analyseEvent( event );
         fork.finishEvent( event );

         e++;
         if( e < 10 || ( e < 100 && e % 10 == 0 ) ||
            ( e < 1000 && e % 100 == 0 ) ||
            ( e < 10000 && e % 1000 == 0 ) ||
            ( e >= 10000 && e % 10000 == 0 ) ) {
            cout << e << " Events analyzed (" << skipped << " skipped)" << endl;
         }

         if( e % 100000 == 0 ) PrintProcessInfo( info );
      }
      input->close();
      delete input;
      ++file_iter;
   }

   double dTime2 = pxl::getCpuTime();
   cout << "Analyzed " << e << " Events, skipped " << skipped << ", elapsed CPU time: " << dTime2-dTime1 << " ("<< double(e)/(dTime2-dTime1) <<" evts per sec)" << endl;
   if( lost_files >= 0.5*( lost_files + analyzed_files ) ) {
      cout << "Error: Too many files lost!" << endl;
      throw runtime_error( "Too many files lost." );
   } else if( lost_files > 0 ) {
      cout << "Warning: " << lost_files << " of " << ( lost_files + analyzed_files ) << " files lost due to timeouts or read errors." << endl;
   }
   if( (e+skipped) == 0 ) {
      cout << "Error: No event analayzed!" << endl;
      throw runtime_error( "No event analayzed!" );
   }
   cout << "\n\n\n" << endl;

   fork.endRun();
   fork.endJob();

   if( runCcEventClass and not runOnData ) {
      if( useOldECMerger ) {
         cout << "Calling ECMerger..." << endl;
         chdir( ".." );
         system( ( Tools::musicAbsPath( "bin/ECMerger" ) + " " + outputDirectory + "/EC_*.root -o " + outputDirectory +  "/EC_Final.root" ).c_str() );
      }
      else {
         cout << "Calling ECMerger2 ..." << endl;
         chdir( ".." );
         system( ( Tools::musicAbsPath( "bin/ECMerger.py" ) + " --jes " + outputDirectory + "/EC_*.root -o " + outputDirectory + "/EC_Final.root" ).c_str() );
      }
      cout << "Done." << endl;
   }

   PrintProcessInfo( info );
   return 0;
}

void PrintProcessInfo( ProcInfo_t &info ) {
   gSystem->GetProcInfo( &info );
   cout.precision( 1 );
   cout << fixed;
   cout << "--> Process info:" << endl;
   cout << "    -------------" << endl;
   cout << "    CPU time elapsed: " << info.fCpuUser << " s" << endl;
   cout << "    Sys time elapsed: " << info.fCpuSys  << " s" << endl;
   cout << "    Resident memory:  " << info.fMemResident / 1024. <<  " MB" << endl;
   cout << "    Virtual memory:   " << info.fMemVirtual / 1024.  <<  " MB" << endl;
}
