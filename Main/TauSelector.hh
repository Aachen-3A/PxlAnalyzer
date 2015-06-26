#ifndef TauSelector_hh
#define TauSelector_hh


/*

This class contains all the muon selections

*/
#include <string>
#include "Tools/MConfig.hh"
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"


class TauSelector {
public:
    // Constructor
    TauSelector(const Tools::MConfig &config );
    // Destruktor
    ~TauSelector();
    bool passTau( pxl::Particle *tau, const bool& isRec) const;

private:

    double const  m_tau_pt_min;
    double const  m_tau_eta_max;
    // Discriminators:
    std::vector< std::string > const m_tau_discriminators;
};
#endif
