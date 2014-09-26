#ifndef MuonSelector_hh
#define MuonSelector_hh


/*

This class contains all the muon selections

*/


#include <string>
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"
#include "Tools/MConfig.hh"
#include "Main/EffectiveArea.hh"


class MuonSelector {
public:
    MuonSelector(const Tools::MConfig &config );
    // Destruktor
    ~MuonSelector();
    bool passMuon( pxl::Particle *muon, const bool& isRec ,double const rho=0. ) const;

private:
    // Methods;
    bool kinematics(pxl::Particle *muon ) const;
    bool muonID( pxl::Particle *muon , double rho) const;
    //Variables
    double const        m_muo_pt_min;
    double const        m_muo_eta_max;
    bool const          m_muo_invertIso;
    std::string const   m_muo_iso_type;
    double const        m_muo_iso_max;
    bool const          m_muo_iso_useDeltaBetaCorr;
    bool const          m_muo_iso_useRhoCorr;
    std::string const   m_muo_id_type;
    double const        m_muo_HighPtSwitchPt;

   EffectiveArea const m_muo_EA;

};
#endif
