#ifndef GENSELECTOR
#define GENSELECTOR

// Class to apply cuts on generator level, which can be given in a config file.
// Several different criteria can be cut on:
//   - generator binning, e.g. pT-hat (pT of the hard interaction)
//   - mass of the resonance particle
//   - pT of the resonance particle


#include <vector>

#include "Tools/MConfig.hh"
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

class GenSelector {
public:
   typedef std::vector< pxl::Particle* > pxlParticles;

   GenSelector( Tools::MConfig const &cfg );
   ~GenSelector() {}

   // Apply all Generator Cuts.
   bool passGeneratorCuts( pxl::EventView const *EvtView,
                           pxlParticles const &s3_particles
                           ) const;

private:
   // Apply cut on generator binning (usually pT-hat).
   bool passBinningCuts( pxl::EventView const *EvtView ) const;
   // Apply cuts on invariant mass of resonance particle.
   bool passMassCuts( pxlParticles const &s3_particles ) const;
   // Apply cuts on transverse momentum of resonance particle.
   bool passTransverseMomentumCuts( pxlParticles const &s3_particles ) const;
   // Check if the given number of particles is sensible.
   bool CheckNumberOfParticles( pxlParticles const &s3_particlesSelected ) const;

   // Cut on generator binning.
   double const m_binningValue_max;
   // Cuts on invariant mass of resonance particle.
   double const m_mass_min;
   double const m_mass_max;
   std::vector< int > m_mass_IDs;
   std::vector< int > m_mass_MotherIDs;
   // Cuts on transverse momentum of resonance particle.
   double const m_pt_min;
   double const m_pt_max;
   std::vector< int > m_pt_IDs;
   std::vector< int > m_pt_MotherIDs;
};

#endif /*GENSELECTOR*/
