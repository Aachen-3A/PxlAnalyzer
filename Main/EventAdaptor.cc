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
   m_jet_res( cfg ),

   m_muo_useCocktail( cfg.GetItem< bool >( "Muon.UseCocktail" ) ),
   m_jet_res_corr_use( cfg.GetItem< bool >( "Jet.Resolutions.Corr.use" ) ),

   // Cache the particle names.
   m_muo_RecName( m_gen_rec_map.get( "Muo" ).RecName ),
   m_jet_RecName( m_gen_rec_map.get( "Jet" ).RecName ),
   m_met_RecName( m_gen_rec_map.get( "MET" ).RecName )
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


// Obviously, this can only be done on MC!
void EventAdaptor::applyJETMETSmearing( pxl::EventView const *GenEvtView,
                                        pxl::EventView const *RecEvtView,
                                        std::string const linkName
                                        ) {
   if( not m_jet_res_corr_use ) {
      if( m_debug > 0 ) {
         std::stringstream warn;
         warn << "[WARNING] (EventAdaptor): " << std::endl;
         warn << "Using 'applyJETMETSmearing(...)', but config file says: ";
         warn << "'Jet.Resolutions.Corr.use = " << m_jet_res_corr_use << "'";
         warn << std::endl << std::endl;

         std::cerr << warn.str();
      }
   }
   // Get all Rec jets with the specified name in the config file.
   pxlParticles recJets;
   pxl::ParticlePtEtaNameCriterion const critJet( m_jet_RecName );
   pxl::ParticleFilter particleFilter;
   particleFilter.apply( RecEvtView->getObjectOwner(), recJets, critJet );

   if( m_debug > 2 ) {
      std::cerr << "[DEBUG] (EventAdaptor): RecEvtView before:" << std::endl;
      RecEvtView->print( 1, std::cerr );
   }

   if( recJets.size() == 0 ) {
      if( m_debug > 1 ) {
         std::cerr << "[INFO] (EventAdaptor): " << std::endl;
         std::cerr << "No jets in this event, no changes applied!" << std::endl;
      }
      return;
   }

   // Sum up corrections for MET coming from jets.
   double fullCorrPx = 0.0;
   double fullCorrPy = 0.0;

   pxlParticles::const_iterator part = recJets.begin();
   for( ; part != recJets.end(); ++part ) {
      pxl::Particle *recJet = *part;

      // Get all relatives (found by the ParticleMatcher) for this particle.
      pxl::SoftRelations const rel = recJet->getSoftRelations();

      // Get the first matched object. We assume, this is OK, but it would be
      // nice to have a small study here some day.
      pxl::Particle const *matchedJet = dynamic_cast< pxl::Particle* >( rel.getFirst( GenEvtView->getObjectOwner(), linkName ) );

      double const jetPtCorrFactor = m_jet_res.getJetPtCorrFactor( recJet, matchedJet );

      double const E  = recJet->getE();
      double const px = recJet->getPx();
      double const py = recJet->getPy();
      double const pz = recJet->getPz();

      // In case the matched jet pt is too far off, we get jetPtCorrFactor = 0.
      // In case we have no matched jet, the Gaussian can (randomly) give you
      // jetPtCorrFactor < 0.
      // In both cases we remove the Rec jet from the event and correct the MET
      // by the full[!] jet pt.
      if( jetPtCorrFactor <= 0.0 ) {
         fullCorrPx -= px;
         fullCorrPy -= py;

         recJet->owner()->remove( recJet );
      } else {
         fullCorrPx += px * ( jetPtCorrFactor - 1 );
         fullCorrPy += py * ( jetPtCorrFactor - 1 );

         // Changing the jet in the event!
         recJet->setP4( jetPtCorrFactor * px,
                        jetPtCorrFactor * py,
                        jetPtCorrFactor * pz,
                        jetPtCorrFactor * E
                        );
      }
   }

   if( m_debug > 1 ) {
      std::cerr << "[INFO] (EventAdaptor): Cumulated jet corrections:" << std::endl;
      std::cerr << "px = " << fullCorrPx << std::endl;
      std::cerr << "py = " << fullCorrPy << std::endl;
   }

   // Don't have to correct MET, if there were no jets left in the event.
   if( recJets.size() > 0 ) {
      // Get all Rec METs with the specified name in the config file.
      pxlParticles recMETs;
      pxl::ParticlePtEtaNameCriterion const critMET( m_met_RecName );
      particleFilter.apply( RecEvtView->getObjectOwner(),
                                  recMETs,
                                  critMET
                                  );

      // Reuse iterator from above!
      for( part = recMETs.begin(); part != recMETs.end(); ++part ) {
         // NOTE: The MET components and the sum of the corrections (fullCorr)
         // are both signed numbers. The minus sign in the following equations
         // takes all four combinations into account.
         double const newPx = (*part)->getPx() - fullCorrPx;
         double const newPy = (*part)->getPy() - fullCorrPy;

         // Changing the MET in the event!
         (*part)->setP4( newPx,
                         newPy,
                         0.0,
                         std::sqrt( newPx * newPx + newPy * newPy )
                         );
      }
   }

   if( m_debug > 2 ) {
      std::cerr << "[DEBUG] (EventAdaptor): RecEvtView after:" << std::endl;
      RecEvtView->print( 1, std::cerr );
   }
}
