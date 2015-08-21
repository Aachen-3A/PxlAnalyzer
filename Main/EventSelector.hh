#ifndef EventSelector_hh
#define EventSelector_hh

/*

This class performs the Final Event Selection based on Final Cuts and Trigger
Decision.

*/
#include <string>
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"
#include "Tools/MConfig.hh"
#include "TriggerSelector.hh"
#include "MuonSelector.hh"
#include "EleSelector.hh"
#include "TauSelector.hh"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#pragma GCC diagnostic pop

#include "Main/GenSelector.hh"
#include "Main/EventCleaning.hh"
#include "Main/GenRecNameMap.hh"
#include "Main/EffectiveArea.hh"



class EventSelector {
public:
   EventSelector( const Tools::MConfig &config );

   // Destruktor
   ~EventSelector();
   // main method to perform the selection
   void performSelection(pxl::EventView*  EvtView, pxl::EventView* TrigEvtView, pxl::EventView* FilterView, const int& JES);
   //synchronize certain values between gen and rec event views
   void preSynchronizeGenRec( pxl::EventView *GenEvtView, pxl::EventView *RecEvtView );
   void synchronizeGenRec( pxl::EventView* GenEvtView, pxl::EventView* RecEvtView );
   // calculate transverse invariant mass of particle of Type1 and Type2
   double TransverseInvariantMass(pxl::EventView* GenEvtView, const std::string& type1, const std::string& type2);
   // calculate invariant mass of particle of Type1 and Type2
   double InvariantMass(pxl::EventView* GenEvtView, const std::string& type1, const std::string& type2);

   bool passEventTopology( int const numMuo,
                           int const numEle,
                           int const numTau,
                           int const numGam,
                           int const numJet,
                           int const numMET
                           ) const;
   TriggerSelector const &getTriggerSelector() const { return m_triggerSelector; }
   void checkOrder( std::vector< pxl::Particle* > const &particles ) const;

private:
    // Methods;
    bool passEventTopology( std::vector< pxl::Particle* > const &muos,
                                    std::vector< pxl::Particle* > const &eles,
                                    std::vector< pxl::Particle* > const &taus,
                                    std::vector< pxl::Particle* > const &gams,
                                    std::vector< pxl::Particle* > const &jets,
                                    std::vector< pxl::Particle* > const &mets
                                    ) const;
    bool passFilterSelection( pxl::EventView *EvtView, const bool isRec );
    bool passFilterSelection( pxl::EventView *EvtView );
    // perform cuts on Particle Level
    //ATTENTION: changes particle vector!
    void applyCutsOnMuon( std::vector< pxl::Particle* > &muons, double const muoRho, const bool& isRec );


    void applyCutsOnEle( std::vector< pxl::Particle* > &eles,
                                double const eleRho,
                                bool const &isRec
                                ) const;
    //bool passEle( pxl::Particle const *ele,
    //                  double const eleRho,
    //                  bool const isRec
    //                  ) const;
    //bool passCBID( pxl::Particle const *ele,
    //                    double const elePt,
    //                    double const eleAbsEta,
    //                    bool const eleBarrel,
    //                    bool const eleEndcap,
    //                    double const eleRho
    //                    ) const;
    //bool passHEEPID( pxl::Particle const *ele,
    //                      double const eleEt,
    //                      bool const eleBarrel,
    //                      bool const eleEndcap,
    //                      double const eleRho
    //                      ) const;

    void applyCutsOnTau( std::vector< pxl::Particle* > &taus, const bool& isRec );

    void applyCutsOnGam( std::vector< pxl::Particle* > &gammas,
                                double const gamRho,
                                bool const isRec
                                ) const;
    bool passGam( pxl::Particle const *gam,
                      double const gamRho,
                      bool const isRec
                      ) const;
    bool passVgamma2011PhotonID( pxl::Particle const *gam,
                                          double const gamRho,
                                          bool const barrel,
                                          bool const endcap
                                          ) const;
    bool passCutBasedPhotonID2012( pxl::Particle const *gam,
                                             double const gamRho,
                                             bool const barrel,
                                             bool const endcap
                                             ) const;

    void applyCutsOnJet( std::vector< pxl::Particle* > &jets, const bool &isRec );
    bool passJet( pxl::Particle *jet, const bool &isRec ) const;

    void countParticles( pxl::EventView *EvtView,
                                std::vector< pxl::Particle* > const &particles,
                                std::string const &name,
                                bool const &isRec
                                ) const;
    void countJets( pxl::EventView *EvtView, std::vector< pxl::Particle* > &jets, const bool &isRec );
    void applyCutsOnMET( std::vector< pxl::Particle* > &mets, const bool &isRec);
    bool passMET( pxl::Particle *met, const bool &isRec ) const;
    //cuts on primary vertices
    void applyCutsOnVertex( pxl::EventView* EvtView, std::vector< pxl::Vertex* > &vertices, const bool &isRec );
    //check for global effects, i.e. calo noise
    bool applyGlobalEventCuts( pxl::EventView* EvtView,
                                        std::vector< pxl::Vertex* > &vertices,
                                        std::vector< pxl::Particle* > &eles,
                                        std::vector< pxl::Particle* > &mets
                                        );
    // Perform "cuts" on topology.
    bool applyCutsOnTopology( std::vector< pxl::Particle* > const &muons,
                                      std::vector< pxl::Particle* > const &eles,
                                      std::vector< pxl::Particle* > const &taus,
                                      std::vector< pxl::Particle* > const &gammas,
                                      std::vector< pxl::Particle* > const &jets,
                                      std::vector< pxl::Particle* > const &mets
                                      );

    // debug method
    void dumpEventView(const pxl::EventView* EvtView);
    // vary Jet Energy Scale
    void varyJES( std::vector< pxl::Particle* > const &jets,
                      int const JES,
                      bool const isRec
                      );
    void varyJESMET( std::vector< pxl::Particle* > const &jets,
                          std::vector< pxl::Particle* > const &mets,
                          int const JES,
                          bool const isRec
                          );

    /////////////////////////////////////////////////////////////////////////////
    ////////////////////////// Selection variables: /////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    // General selection:

    bool const m_data;
    // Don't check for overlaps?
    bool const m_ignoreOverlaps;
    // Running on FASTSIM?
    bool const m_runOnFastSim;
    //Use the music triiger system?
    bool const m_useTrigger;


    // Generator selection:
    bool const m_gen_use;
    GenSelector const m_gen_accept;

    // Filters
    std::string const                     m_filterSet_name;
    std::vector< std::string > const m_filterSet_genList;
    std::vector< std::string > const m_filterSet_recList;
    std::vector< std::string > const m_filterHLT_recList;

    // Primary vertex:
    int const     m_PV_num_min;
    double const m_PV_z_max;
    double const m_PV_rho_max;
    double const m_PV_ndof_min;

    std::string const m_rho_use;

    // Tracks:
    bool const            m_tracks_use;
    std::string const  m_tracks_type;
    unsigned int const m_tracks_num_max;

     // Muons:
    bool const          m_muo_use;
    bool const          m_muo_idtag;
    std::string const m_muo_rho_label;
    MuonSelector const m_muo_selector;
    //double const        m_muo_pt_min;
    //double const        m_muo_eta_max;
    //bool const          m_muo_invertIso;
    //bool const          m_muo_requireIsGlobal;
    //bool const          m_muo_requireIsTracker;
    //bool const          m_muo_requireIsPF;
    //std::string const m_muo_iso_type;
    //double const        m_muo_iso_max;
    //bool const          m_muo_iso_useDeltaBetaCorr;
    //int const            m_muo_NPixelHits_min;
    //int const            m_muo_NMuonHits_min;
    //int const            m_muo_NMatchedStations_min;
    //int const            m_muo_NTrackerLayersWithMeas_min;
    //double const        m_muo_XYImpactParameter_max;
    //double const        m_muo_ZImpactParameter_max;
    //double const        m_muo_globalChi2_max;
    //bool const          m_muo_HighPtMuonID_use;
    //double const        m_muo_dPtRelTrack_max;

    // Electrons:
    bool const          m_ele_use;
    bool const          m_ele_idtag;
    std::string const m_ele_rho_label;
    EleSelector const m_ele_selector;

    // Taus:
    bool const    m_tau_use;
    TauSelector const m_tau_selector;


    // Photons:
    bool const    m_gam_use;
    double const m_gam_pt_min;
    double const m_gam_eta_barrel_max;
    double const m_gam_eta_endcap_min;
    double const m_gam_eta_endcap_max;
    bool const    m_gam_useEndcap;
    bool const    m_gam_useConverted;
    bool const    m_gam_rejectOutOfTime;
    double const m_gam_corrFactor_max;

    double const m_gam_barrel_sigmaIetaIeta_min;
    double const m_gam_barrel_sigmaIetaIeta_max;
    double const m_gam_endcap_sigmaIetaIeta_max;

    bool const    m_gam_CutBasedPhotonID2012Flag_use;
    std::string const m_gam_IDFlag;


    // CutBasedPhotonID2012:
    bool const m_gam_CutBasedPhotonID2012_use;
    EffectiveArea const m_gam_EA;
    std::string const m_gam_rho_label;
    // Barrel:
    bool const    m_gam_barrel_electronVeto_require;
    double const m_gam_barrel_HoEm2012_max;
    double const m_gam_barrel_PFIsoChargedHadron_max;
    double const m_gam_barrel_PFIsoNeutralHadron_offset;
    double const m_gam_barrel_PFIsoNeutralHadron_slope;
    double const m_gam_barrel_PFIsoPhoton_offset;
    double const m_gam_barrel_PFIsoPhoton_slope;
    // Endcap:
    bool const    m_gam_endcap_electronVeto_require;
    double const m_gam_endcap_HoEm2012_max;
    double const m_gam_endcap_PFIsoChargedHadron_max;
    double const m_gam_endcap_PFIsoNeutralHadron_offset;
    double const m_gam_endcap_PFIsoNeutralHadron_slope;
    double const m_gam_endcap_PFIsoPhoton_offset;
    double const m_gam_endcap_PFIsoPhoton_slope;

    // Vgamma2011PhotonID:
    bool const    m_gam_Vgamma2011PhotonID_use;
    bool const    m_gam_useSeedVeto;
    double const m_gam_HoEm_max;
    // Barrel:
    double const m_gam_barrel_TrkIso_offset;
    double const m_gam_barrel_TrkIso_slope;
    double const m_gam_barrel_TrkIso_rhoSlope;
    double const m_gam_barrel_EcalIso_offset;
    double const m_gam_barrel_EcalIso_slope;
    double const m_gam_barrel_EcalIso_rhoSlope;
    double const m_gam_barrel_HcalIso_offset;
    double const m_gam_barrel_HcalIso_slope;
    double const m_gam_barrel_HcalIso_rhoSlope;
    // Endcap:
    double const m_gam_endcap_TrkIso_offset;
    double const m_gam_endcap_TrkIso_slope;
    double const m_gam_endcap_TrkIso_rhoSlope;
    double const m_gam_endcap_EcalIso_offset;
    double const m_gam_endcap_EcalIso_slope;
    double const m_gam_endcap_EcalIso_rhoSlope;
    double const m_gam_endcap_HcalIso_offset;
    double const m_gam_endcap_HcalIso_slope;
    double const m_gam_endcap_HcalIso_rhoSlope;

    // ID:
    bool const          m_gam_ID_use;
    std::string const m_gam_ID_name;

    // Jets:
    bool const          m_jet_use;
    double const        m_jet_pt_min;
    double const        m_jet_eta_max;
    bool const          m_jet_isPF;
    bool const          m_jet_ID_use;
    std::string const m_jet_ID_name;
    double const        m_jet_gen_hadOverEm_min;
    double const        m_jet_gen_hadEFrac_min;

    // In case we do the ID on our own:
    double const m_jet_nHadEFrac_max;
    double const m_jet_nEMEFrac_max;
    unsigned long const     m_jet_numConstituents_min;
    double const m_jet_cHadEFrac_min;
    double const m_jet_cEMEFrac_max;
    unsigned long const     m_jet_cMultiplicity_min;

    bool const          m_jet_bJets_use;
    std::string const m_jet_bJets_algo;
    double const        m_jet_bJets_discr_min;
    std::string const m_jet_bJets_genFlavourAlgo;
    std::string const m_jet_bJets_gen_label;

    // MET:
    bool const    m_met_use;
    double const m_met_pt_min;
    double const m_met_dphi_ele_min;

    //HCAL noise ID
    bool const          m_hcal_noise_ID_use;
    std::string const m_hcal_noise_ID_name;

    /////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// Other variables: /////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    // To access the JEC uncertainties from file.
    // New recipe:
    // https://twiki.cern.ch/twiki/bin/view/CMS/JECUncertaintySources?rev=19#Code_example
    std::string const m_jecType;
    JetCorrectorParameters const m_jecPara;
    JetCorrectionUncertainty m_jecUnc;

    // Class mapping Gen and Rec particle names.
    GenRecNameMap const m_gen_rec_map;

    std::string const m_RecMuoName;
    std::string const m_RecEleName;
    std::string const m_RecTauName;
    std::string const m_RecGamName;
    std::string const m_RecJetName;
    std::string const m_RecMETName;

    std::string const m_GenMuoName;
    std::string const m_GenEleName;
    std::string const m_GenTauName;
    std::string const m_GenGamName;
    std::string const m_GenJetName;
    std::string const m_GenMETName;

    EventCleaning const m_eventCleaning;

    TriggerSelector const m_triggerSelector;
};
#endif
