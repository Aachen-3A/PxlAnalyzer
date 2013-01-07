#ifndef EVENTCLEANING
#define EVENTCLEANING

#include <vector>

// Class to check for duplicate reconstructed objects and to decide for only one of them.
// Must be applied AFTER final physics object selection and BEFORE check of event topology and redoing the gen-rec matching.
// The actual pxl::Particles are removed from the containers and from the EventView.
// More information on Event Cleaning (please read):
// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePATCrossCleaning

namespace pxl {
   class Particle;
}

namespace Tools {
   class MConfig;
}

class EventCleaning {
public:
   EventCleaning( Tools::MConfig const &cfg );
   ~EventCleaning() {}

   // Main function to be called from outside with given set of particles.
   void cleanEvent( std::vector< pxl::Particle* > &muos,
                    std::vector< pxl::Particle* > &eles,
                    std::vector< pxl::Particle* > &taus,
                    std::vector< pxl::Particle* > &gams,
                    std::vector< pxl::Particle* > &jets,
                    bool const isRec
                    ) const;
private:
   // Remove object pointed at by iterator from the vector.
   // Also remove the object from the EventView.
   // The iterator is decremented, so it is still valid.
   void removeParticle( std::vector< pxl::Particle* > &particles,
                        std::vector< pxl::Particle* >::iterator &particle ) const;

   // Remove duplicate muons (ghosts).
   void cleanMuos( std::vector< pxl::Particle* > &muons, bool const isRec ) const;

   // Remove electron duplicates, clean muons from electrons.
   void cleanEles( std::vector< pxl::Particle* > &eles,
                   std::vector< pxl::Particle* > const &muos,
                   bool const isRec
                   ) const;

   // Remove gamma duplicates, clean electrons and muons from gammas.
   void cleanGams( std::vector< pxl::Particle* > &gams,
                   std::vector< pxl::Particle* > const &eles,
                   std::vector< pxl::Particle* > const &muos,
                   bool const isRec
                   ) const;

   // In Particle Flow "everything" is potentially a jet, so check against all
   // objects and remove every jet that is in close proximity to anything.
   // ATTENTION: jet-jet not checked atm., jets should be non-overlapping by
   // design in kt algorithm and for anti-kt it is OK.
   // For details on anti-kt, see also:
   // http://iopscience.iop.org/1126-6708/2008/04/063
   void cleanJets( std::vector< pxl::Particle* > &jets,
                   std::vector< pxl::Particle* > const &gams,
                   std::vector< pxl::Particle* > const &eles,
                   std::vector< pxl::Particle* > const &muos,
                   std::vector< pxl::Particle* > const &taus
                   ) const;

   // Checks mu-mu overlap and returns -1,0,1 to indicate the removal of the
   // first, none or the second particle, respectively.
   // Chooses the mu with lower chi2/ndf.
   // For Gen: Choose the mu with higher pT.
   int checkMuonOverlap( pxl::Particle const *paI,
                         pxl::Particle const *paJ,
                         bool const isRec
                         ) const;

   // Checks ele-ele overlap and returns -1,0,1 to indicate the removal of the
   // first, none or the second particle, respectively.
   // Chooses ele with higher energy.
   int checkElectronOverlap( pxl::Particle const *paI,
                             pxl::Particle const *paJ,
                             bool const isRec
                             ) const;

   // Checks gamma-gamma overlap and returns -1,0,1 to indicate the removal of
   // the first, none or the second particle, respectively.
   // Chooses gamma with higher energy.
   int checkGammaOverlap( pxl::Particle const *paI,
                          pxl::Particle const *paJ,
                          bool const isRec
                          ) const;

   // Check which particle has the lower chi2/ndf and return -1,0,1 to indicate
   // the removal of the first, none or the second particle, respectively.
   int checkNormChi2( pxl::Particle const *p1,
                      pxl::Particle const *p2
                      ) const;

   // Alternative to simply checking for smallest chi2/ndof.
   int checkProbability( pxl::Particle const *p1,
                         pxl::Particle const *p2
                         ) const;

   // Returns true if particle2 is within DeltaR_max of particle1.
   bool checkParticleOverlap( pxl::Particle const *p1,
                              pxl::Particle const *p2,
                              double const DeltaR_max
                              ) const;

   // Returns true if seedID is same for both particles.
   bool checkSeedOverlap( pxl::Particle const *p1,
                          pxl::Particle const *p2
                          ) const;

   // Returns true if TrackerP is the same for both particles.
   bool checkTrack( pxl::Particle const *p1,
                    pxl::Particle const *p2
                    ) const;

   // Parameters controlling overlaps.
   bool const m_muo_cleanDuplicates;

   double const m_muo_DeltaR_max;
   double const m_ele_DeltaR_max;
   double const m_tau_DeltaR_max;
   double const m_gam_DeltaR_max;
   double const m_jet_DeltaR_max;
};

#endif /*EVENTCLEANING*/
