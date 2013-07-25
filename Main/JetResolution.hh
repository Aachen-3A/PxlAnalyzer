#ifndef JETRESOLUTION
#define JETRESOLUTION

#include "TRandom3.h"

#include "Tools/MConfig.hh"
#include "BinnedMapping.hh"

namespace pxl {
   class Particle;
}

class JetResolution {
   public:
      JetResolution( Tools::MConfig const &config );
      ~JetResolution() {}

      double getScalingFactor( double const eta ) const;
      double getJetPtCorrFactor( pxl::Particle const *recJet,
                                 pxl::Particle const *genJet
                                 );

   private:
      Tools::MConfig const m_jet_res_config;

      double const m_sigma_MC;

      BinnedMapping const m_eta_corr_map;

      TRandom3 m_rand;
};

#endif /*JETRESOLUTION*/
