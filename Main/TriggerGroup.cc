#include "TriggerGroup.hh"

#include <sstream>
#include <stdexcept>

#include "Tools/Tools.hh"
#include "Tools/PXL/PXL.hh"

using std::string;

TriggerGroup::TriggerGroup( Tools::MConfig const &cfg, unsigned int const group_num ) :
   m_prefix( "Trigger." + Tools::toString( group_num ) + "." ),
   m_name(    cfg.GetItem< string >( m_prefix + "Name" ) ),
   m_trigger( cfg.GetItem< string >( m_prefix + "Trigger" ) ),
   m_require( cfg.GetItem< bool   >( m_prefix + "Require" ) ),
   m_reject(  cfg.GetItem< bool   >( m_prefix + "Reject" ) ),
   m_cuts_map( initCuts( cfg ) ),

   // Store the cut values at the initialisation, so they don't have to
   // be read from the config everytime.
   m_muoPtMin( cfg.GetItem< double >( "Muon.pt.min" ) ),
   m_elePtMin( cfg.GetItem< double >( "Ele.pt.min" ) ),
   m_tauPtMin( cfg.GetItem< double >( "Tau.pt.min" ) ),
   m_gamPtMin( cfg.GetItem< double >( "Gamma.pt.min" ) ),
   m_jetPtMin( cfg.GetItem< double >( "Jet.pt.min" ) ),
   m_metPtMin( cfg.GetItem< double >( "MET.pt.min" ) )
{
   if( m_require and m_reject ) {
      std::stringstream err;
      err << "Trigger group '";
      err << m_name << "' ";
      err << "cannot be 'required' and 'rejected' at the same time.";
      throw Tools::config_error( err.str() );
   }
}


std::map< string, TriggerGroup::TriggerCuts > TriggerGroup::initCuts( Tools::MConfig const &cfg ) const {
   std::map< string, TriggerCuts > cuts_map;

   TriggerCuts muoCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.Mu",    "" ), true );
   TriggerCuts eleCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.E",     "" ), true );
   TriggerCuts tauCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.Tau",   "" ), true );
   TriggerCuts gamCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.Gamma", "" ), true );
   TriggerCuts jetCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.Jet",   "" ), true );
   TriggerCuts metCuts = Tools::splitString< double >( cfg.GetItem< string >( m_prefix + "Cuts.MET",   "" ), true );

   std::sort( muoCuts.begin(), muoCuts.end() );
   std::sort( eleCuts.begin(), eleCuts.end() );
   std::sort( tauCuts.begin(), tauCuts.end() );
   std::sort( gamCuts.begin(), gamCuts.end() );
   std::sort( jetCuts.begin(), jetCuts.end() );
   std::sort( metCuts.begin(), metCuts.end() );

   cuts_map[ "Muo" ] = muoCuts;
   cuts_map[ "Ele" ] = eleCuts;
   cuts_map[ "Tau" ] = tauCuts;
   cuts_map[ "Gam" ] = gamCuts;
   cuts_map[ "Jet" ] = jetCuts;
   cuts_map[ "MET" ] = metCuts;

   return cuts_map;
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

   bool const particles_accepted = checkTriggerParticles( getCuts( "Muo" ), muos ) and
                                   checkTriggerParticles( getCuts( "Ele" ), eles ) and
                                   checkTriggerParticles( getCuts( "Tau" ), taus ) and
                                   checkTriggerParticles( getCuts( "Jet" ), jets ) and
                                   checkTriggerParticles( getCuts( "Gam" ), gams ) and
                                   checkTriggerParticles( getCuts( "MET" ), mets );
   return particles_accepted;
}


bool TriggerGroup::checkTriggerParticles( TriggerCuts const &cuts,
                                          std::vector< pxl::Particle* > const &particles
                                          ) const {
   // Do we have anything to do?
   if( cuts.empty() ) return true;

   // Do we have enough partices of the given type in this event?
   if( cuts.size() > particles.size() ) return false;

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
    // The number of cuts defines the triggertopology for each particle type!
    if( numMuo >= getNCuts( "Muo" ) and
        numEle >= getNCuts( "Ele" ) and
        numTau >= getNCuts( "Tau" ) and
        numGam >= getNCuts( "Gam" ) and
        numJet >= getNCuts( "Jet" ) and
        numMET >= getNCuts( "MET" )
        ) return true;

    return false;
}


double TriggerGroup::computeSumptMin( int numMuo,
                                      int numEle,
                                      int numTau,
                                      int numGam,
                                      int numJet,
                                      int numMET,
                                      bool const inclusive
                                      ) const {
   double sumptMin = 0.0;

   TriggerCuts::const_iterator cut;
   if( numMuo > 0 and not inclusive ) {
      for( cut = getCuts( "Muo" ).begin(); cut != getCuts( "Muo" ).end(); ++cut ) {
         sumptMin += *cut;
         numMuo--;
      }
   }
   if( numMuo < 0 ) {
      string const err = "In TriggerGroup::computeSumptMin(...): number of muons smaller than expected. Something is wrong with your EventClass!";
      throw std::underflow_error( err );
   }

   sumptMin += numMuo * m_muoPtMin;

   if( numEle > 0 and not inclusive ) {
      for( cut = getCuts( "Ele" ).begin(); cut != getCuts( "Ele" ).end(); ++cut ) {
         sumptMin += *cut;
         numEle--;
      }
   }
   if( numEle < 0 ) {
      string const err = "In TriggerGroup::computeSumptMin(...): number of electrons smaller than expected. Something is wrong with your EventClass!";
      throw std::underflow_error( err );
   }

   sumptMin += numEle * m_elePtMin;

   if( numTau > 0 and not inclusive ) {
      for( cut = getCuts( "Tau" ).begin(); cut != getCuts( "Tau" ).end(); ++cut ) {
         sumptMin += *cut;
         numTau--;
      }
   }
   if( numTau < 0 ) {
      string const err = "In TriggerGroup::computeSumptMin(...): number of taus smaller than expected. Something is wrong with your EventClass!";
      throw std::underflow_error( err );
   }

   sumptMin += numTau * m_tauPtMin;

   if( numGam > 0 and not inclusive ) {
      for( cut = getCuts( "Gam" ).begin(); cut != getCuts( "Gam" ).end(); ++cut ) {
         sumptMin += *cut;
         numGam--;
      }
   }
   if( numGam < 0 ) {
      string const err = "In TriggerGroup::computeSumptMin(...): number of gammas smaller than expected. Something is wrong with your EventClass!";
      throw std::underflow_error( err );
   }

   sumptMin += numGam * m_gamPtMin;

   if( numJet > 0 and not inclusive ) {
      for( cut = getCuts( "Jet" ).begin(); cut != getCuts( "Jet" ).end(); ++cut ) {
         sumptMin += *cut;
         numJet--;
      }
   }
   if( numJet < 0 ) {
      string const err = "In TriggerGroup::computeSumptMin(...): number of jets smaller than expected. Something is wrong with your EventClass!";
      throw std::underflow_error( err );
   }

   sumptMin += numJet * m_jetPtMin;

   if( numMET > 0 and not inclusive ) {
      for( cut = getCuts( "MET" ).begin(); cut != getCuts( "MET" ).end(); ++cut ) {
         sumptMin += *cut;
         numMET--;
      }
   }
   if( numMET < 0 ) {
      string const err = "In TriggerGroup::computeSumptMin(...): number of METs smaller than expected. Something is wrong with your EventClass!";
      throw std::underflow_error( err );
   }

   sumptMin += numMET * m_metPtMin;

   return sumptMin;
}
