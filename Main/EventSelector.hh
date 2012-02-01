#ifndef EventSelector_hh
#define EventSelector_hh

/*

This class performs the Final Event Selection based on Final Cuts and Trigger
Decision.

*/
#include <string>
#include "Tools/PXL/PXL.hh"
#include "Tools/MConfig.hh"
#include "ParticleMatcher.hh"
#include "TriggerInfo.hh"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "Main/DuplicateObjects.hh"

class EventSelector {
public:
   EventSelector( const Tools::MConfig &config );

   // Destruktor
   ~EventSelector();
   // Matching Redo
   void redoMatching(pxl::EventView* GenEvtView, pxl::EventView* RecEvtView);
   // main method to perform the selection
   void performSelection(pxl::EventView*  EvtView, const int& JES);
   //synchronize certain values between gen and rec event views
   void preSynchronizeGenRec( pxl::EventView *GenEvtView, pxl::EventView *RecEvtView );
   void synchronizeGenRec( pxl::EventView* GenEvtView, pxl::EventView* RecEvtView );
   // calculate transverse invariant mass of particle of Type1 and Type2
   double TransverseInvariantMass(pxl::EventView* GenEvtView, const std::string& type1, const std::string& type2);
   // calculate invariant mass of particle of Type1 and Type2
   double InvariantMass(pxl::EventView* GenEvtView, const std::string& type1, const std::string& type2);

   //what MET to use
   std::string getMETType() const { return m_met_type; }
   //what JetAlgo to use
   std::string getJetAlgo() const { return m_jet_algo; }
   //requested number of objects for accepted events
   int getMinNumMuons() const { return m_muo_num_min; }
   int getMinNumEle()   const { return m_ele_num_min; }
   int getMinNumTau()   const { return m_tau_num_min; }
   int getMinNumGamma() const { return m_gam_num_min; }
   int getMinNumJet()   const { return m_jet_num_min; }
   int getMinNumMET()   const { return m_met_num_min; }

   //value of the b-jet discriminator cut
   double getBcut() const { return m_jet_bJets_discr_min; }

   bool checkTopology( const int muons, const int eles, const int taus, const int gammas, const int jets, const int METs );

   void adaptMuons( const pxl::EventView *EvtView );


private:
   std::vector< trigger_info > getTriggerGroups( const Tools::MConfig &config ) const;
   // Methods;
   // check if EventView passes Trigger Selection
   bool passTriggerSelection( pxl::EventView *EvtView,
                              bool const isRec,
                              std::vector< pxl::Particle* > const &muons,
                              std::vector< pxl::Particle* > const &eles,
                              std::vector< pxl::Particle* > const &taus,
                              std::vector< pxl::Particle* > const &gammas,
                              std::vector< pxl::Particle* > const &jets,
                              std::vector< pxl::Particle* > const &mets
                              );
   bool passL1Trigger( pxl::EventView *EvtView, const bool isRec );
   bool passHLTrigger( pxl::EventView *EvtView, const bool isRec );
   //check if any particles after selection passes the trigger thresholds

   bool passFilterSelection( pxl::EventView *EvtView, const bool isRec );
   bool checkVeto( pxl::EventView *EvtView, const bool isRec );
   bool passTriggerParticles( const bool isRec,
                              const std::vector< pxl::Particle* > &muons,
                              const std::vector< pxl::Particle* > &eles,
                              const std::vector< pxl::Particle* > &taus,
                              const std::vector< pxl::Particle* > &gammas,
                              const std::vector< pxl::Particle* > &jets,
                              const std::vector< pxl::Particle* > &mets
                              ) const;
   // perform cuts on Particle Level
   //ATTENTION: changes particle vector!
   void applyCutsOnMuon( pxl::EventView *EvtView, std::vector< pxl::Particle* > &muons, const bool& isRec );
   bool passMuon( pxl::Particle *muon, const bool& isRec );
   void applyCutsOnEle( pxl::EventView *EvtView, std::vector< pxl::Particle* > &eles, const bool &isRec );
   bool passEle( pxl::Particle *ele, const bool& isRec );
   void applyCutsOnTau( pxl::EventView *EvtView, std::vector< pxl::Particle* > &taus, const bool& isRec );
   bool passTau( pxl::Particle *tau, const bool& isRec );
   bool passGamma( pxl::Particle *gam, const bool& isRec );
   void applyCutsOnGamma( pxl::EventView *EvtView, std::vector< pxl::Particle* > &gammas, const bool &isRec );
   void applyCutsOnJet( pxl::EventView *EvtView, std::vector< pxl::Particle* > &jets, const bool &isRec );
   bool passJet( pxl::Particle *jet, const bool &isRec ) const;
   void countJets( pxl::EventView *EvtView, std::vector< pxl::Particle* > &jets, const bool &isRec );
   void applyCutsOnMET( pxl::EventView *EvtView, std::vector< pxl::Particle* > &mets, const bool &isRec);
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

   //cut on binning value
   bool applyGeneratorCuts( pxl::EventView* EvtView, std::vector< pxl::Particle *> &particles );

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

   // Data?
   bool const m_runOnData;
   // Don't check for overlaps?
   bool const m_useCharge;
   bool const m_ignoreOverlaps;

   // Generator selection:

   // Maximal accepted binning value (e.g. pt-hat).
   double const m_binningValue_max;
   // Minimal accepted resonance mass.
   double const m_mass_min;
   // Maximal accepted resonance mass.
   double const m_mass_max;
   // IDs of the particle used for mass calculation.
   std::vector< int > m_massIDs, m_massMotherIDs;

   // Filters
   std::string const                m_filterSet_name;
   std::vector< std::string > const m_filterSet_genList;
   std::vector< std::string > const m_filterSet_recList;

   // Primary vertex:
   int const    m_PV_num_min;
   double const m_PV_z_max;
   double const m_PV_rho_max;
   double const m_PV_ndof_min;

   // Triggers:
   bool const        m_ignoreL1;
   bool const        m_ignoreHLT;
   std::string const m_trigger_prefix;
   std::vector< trigger_info > const m_trigger_groups;

   // Electrons:
   int const    m_ele_num_min;
   double const m_ele_trigger_pt_min;
   double const m_ele_pt_min;
   double const m_ele_eta_barrel_max;
   double const m_ele_eta_endcap_min;
   double const m_ele_eta_endcap_max;
   bool const   m_ele_invertIso;
   bool const   m_ele_requireEcalDriven;
   bool const   m_ele_rejectOutOfTime;
   // Barrel:
   double const m_ele_EoP_max;
   double const m_ele_barrel_deltaEta_max;
   double const m_ele_barrel_deltaPhi_max;
   double const m_ele_barrel_HoEM_max;
   double const m_ele_barrel_trackiso_max;
   double const m_ele_barrel_HcalD1_offset;
   double const m_ele_barrel_HcalD1_slope;
   double const m_ele_barrel_HcalD1_rhoSlope;
   double const m_ele_barrel_NInnerLayerLostHits_max;
   double const m_ele_barrel_swissCross_max;
   double const m_ele_barrel_r19_max;
   double const m_ele_barrel_r29_max;
   double const m_ele_barrel_e1x5_min;
   double const m_ele_barrel_e2x5_min;
   // Endcap:
   double const m_ele_endcap_deltaEta_max;
   double const m_ele_endcap_deltaPhi_max;
   double const m_ele_endcap_HoEM_max;
   double const m_ele_endcap_trackiso_max;
   double const m_ele_endcap_HcalD1_offset;
   double const m_ele_endcap_HcalD1_slope;
   double const m_ele_endcap_HcalD1_rhoSlope;
   double const m_ele_endcap_NInnerLayerLostHits_max;
   double const m_ele_endcap_sigmaIetaIeta_max;
   // ID:
   bool const m_ele_ID_use;
   std::string const m_ele_ID_name;

   // Muons:
   int const         m_muo_num_min;
   double const      m_muo_trigger_pt_min;
   double const      m_muo_pt_min;
   double const      m_muo_eta_max;
   bool const        m_muo_invertIso;
   bool const        m_muo_useCocktail;
   bool const        m_muo_requireIsGlobal;
   bool const        m_muo_requireIsTracker;
   double const      m_muo_dPtRelTrack_max;
   std::string const m_muo_iso_type;
   double const      m_muo_iso_max;
   int const         m_muo_NPixelHits_min;
   int const         m_muo_NMuonHits_min;
   int const         m_muo_NMatchedStations_min;
   int const         m_muo_NTrackerLayersWithMeas_min;
   double const      m_muo_XYImpactParameter_max;
   double const      m_muo_globalChi2_max;

   // Taus:
   const int    m_tau_num_min;
   const double m_tau_pt_min;
   const double m_tau_eta_max;
   // Discriminators:
   const std::vector< std::string > m_tau_discriminators;

   // Photons:
   int const    m_gam_num_min;
   double const m_gam_trigger_pt_min;
   double const m_gam_pt_min;
   double const m_gam_eta_barrel_max;
   double const m_gam_eta_endcap_min;
   double const m_gam_eta_endcap_max;
   bool const   m_gam_useEndcap;
   bool const   m_gam_useConverted;
   bool const   m_gam_useSeedVeto;
   bool const   m_gam_rejectOutOfTime;
   double const m_gam_HoEm_max;
   double const m_gam_corrFactor_max;
   // Barrel:
   double const m_gam_barrel_sigmaIetaIeta_min;
   double const m_gam_barrel_sigmaIetaIeta_max;
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
   double const m_gam_endcap_sigmaIetaIeta_max;
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
   bool const        m_gam_ID_use;
   std::string const m_gam_ID_name;
   // SwissCross, R19, R29 are ignored if AdditionalSpikeCleaning = True.
   bool const        m_gam_addSpikeCleaning;
   double const      m_gam_swissCross_max;
   double const      m_gam_r19_max;
   double const      m_gam_r29_max;

   // Jets:
   int const         m_jet_num_min;
   double const      m_jet_trigger_pt_min;
   std::string const m_jet_algo;
   double const      m_jet_pt_min;
   double const      m_jet_eta_max;
   bool const        m_jet_isPF;
   bool const        m_jet_ID_use;
   std::string const m_jet_ID_name;
   double const      m_jet_gen_hadOverEm_min;
   double const      m_jet_gen_hadEFrac_min;
   bool const        m_jet_bJets_use;
   std::string const m_jet_bJets_algo;
   double const      m_jet_bJets_discr_min;
   std::string const m_jet_bJets_genFlavourAlgo;

   // MET:
   int const         m_met_num_min;
   double const      m_met_trigger_pt_min;
   std::string const m_met_type;
   double const      m_met_pt_min;
   double const      m_met_dphi_ele_min;

   //HCAL noise ID
   bool const        m_hcal_noise_ID_use;
   std::string const m_hcal_noise_ID_name;

   // Do no opology cuts when not needed.
   bool const m_no_topo_cut;

   /////////////////////////////////////////////////////////////////////////////
   ////////////////////////////// Other variables: /////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

   // To access the JEC uncertainties from file.
   JetCorrectionUncertainty m_jecUnc;

   ParticleMatcher m_matcher;
   DuplicateObjects m_duplicate;

   // Event variables:
   double m_rho25;
};
#endif
