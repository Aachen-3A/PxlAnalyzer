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
    // Methods
    int muonID( pxl::Particle *muon, double rho) const;
    bool passKinematics(pxl::Particle *muon ) const;
    bool passHighPtID(pxl::Particle *muon) const;
    bool passTightID(pxl::Particle *muon) const;
    bool passMediumID(pxl::Particle *muon) const;
    bool passSoftID(pxl::Particle *muon) const;
    bool passPFIso(pxl::Particle *muon, double rho) const;
    bool passMiniIso(pxl::Particle *muon) const;
    bool passTrackerIso(pxl::Particle *muon) const;

    // Variables
    std::string const   m_muo_id_type;
    double const        m_muo_ptSwitch;
    double const        m_muo_pt_min;
    double const        m_muo_eta_max;
    bool const          m_muo_invertIso;

    // Isolation
    std::string const   m_muo_iso_type;
    double const        m_muo_iso_max;
    bool const          m_muo_iso_useDeltaBetaCorr;
    bool const          m_muo_iso_useRhoCorr;

    // Effective area
    EffectiveArea const m_muo_EA;

    // Low Pt Id
    //~ int const           m_globalChi2_max;
    //~ int const           m_nMuonHits_min;
    //~ int const           m_nMatchedStations_min;
    //~ double const        m_zImpactParameter_max;
    //~ double const        m_xyImpactParameter_max;
    //~ int const           m_nPixelHits_min;
    //~ int const           m_nTrackerLayersWithMeas_min;
    //~ double const        m_dPtRelTrack_max;

	//Tight ID variables
	const bool		  m_muo_tightid_useBool;
	const std::string m_muo_tightid_boolName;
	const bool		  m_muo_tightid_isGlobalMuon;
	const bool		  m_muo_tightid_isPFMuon;
	const double	  m_muo_tightid_normalizedChi2_max;
	const int		  m_muo_tightid_vHitsMuonSys_min;
	const int		  m_muo_tightid_nMatchedStations_min;
	const double	  m_muo_tightid_dxy_max;
	const double	  m_muo_tightid_dz_max;
	const int		  m_muo_tightid_vHitsPixel_min;
	const int		  m_muo_tightid_trackerLayersWithMeas_min;
	
    // High Pt ID variables
    const bool        m_muo_highptid_useBool;
    const std::string m_muo_highptid_boolName;
    const bool        m_muo_highptid_isGlobalMuon;
    const double      m_muo_highptid_ptRelativeError_max;
    const int         m_muo_highptid_nMatchedStations_min;
    const int         m_muo_highptid_vHitsMuonSys_min;
    const int         m_muo_highptid_vHitsPixel_min;
    const int         m_muo_highptid_vHitsTracker_min;
    const double      m_muo_highptid_dxy_max;
    const double      m_muo_highptid_dz_max;

	

    //~ bool mutable        m_useAlternative;
    std::map<std::string,std::string> mutable m_alternativeUserVariables;
};
#endif
