#include "TriggerGroup.hh"

#include <exception>
#include <sstream>
#include <stdexcept>

#include "Tools/Tools.hh"
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

using std::string;

TriggerGroup::TriggerGroup( Tools::MConfig const &cfg,
                            string const &triggerPrefix,
                            string const &groupName
                            ) :
   m_triggerPrefix( triggerPrefix ),
   m_prefix( "Trigger." + groupName + "." ),
   m_name( cfg.GetItem< string >( m_prefix + "Name" ) ),
   m_triggers( initTriggers( cfg ) ),
   m_require( cfg.GetItem< bool >( m_prefix + "Require" ) ),
   m_reject(  cfg.GetItem< bool >( m_prefix + "Reject" ) ),
   m_cuts_map( initCuts( cfg ) ),
   m_ptMin_map( initPtMin( cfg ) )
{
   if( m_require and m_reject ) {
      std::stringstream err;
      err << "Trigger group '";
      err << m_name << "' ";
      err << "cannot be 'required' and 'rejected' at the same time.";
      throw Tools::config_error( err.str() );
   }
}


std::set< string > TriggerGroup::initTriggers( Tools::MConfig const &cfg ) const {
   std::vector< string > const trig_vec = Tools::splitString< string >( cfg.GetItem< string >( m_prefix + "Triggers" ) );

   std::set< string > const triggers( trig_vec.begin(), trig_vec.end() );

   return triggers;
}


TriggerGroup::TriggerCutsCollection TriggerGroup::initCuts( Tools::MConfig const &cfg ) const {
   TriggerCutsCollection cuts_map;

   TriggerCuts muoCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.Mu",    "" ), true );
   TriggerCuts eleCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.E",     "" ), true );
   TriggerCuts tauCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.Tau",   "" ), true );
   TriggerCuts gamCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.Gamma", "" ), true );
   TriggerCuts jetCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.Jet",   "" ), true );
   TriggerCuts metCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.MET",   "" ), true );

   if( not muoCuts.empty() ) {
      std::sort( muoCuts.begin(), muoCuts.end() );
      cuts_map[ "Muo" ] = muoCuts;
   }

   if( not eleCuts.empty() ) {
      std::sort( eleCuts.begin(), eleCuts.end() );
      cuts_map[ "Ele" ] = eleCuts;
   }

   if( not tauCuts.empty() ) {
      std::sort( tauCuts.begin(), tauCuts.end() );
      cuts_map[ "Tau" ] = tauCuts;
   }

   if( not gamCuts.empty() ) {
      std::sort( gamCuts.begin(), gamCuts.end() );
      cuts_map[ "Gam" ] = gamCuts;
   }

   if( not jetCuts.empty() ) {
      std::sort( jetCuts.begin(), jetCuts.end() );
      cuts_map[ "Jet" ] = jetCuts;
   }

   if( not metCuts.empty() ) {
      std::sort( metCuts.begin(), metCuts.end() );
      cuts_map[ "MET" ] = metCuts;
   }

   return cuts_map;
}


// Store the cut values at the initialisation, so they don't have to
// be read from the config everytime.
TriggerGroup::PtMap TriggerGroup::initPtMin( Tools::MConfig const &cfg ) const {
   PtMap ptMinMap;

   ptMinMap[ "Muo" ] = cfg.GetItem< double >( "Muon.Pt.min" );
   ptMinMap[ "Ele" ] = cfg.GetItem< double >( "Ele.pt.min" );
   ptMinMap[ "Tau" ] = cfg.GetItem< double >( "Tau.pt.min" );
   ptMinMap[ "Gam" ] = cfg.GetItem< double >( "Gamma.pt.min" );
   ptMinMap[ "Jet" ] = cfg.GetItem< double >( "Jet.pt.min" );
   ptMinMap[ "MET" ] = cfg.GetItem< double >( "MET.pt.min" );

   return ptMinMap;
}


bool TriggerGroup::passTriggerParticles( bool const isRec,
                                         std::vector< pxl::Particle* > const &muos,
                                         std::vector< pxl::Particle* > const &eles,
                                         std::vector< pxl::Particle* > const &taus,
                                         std::vector< pxl::Particle* > const &gams,
                                         std::vector< pxl::Particle* > const &jets,
                                         std::vector< pxl::Particle* > const &mets
                                         ) const {
   // No trigger on "Gen" level.
   if( not isRec ) return true;

   bool const particles_accepted = checkTriggerParticles( "Muo", muos ) and
                                   checkTriggerParticles( "Ele", eles ) and
                                   checkTriggerParticles( "Tau", taus ) and
                                   checkTriggerParticles( "Jet", jets ) and
                                   checkTriggerParticles( "Gam", gams ) and
                                   checkTriggerParticles( "MET", mets );
   return particles_accepted;
}


bool TriggerGroup::checkTriggerParticles( string const &particleType,
                                          std::vector< pxl::Particle* > const &particles
                                          ) const {
   TriggerCuts cuts;
   bool const cutsSet = getCuts( particleType, cuts );

   // Do we have anything to do?
   if( not cutsSet ) return true;

   // Do we have enough partices of the given type in this event?
   if( particles.size() < cuts.size()  ) return false;

   std::vector< pxl::Particle* >::const_iterator particle = particles.begin();
   for( TriggerCuts::const_iterator cut = cuts.begin(); cut != cuts.end(); cut++, particle++ ) {
      if( (*particle)->getPt() < *cut ) {
         return false;
      }
   }

   return true;
}


bool TriggerGroup::checkTopology( int const numMuo,
                                  int const numEle,
                                  int const numTau,
                                  int const numGam,
                                  int const numJet,
                                  int const numMET
                                  ) const {
   // Check if we demand any cuts on each particle and then check if there are
   // enough particles in the given topology.
   bool all_accept = true;
   if( all_accept and getNCuts( "Muo" ) >= 0 ) all_accept = numMuo >= getNCuts( "Muo" );
   if( all_accept and getNCuts( "Ele" ) >= 0 ) all_accept = numEle >= getNCuts( "Ele" );
   if( all_accept and getNCuts( "Tau" ) >= 0 ) all_accept = numTau >= getNCuts( "Tau" );
   if( all_accept and getNCuts( "Gam" ) >= 0 ) all_accept = numGam >= getNCuts( "Gam" );
   if( all_accept and getNCuts( "Jet" ) >= 0 ) all_accept = numJet >= getNCuts( "Jet" );
   if( all_accept and getNCuts( "MET" ) >= 0 ) all_accept = numMET >= getNCuts( "MET" );

   return all_accept;
}


double TriggerGroup::sumPtMinForParticles( string const &particleType,
                                           bool const &inclusive,
                                           int numParticles
                                           ) const {
   double sumptMin = 0.0;
   // For inclusive EventClasses the trigger cuts are not relevant.
   if( numParticles > 0 and not inclusive ) {
      TriggerCuts cuts;
      bool const cutsSet = getCuts( particleType, cuts );
      if( cutsSet ) {
         // The given pt-cuts are added and for each given cut one particle is
         // removed.
         for( TriggerCuts::const_iterator cut = cuts.begin(); cut != cuts.end(); ++cut ) {
            sumptMin += *cut;
            numParticles--;
         }
      }
   }

   // If there were less particles than cuts, something went really wrong!
   if( numParticles < 0 ) {
      string err = "In TriggerGroup::sumPtMinForParticles(...): number of particles '";
      err += particleType;
      err += "' smaller than expected. Something is wrong with your EventClass!";
      throw std::underflow_error( err );
   }

   // For non-triggering particles the minimum sum(pt) is simply the pt-cut
   // times the number of particles of that type.
   sumptMin += numParticles * (*m_ptMin_map.find( particleType )).second;

   return sumptMin;
}


double TriggerGroup::getSumptMin( int const numMuo,
                                  int const numEle,
                                  int const numTau,
                                  int const numGam,
                                  int const numJet,
                                  int const numMET,
                                  bool const inclusive
                                  ) const {
   double sumptMin = 0.0;

   sumptMin += sumPtMinForParticles( "Muo", inclusive, numMuo );
   sumptMin += sumPtMinForParticles( "Ele", inclusive, numEle );
   sumptMin += sumPtMinForParticles( "Tau", inclusive, numTau );
   sumptMin += sumPtMinForParticles( "Gam", inclusive, numGam );
   sumptMin += sumPtMinForParticles( "Jet", inclusive, numJet );
   sumptMin += sumPtMinForParticles( "MET", inclusive, numMET );

   return sumptMin;
}


double TriggerGroup::getMETMin() const {
   TriggerCuts cuts;
   bool const cutsSet = getCuts( "MET", cuts );

   if( cutsSet ) return cuts.at( 0 );
   else          return 0.0;
}


TriggerGroup::TriggerResults TriggerGroup::getTriggerResults( pxl::EventView const *triggerEvtView ) const {
   TriggerResults triggerResults;

   bool any_trigger_found = false;

   for( Triggers::const_iterator trigger = m_triggers.begin(); trigger != m_triggers.end(); ++trigger ) {
      string const triggerName = m_triggerPrefix + *trigger;
      try{
         if( triggerEvtView->hasUserRecord( triggerName ) ){
             triggerResults[ triggerName ] = true;
             any_trigger_found = true;
         }else{
             triggerResults[ triggerName ] = false;
        }
      } catch( std::runtime_error &exc ) {
         std::cout << "Weird try catch in getTriggerResults in TriggerGroup.cc" << std::endl;
      }
   }

   pxl::UserRecords allTriggerRecords = triggerEvtView->getUserRecords();

   for(  pxl::UserRecords::const_iterator trigger = allTriggerRecords.begin(); trigger != allTriggerRecords.end(); ++trigger ){
      if ( trigger->first.find( m_triggerPrefix ) != std::string::npos ){
         any_trigger_found = true;
      }
   }
   if( not any_trigger_found ) {
      std::stringstream err;
      err << "In TriggerSelector::passHLTrigger(...): ";
      err << "None of the specified triggers in trigger group '";
      err << m_name;
      err << "' found!";
      throw std::runtime_error( err.str() );
   }

   return triggerResults;
}
