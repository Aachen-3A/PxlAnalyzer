#ifndef MuonSelector_hh
#define MuonSelector_hh


/*

This class contains all the muon selections

*/


#include <string>
#include <map>
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"
#include "Tools/MConfig.hh"
#include "Main/EffectiveArea.hh"


class MuonSelector {
public:
    MuonSelector(const Tools::MConfig &config );
    // Destruktor
    ~MuonSelector();
    int passMuon( pxl::Particle *muon, const bool& isRec ,double const rho=0. ) const;

private:
    // Methods;
    bool kinematics(pxl::Particle *muon ) const;
    int muonID( pxl::Particle *muon , double rho) const;
    bool tightMuonIDCut(pxl::Particle *muon) const;
    bool HighptMuonIDCut(pxl::Particle *muon) const;
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
    //cut variables
    int const           m_globalChi2_max;
    int const           m_nMuonHits_min;
    int const           m_nMatchedStations_min;
    double const        m_zImpactParameter_max;
    double const        m_xyImpactParameter_max;
    int const           m_nPixelHits_min;
    int const           m_nTrackerLayersWithMeas_min;
    double const        m_dPtRelTrack_max;

    // High Pt ID variables
    // const bool        m_muo_highptid_useBool;
    // const std::string m_muo_highptid_boolName
    // const bool        m_muo_highptid_isGlobalMuon;
    // const double      m_muo_highptid_PtRelativeError_max;
    // const int         m_muo_highptid_NMatchedStations_min;
    // const int         m_muo_highptid_VHitsMuonSys_min;
    // const int         m_muo_highptid_VHitsPixel_min;
    // const int         m_muo_highptid_VHitsTracker_min;
    // const double      m_muo_highptid_Dxy_max;
    // const double      m_muo_highptid_Dz_max;

    bool mutable        m_useAlternative;
    std::map<std::string,std::string> mutable m_alternativeUserVariables;
};
#endif
