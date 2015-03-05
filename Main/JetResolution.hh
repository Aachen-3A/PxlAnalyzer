#ifndef JETRESOLUTION
#define JETRESOLUTION

#include "TRandom3.h"
#include "TFormula.h"

#include "Tools/MConfig.hh"
#include "BinnedMapping.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#pragma GCC diagnostic pop

namespace pxl {
    class Particle;
}

class JetResolution {
    public:
        JetResolution( Tools::MConfig const &config );
        ~JetResolution() {}

        double getScalingFactor( double const eta ) const;
        double getJetPtCorrFactor( pxl::Particle const *recJet,
                                            pxl::Particle const *genJet,
                                            double truthpu
                                            );
        double getSigmaMC( double const pt, double const eta, double const pu ) const;

    private:
        Tools::MConfig const m_jet_res_config;

        BinnedMapping const m_eta_corr_map;

        TRandom3 m_rand;

        JetCorrectorParameters CorParr;

        double m_sigma_MC;

        TFormula *mFunc;
};

#endif /*JETRESOLUTION*/
