#include "TauSelector.hh"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "Tools/Tools.hh"
#include "Tools/PXL/Sort.hh"


//--------------------Constructor-----------------------------------------------------------------

TauSelector::TauSelector( const Tools::MConfig &cfg ):

   // Taus:
   m_tau_pt_min(  cfg.GetItem< double >( "Tau.pt.min" ) ),
   m_tau_eta_max( cfg.GetItem< double >( "Tau.Eta.max" ) ),
   //Get Tau-Discriminators and save them
   m_tau_discriminators( Tools::splitString< std::string >( cfg.GetItem< std::string >( "Tau.Discriminators" ), true ) )
{
}





//--------------------Destructor------------------------------------------------------------------

TauSelector::~TauSelector(){
}



bool TauSelector::passTau( pxl::Particle *tau, const bool &isRec ) const{
   //pt cut
   if( tau->getPt() < m_tau_pt_min ) return false;

   //eta cut
   if( fabs( tau->getEta() ) > m_tau_eta_max )
      return false;
   if( isRec ) {
      for( std::vector< std::string >::const_iterator discr = m_tau_discriminators.begin(); discr != m_tau_discriminators.end(); ++discr ) {
         // In theory all tau discriminators have a value between 0 and 1.
         // Thus, they are saved as a float and the cut value is 0.5.
         // In practice most (or all) discriminators are boolean.
         // See also:
         // https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePFTauID#Discriminators
         if( tau->getUserRecord( *discr ).toDouble() < 0.5 ) {
            return false;
         }
      }
   }
   return true;
}
