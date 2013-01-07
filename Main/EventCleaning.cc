#include "EventCleaning.hh"

#include "TMath.h"

#include "Tools/PXL/PXL.hh"
#include "Tools/MConfig.hh"


EventCleaning::EventCleaning( Tools::MConfig const &cfg ) :
   m_muo_cleanDuplicates( cfg.GetItem< bool >( "Muon.cleanDuplicates" ) ),

   m_muo_DeltaR_max( cfg.GetItem< double >( "Muon.DeltaR.max" ) ),
   m_ele_DeltaR_max( cfg.GetItem< double >( "Ele.DeltaR.max" ) ),
   m_tau_DeltaR_max( cfg.GetItem< double >( "Tau.DeltaR.max" ) ),
   m_gam_DeltaR_max( cfg.GetItem< double >( "Gamma.DeltaR.max" ) ),
   m_jet_DeltaR_max( cfg.GetItem< double >( "Jet.DeltaR.max" ) )
{
}


void EventCleaning::cleanEvent( std::vector< pxl::Particle* > &muos,
                                std::vector< pxl::Particle* > &eles,
                                std::vector< pxl::Particle* > &taus,
                                std::vector< pxl::Particle* > &gams,
                                std::vector< pxl::Particle* > &jets,
                                bool const isRec
                                ) const {
   cleanMuos( muos, isRec );
   cleanEles( eles, muos, isRec );
   cleanGams( gams, eles, muos, isRec );
   cleanJets( jets, gams, eles, muos, taus );
}


void EventCleaning::removeParticle( std::vector< pxl::Particle* > &particles,
                                    std::vector< pxl::Particle* >::iterator &particle
                                    ) const {
   // Remove the particle from the EventView!
   (*particle)->owner()->remove( *particle );

   // Erasing an object from a vector invalidates the iterator,
   // hence go one back to find the new next one during the next increment.
   particles.erase( particle-- );
}


void EventCleaning::cleanMuos( std::vector< pxl::Particle* > &muos,
                               bool const isRec
                               ) const {
   std::vector< pxl::Particle* >::iterator part1;
   std::vector< pxl::Particle* >::iterator part2;

   // It is not perfectly clear, if we really want to clean muons against muons.
   // In general, there can be ambiguities in the reconstruction, but e.g. for
   // high-pt Z candidates you expect two muons to be very close in Delta R.
   // TODO: We need a study/reference here.
   if( m_muo_cleanDuplicates ) {
      for( part1 = muos.begin(); part1 != muos.end(); ++part1 ) {
         // Start inner loop with "next" particle.
         for( part2 = part1 + 1; part2 != muos.end(); ++part2 ) {
            int const ret = checkMuonOverlap( *part1, *part2, isRec );
            if( ret == -1 ) {
               removeParticle( muos, part1 );

               // No use to continue inner loop once part1 is removed.
               break;
            } else if( ret == 1 ) {
               removeParticle( muos, part2 );
            }
         }
      }
   }
}


void EventCleaning::cleanEles( std::vector< pxl::Particle* > &eles,
                               std::vector< pxl::Particle* > const &muos,
                               bool const isRec
                               ) const {
   std::vector< pxl::Particle* >::iterator part1;
   std::vector< pxl::Particle* >::iterator part2;

   for( part1 = eles.begin(); part1 != eles.end(); ++part1 ) {
      // Start inner loop with next particle
      for( part2 = part1 + 1; part2 != eles.end(); ++part2 ) {
         int const ret = checkElectronOverlap( *part1, *part2, isRec );
         if( ret == -1 ) {
            removeParticle( eles, part1 );

            // No use to continue inner loop once part1 is removed.
            break;
         } else if( ret == 1 ) {
            removeParticle( eles, part2 );
         }
      }
   }

   // NOTE: As of end 2013, the official muon reconstruction does not handle
   // bremsstrahlung photons reliably/at all. Thus, we remove all electrons that
   // are close to a muon because they might be a fake arising from a radiated
   // photon. This should be updated if the algorithms change.

   // Remove eles in proximity to muons.
   std::vector< pxl::Particle* >::iterator ele;
   for( ele = eles.begin(); ele != eles.end(); ++ele ) {
      std::vector< pxl::Particle* >::const_iterator muo;
      for( muo = muos.begin(); muo != muos.end(); ++muo ) {
         if( checkParticleOverlap( *ele, *muo, m_muo_DeltaR_max ) ) {
            removeParticle( eles, ele );

            // No use to continue inner loop once ele is removed.
            break;
         }
      }
   }
}


void EventCleaning::cleanGams( std::vector< pxl::Particle* > &gams,
                               std::vector< pxl::Particle* > const &eles,
                               std::vector< pxl::Particle* > const &muos,
                               bool const isRec
                               ) const {
   // Overlap removal of gammas:
   std::vector< pxl::Particle* >::iterator part1;
   std::vector< pxl::Particle* >::iterator part2;

   for( part1 = gams.begin(); part1 != gams.end(); ++part1 ) {
      for( part2 = part1 + 1; part2 != gams.end(); ++part2 ) {
         int const ret = checkGammaOverlap( *part1, *part2, isRec );
         if( ret == -1 ) {
            removeParticle( gams, part1 );

            // No use to continue inner loop once part1 is removed.
            break;
         } else if( ret == 1 ) {
            removeParticle( gams, part2 );
         }
      }
   }

   // Remove gammas in proximity to electrons.
   std::vector< pxl::Particle* >::iterator gam;
   for( gam = gams.begin(); gam != gams.end(); ++gam ) {
      std::vector< pxl::Particle* >::const_iterator ele;
      for( ele = eles.begin(); ele != eles.end(); ++ele ) {
         if( checkParticleOverlap( *gam, *ele, m_ele_DeltaR_max ) and
             ( not isRec or checkSeedOverlap( *gam, *ele ) )
             ) {
            removeParticle( gams, gam );

            // No use to continue inner loop once gamma is removed.
            break;
         }
      }
   }

   // NOTE: As of end 2013, the official muon reconstruction does not handle
   // bremsstrahlung photons reliably/at all. Thus, we remove all photons that
   // are close to a muon. This should be updated if the algorithms change.

   // Remove gammas in proximity to muons.
   for( gam = gams.begin(); gam != gams.end(); ++gam ) {
      std::vector< pxl::Particle* >::const_iterator muo;
      for( muo = muos.begin(); muo != muos.end(); ++muo ) {
         if( checkParticleOverlap( *gam, *muo, m_muo_DeltaR_max ) ) {
            removeParticle( gams, gam );

            // No use to continue inner loop once gamma is removed.
            break;
         }
      }
   }
}


void EventCleaning::cleanJets( std::vector< pxl::Particle* > &jets,
                               std::vector< pxl::Particle* > const &gams,
                               std::vector< pxl::Particle* > const &eles,
                               std::vector< pxl::Particle* > const &muos,
                               std::vector< pxl::Particle* > const &taus
                               ) const {
   std::vector< pxl::Particle* >::iterator jet;
   for( jet = jets.begin(); jet != jets.end(); ++jet ) {
      std::vector< pxl::Particle* >::const_iterator muo;
      for( muo = muos.begin(); muo != muos.end(); ++muo ) {
         if( checkParticleOverlap( *jet, *muo, m_muo_DeltaR_max ) ) {
            removeParticle( jets, jet );

            // No use to continue inner loop once jet is removed.
            break;
         }
      }
   }

   for( jet = jets.begin(); jet != jets.end(); ++jet ) {
      std::vector< pxl::Particle* >::const_iterator ele;
      for( ele = eles.begin(); ele != eles.end(); ++ele ) {
         if( checkParticleOverlap( *jet, *ele, m_ele_DeltaR_max ) ) {
            removeParticle( jets, jet );

            // No use to continue inner loop once jet is removed.
            break;
         }
      }
   }

   for( jet = jets.begin(); jet != jets.end(); ++jet ) {
      std::vector< pxl::Particle* >::const_iterator gam;
      for( gam = gams.begin(); gam != gams.end(); ++gam ) {
         if( checkParticleOverlap( *jet, *gam, m_gam_DeltaR_max ) ) {
            removeParticle( jets, jet );

            // No use to continue inner loop once jet is removed.
            break;
         }
      }
   }

   for( jet = jets.begin(); jet != jets.end(); ++jet ) {
      std::vector< pxl::Particle* >::const_iterator tau;
      for( tau = taus.begin(); tau != taus.end(); ++tau ) {
         if( checkParticleOverlap( *jet, *tau, m_tau_DeltaR_max ) ) {
            removeParticle( jets, jet );

            // No use to continue inner loop once jet is removed.
            break;
         }
      }
   }
}


int EventCleaning::checkMuonOverlap( pxl::Particle const *paI,
                                     pxl::Particle const *paJ,
                                     bool const isRec
                                     ) const {
   // Check particle DeltaR.
   if( checkParticleOverlap( paI, paJ, m_muo_DeltaR_max ) ) {
      if( isRec ) {
         try {
            // In future, get the track prbability from chi2 and ndf.
            return checkProbability( paI, paJ );
         } catch( std::runtime_error ) {
            // So far decide using smallest chi2/ndf, probably not the best choice...
            return checkNormChi2( paI, paJ );
         }
      } else {
         //for gen: take the harder one
         double const ptI = paI->getPt();
         double const ptJ = paJ->getPt();
         return ptI < ptJ ? -1 : 1;
      }
   }
   return 0;
}


int EventCleaning::checkElectronOverlap( pxl::Particle const *paI,
                                         pxl::Particle const *paJ,
                                         bool const isRec
                                         ) const {
   // Check particle DeltaR.
   if( checkParticleOverlap( paI, paJ, m_ele_DeltaR_max ) ) {
      // Check if both electrons have same SC-seed (and different track) or same track (and different SuperCluster).
      // For Gen: Assume electrons to be the same.
      if( not isRec or
          checkSeedOverlap( paI, paJ ) or
          checkTrack( paI, paJ )
          ) {
         double const eI = paI->getE();
         double const eJ = paJ->getE();

         return eI < eJ ? -1 : 1;
      }
   }
   return 0;
}


int EventCleaning::checkGammaOverlap( pxl::Particle const *paI,
                                      pxl::Particle const *paJ,
                                      bool const isRec
                                      ) const {
   // Check DeltaR.
   if( checkParticleOverlap( paI, paJ, m_gam_DeltaR_max ) and
       ( not isRec or checkSeedOverlap( paI, paJ ) )
       ) {
      // So far decide using higher energy, probably not the best choice...
      double const eI = paI->getE();
      double const eJ = paJ->getE();

      return eI < eJ ? -1 : 1;
   }
   return 0;
}


int EventCleaning::checkNormChi2( pxl::Particle const *p1,
                                  pxl::Particle const *p2
                                  ) const {
   double const normChi2_1 = p1->findUserRecord< double >( "NormChi2" );
   double const normChi2_2 = p2->findUserRecord< double >( "NormChi2" );

   return normChi2_1 >= normChi2_2 ? -1 : 1;
}


int EventCleaning::checkProbability( pxl::Particle const *p1,
                                     pxl::Particle const *p2
                                     ) const {
   double const chi2_1 = p1->findUserRecord< double >( "chi2" );
   double const ndof_1 = p1->findUserRecord< double >( "ndof" );

   double const chi2_2 = p2->findUserRecord< double >( "chi2" );
   double const ndof_2 = p2->findUserRecord< double >( "ndof" );

   double const prob_1 = TMath::Prob( chi2_1, ndof_1 );
   double const prob_2 = TMath::Prob( chi2_2, ndof_2 );

   // Higher probability wins.
   return prob_1 < prob_2 ? -1 : 1;
}


bool EventCleaning::checkParticleOverlap( pxl::Particle const *p1,
                                          pxl::Particle const *p2,
                                          double const DeltaR_max
                                          ) const {
   pxl::LorentzVector const vec1 = p1->getVector();
   pxl::LorentzVector const vec2 = p2->getVector();

   double const deltaR = vec1.deltaR( &vec2 );
   if( deltaR < DeltaR_max ) return true;

   return false;
}


bool EventCleaning::checkSeedOverlap( pxl::Particle const *p1,
                                      pxl::Particle const *p2
                                      ) const {
   unsigned int const seedID1 = p1->findUserRecord< unsigned int >( "seedId" );
   unsigned int const seedID2 = p2->findUserRecord< unsigned int >( "seedId" );

   if( seedID1 == seedID2 ) return true;

   return false;
}


bool EventCleaning::checkTrack( pxl::Particle const *p1,
                                pxl::Particle const *p2
                                ) const {
   double const TrackerP1 = p1->findUserRecord< double >( "TrackerP" );
   double const TrackerP2 = p2->findUserRecord< double >( "TrackerP" );

   if( TrackerP1 == TrackerP2 ) return true;

   return false;
}
