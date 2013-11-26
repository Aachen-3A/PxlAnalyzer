#ifndef JETRESOLUTION
#define JETRESOLUTION

#include "TH1D.h"
#include "TRandom3.h"

#include "Tools/MConfig.hh"

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
      TH1D initHisto();

      Tools::MConfig const m_jet_res_config;

      bool const m_abs_eta;
      double const m_sigma_MC;

      bool m_use_overflow;
      bool m_use_underflow;

      TH1D m_ratios;

      TRandom3 m_rand;
};

#endif /*JETRESOLUTION*/
