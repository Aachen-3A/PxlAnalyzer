#include "EventAdaptor.hh"

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

#include "Tools/MConfig.hh"
#include "Tools/PXL/PXL.hh"
#include "Tools/PXL/Sort.hh"

EventAdaptor::EventAdaptor( Tools::MConfig const &cfg,
                            unsigned int const debug
                            ) :
   m_debug( debug ),
   m_gen_rec_map( cfg ),

   m_muo_useCocktail( cfg.GetItem< bool >( "Muon.UseCocktail" ) ),

   // Cache the particle names.
   m_muo_RecName( m_gen_rec_map.get( "Muo" ).RecName )
{
}


// Works on data as well as MC.
void EventAdaptor::applyCocktailMuons( pxl::EventView const *RecEvtView
                                       ) const {
   // Do we even want to use cocktail muons?
   if( not m_muo_useCocktail ) {
      if( m_debug > 0 ) {
         std::stringstream warn;
         warn << "[WARNING] (EventAdaptor): " << std::endl;
         warn << "Using 'applyCocktailMuons(...)', but config file says: ";
         warn << "'Muon.UseCocktail = " << m_muo_useCocktail << "'";
         warn << std::endl << std::endl;

         std::cerr << warn.str();
      }
   }

   // Get all particles in this event.
   pxlParticles allparticles;
   RecEvtView->getObjectsOfType< pxl::Particle >( allparticles );
   pxl::sortParticles( allparticles );

   pxlParticles::iterator part = allparticles.begin();
   for( ; part != allparticles.end(); ++part ) {
      if( (*part)->getName() == m_muo_RecName ) {
         adaptMuon( *part );
      }
   }
}


// This function changes the Muon quantities from "normal" to "cocktail".
// (Only doing this for the four momentum of the muon atm. To be extended in the
// future.)
void EventAdaptor::adaptMuon( pxl::Particle *muon ) const {
   // We are only interested in muons here.
   if( muon->findUserRecord< bool >( "validCocktail" ) ) {
      double const mass = muon->getMass();

      double const pxCocktail = muon->findUserRecord< double >( "pxCocktail" );
      double const pyCocktail = muon->findUserRecord< double >( "pyCocktail" );
      double const pzCocktail = muon->findUserRecord< double >( "pzCocktail" );

      double const E = std::sqrt( mass * mass +
                                  pxCocktail * pxCocktail +
                                  pyCocktail * pyCocktail +
                                  pzCocktail * pzCocktail
                                  );

      muon->setP4( pxCocktail, pyCocktail, pzCocktail, E );
   }
}
