#include "EventSelector.hh"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "Tools/Tools.hh"
#include "EventClassFactory/CcEventClass.hh"
#include "Tools/PXL/Sort.hh"

using namespace pxl;
using namespace std;

//--------------------Constructor-----------------------------------------------------------------

EventSelector::EventSelector( const Tools::MConfig &cfg ) :
   // General selection:
   m_ignoreOverlaps( cfg.GetItem< bool >( "General.IgnoreOverlaps" ) ),

   // Generator selection:
   m_binningValue_max( cfg.GetItem< double >( "Generator.BinningValue.max" ) ),
   m_mass_min(         cfg.GetItem< double >( "Generator.Mass.min", 0 ) ),
   m_mass_max(         cfg.GetItem< double >( "Generator.Mass.max", 0 ) ),
   m_massIDs(       Tools::splitString< int >( cfg.GetItem< string >( "Generator.Mass.IDs" ), true ) ),
   m_massMotherIDs( Tools::splitString< int >( cfg.GetItem< string >( "Generator.Mass.mothers" ), true ) ),

   // Filters:
   m_filterSet_name( cfg.GetItem< string >( "FilterSet.Name" ) ),
   m_filterSet_genList( Tools::splitString< string >( cfg.GetItem< string >( "FilterSet.GenList" ), true  ) ),
   m_filterSet_recList( Tools::splitString< string >( cfg.GetItem< string >( "FilterSet.RecList" ), true  ) ),

   // Primary vertex:
   m_PV_num_min(  cfg.GetItem< int    >( "PV.N.min" ) ),
   m_PV_z_max(    cfg.GetItem< double >( "PV.Z.max" ) ),
   m_PV_rho_max(  cfg.GetItem< double >( "PV.Rho.max" ) ),
   m_PV_ndof_min( cfg.GetItem< double >( "PV.NDOF.min" ) ),

   // Electrons:
   m_ele_use(                            cfg.GetItem< bool   >( "Ele.use" ) ),
   m_ele_pt_min(                         cfg.GetItem< double >( "Ele.pt.min" ) ),
   m_ele_eta_barrel_max(                 cfg.GetItem< double >( "Ele.eta.Barrel.max" ) ),
   m_ele_eta_endcap_min(                 cfg.GetItem< double >( "Ele.eta.Endcap.min" ) ),
   m_ele_eta_endcap_max(                 cfg.GetItem< double >( "Ele.eta.Endcap.max" ) ),
   m_ele_invertIso(                      cfg.GetItem< bool   >( "Ele.InvertIsolation" ) ),
   m_ele_requireEcalDriven(              cfg.GetItem< bool   >( "Ele.RequireEcalDriven" ) ),
   m_ele_rejectOutOfTime(                cfg.GetItem< bool   >( "Ele.RejectOutOfTime" ) ),
   m_ele_EoP_max(                        cfg.GetItem< double >( "Ele.EoP.max" ) ),
   m_ele_barrel_deltaEta_max(            cfg.GetItem< double >( "Ele.Barrel.DEta.max" ) ),
   m_ele_barrel_deltaPhi_max(            cfg.GetItem< double >( "Ele.Barrel.DPhi.max" ) ),
   m_ele_barrel_HoEM_max(                cfg.GetItem< double >( "Ele.Barrel.HoEM.max" ) ),
   m_ele_barrel_trackiso_max(            cfg.GetItem< double >( "Ele.Barrel.TrkIso.max" ) ),
   m_ele_barrel_HcalD1_offset(           cfg.GetItem< double >( "Ele.Barrel.HcalD1.Offset" ) ),
   m_ele_barrel_HcalD1_slope(            cfg.GetItem< double >( "Ele.Barrel.HcalD1.Slope" ) ),
   m_ele_barrel_HcalD1_rhoSlope(         cfg.GetItem< double >( "Ele.Barrel.HcalD1.RhoSlope" ) ),
   m_ele_barrel_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.Barrel.NInnerLayerLostHits.max" ) ),
   m_ele_barrel_swissCross_max(          cfg.GetItem< double >( "Ele.Barrel.SwissCross.max" ) ),
   m_ele_barrel_r19_max(                 cfg.GetItem< double >( "Ele.Barrel.R19.max" ) ),
   m_ele_barrel_r29_max(                 cfg.GetItem< double >( "Ele.Barrel.R29.max" ) ),
   m_ele_barrel_e1x5_min(                cfg.GetItem< double >( "Ele.Barrel.E1x5.min" ) ),
   m_ele_barrel_e2x5_min(                cfg.GetItem< double >( "Ele.Barrel.E2x5.min" ) ),
   m_ele_endcap_deltaEta_max(            cfg.GetItem< double >( "Ele.Endcap.DEta.max" ) ),
   m_ele_endcap_deltaPhi_max(            cfg.GetItem< double >( "Ele.Endcap.DPhi.max" ) ),
   m_ele_endcap_HoEM_max(                cfg.GetItem< double >( "Ele.Endcap.HoEM.max" ) ),
   m_ele_endcap_trackiso_max(            cfg.GetItem< double >( "Ele.Endcap.TrkIso.max" ) ),
   m_ele_endcap_HcalD1_offset(           cfg.GetItem< double >( "Ele.Endcap.HcalD1.Offset" ) ),
   m_ele_endcap_HcalD1_slope(            cfg.GetItem< double >( "Ele.Endcap.HcalD1.Slope" ) ),
   m_ele_endcap_HcalD1_rhoSlope(         cfg.GetItem< double >( "Ele.Endcap.HcalD1.RhoSlope" ) ),
   m_ele_endcap_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.Endcap.NInnerLayerLostHits.max" ) ),
   m_ele_endcap_sigmaIetaIeta_max(       cfg.GetItem< double >( "Ele.Endcap.SigmaIetaIeta.max" ) ),
   m_ele_ID_use(                         cfg.GetItem< bool   >( "Ele.ID.use" ) ),
   m_ele_ID_name(                        cfg.GetItem< string >( "Ele.ID.name" ) ),

   // Muons:
   m_muo_use(                        cfg.GetItem< bool   >( "Muon.use" ) ),
   m_muo_pt_min(                     cfg.GetItem< double >( "Muon.pt.min" ) ),
   m_muo_eta_max(                    cfg.GetItem< double >( "Muon.eta.max" ) ),
   m_muo_invertIso(                  cfg.GetItem< bool   >( "Muon.InvertIsolation" ) ),
   m_muo_useCocktail(                cfg.GetItem< bool   >( "Muon.UseCocktail" ) ),
   m_muo_requireIsGlobal(            cfg.GetItem< bool   >( "Muon.RequireIsGlobal" ) ),
   m_muo_requireIsTracker(           cfg.GetItem< bool   >( "Muon.RequireIsTracker" ) ),
   m_muo_dPtRelTrack_max(            cfg.GetItem< double >( "Muon.dPtRelTrack.max" ) ),
   m_muo_iso_type(                   cfg.GetItem< string >( "Muon.Iso.Type" ) ),
   m_muo_iso_max(                    cfg.GetItem< double >( "Muon.Iso.max" ) ),
   m_muo_NPixelHits_min(             cfg.GetItem< int    >( "Muon.NPixelHits.min" ) ),
   m_muo_NMuonHits_min(              cfg.GetItem< int    >( "Muon.NMuonHits.min" ) ),
   m_muo_NMatchedStations_min(       cfg.GetItem< int    >( "Muon.NMatchedStations.min" ) ),
   m_muo_NTrackerLayersWithMeas_min( cfg.GetItem< int    >( "Muon.NTrackerLayersWithMeas.min" ) ),
   m_muo_XYImpactParameter_max(      cfg.GetItem< double >( "Muon.XYImpactParameter.max" ) ),
   m_muo_globalChi2_max(             cfg.GetItem< double >( "Muon.GlobalChi2.max" ) ),

   // Taus:
   m_tau_use(     cfg.GetItem< bool   >( "Tau.use" ) ),
   m_tau_pt_min(  cfg.GetItem< double >( "Tau.pt.min" ) ),
   m_tau_eta_max( cfg.GetItem< double >( "Tau.Eta.max" ) ),
   //Get Tau-Discriminators and save them
   m_tau_discriminators( Tools::splitString< string >( cfg.GetItem< string >( "Tau.Discriminators" ), true ) ),

   // Photons:
   m_gam_use(                      cfg.GetItem< bool   >( "Gamma.use" ) ),
   m_gam_pt_min(                   cfg.GetItem< double >( "Gamma.pt.min" ) ),
   m_gam_eta_barrel_max(           cfg.GetItem< double >( "Gamma.Eta.Barrel.max" ) ),
   m_gam_eta_endcap_min(           cfg.GetItem< double >( "Gamma.Eta.Endcap.min" ) ),
   m_gam_eta_endcap_max(           cfg.GetItem< double >( "Gamma.Eta.Endcap.max" ) ),
   m_gam_useEndcap(                cfg.GetItem< bool   >( "Gamma.UseEndcap" ) ),
   m_gam_useConverted(             cfg.GetItem< bool   >( "Gamma.UseConverted" ) ),
   m_gam_rejectOutOfTime(          cfg.GetItem< bool   >( "Gamma.RejectOutOfTime" ) ),
   m_gam_corrFactor_max(           cfg.GetItem< double >( "Gamma.CorrFactor.max" ) ),

   m_gam_barrel_sigmaIetaIeta_min( cfg.GetItem< double >( "Gamma.Barrel.SigmaIetaIeta.min" ) ),
   m_gam_barrel_sigmaIetaIeta_max( cfg.GetItem< double >( "Gamma.Barrel.SigmaIetaIeta.max" ) ),
   m_gam_endcap_sigmaIetaIeta_max( cfg.GetItem< double >( "Gamma.Endcap.SigmaIetaIeta.max" ) ),

   // CutBasedPhotonID2012:
   m_gam_CutBasedPhotonID2012_use( cfg.GetItem< bool >( "Gamma.CutBasedPhotonID2012.use" ) ),
   m_gam_EA( cfg ),
   // Barrel:
   m_gam_barrel_electronVeto_require(      cfg.GetItem< bool   >( "Gamma.Barrel.ElectronVeto.Require" ) ),
   m_gam_barrel_HoEm2012_max(              cfg.GetItem< double >( "Gamma.Barrel.HoEm2012.max" ) ),
   m_gam_barrel_PFIsoChargedHadron_max(    cfg.GetItem< double >( "Gamma.Barrel.PFIsoChargedHadron.max" ) ),
   m_gam_barrel_PFIsoNeutralHadron_offset( cfg.GetItem< double >( "Gamma.Barrel.PFIsoNeutralHadron.Offset" ) ),
   m_gam_barrel_PFIsoNeutralHadron_slope(  cfg.GetItem< double >( "Gamma.Barrel.PFIsoNeutralHadron.Slope" ) ),
   m_gam_barrel_PFIsoPhoton_offset(        cfg.GetItem< double >( "Gamma.Barrel.PFIsoPhoton.Offset" ) ),
   m_gam_barrel_PFIsoPhoton_slope(         cfg.GetItem< double >( "Gamma.Barrel.PFIsoPhoton.Slope" ) ),
   // Endcap:
   m_gam_endcap_electronVeto_require(      cfg.GetItem< bool   >( "Gamma.Endcap.ElectronVeto.Require" ) ),
   m_gam_endcap_HoEm2012_max(              cfg.GetItem< double >( "Gamma.Endcap.HoEm2012.max" ) ),
   m_gam_endcap_PFIsoChargedHadron_max(    cfg.GetItem< double >( "Gamma.Endcap.PFIsoChargedHadron.max" ) ),
   m_gam_endcap_PFIsoNeutralHadron_offset( cfg.GetItem< double >( "Gamma.Endcap.PFIsoNeutralHadron.Offset" ) ),
   m_gam_endcap_PFIsoNeutralHadron_slope(  cfg.GetItem< double >( "Gamma.Endcap.PFIsoNeutralHadron.Slope" ) ),
   m_gam_endcap_PFIsoPhoton_offset(        cfg.GetItem< double >( "Gamma.Endcap.PFIsoPhoton.Offset" ) ),
   m_gam_endcap_PFIsoPhoton_slope(         cfg.GetItem< double >( "Gamma.Endcap.PFIsoPhoton.Slope" ) ),

   // Vgamma2011PhotonID:
   m_gam_Vgamma2012PhotonID_use(   cfg.GetItem< bool   >( "Gamma.Vgamma2011PhotonID.use" ) ),
   m_gam_useSeedVeto(              cfg.GetItem< bool   >( "Gamma.UseSeedVeto" ) ),
   m_gam_HoEm_max(                 cfg.GetItem< double >( "Gamma.HoEm.max" ) ),
   // Barrel:
   m_gam_barrel_TrkIso_offset(     cfg.GetItem< double >( "Gamma.Barrel.TrackIso.Offset" ) ),
   m_gam_barrel_TrkIso_slope(      cfg.GetItem< double >( "Gamma.Barrel.TrackIso.Slope" ) ),
   m_gam_barrel_TrkIso_rhoSlope(   cfg.GetItem< double >( "Gamma.Barrel.TrackIso.RhoSlope" ) ),
   m_gam_barrel_EcalIso_offset(    cfg.GetItem< double >( "Gamma.Barrel.ECALIso.Offset" ) ),
   m_gam_barrel_EcalIso_slope(     cfg.GetItem< double >( "Gamma.Barrel.ECALIso.Slope" ) ),
   m_gam_barrel_EcalIso_rhoSlope(  cfg.GetItem< double >( "Gamma.Barrel.ECALIso.RhoSlope" ) ),
   m_gam_barrel_HcalIso_offset(    cfg.GetItem< double >( "Gamma.Barrel.HCALIso.Offset" ) ),
   m_gam_barrel_HcalIso_slope(     cfg.GetItem< double >( "Gamma.Barrel.HCALIso.Slope" ) ),
   m_gam_barrel_HcalIso_rhoSlope(  cfg.GetItem< double >( "Gamma.Barrel.HCALIso.RhoSlope" ) ),
   // Endcap:
   m_gam_endcap_TrkIso_offset(     cfg.GetItem< double >( "Gamma.Endcap.TrackIso.Offset" ) ),
   m_gam_endcap_TrkIso_slope(      cfg.GetItem< double >( "Gamma.Endcap.TrackIso.Slope" ) ),
   m_gam_endcap_TrkIso_rhoSlope(   cfg.GetItem< double >( "Gamma.Endcap.TrackIso.RhoSlope" ) ),
   m_gam_endcap_EcalIso_offset(    cfg.GetItem< double >( "Gamma.Endcap.ECALIso.Offset" ) ),
   m_gam_endcap_EcalIso_slope(     cfg.GetItem< double >( "Gamma.Endcap.ECALIso.Slope" ) ),
   m_gam_endcap_EcalIso_rhoSlope(  cfg.GetItem< double >( "Gamma.Endcap.ECALIso.RhoSlope" ) ),
   m_gam_endcap_HcalIso_offset(    cfg.GetItem< double >( "Gamma.Endcap.HCALIso.Offset" ) ),
   m_gam_endcap_HcalIso_slope(     cfg.GetItem< double >( "Gamma.Endcap.HCALIso.Slope" ) ),
   m_gam_endcap_HcalIso_rhoSlope(  cfg.GetItem< double >( "Gamma.Endcap.HCALIso.RhoSlope" ) ),

   // ID:
   m_gam_ID_use(                   cfg.GetItem< bool   >( "Gamma.ID.use" ) ),
   m_gam_ID_name(                  cfg.GetItem< string >( "Gamma.ID.name" ) ),
   // Additional:
   m_gam_addSpikeCleaning(         cfg.GetItem< bool   >( "Gamma.AdditionalSpikeCleaning" ) ),
   m_gam_swissCross_max(           cfg.GetItem< double >( "Gamma.SwissCross.max" ) ),
   m_gam_r19_max(                  cfg.GetItem< double >( "Gamma.R19.max" ) ),
   m_gam_r29_max(                  cfg.GetItem< double >( "Gamma.R29.max" ) ),

   // Jets:
   m_jet_use(                  cfg.GetItem< bool   >( "Jet.use" ) ),
   m_jet_algo(                 cfg.GetItem< string >( "Jet.Algo" ) ),
   m_jet_pt_min(               cfg.GetItem< double >( "Jet.pt.min" ) ),
   m_jet_eta_max(              cfg.GetItem< double >( "Jet.eta.max" ) ),
   m_jet_isPF(                 cfg.GetItem< bool   >( "Jet.isPF" ) ),
   m_jet_ID_use(               cfg.GetItem< bool   >( "Jet.ID.use" ) ),
   m_jet_ID_name(              cfg.GetItem< string >( "Jet.ID.name" ) ),
   m_jet_gen_hadOverEm_min(    cfg.GetItem< double >( "Jet.Gen.HadOverEm.min" ) ),
   m_jet_gen_hadEFrac_min(     cfg.GetItem< double >( "Jet.Gen.HadEFrac.min" ) ),
   // bJets?
   m_jet_bJets_use(            cfg.GetItem< bool   >( "Jet.BJets.use" ) ),
   m_jet_bJets_algo(           cfg.GetItem< string >( "Jet.BJets.Algo" ) ),
   m_jet_bJets_discr_min(      cfg.GetItem< double >( "Jet.BJets.Discr.min" ) ),
   m_jet_bJets_genFlavourAlgo( cfg.GetItem< string >( "Jet.BJets.genFlavourAlgo" ) ),

   // MET:
   m_met_use(            cfg.GetItem< bool   >( "MET.use" ) ),
   m_met_type(           cfg.GetItem< string >( "MET.Type" ) ),
   m_met_pt_min(         cfg.GetItem< double >( "MET.pt.min" ) ),
   m_met_dphi_ele_min(   cfg.GetItem< double >( "MET.dPhi.Ele.min" ) ),

   // HCAL noise:
   m_hcal_noise_ID_use(  cfg.GetItem< bool   >( "HCAL.Noise.ID.use" ) ),
   m_hcal_noise_ID_name( cfg.GetItem< string >( "HCAL.Noise.ID.name" ) ),

   // To access the JEC uncertainties from file.
   m_jecUnc( Tools::ExpandPath( cfg.GetItem< string >( "Jet.Error.JESFile" ) ) ),

   // Particle Matcher.
   m_matcher(),

   m_eventCleaning( cfg ),
   m_triggerSelector( cfg ),

   m_rho( 0.0 ),
   m_rho25( 0.0 )
{
   if( not m_gam_Vgamma2012PhotonID_use xor m_gam_CutBasedPhotonID2012_use ) {
      stringstream error;
      error << "In config file: ";
      error << "'" << cfg.GetConfigFilePath() << "': ";
      error << "exactly one of 'Gamma.Vgamma2011PhotonID.use' and 'Gamma.CutBasedPhotonID2012.use' ";
      error << "must be true!";
      throw Tools::config_error( error.str() );
   }
}

//--------------------Destructor-----------------------------------------------------------------

EventSelector::~EventSelector() {
}

//-------------------- Redo Matching -----------------------------------------------------------------

void EventSelector::redoMatching(EventView* GenEvtView, EventView* RecEvtView) {

   std::vector<string> jetalgos;
   jetalgos.push_back( m_jet_algo );
   m_matcher.matchObjects( GenEvtView, RecEvtView, jetalgos, m_jet_bJets_algo, m_met_type, m_jet_bJets_use, true );

}


// This function looks for gen filters that are written in the RecEvtView and
// fill them into the GenEvtView for further processing.
//
void EventSelector::preSynchronizeGenRec( pxl::EventView *GenEvtView, pxl::EventView *RecEvtView ) {
   for( vector< string >::const_iterator filter = m_filterSet_genList.begin(); filter != m_filterSet_genList.end(); ++filter ) {
      const string filterName = m_filterSet_name + "_p_" + *filter;
      bool filterResult = RecEvtView->findUserRecord< bool >( filterName );

      GenEvtView->setUserRecord< bool >( filterName, filterResult );
      RecEvtView->removeUserRecord( filterName );
   }
}


void EventSelector::synchronizeGenRec( pxl::EventView* GenEvtView, pxl::EventView* RecEvtView ) {
   bool binning_accept = GenEvtView->findUserRecord< bool >( "binning_accept" );
   bool gen_non_topo_accepted = GenEvtView->findUserRecord< bool >( "non_topo_accept" );
   bool rec_non_topo_accepted = RecEvtView->findUserRecord< bool >( "non_topo_accept" );
   bool gen_accepted = GenEvtView->findUserRecord< bool >( "accepted" );
   bool rec_accepted = RecEvtView->findUserRecord< bool >( "accepted" );
   bool gen_filter_accepted = GenEvtView->findUserRecord< bool >( "filter_accept" );

   //gen and rec are both only accepted when the binning value is accepted, too
   GenEvtView->setUserRecord< bool >( "non_topo_accept", gen_non_topo_accepted && binning_accept && gen_filter_accepted );
   RecEvtView->setUserRecord< bool >( "non_topo_accept", rec_non_topo_accepted && binning_accept && gen_filter_accepted );
   GenEvtView->setUserRecord< bool >( "accepted", gen_accepted && binning_accept && gen_filter_accepted );
   RecEvtView->setUserRecord< bool >( "accepted", rec_accepted && binning_accept && gen_filter_accepted );
}


// ------------------ Check if the given filters have fired -------------------
bool EventSelector::passFilterSelection( pxl::EventView *EvtView, const bool isRec ) {
   vector< string > list;
   if( isRec ) {
      list = m_filterSet_recList;
   } else {
      list = m_filterSet_genList;
   }

   for( vector< string >::const_iterator filter = list.begin(); filter != list.end(); ++filter ) {
      const string filterName = m_filterSet_name + "_p_" + *filter;
      bool filterResult = EvtView->findUserRecord< bool >( filterName );

      //If the filter has not fired this means that the event did not pass the
      //selection criteria and so we don't want it.
      //
      if( ! filterResult ) return false;
   }

   return true;
}


//--------------------Helper Method to calculate the transverse invariant mass-----------------------------------------------------------------

double EventSelector::TransverseInvariantMass(EventView* EvtView, const std::string& type1, const std::string& type2) {
   // take the particles with the highest pT of type1 and type2 and calculate transverse invariant mass
   if (EvtView->findUserRecord<int>("Num"+type1) < 1 || EvtView->findUserRecord<int>("Num"+type2) < 1) return 0.;
   vector<Particle*> type1_particles;
   vector<Particle*> type2_particles;
   ParticleFilter::apply( EvtView->getObjectOwner(), type1_particles, ParticlePtEtaNameCriterion(type1) );
   ParticleFilter::apply( EvtView->getObjectOwner(), type2_particles, ParticlePtEtaNameCriterion(type2) );
   // take first element of each list and calculate transverse inv mass.
   Particle* part1 = type1_particles.front();
   Particle* part2 = type2_particles.front();
   double transInvMass2 =   (part1->getVector().getEt() + part2->getVector().getEt())*(part1->getVector().getEt() + part2->getVector().getEt())
                          - (part1->getPx() + part2->getPx())*(part1->getPx() + part2->getPx())
                          - (part1->getPy() + part2->getPy())*(part1->getPy() + part2->getPy());

   if (transInvMass2 < 0) cout << "Inv Mass Square negative: " << transInvMass2 << endl;
   return std::sqrt(transInvMass2);
}

//--------------------Helper Method to calculate the invariant mass-----------------------------------------------------------------

double EventSelector::InvariantMass(EventView* EvtView, const std::string& type1, const std::string& type2) {
   // take the particles with the highest pT of type1 and type2 and calculate transverse invariant mass
   if (EvtView->findUserRecord<int>("Num"+type1) < 1 || EvtView->findUserRecord<int>("Num"+type2) < 1) return 0;
   vector<Particle*> type1_particles;
   vector<Particle*> type2_particles;
   ParticleFilter::apply( EvtView->getObjectOwner(), type1_particles, ParticlePtEtaNameCriterion(type1) );
   ParticleFilter::apply( EvtView->getObjectOwner(), type2_particles, ParticlePtEtaNameCriterion(type2) );
   //in case both particles same type take second leading particle
   Particle* part1 = type1_particles.front();
   Particle* part2 = type2_particles.front();
   if (type1 == type2) {
      if (EvtView->findUserRecord<int>("Num"+type2) > 1) part2 = type1_particles[1];
      else cout << "only one particle of type " << type1 << " available!!!" << endl;
   }
   double InvMass2 =   (part1->getE() + part2->getE())  *(part1->getE() + part2->getE())
                     - (part1->getPx() + part2->getPx())*(part1->getPx() + part2->getPx())
                     - (part1->getPy() + part2->getPy())*(part1->getPy() + part2->getPy())
                     - (part1->getPz() + part2->getPz())*(part1->getPz() + part2->getPz());
   if (InvMass2 < 0) cout << "Mass Square negative: " << InvMass2 << endl;
   return std::sqrt(InvMass2);
}

//--------------------Helper Method to perform JES modifications -----------------------------------------------------------------
//pxl particle is altered implicitly !!!
void EventSelector::varyJES( vector< pxl::Particle* > const &jets,
                             int const JES,
                             bool const isRec
                             ) {
   if( JES != 0 && isRec == true ) {
      //loop over all selected jets
      for( vector< Particle* >::const_iterator rec_jet = jets.begin(); rec_jet != jets.end(); ++rec_jet ) {
         Particle *thisJet = *rec_jet;

         //The uncertainty is a function of eta and the (corrected) p_t of a jet.
         //At the moment (CMSSW 3_8_7) L2Relative, L3Absolute (and L2L3Residual) corrections are applied to MC (data) jets.
         m_jecUnc.setJetEta( thisJet->getEta() );
         m_jecUnc.setJetPt( thisJet->getPt() );
         double const JESFactor = 1. + ( JES * m_jecUnc.getUncertainty( true ) );

         //WARNING: Change the actual PXL particle:
         thisJet->setP4( JESFactor * thisJet->getPx(), JESFactor * thisJet->getPy(), JESFactor * thisJet->getPz(), JESFactor * thisJet->getE() );
      }
   }
}

//--------------------Helper Method to perform JES/MET modifications -----------------------------------------------------------------
//When the jet energy is varied by the varyJES(...) method the missing energy must be adapted.
//pxl particle is altered implicitly !!!
void EventSelector::varyJESMET( vector< pxl::Particle* > const &jets,
                                vector< pxl::Particle* > const &met,
                                int const JES,
                                bool const isRec
                                ) {
   if( JES != 0 && isRec == true ) {
      //Loop over remaining jets and get the absolute amount of (transversal) energy corrected for each jet.
      double dPx = 0, dPy = 0;
      for( vector< Particle* >::const_iterator rec_jet = jets.begin(); rec_jet != jets.end(); ++rec_jet ) {
         Particle *thisJet = *rec_jet;
         m_jecUnc.setJetEta( thisJet->getEta() );
         m_jecUnc.setJetPt( thisJet->getPt() );
         double const unc       = JES * m_jecUnc.getUncertainty( true );
         double const JESFactor = 1. + unc;

         //unapply the correction
         double uncorPx = thisJet->getPx() / JESFactor;
         double uncorPy = thisJet->getPy() / JESFactor;

         //and multiply with the uncertainty to get the enery amount
         //add it up
         dPx += uncorPx * unc;
         dPy += uncorPy * unc;
      }

      //loop over met and correct for residual JES-variation
      for( vector< Particle* >::const_iterator rec_met = met.begin(); rec_met != met.end(); ++rec_met ) {
         Particle *thisMET = *rec_met;
         double Px = thisMET->getPx() - dPx;
         double Py = thisMET->getPy() - dPy;

         //WARNING: Change the actual PXL particle:
         thisMET->setP4( Px, Py, 0., sqrt(Px*Px + Py*Py) );
         //check if MET fullfilles cuts will be done below!
      }
   }
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: muon-vector is changed!
void EventSelector::applyCutsOnMuon( pxl::EventView* EvtView, std::vector< pxl::Particle* > &muons, const bool &isRec) {
   vector< pxl::Particle* > muonsAfterCut;

   for( vector< Particle* >::const_iterator muon = muons.begin(); muon != muons.end(); ++muon ) {
      Particle* thisMuon = *muon;
      if( passMuon( thisMuon, isRec ) ) {
         muonsAfterCut.push_back( thisMuon );
      } else {
         thisMuon->owner()->remove( thisMuon );
      }
   }

   //ATTENTION: changing muon-vector!
   muons = muonsAfterCut;
}


bool EventSelector::passMuon( pxl::Particle *muon, const bool &isRec ) {
   double const muonPt = muon->getPt();
   // pt cut
   if( muonPt < m_muo_pt_min ) return false;

   //eta cut
   if( fabs( muon->getEta() ) > m_muo_eta_max ) return false;

   // Reconstructed muons cuts.
   // According to:
   // https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Basline_muon_selections_for_2011
   if( isRec ) {
      //relative pt error
      if( muon->findUserRecord< double >( "dPtRelTrack_off" ) > m_muo_dPtRelTrack_max ) return false;

      //is the muon global? do we care?
      if( m_muo_requireIsGlobal && !muon->findUserRecord< bool >( "isGlobalMuon" ) ) return false;

      //is it a Tracker muon? do we care ?
      if( m_muo_requireIsTracker && !muon->findUserRecord< bool >( "isTrackerMuon" ) ) return false;

      // Muon isolation.
      double muon_iso;
      if( m_muo_iso_type == "Combined" ) {
         muon_iso = muon->findUserRecord< double >( "TrkIso" )
                  + muon->findUserRecord< double >( "ECALIso" )
                  + muon->findUserRecord< double >( "HCALIso" );
      } else if( m_muo_iso_type == "Tracker" ) {
         muon_iso = muon->findUserRecord< double >( "TrkIso" );
      } else if( m_muo_iso_type == "PFCombined03" ) {
         muon_iso = muon->findUserRecord< double >( "PFIsoR03ChargedHadrons" )
                  + muon->findUserRecord< double >( "PFIsoR03NeutralHadrons" )
                  + muon->findUserRecord< double >( "PFIsoR03Photons" );
      } else if( m_muo_iso_type == "PFCombined04" ) {
         muon_iso = muon->findUserRecord< double >( "PFIsoR04ChargedHadrons" )
                  + muon->findUserRecord< double >( "PFIsoR04NeutralHadrons" )
                  + muon->findUserRecord< double >( "PFIsoR04Photons" );
      } else {
         throw Tools::config_error( "In passMuon(...): Invalid isolation type: '" + m_muo_iso_type + "'" );
      }

      double const muon_rel_iso = muon_iso / muonPt;

      bool iso_failed = muon_rel_iso > m_muo_iso_max;
      //turn around for iso-inversion
      if( m_muo_invertIso ) iso_failed = !iso_failed;
      //now check
      if( iso_failed ) return false;

      //number of hits in the pixel detector
      if( muon->findUserRecord< int >( "VHitsPixel" ) < m_muo_NPixelHits_min ) return false;

      // Mouns tracker track has a transverse impact parameter dxy < 2 mm w.r.t. the primary vertex.
      if( muon->findUserRecord< double >( "dB" ) > m_muo_XYImpactParameter_max ) return false;

      //normalised chisquare of the global track
      if( muon->findUserRecord< double >( "NormChi2" ) > m_muo_globalChi2_max ) return false;

      // Muon segments in at least two muon stations. (TODO: Typo in Skimmer already corrected, so remove the try block.)
      try {
         if( muon->findUserRecord< int >( "NMachedStations" ) < m_muo_NMatchedStations_min ) return false;
      } catch( std::runtime_error ) {
         if( muon->findUserRecord< int >( "NMatchedStations" ) < m_muo_NMatchedStations_min ) return false;
      }

      // Number of tracker layers with hits.
      if( muon->findUserRecord< int >( "TrackerLayersWithMeas" ) < m_muo_NTrackerLayersWithMeas_min ) return false;

      //number of muon hits surviving in the global fit
      if( muon->findUserRecord< int >( "VHitsMuonSys" ) < m_muo_NMuonHits_min ) return false;

   //generator muon cuts
   } else {
      double const muon_rel_iso = muon->findUserRecord< float >( "GenIso" ) / muonPt;
      // Gen iso cut.
      bool iso_failed = muon_rel_iso > m_muo_iso_max;
      //turn around for iso-inversion
      if( m_muo_invertIso ) iso_failed = !iso_failed;
      //now check
      if( iso_failed ) return false;
   }

   //no cut failed
   return true;
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: ele-vector is changed!
void EventSelector::applyCutsOnEle( pxl::EventView* EvtView, std::vector< pxl::Particle* > &eles, const bool &isRec ) {
   vector<pxl::Particle*> elesAfterCut;

   for( vector< Particle* >::const_iterator ele = eles.begin(); ele != eles.end(); ++ele ) {
      if( passEle( *ele, isRec ) ) {
         elesAfterCut.push_back( *ele );
      } else {
         (*ele)->owner()->remove(*ele);
      }
   }

   //ATTENTION: changing eles-vector!
   eles = elesAfterCut;
}



bool EventSelector::passEle( pxl::Particle *ele, const bool& isRec ) {
   double const elePt = ele->getPt();

   //pt cut
   if( elePt < m_ele_pt_min ) return false;

   // eta
   double const abseta = isRec ? fabs( ele->findUserRecord< double >( "SCeta" ) ) : fabs( ele->getEta() );

   //out of endcap
   if( abseta > m_ele_eta_endcap_max ) return false;
   //between endcap and barrel
   if( abseta < m_ele_eta_endcap_min and abseta > m_ele_eta_barrel_max ) return false;
   //ele in barrel?
   bool const barrel = abseta <= m_ele_eta_barrel_max;
   //ele in endcap?
   bool const endcap = abseta >= m_ele_eta_endcap_min and abseta <= m_ele_eta_endcap_max;

   if( barrel and endcap ) throw Tools::value_error( "In passEle(...): Electron cannot be in both barrel and endcap!" );
   if( not barrel and not endcap ) {
      stringstream warning;
      warning << "WARNING: In passEle(...): ";
      warning << "At this point ";
      if( isRec ) warning << "(Rec) ";
      else        warning << "(Gen) ";
      warning << "electron should be in barrel or endcap. But:" << endl;
      warning << "eta = " << abseta << endl;
      warning << "pt  = " << elePt << endl;
      warning << "Ignoring this particle!" << endl;

      cerr << warning;

      return false;
   }

   //rec ele cuts
   if( isRec ) {
      //must be ECAL driven
      if( m_ele_requireEcalDriven and not ele->findUserRecord< bool >( "ecalDriven" ) ) return false;

      if( ele->findUserRecord< double >( "EoP" ) > m_ele_EoP_max ) return false;

      double const ele_absDeltaEta = fabs( ele->findUserRecord< double >( "DEtaSCVtx" ) );
      double const ele_absDeltaPhi = fabs( ele->findUserRecord< double >( "DPhiSCVtx" ) );
      double const ele_HoEM        = ele->findUserRecord< double >( "HoEm" );
      double const ele_TrkIso      = ele->findUserRecord< double >( "TrkIso03" );

      // TODO: Remove this construct when FA11 or older samples are not used anymore.
      // (Typo in skimmer already fixed. UserRecord: NinnerLayerLostHits.)
      int ele_innerLayerLostHits;
      try{
         ele_innerLayerLostHits = ele->findUserRecord< int >( "NMissingHits: " );
      } catch( std::runtime_error ) {
         ele_innerLayerLostHits = ele->findUserRecord< int >( "NinnerLayerLostHits" );
      } catch( ... ) {
         throw;
      }

      //ele in barrel
      if( barrel ) {
         //delta eta between SC and track
         if( ele_absDeltaEta > m_ele_barrel_deltaEta_max ) return false;

         //delta phi between SC and track
         if( ele_absDeltaPhi > m_ele_barrel_deltaPhi_max ) return false;

         //hadronic over EM
         if( ele_HoEM > m_ele_barrel_HoEM_max ) return false;

         //shower shape
         double const e5x5 = ele->findUserRecord< double >( "e5x5" );
         double const e1x5 = ele->findUserRecord< double >( "e1x5" );
         double const e2x5 = ele->findUserRecord< double >( "e2x5" );

         if( e1x5/e5x5 < m_ele_barrel_e1x5_min and e2x5/e5x5 < m_ele_barrel_e2x5_min ) return false;

         //Isolation
         bool iso_ok = true;
         //HCAL iso depth 1
         double const maxIso = m_ele_barrel_HcalD1_offset + m_ele_barrel_HcalD1_slope * ele->getEt() + m_ele_barrel_HcalD1_rhoSlope * m_rho25;
         if( iso_ok && ele->findUserRecord< double >( "HCALIso03d1" ) > maxIso ) iso_ok = false;
         //Track iso
         if( iso_ok && ele_TrkIso > m_ele_barrel_trackiso_max ) iso_ok = false;

         //turn around for iso-inversion
         if( m_ele_invertIso ) iso_ok = not iso_ok;
         //now test
         if( !iso_ok ) return false;

         //swiss cross spike tagging
         if( ele->findUserRecord< double >( "SwissCross" ) > m_ele_barrel_swissCross_max ) return false;

         if( ele->findUserRecord< double >( "r19" ) > m_ele_barrel_r19_max ) return false;

         if( m_ele_barrel_r29_max > 0 ) {
            double const Emax = ele->findUserRecord< double >( "Emax" );
            double const E2nd = ele->findUserRecord< double >( "E2nd" );
            double const e3x3 = ele->findUserRecord< double >( "e3x3" );
            double const R29  = ( Emax + E2nd ) / e3x3;

            if( R29 > m_ele_barrel_r29_max ) return false;
         }

         if( ele_innerLayerLostHits > m_ele_barrel_NInnerLayerLostHits_max ) return false;

         if( m_ele_rejectOutOfTime and ele->findUserRecord< unsigned int >( "recoFlag", 0 ) == 2 ) return false;
      }

      //ele in endcap
      if( endcap ) {
         //delta eta between SC and track
         if( ele_absDeltaEta > m_ele_endcap_deltaEta_max ) return false;

         //delta phi between SC and track
         if( ele_absDeltaPhi > m_ele_endcap_deltaPhi_max ) return false;

         //hadronic over EM
         if( ele_HoEM > m_ele_endcap_HoEM_max ) return false;

         //sigma iEta-iEta
         if( ele->findUserRecord< double >( "sigmaIetaIeta" ) > m_ele_endcap_sigmaIetaIeta_max ) return false;

         //Isolation
         bool iso_ok = true;
         //HCAL iso depth 1
         double maxIso = m_ele_endcap_HcalD1_offset + m_ele_endcap_HcalD1_rhoSlope * m_rho25;
         //add a slope for high energy electrons
         if( ele->getEt() > 50.0 ) maxIso += m_ele_endcap_HcalD1_slope * ( ele->getEt() - 50.0 );
         //now test
         if( iso_ok && ele->findUserRecord< double >( "HCALIso03d1" ) > maxIso ) iso_ok = false;
         //Track iso
         if( iso_ok and ele_TrkIso > m_ele_endcap_trackiso_max ) iso_ok = false;
         //turn around for iso-inversion
         if( m_ele_invertIso ) iso_ok = not iso_ok;
         //now test
         if( !iso_ok ) return false;

         if( ele_innerLayerLostHits > m_ele_endcap_NInnerLayerLostHits_max ) return false;
      }

      //ID cuts
      if( m_ele_ID_use && !ele->findUserRecord< bool >( m_ele_ID_name ) ) return false;
   } else {
      //gen ele cuts
      if( barrel ) {
         //ele in barrel
         bool iso_failed = ele->findUserRecord< double >( "GenIso" ) > m_ele_barrel_trackiso_max;
         //turn around for iso-inversion
         if( m_ele_invertIso ) iso_failed = not iso_failed;
         //now test
         if( iso_failed ) return false;
      }

      //gen ele in endcap
      if( endcap ) {
         bool iso_failed = ele->findUserRecord< double >( "GenIso" ) > m_ele_endcap_trackiso_max;
         //turn around for iso-inversion
         if( m_ele_invertIso ) iso_failed = not iso_failed;
         //now test
         if( iso_failed ) return false;
      }
   }

   //no cut failed
   return true;
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: tau-vector is changed!
void EventSelector::applyCutsOnTau( pxl::EventView* EvtView, std::vector< pxl::Particle* > &taus, const bool &isRec ) {
   int numTau = 0;

   vector< pxl::Particle* > tausAfterCut;

   for( vector< Particle* >::const_iterator tau = taus.begin(); tau != taus.end(); ++tau ) {
      Particle* thisTau = *tau;
      if( passTau( thisTau, isRec ) ) {
         ++numTau;
         tausAfterCut.push_back( thisTau );
      } else {
         thisTau->owner()->remove( thisTau );
      }
   }

   //ATTENTION: changing tau-vector!
   taus = tausAfterCut;
}


bool EventSelector::passTau( pxl::Particle *tau, const bool &isRec ) {
   //pt cut
   if( tau->getPt() < m_tau_pt_min ) return false;

   //eta cut
   if( fabs( tau->getEta() ) > m_tau_eta_max )
      return false;
   if( isRec ) {
      for( std::vector< std::string >::const_iterator discr = m_tau_discriminators.begin(); discr != m_tau_discriminators.end(); ++discr ) {
         // In theory all tau discriminators have a value between 0 and 1.
         // Thus, they are saved as a float and the cut value is 0.5.
         // In practice most (or all) discriminators are boolean.
         // See also:
         // https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePFTauID#Discriminators
         if( tau->findUserRecord< float >( *discr ) < 0.5 ) {
            return false;
         }
      }
   }
   return true;
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: gamma-vector is changed!
void EventSelector::applyCutsOnGamma( pxl::EventView* EvtView, std::vector<pxl::Particle* > &gammas, const bool &isRec ) {
   vector< pxl::Particle* > gammasAfterCut;

   for( vector< Particle* >::const_iterator gamma = gammas.begin(); gamma != gammas.end(); ++gamma ) {
      if( passGamma( *gamma, isRec ) ) {
         gammasAfterCut.push_back( *gamma );
      } else {
         (*gamma)->owner()->remove( *gamma );
      }
   }

   //ATTENTION: changing gammas-vector!
   gammas = gammasAfterCut;
}


bool EventSelector::passGamma( pxl::Particle *gam, const bool& isRec ) {
   double const gamPt = gam->getPt();
   //pt cut
   if( gamPt < m_gam_pt_min ) return false;

   // eta
   double const abseta = fabs( gam->getEta() );

   //out of endcap ?
   if( abseta > m_gam_eta_endcap_max ) return false;
   //between endcap and barrel ?
   if( abseta < m_gam_eta_endcap_min && abseta > m_gam_eta_barrel_max ) return false;
   //gamma in barrel?
   bool const barrel = abseta <= m_gam_eta_barrel_max;
   //gamma in endcap?
   bool const endcap = abseta >= m_gam_eta_endcap_min and abseta <= m_gam_eta_endcap_max;

   // Do we want endcap photons?
   if( endcap and not m_gam_useEndcap ) return false;

   if( barrel and endcap ) throw Tools::value_error( "In passGamma(...): Gamma cannot be in both barrel and endcap!" );
   if( not barrel and not endcap ) {
      stringstream warning;
      warning << "WARNING: In passGamma(...): ";
      warning << "At this point ";
      if( isRec ) warning << "Rec ";
      else        warning << "Gen ";
      warning << "photon should be in barrel or endcap. But:" << endl;
      warning << "eta = " << abseta << endl;
      warning << "pt  = " << gamPt << endl;
      warning << "Ignoring this particle!" << endl;

      cerr << warning;

      return false;
   }

   if( isRec ) {
      //cut on sigmaietaieta ("eta width") which is different for EB and EE
      double const gam_sigma_ieta_ieta = gam->findUserRecord< double >( "iEta_iEta" );

      if( barrel ) {
         //Additional spike cleaning
         if( gam_sigma_ieta_ieta < m_gam_barrel_sigmaIetaIeta_min ) return false;
         if( gam_sigma_ieta_ieta > m_gam_barrel_sigmaIetaIeta_max ) return false;
      }

      if( endcap ) {
         if( gam_sigma_ieta_ieta > m_gam_endcap_sigmaIetaIeta_max ) return false;
      }

      if( m_gam_Vgamma2012PhotonID_use ) {
         bool const passed = passVgamma2011PhotonID( gam, barrel, endcap );
         if( not passed ) return false;
      }

      if( m_gam_CutBasedPhotonID2012_use ) {
         bool const passed = passCutBasedPhotonID2012( gam, barrel, endcap );
         if( not passed ) return false;
      }

      if( m_gam_addSpikeCleaning ) {
         //swiss cross spike tagging
         if( gam->findUserRecord< double >( "SwissCross" ) > m_gam_swissCross_max ) return false;

         if( gam->findUserRecord< double >( "r19" ) > m_gam_r19_max ) return false;

         if( m_gam_r29_max > 0 ) {
            double const Emax = gam->findUserRecord< double >( "Emax" );
            double const E2nd = gam->findUserRecord< double >( "E2nd" );
            double const e3x3 = gam->findUserRecord< double >( "e3x3" );
            double const R29  = ( Emax + E2nd ) / e3x3;

            if( R29 > m_gam_r29_max ) return false;
         }
      }

      //recoFlag might not be set, assume it fine then
      if( m_gam_rejectOutOfTime and gam->findUserRecord< unsigned int >( "recoFlag", 0 ) == 2 ) return false;

      //do we care about converted photons?
      if( not m_gam_useConverted and gam->findUserRecord< bool >( "Converted" ) ) return false;

      if( m_gam_corrFactor_max > 0.0 ) {
         //too large correction factors are not good for photons
         if( gam->getE() / gam->findUserRecord< double >( "rawEnergy" ) > m_gam_corrFactor_max ) return false;
      }

      //do we care about gamma ID?
      if( m_gam_ID_use and not gam->findUserRecord< bool >( m_gam_ID_name ) ) return false;
   } else {
      //we presume the track iso is roughly equivalent to the gen iso
      double const maxIso = m_gam_barrel_TrkIso_offset + m_gam_barrel_TrkIso_slope * gamPt;

      if( gam->findUserRecord< double >( "GenIso" ) > maxIso ) return false;
   }

   //no cut failed
   return true;
}


bool EventSelector::passVgamma2011PhotonID( pxl::Particle const *gam,
                                            bool const barrel,
                                            bool const endcap
                                            ) const {
   // Track (pixel seed) veto.
   if( m_gam_useSeedVeto and gam->findUserRecord< bool >( "HasSeed" ) ) return false;

   if( gam->findUserRecord< double >( "HoEm" ) > m_gam_HoEm_max ) return false;

   double const gamPt = gam->getPt();

   double maxTrackIso = m_gam_barrel_TrkIso_offset
                      + m_gam_barrel_TrkIso_slope * gamPt
                      + m_gam_barrel_TrkIso_rhoSlope * m_rho25;

   double maxEcalIso  = m_gam_barrel_EcalIso_offset
                      + m_gam_barrel_EcalIso_slope * gamPt
                      + m_gam_barrel_EcalIso_rhoSlope * m_rho25;

   double maxHcalIso  = m_gam_barrel_HcalIso_offset
                      + m_gam_barrel_HcalIso_slope * gamPt
                      + m_gam_barrel_HcalIso_rhoSlope * m_rho25;

   if( endcap ) {
      maxTrackIso = m_gam_endcap_TrkIso_offset
                  + m_gam_endcap_TrkIso_slope * gamPt
                  + m_gam_endcap_TrkIso_rhoSlope * m_rho25;

      maxEcalIso  = m_gam_endcap_EcalIso_offset
                  + m_gam_endcap_EcalIso_slope * gamPt
                  + m_gam_endcap_EcalIso_rhoSlope * m_rho25;

      maxHcalIso  = m_gam_endcap_HcalIso_offset
                  + m_gam_endcap_HcalIso_slope * gamPt
                  + m_gam_endcap_HcalIso_rhoSlope * m_rho25;
   }

   // New (uniform) naming convention in Skimmer.
   // TODO: Remove try-block once no old samples are used anymore.
   try {
      //Jurrasic ECAL Isolation
      if( gam->findUserRecord< double >( "ID_ECALIso" ) > maxEcalIso ) return false;
      //Tower-based HCAL Isolation
      if( gam->findUserRecord< double >( "ID_HCALIso" ) > maxHcalIso ) return false;
      //hollow cone track isolation
      if( gam->findUserRecord< double >( "ID_TrkIso" ) > maxTrackIso ) return false;
   } catch( std::runtime_error ) {
      // Jurrasic ECAL Isolation.
      if( gam->findUserRecord< double >( "ECALIso" ) > maxEcalIso ) return false;
      // Tower-based HCAL Isolation.
      if( gam->findUserRecord< double >( "HCALIso" ) > maxHcalIso ) return false;
      // Hollow cone track isolation.
      if( gam->findUserRecord< double >( "TrkIso" ) > maxTrackIso ) return false;
   }

   return true;
}


bool EventSelector::passCutBasedPhotonID2012( pxl::Particle const *gam,
                                              bool const barrel,
                                              bool const endcap
                                              ) const {
   double const gamPt  = gam->getPt();
   double const abseta = fabs( gam->getEta() );

   double const chargedHadronEA = m_gam_EA.getEffectiveArea( abseta, PhotonEffectiveArea::chargedHadron );
   double const neutralHadronEA = m_gam_EA.getEffectiveArea( abseta, PhotonEffectiveArea::neutralHadron );
   double const photonEA        = m_gam_EA.getEffectiveArea( abseta, PhotonEffectiveArea::photon );

   bool eleVeto_require = m_gam_barrel_electronVeto_require;

   double HoEm2012_max = m_gam_barrel_HoEm2012_max;

   double PFIsoChargedHadron_max = m_gam_barrel_PFIsoChargedHadron_max;

   double PFneutralHadronIso_max = m_gam_barrel_PFIsoNeutralHadron_offset
                                 + m_gam_barrel_PFIsoNeutralHadron_slope * gamPt;

   double PFphotonIso_max = m_gam_barrel_PFIsoPhoton_offset
                          + m_gam_barrel_PFIsoPhoton_slope * gamPt;

   if( endcap ) {
      eleVeto_require = m_gam_endcap_electronVeto_require;

      HoEm2012_max = m_gam_endcap_HoEm2012_max;

      PFIsoChargedHadron_max = m_gam_endcap_PFIsoChargedHadron_max;

      PFneutralHadronIso_max = m_gam_endcap_PFIsoNeutralHadron_offset
                             + m_gam_endcap_PFIsoNeutralHadron_slope * gamPt;

      PFphotonIso_max = m_gam_endcap_PFIsoPhoton_offset
                      + m_gam_endcap_PFIsoPhoton_slope * gamPt;
   }

   if( eleVeto_require and gam->findUserRecord< bool >( "hasMatchedPromptElectron" ) ) return false;

   if( gam->findUserRecord< double >( "HoverE2012" ) > HoEm2012_max ) return false;

   // Correct the isolation variables accrding to:
   // https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonID2012#Effective_Areas_for_rho_correcti
   // (r20: 2013-01-10)
   double const chargedIsoCorr = max( gam->findUserRecord< double >( "PFIso03ChargedHadron" ) - m_rho25 * chargedHadronEA, 0.0 );
   double const neutralIsoCorr = max( gam->findUserRecord< double >( "PFIso03NeutralHadron" ) - m_rho25 * neutralHadronEA, 0.0 );
   double const photonIsoCorr  = max( gam->findUserRecord< double >( "PFIso03Photon" ) - m_rho25 * photonEA, 0.0 );

   if( chargedIsoCorr > PFIsoChargedHadron_max ) return false;

   if( neutralIsoCorr > PFneutralHadronIso_max ) return false;

   if( photonIsoCorr > PFphotonIso_max ) return false;

   return true;
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: jet-vector is changed!
void EventSelector::applyCutsOnJet( pxl::EventView* EvtView, std::vector< pxl::Particle* > &jets, const bool &isRec ) {
   vector< pxl::Particle* > jetsAfterCuts;
   for( vector< Particle* >::const_iterator jet = jets.begin(); jet != jets.end(); ++jet ) {
      Particle *thisJet = *jet;
      bool acceptJet = passJet( thisJet, isRec );

      // TODO: Update to use new variable "isPFJet" implemented in Skimmer that
      // should be available in 5XY skimmed samples!
      // If any of the three is false, we don't count the jet as PF.
      thisJet->setUserRecord< bool >( "isPF", isRec and m_jet_isPF and acceptJet );

      // bJet?
      if( m_jet_bJets_use and acceptJet ) {
         if( isRec ) {
            if( thisJet->findUserRecord< float >( m_jet_bJets_algo ) > m_jet_bJets_discr_min ) {
               thisJet->setUserRecord< string >( "bJetType", m_jet_bJets_algo );
            } else {
               thisJet->setUserRecord< string >( "bJetType", "nonB" );
            }
         } else {
            if( abs( thisJet->findUserRecord< int >( m_jet_bJets_genFlavourAlgo ) ) == 5 ) {
               thisJet->setUserRecord< string >( "bJetType", "genBJet" );
            } else {
               thisJet->setUserRecord< string >( "bJetType", "nonB" );
            }
         }
      }

      if( acceptJet ) jetsAfterCuts.push_back( thisJet );
      else            thisJet->owner()->remove( thisJet );
   }
   jets = jetsAfterCuts;
}


bool EventSelector::passJet( pxl::Particle *jet, const bool &isRec ) const {
   if( jet->getPt() < m_jet_pt_min )           return false;
   if( fabs( jet->getEta() ) > m_jet_eta_max ) return false;

   if( isRec ) {
      if( m_jet_ID_use ) {
         if( not jet->findUserRecord< bool >( m_jet_ID_name ) ) return false;
      }
   } else {
      double const HadOverEm = jet->findUserRecord< double >( "HadE" ) / jet->findUserRecord< double >( "EmE" );
      double const HadEFrac  = jet->findUserRecord< double >( "HadE" ) / jet->getE();

      if( HadOverEm < m_jet_gen_hadOverEm_min ) return false;
      if( HadEFrac  < m_jet_gen_hadEFrac_min )  return false;
   }

   return true;
}



void EventSelector::countJets( pxl::EventView *EvtView, std::vector< pxl::Particle* > &jets, const bool &isRec ) {
   unsigned int numJet = 0;
   unsigned int numB = 0;
   if( m_jet_bJets_use ) {
      for( vector< Particle* >::const_iterator jet = jets.begin(); jet != jets.end(); ++jet ) {
         if( (*jet)->findUserRecord< string >( "bJetType" ) == "nonB" ) {
            ++numJet;
         } else {
            ++numB;
         }
      }
   } else {
      numJet = jets.size();
   }
   EvtView->setUserRecord< int >( "Num" + m_jet_algo, numJet );
   EvtView->setUserRecord< int >( "Num" + m_jet_bJets_algo, numB );
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------

void EventSelector::applyCutsOnMET( pxl::EventView *EvtView, std::vector< pxl::Particle* > &mets, const bool &isRec ) {
   int numMET = 0;

   vector< pxl::Particle* > metsAfterCuts;

   for( vector< Particle* >::const_iterator met = mets.begin(); met != mets.end(); ++met ) {
      Particle* thisMet = *met;
      if( passMET( thisMet, isRec ) ) {
         ++numMET;
         metsAfterCuts.push_back( thisMet );
      } else {
         thisMet->owner()->remove( thisMet );
      }
   }

   //ATTENTION: changing met-vector!
   mets = metsAfterCuts;
}


bool EventSelector::passMET( pxl::Particle *met, const bool &isRec ) const {
   if( met->getPt() < m_met_pt_min ) return false;

   return true;
}


//-------------------------------------------------------------------------------------
void EventSelector::applyCutsOnVertex( pxl::EventView* EvtView, std::vector< pxl::Vertex* > &vertices, const bool &isRec ) {
   int numPV = 0;

   vector< pxl::Vertex* > PVafterCuts;

   for( vector< pxl::Vertex* >::const_iterator PV = vertices.begin(); PV != vertices.end(); PV++ ) {
      //we're only interested in primary vertices
      if( (*PV)->getName() != "PV" ) continue;

      //get position
      double x = (*PV)->getX();
      double y = (*PV)->getY();
      double z = (*PV)->getZ();
      double rho = sqrt( x*x + y*y );

      //check position
      bool const position_OK = ( fabs(z) <= m_PV_z_max && rho <= m_PV_rho_max );

      //check quality
      bool quality_OK;
      if( isRec ) {
         quality_OK = ( !(*PV)->findUserRecord< bool >( "IsFake" ) and (*PV)->findUserRecord< double >( "ndof" ) >= m_PV_ndof_min );
      } else {
         //gen level quality is alway fine
         quality_OK = true;
      }

      if( position_OK && quality_OK ) {
         numPV++;
         PVafterCuts.push_back( *PV );
      } else {
         (*PV)->owner()->remove(*PV);
      }
   }

   //ATTENTION: changing PV-vector!
   vertices = PVafterCuts;

   EvtView->setUserRecord< int >( "NumPV", numPV );
}



//-------------------------------------------------------------------------------------
bool EventSelector::applyGlobalEventCuts( pxl::EventView* EvtView,
                                          std::vector< pxl::Vertex* > &vertices,
                                          std::vector< pxl::Particle* > &eles,
                                          std::vector< pxl::Particle* > &mets
                                          ) {
   //get the number of primary vertices
   int numPV = vertices.size();
   //check that we got enough PVs
   if( numPV < m_PV_num_min ) return false;

   if( EvtView->findUserRecord< std::string >( "Type" ) == "Rec" ){
      //only HCAL noise ID cut on rec level in the moment
      //return true when empty to disable cut
      if( m_hcal_noise_ID_use and EvtView->findUserRecord< bool >( m_hcal_noise_ID_name ) ) return false;
   }

   for( std::vector< pxl::Particle* >::const_iterator ele = eles.begin(); ele != eles.end(); ++ele ){
      for( std::vector< pxl::Particle* >::const_iterator met = mets.begin(); met != mets.end(); ++met ){
         double delta_phi = fabs( (*ele)->getVector().deltaPhi( dynamic_cast< const pxl::Basic3Vector* >( &((*met)->getVector()) ) ) );
         if( delta_phi < m_met_dphi_ele_min ) return false;
      }
   }

   //nothing went wrong, good event
   return true;
}


bool EventSelector::applyGeneratorCuts( pxl::EventView* EvtView, std::vector< pxl::Particle *> &particles ) {
   //check binning value
   if( m_binningValue_max > 0 && EvtView->findUserRecord< double >( "binScale" ) > m_binningValue_max ) {
      return false;
   }

   //check invariant mass
   //do we actually need to do something?
   if( m_mass_min <= 0 and m_mass_max <= 0 ) return true;
   //apparently we do
   if( particles.size() < 2 ) {
      for( std::vector< pxl::Particle *>::const_iterator part = particles.begin(); part != particles.end(); ++part ) {
         cerr << "ID=" << (*part)->findUserRecord< int >( "id" ) << " mother=" << (*part)->findUserRecord< int >( "mother_id" ) << endl;
      }
      throw std::length_error( "Can't calculate invariant mass with less than 2 particles." );
   }
   else if( particles.size() > 3 ) {
      for( std::vector< pxl::Particle *>::const_iterator part = particles.begin(); part != particles.end(); ++part ) {
         cerr << "ID=" << (*part)->findUserRecord< int >( "id" ) << " mother=" << (*part)->findUserRecord< int >( "mother_id" ) << endl;
      }
      throw std::length_error( "More than 2 particles to calculate invariant mass." );
   }
   else {
      pxl::LorentzVector sum;
      sum += particles.at( 0 )->getVector();
      sum += particles.at( 1 )->getVector();

      if( m_mass_min <= 0 && m_mass_max > 0 ) {
         return sum.getMass() <= m_mass_max;
      }
      else if( m_mass_min > 0 && m_mass_max <= 0 ) {
         return sum.getMass() >= m_mass_min;
      }
      else {   // m_mass_min > 0 && m_mass_max > 0
         return ( sum.getMass() >= m_mass_min && sum.getMass() <= m_mass_max );
      }
   }
}



void checkOrder( std::vector< pxl::Particle *> &particles ) {
   if( particles.size() < 2 ) return;
   std::vector< pxl::Particle *>::const_iterator part = particles.begin();
   double first_pt = (*part)->getPt();
   ++part;
   for( ; part != particles.end(); ++part ) {
      double pt = (*part)->getPt();
      if( pt > first_pt ) {
         cout << "ERROR: Unsorted particle " << part-particles.begin() << ": " << (*part)->getName() << endl;
         cout << "first pt: " << first_pt << endl;
         for( std::vector< pxl::Particle *>::const_iterator part2 = particles.begin(); part2 != particles.end(); ++part2 ) {
            cout << "Pt: " << (*part2)->getPt() << " (" << pt << ")" << endl;
         }
         exit(1);
      } else {
         first_pt = pt;
      }
   }
}



//--------------------This is the main method to perform the selection-----------------------------------------
void EventSelector::performSelection(EventView* EvtView, const int& JES) {   //used with either GenEvtView or RecEvtView
   string process = EvtView->findUserRecord<std::string>("Process");
   bool isRec = (EvtView->findUserRecord<std::string>("Type") == "Rec");
   if (JES == -1){
       process += "_JES_DOWN";
       EvtView->setUserRecord<std::string>("Process", process);
    } else if(JES == 1){
       process += "_JES_UP";
       EvtView->setUserRecord<std::string>("Process", process);
    }

   const bool filterAccept = passFilterSelection( EvtView, isRec );
   EvtView->setUserRecord< bool >( "filter_accept", filterAccept );

   const bool L1_accept = m_triggerSelector.passL1Trigger( EvtView, isRec );
   EvtView->setUserRecord< bool >( "L1_accept", L1_accept );

   // rho is only available in Rec.
   // It defaults to 0!
   if( isRec ) {
      m_rho25 = EvtView->findUserRecord< double >( "rho25" );
      // TODO: Remove this block once corrected.
      // In the current version of the Skimmer, the wrong number value is stored
      // in the rho25 variable. Until the correct values are available, the
      // "wrong" one will be corrected.
      try {
         m_rho = EvtView->findUserRecord< double >( "rho" );
      } catch( std::runtime_error &e ) {
         // The correction factor is purley empirical and extracted from a very
         // limited set of MC (DY) and SingleMu events. This is only a temporary
         // solution!
         m_rho25 *= 0.9;
      }
   }

   // get all particles
   vector<pxl::Particle*> allparticles;
   EvtView->getObjectsOfType<pxl::Particle>(allparticles);
   pxl::sortParticles( allparticles );

   vector< pxl::Particle* > muons, eles, taus, gammas, jets, mets, doc_particles;     // no 'bJets' because jets is only filled into varyJESMET, where all jets are treated exactly the same way
   for (vector<pxl::Particle*>::const_iterator part = allparticles.begin(); part != allparticles.end(); ++part) {
      string name = (*part)->getName();
      // Only fill the collection if we want to use the particle!
      // If the collections are not filled, the particles are also ignored in
      // the event cleaning.
      if(      m_muo_use and name == "Muon"     ) muons.push_back( *part );
      else if( m_ele_use and name == "Ele"      ) eles.push_back( *part );
      else if( m_tau_use and name == "Tau"      ) taus.push_back( *part );
      else if( m_gam_use and name == "Gamma"    ) gammas.push_back( *part );
      else if( m_jet_use and name == m_jet_algo ) jets.push_back( *part );
      else if( m_met_use and name == m_met_type ) mets.push_back( *part );
      //no need to store the status 3 particles if we're not going to check them anyway
      else if( ( m_mass_min > 0 || m_mass_max > 0 ) && name == "S3" ) {
         if( ( m_massIDs.size() == 0
               || std::find( m_massIDs.begin(), m_massIDs.end(), (*part)->findUserRecord< int >( "id" ) ) != m_massIDs.end() )
             &&
             ( m_massMotherIDs.size() == 0
               || std::find( m_massMotherIDs.begin(), m_massMotherIDs.end(), (*part)->findUserRecord< int >( "mother_id" ) ) != m_massMotherIDs.end() )
             ) {
            doc_particles.push_back(*part);
         }
      }
   }

   //check that the particles are ordered by Pt
   checkOrder( taus );
   checkOrder(muons);
   checkOrder(eles);
   checkOrder(gammas);
   checkOrder(jets);
   checkOrder(mets);

   //get vertices
   vector< pxl::Vertex* > vertices;
   EvtView->getObjectsOfType< pxl::Vertex >( vertices );

   applyCutsOnMuon( EvtView, muons, isRec );
   applyCutsOnEle( EvtView, eles, isRec );
   applyCutsOnTau( EvtView, taus, isRec );
   applyCutsOnGamma( EvtView, gammas, isRec );
   //first vary JES and then check corrected jets to pass cuts
   varyJES(jets, JES, isRec);
   applyCutsOnJet( EvtView, jets, isRec );     //distribution into jets and b-jets
   //consistently check GenView for duplicates, important especially for GenJets and efficiency-normalization
   if( not m_ignoreOverlaps ) m_eventCleaning.cleanEvent( muons,
                                                          eles,
                                                          taus,
                                                          gammas,
                                                          jets,
                                                          isRec
                                                          );

   //now vary also MET using ONLY selected and JES-modified jets. Maybe use dedicated jet cuts here?
   varyJESMET(jets, mets, JES, isRec);
   //after MET varied check also cuts
   applyCutsOnMET( EvtView, mets, isRec );

   //now store the number of particles of each type
   EvtView->setUserRecord< int >( "NumTau", taus.size() );
   EvtView->setUserRecord< int >( "NumMuon", muons.size() );
   EvtView->setUserRecord< int >( "NumEle" , eles.size() );
   EvtView->setUserRecord< int >( "NumGamma", gammas.size() );
   countJets( EvtView, jets, isRec );
   EvtView->setUserRecord< int >( "Num" + m_met_type, mets.size() );

   applyCutsOnVertex( EvtView, vertices, isRec );

   //check if the events must be vetoed
   bool const vetoed = m_triggerSelector.checkVeto( isRec,
                                                    muons,
                                                    eles,
                                                    taus,
                                                    gammas,
                                                    jets,
                                                    mets,
                                                    EvtView
                                                    );
   EvtView->setUserRecord< bool >( "Veto", vetoed );

   bool const HLT_accept = m_triggerSelector.passHLTrigger( isRec,
                                                            muons,
                                                            eles,
                                                            taus,
                                                            gammas,
                                                            jets,
                                                            mets,
                                                            EvtView
                                                            );
   EvtView->setUserRecord< bool >( "HLT_accept", HLT_accept );

   bool const triggerAccept = HLT_accept and L1_accept;
   EvtView->setUserRecord< bool >( "trigger_accept", triggerAccept );

   //for gen: check the binning value
   if( !isRec ) {
      EvtView->setUserRecord< bool >( "binning_accept", applyGeneratorCuts( EvtView, doc_particles ) );
   }

   //check global effects, e.g. HCAL noise
   bool global_accept = applyGlobalEventCuts( EvtView, vertices, eles, mets );

   bool topo_accept = passEventTopology( muons, eles, taus, gammas, jets, mets );
   EvtView->setUserRecord< bool >( "topo_accept", topo_accept );

   //event accepted by non-topo cuts, e.g. trigger and global effects
   bool non_topo_accept = global_accept && triggerAccept && filterAccept && !vetoed;
   EvtView->setUserRecord< bool >( "non_topo_accept", non_topo_accept );

   //event accepted after all cuts
   bool accepted = topo_accept && non_topo_accept;
   EvtView->setUserRecord< bool >( "accepted", accepted );

   //cout << endl << "After Everything" << endl;
   //dumpEventView(EvtView);
   //cout << endl << endl;
}


bool EventSelector::passEventTopology( std::vector< pxl::Particle* > const &muos,
                                       std::vector< pxl::Particle* > const &eles,
                                       std::vector< pxl::Particle* > const &taus,
                                       std::vector< pxl::Particle* > const &gams,
                                       std::vector< pxl::Particle* > const &jets,
                                       std::vector< pxl::Particle* > const &mets
                                       ) const {
   return passEventTopology( muos.size(),
                             eles.size(),
                             taus.size(),
                             gams.size(),
                             jets.size(),
                             mets.size()
                             );
}


bool EventSelector::passEventTopology( int const numMuo,
                                       int const numEle,
                                       int const numTau,
                                       int const numGam,
                                       int const numJet,
                                       int const numMET
                                       ) const {
   return m_triggerSelector.passEventTopology( numMuo,
                                               numEle,
                                               numTau,
                                               numGam,
                                               numJet,
                                               numMET
                                               );
}


//--------------Dump all the stuff inside the EventView--------------------------------------------------------

void EventSelector::dumpEventView(const EventView* EvtView) {

   cout << endl << "   Starting EventViewDump: " << endl
                << "   ======================= " << endl << endl;
   cout << "      in detail : " << " #Ele = " << EvtView->findUserRecord<int>("NumEle")
                                << ", #Mu = " << EvtView->findUserRecord<int>("NumMuon")
                                << ", #Gam = " << EvtView->findUserRecord<int>("NumGamma")
            << ", #" << m_met_type << " = " << EvtView->findUserRecord< int >( "Num" + m_met_type )
            << ", #" << m_jet_algo << " = " << EvtView->findUserRecord< int >( "Num" + m_jet_algo )
            << ", #" << m_jet_bJets_algo  << " = " << EvtView->findUserRecord< int >( "Num" + m_jet_bJets_algo )
            << ", #Vertices = " << EvtView->findUserRecord<int>("NumVertices") << endl;
   cout << "      Type      : " <<  EvtView->findUserRecord<string>("Type") << endl;
   // loop over all particles:
   vector<Particle*> particles;
   EvtView->getObjectsOfType<Particle>(particles);
   pxl::sortParticles( particles );
   for (vector<Particle*>::const_iterator part = particles.begin(); part != particles.end(); ++part) {
      cout << "      ";
      (*part)->print(0);
   }
   if (EvtView->findUserRecord<string>("Type") == "Rec") {

      //this record steers if event is globally accepted by trigger, depending on "OR" of the HLT-path selected
      cout << endl << "      HLT accept?   : " << EvtView->findUserRecord<bool>("HLT_accept", false) << endl;
   }
   //this record steers if event is globally accepted by event topology criteria
   cout << "      Topo accept?   : " << EvtView->findUserRecord<bool>("Topo_accept", false) << endl;
}

// This function changes the Muon quantities from "normal" to cocktail".
// (Only doing this for the four momentum of the muon atm. To be extended in the
// future.)
//
void EventSelector::adaptMuons( const EventView* EvtView ) {
   // Do we even want to use cocktail muons?
   //
   if( ! m_muo_useCocktail ) return;

   // Get all particles in this event.
   //
   vector< pxl::Particle* > allparticles;
   EvtView->getObjectsOfType< pxl::Particle >( allparticles );
   pxl::sortParticles( allparticles );

   for( vector< pxl::Particle* >::iterator part = allparticles.begin(); part != allparticles.end(); ++part ) {
      pxl::Particle* thisPart = ( *part );
      // We are only interested in muons here.
      //
      if( thisPart->getName() == "Muon" ) {
         if( thisPart->findUserRecord< bool >( "validCocktail" ) ) {
            double m  = thisPart->getMass();

            double pxCocktail = thisPart->findUserRecord< double >( "pxCocktail" );
            double pyCocktail = thisPart->findUserRecord< double >( "pyCocktail" );
            double pzCocktail = thisPart->findUserRecord< double >( "pzCocktail" );

            double E = sqrt( m * m + pxCocktail * pxCocktail + pyCocktail * pyCocktail + pzCocktail * pzCocktail );

            thisPart->setP4( pxCocktail, pyCocktail, pzCocktail, E );
         }
      }
   }
}
