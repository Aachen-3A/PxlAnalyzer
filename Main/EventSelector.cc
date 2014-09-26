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
   m_data(           cfg.GetItem< bool >( "General.RunOnData" ) ),
   m_ignoreOverlaps( cfg.GetItem< bool >( "General.IgnoreOverlaps" ) ),
   // When running on data, FastSim is always false!
   m_runOnFastSim( not m_data and cfg.GetItem< bool >( "General.FastSim" ) ),
   m_useTrigger( cfg.GetItem< bool >( "General.UseTriggers" ) ),

   // Generator selection:
   m_gen_use( cfg.GetItem< bool >( "Generator.use" ) ),
   m_gen_accept( cfg ),

   // Filters:
   m_filterSet_name( cfg.GetItem< string >( "FilterSet.Name" ) ),
   m_filterSet_genList( Tools::splitString< string >( cfg.GetItem< string >( "FilterSet.GenList" ), true  ) ),
   m_filterSet_recList( Tools::splitString< string >( cfg.GetItem< string >( "FilterSet.RecList" ), true  ) ),

   // Primary vertex:
   m_PV_num_min(  cfg.GetItem< int    >( "PV.N.min" ) ),
   m_PV_z_max(    cfg.GetItem< double >( "PV.Z.max" ) ),
   m_PV_rho_max(  cfg.GetItem< double >( "PV.Rho.max" ) ),
   m_PV_ndof_min( cfg.GetItem< double >( "PV.NDOF.min" ) ),

   //rho default
   m_rho_use(            cfg.GetItem< string   >( "Rho.Label" ) ),

   // Tracks:
   m_tracks_use(     cfg.GetItem< bool         >( "Tracks.use" ) ),
   m_tracks_type(    cfg.GetItem< string       >( "Tracks.type" ) ),
   m_tracks_num_max( cfg.GetItem< unsigned int >( "Tracks.num.max" ) ),

   //Muons
   m_muo_use(    cfg.GetItem< bool   >( "Muon.use" ) ),
   m_muo_idtag( cfg.GetItem< bool   >( "Muon.IdTag") ),
   m_muo_selector(cfg),

   // Electrons:
   m_ele_use(        cfg.GetItem< bool   >( "Ele.use" ) ),
   m_ele_idtag(      cfg.GetItem< bool   >( "Ele.IdTag") ),
   m_ele_rho_label(  cfg.GetItem< string >( "Ele.Rho.Label" ) ),
   m_ele_selector(cfg),

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

   //CutBasedPhotonFlag:


   m_gam_CutBasedPhotonID2012Flag_use(  cfg.GetItem< bool >( "Gamma.CutBasedPhotonID2012Flag.use" ) ),
   m_gam_IDFlag(                        cfg.GetItem< string >( "Gamma.IDFlag" ) ),


   // CutBasedPhotonID2012:
   m_gam_CutBasedPhotonID2012_use( cfg.GetItem< bool >( "Gamma.CutBasedPhotonID2012.use" ) ),
   m_gam_EA( cfg , "Gamma.EffectiveArea.File" ),
   m_gam_rho_label( cfg.GetItem< string >( "Gamma.Rho.Label" ) ),
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
   m_gam_Vgamma2011PhotonID_use(   cfg.GetItem< bool   >( "Gamma.Vgamma2011PhotonID.use" ) ),
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

   // Jets:
   m_jet_use(                  cfg.GetItem< bool   >( "Jet.use" ) ),
   m_jet_pt_min(               cfg.GetItem< double >( "Jet.pt.min" ) ),
   m_jet_eta_max(              cfg.GetItem< double >( "Jet.eta.max" ) ),
   m_jet_isPF(                 cfg.GetItem< bool   >( "Jet.isPF" ) ),
   m_jet_ID_use(               cfg.GetItem< bool   >( "Jet.ID.use" ) ),
   m_jet_ID_name(              cfg.GetItem< string >( "Jet.ID.name" ) ),
   m_jet_gen_hadOverEm_min(    cfg.GetItem< double >( "Jet.Gen.HadOverEm.min" ) ),
   m_jet_gen_hadEFrac_min(     cfg.GetItem< double >( "Jet.Gen.HadEFrac.min" ) ),

   // In case we do the ID on our own:
   m_jet_nHadEFrac_max(       cfg.GetItem< double >( "Jet.NeutralHadronEnergyFraction.max" ) ),
   m_jet_nEMEFrac_max(        cfg.GetItem< double >( "Jet.NeutralEMEnergyFraction.max" ) ),
   m_jet_numConstituents_min( cfg.GetItem< unsigned long    >( "Jet.NumConstituents.min" ) ),
   // Only for |eta| > 2.4:
   m_jet_cHadEFrac_min(     cfg.GetItem< double >( "Jet.ChargedHadronEnergyFraction.min" ) ),
   m_jet_cEMEFrac_max(      cfg.GetItem< double >( "Jet.ChargedEMEnergyFraction.max" ) ),
   m_jet_cMultiplicity_min( cfg.GetItem< unsigned long      >( "Jet.chargedMultiplicity.min" ) ),

   // bJets?
   m_jet_bJets_use(            cfg.GetItem< bool   >( "Jet.BJets.use" ) ),
   m_jet_bJets_algo(           cfg.GetItem< string >( "Jet.BJets.Algo" ) ),
   m_jet_bJets_discr_min(      cfg.GetItem< double >( "Jet.BJets.Discr.min" ) ),
   m_jet_bJets_genFlavourAlgo( cfg.GetItem< string >( "Jet.BJets.genFlavourAlgo" ) ),
   m_jet_bJets_gen_label(      cfg.GetItem< string >( "Jet.BJets.Gen.Label" ) ),

   // MET:
   m_met_use(            cfg.GetItem< bool   >( "MET.use" ) ),
   m_met_pt_min(         cfg.GetItem< double >( "MET.pt.min" ) ),
   m_met_dphi_ele_min(   cfg.GetItem< double >( "MET.dPhi.Ele.min" ) ),

   // HCAL noise:
   m_hcal_noise_ID_use(  cfg.GetItem< bool   >( "HCAL.Noise.ID.use" ) ),
   m_hcal_noise_ID_name( cfg.GetItem< string >( "HCAL.Noise.ID.name" ) ),

   // To access the JEC uncertainties from file.
   m_jecType( Tools::ExpandPath( cfg.GetItem< string >( "Jet.Error.JESType" ) ) ),
   m_jecPara( Tools::ExpandPath( cfg.GetItem< string >( "Jet.Error.JESFile" ) ), m_jecType ),
   m_jecUnc( m_jecPara ),

   m_gen_rec_map( cfg ),

   // Get particle names that shall be used from config and cache them.
   m_RecMuoName( m_gen_rec_map.get( "Muo" ).RecName ),
   m_RecEleName( m_gen_rec_map.get( "Ele" ).RecName ),
   m_RecTauName( m_gen_rec_map.get( "Tau" ).RecName ),
   m_RecGamName( m_gen_rec_map.get( "Gam" ).RecName ),
   m_RecJetName( m_gen_rec_map.get( "Jet" ).RecName ),
   m_RecMETName( m_gen_rec_map.get( "MET" ).RecName ),

   m_GenMuoName( m_gen_rec_map.get( "Muo" ).GenName ),
   m_GenEleName( m_gen_rec_map.get( "Ele" ).GenName ),
   m_GenTauName( m_gen_rec_map.get( "Tau" ).GenName ),
   m_GenGamName( m_gen_rec_map.get( "Gam" ).GenName ),
   m_GenJetName( m_gen_rec_map.get( "Jet" ).GenName ),
   m_GenMETName( m_gen_rec_map.get( "MET" ).GenName ),

   m_eventCleaning( cfg ),
   m_triggerSelector( cfg )
{
   if( (not m_gam_Vgamma2011PhotonID_use xor m_gam_CutBasedPhotonID2012_use xor m_gam_CutBasedPhotonID2012Flag_use ) && ( not m_gam_Vgamma2011PhotonID_use && m_gam_CutBasedPhotonID2012_use && m_gam_CutBasedPhotonID2012Flag_use ) ) {
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

// This function looks for gen filters that are written in the RecEvtView and
// fill them into the GenEvtView for further processing.
//
void EventSelector::preSynchronizeGenRec( pxl::EventView *GenEvtView, pxl::EventView *RecEvtView ) {
   for( vector< string >::const_iterator filter = m_filterSet_genList.begin(); filter != m_filterSet_genList.end(); ++filter ) {
      const string filterName = m_filterSet_name + "_p_" + *filter;
      bool filterResult = RecEvtView->getUserRecord( filterName );

      GenEvtView->setUserRecord( filterName, filterResult );
      RecEvtView->eraseUserRecord( filterName );
   }
}


void EventSelector::synchronizeGenRec( pxl::EventView* GenEvtView, pxl::EventView* RecEvtView ) {
   bool generator_accept = GenEvtView->getUserRecord( "generator_accept" );
   bool gen_non_topo_accepted = GenEvtView->getUserRecord( "non_topo_accept" );
   bool rec_non_topo_accepted = RecEvtView->getUserRecord( "non_topo_accept" );
   bool gen_accepted = GenEvtView->getUserRecord( "accepted" );
   bool rec_accepted = RecEvtView->getUserRecord( "accepted" );
   bool gen_filter_accepted = GenEvtView->getUserRecord( "filter_accept" );

   //gen and rec are both only accepted when the binning value is accepted, too
   GenEvtView->setUserRecord( "non_topo_accept", gen_non_topo_accepted && generator_accept && gen_filter_accepted );
   RecEvtView->setUserRecord( "non_topo_accept", rec_non_topo_accepted && generator_accept && gen_filter_accepted );
   GenEvtView->setUserRecord( "accepted", gen_accepted && generator_accept && gen_filter_accepted );
   RecEvtView->setUserRecord( "accepted", rec_accepted && generator_accept && gen_filter_accepted );
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
      bool filterResult = EvtView->getUserRecord( filterName );

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
   if (EvtView->getUserRecord("Num"+type1).toInt32() < 1 || EvtView->getUserRecord("Num"+type2).toInt32() < 1) return 0.;
   vector<Particle*> type1_particles;
   vector<Particle*> type2_particles;
   pxl::ParticleFilter particleFilter;
   particleFilter.apply( EvtView->getObjectOwner(), type1_particles, ParticlePtEtaNameCriterion(type1) );
   particleFilter.apply( EvtView->getObjectOwner(), type2_particles, ParticlePtEtaNameCriterion(type2) );
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
   if (EvtView->getUserRecord("Num"+type1).toInt32() < 1 || EvtView->getUserRecord("Num"+type2).toInt32() < 1) return 0;
   vector<Particle*> type1_particles;
   vector<Particle*> type2_particles;
   pxl::ParticleFilter particleFilter;
   particleFilter.apply( EvtView->getObjectOwner(), type1_particles, ParticlePtEtaNameCriterion(type1) );
   particleFilter.apply( EvtView->getObjectOwner(), type2_particles, ParticlePtEtaNameCriterion(type2) );
   //in case both particles same type take second leading particle
   Particle* part1 = type1_particles.front();
   Particle* part2 = type2_particles.front();
   if (type1 == type2) {
      if (EvtView->getUserRecord("Num"+type2).toInt32() > 1) part2 = type1_particles[1];
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
         double const Px = thisMET->getPx() - dPx;
         double const Py = thisMET->getPy() - dPy;

         //WARNING: Change the actual PXL particle:
         thisMET->setP4( Px, Py, 0., sqrt(Px*Px + Py*Py) );
         //check if MET fullfilles cuts will be done below!
      }
   }
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: muon-vector is possibly changed!
// void EventSelector::applyCutsOnMuon( std::vector< pxl::Particle* > &muons, const bool &isRec) {
//    vector< pxl::Particle* > muonsAfterCut;
//
//    for( vector< Particle* >::const_iterator muon = muons.begin(); muon != muons.end(); ++muon ) {
//       Particle* thisMuon = *muon;
//       if( m_muo_selector.passMuon(thisMuon,isRec) ) {
//          if (m_muo_idtag){
//             thisMuon->setUserRecord("passed",true);
//          }else{
//             muonsAfterCut.push_back( thisMuon );
//          }
//       } else {
//          if (m_muo_idtag){
//             thisMuon->setUserRecord("passed",false);
//          }else{
//             thisMuon->owner()->remove( thisMuon );
//          }
//       }
//    }
//    if (not m_muo_idtag){
//       //ATTENTION: changing muon-vector!
//       muons = muonsAfterCut;
//    }
// }
void EventSelector::applyCutsOnMuon( std::vector< pxl::Particle* > &muons, const bool &isRec) {
   if(m_muo_idtag){ // muons are tagged, not discarded
      for(vector< Particle* >::const_iterator muon = muons.begin(); muon != muons.end(); ++muon) {
         if(m_muo_selector.passMuon(*muon,isRec)) {
            (*muon)->setUserRecord("passed",true);
         } else {
            (*muon)->setUserRecord("passed",false);
         }
      }
      return; // method has finished
   }


   vector< pxl::Particle* > muonsAfterCut;
   Particle* thisMuon;
   for( vector< Particle* >::const_iterator muon = muons.begin(); muon != muons.end(); ++muon ) {
      thisMuon = *muon;
      if( m_muo_selector.passMuon(thisMuon,isRec) ) {
            muonsAfterCut.push_back(thisMuon);
      } else {
            thisMuon->owner()->remove(thisMuon);
      }
   }
   //ATTENTION: changing muon-vector!
   muons = muonsAfterCut;
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: ele-vector is changed!
void EventSelector::applyCutsOnEle( std::vector< pxl::Particle* > &eles,
                                    double const eleRho,
                                    bool const &isRec
                                    ) const {
   if(m_ele_idtag){ // electrons are tagged, not discarded
      for( vector< Particle* >::const_iterator ele = eles.begin(); ele != eles.end(); ++ele ) {
         if( m_ele_selector.passEle( *ele, eleRho, isRec ) ) {
            (*ele)->setUserRecord("passed",true);
         } else {
            (*ele)->setUserRecord("passed",false);
         }
      }
      return; // method has finished
   }
   
   // else if(not m_ele_idtag):
   vector<pxl::Particle*> elesAfterCut;
   Particle* thisEle;
   for( vector< Particle* >::const_iterator ele = eles.begin(); ele != eles.end(); ++ele ) {
      thisEle = *ele;
      if( m_ele_selector.passEle(thisEle, eleRho, isRec) ) {
         elesAfterCut.push_back(thisEle);
      } else {
         (*ele)->owner()->remove(thisEle);
      }
   }

   //ATTENTION: changing eles-vector!
   eles = elesAfterCut;
}



//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: tau-vector is changed!
void EventSelector::applyCutsOnTau( std::vector< pxl::Particle* > &taus, const bool &isRec ) {
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
         if( tau->getUserRecord( *discr ).toDouble() < 0.5 ) {
            return false;
         }
      }
   }
   return true;
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: gamma-vector is changed!
void EventSelector::applyCutsOnGam( std::vector< pxl::Particle* > &gammas,
                                    double const gamRho,
                                    bool const isRec
                                    ) const {
   vector< pxl::Particle* > gammasAfterCut;

   for( vector< Particle* >::const_iterator gamma = gammas.begin(); gamma != gammas.end(); ++gamma ) {
      if( passGam( *gamma, gamRho, isRec ) ) {
         gammasAfterCut.push_back( *gamma );
      } else {
         (*gamma)->owner()->remove( *gamma );
      }
   }

   //ATTENTION: changing gammas-vector!
   gammas = gammasAfterCut;
}


bool EventSelector::passGam( pxl::Particle const *gam,
                             double const gamRho,
                             bool const isRec
                             ) const {
   double const gamPt = gam->getPt();
   //pt cut
   if( gamPt < m_gam_pt_min ) return false;

   // eta
   // TODO: Always use SCeta for Rec (try block) as soon as all samples have
   // been skimmed with the updated Skimmer.
   double abseta = 0;
   try {
      abseta = isRec ? fabs( gam->getUserRecord( "SCeta" ).toDouble() ) : fabs( gam->getEta() );
   } catch( std::runtime_error ) {
      abseta = fabs( gam->getEta() );
   }

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

      cerr << warning.str();

      return false;
   }

   if( isRec ) {
      //cut on sigmaietaieta ("eta width") which is different for EB and EE
      double const gam_sigma_ieta_ieta = gam->getUserRecord( "iEta_iEta" );

      if( barrel ) {
         //Additional spike cleaning
         if( gam_sigma_ieta_ieta < m_gam_barrel_sigmaIetaIeta_min ) return false;
         if( gam_sigma_ieta_ieta > m_gam_barrel_sigmaIetaIeta_max ) return false;
      }

      if( endcap ) {
         if( gam_sigma_ieta_ieta > m_gam_endcap_sigmaIetaIeta_max ) return false;
      }

      if( m_gam_Vgamma2011PhotonID_use ) {
         bool const passed = passVgamma2011PhotonID( gam, gamRho, barrel, endcap );
         if( not passed ) return false;
      }

      if( m_gam_CutBasedPhotonID2012_use ) {
         bool const passed = passCutBasedPhotonID2012( gam, gamRho, barrel, endcap );
         if( not passed ) return false;
      }

      if( m_gam_CutBasedPhotonID2012Flag_use ){
          if( not gam->getUserRecord( m_gam_IDFlag ).asBool() ) return false;
      }

      if( m_gam_rejectOutOfTime )
        try {
            if( gam->getUserRecord_def( "recoFlag",0 ) == 2 )
                return false;
        } catch( std::runtime_error ) {
            // In case "recoFlag" is not set, we assume, it is *not* out of
            // time!
        }

      //do we care about converted photons?
      if( not m_gam_useConverted and gam->getUserRecord( "Converted" ).asBool() ) return false;

      if( m_gam_corrFactor_max > 0.0 ) {
         //too large correction factors are not good for photons
         if( gam->getE() / gam->getUserRecord( "rawEnergy" ).toDouble() > m_gam_corrFactor_max ) return false;
      }

      //do we care about gamma ID?
      if( m_gam_ID_use and not gam->getUserRecord( m_gam_ID_name ).asBool() ) return false;
   } else {
      double maxIso = 0;
      if( m_gam_Vgamma2011PhotonID_use ) {
         // Take the track iso (without the term including rho, since this is
         // only available in Rec) to be roughly equivalent to the gen iso
         // calculated in MUSiCSkimmer::IsoGenSum. The gen iso only includes
         // stable, changed particles.
         if( barrel ){
            maxIso = m_gam_barrel_TrkIso_offset
                   + m_gam_barrel_TrkIso_slope * gamPt;
         }
         if( endcap ){
            maxIso = m_gam_endcap_TrkIso_offset
                   + m_gam_endcap_TrkIso_slope * gamPt;
         }
      }

      if( m_gam_CutBasedPhotonID2012_use ) {
         // Take the charged hadron isolation to be roughly equivalent to the gen
         // iso calculated in MUSiCSkimmer::IsoGenSum. The gen iso only includes
         // stable, changed particles. Ideally one should calculate the PF
         // isolations for gen and compare these with the reco.
         if( barrel ){
            maxIso = m_gam_barrel_PFIsoChargedHadron_max;
         }
         if( endcap ){
            maxIso = m_gam_endcap_PFIsoChargedHadron_max;
         }
      }

      if( gam->getUserRecord( "GenIso" ).toDouble() > maxIso ) return false;
   }

   //no cut failed
   return true;
}


bool EventSelector::passVgamma2011PhotonID( pxl::Particle const *gam,
                                            double const gamRho,
                                            bool const barrel,
                                            bool const endcap
                                            ) const {
   // Track (pixel seed) veto.
   if( m_gam_useSeedVeto and gam->getUserRecord( "HasSeed" ).asBool() ) return false;

   if( gam->getUserRecord( "HoEm" ).toDouble() > m_gam_HoEm_max ) return false;

   double const gamPt = gam->getPt();

   double maxTrackIso = m_gam_barrel_TrkIso_offset
                      + m_gam_barrel_TrkIso_slope * gamPt
                      + m_gam_barrel_TrkIso_rhoSlope * gamRho;

   double maxEcalIso  = m_gam_barrel_EcalIso_offset
                      + m_gam_barrel_EcalIso_slope * gamPt
                      + m_gam_barrel_EcalIso_rhoSlope * gamRho;

   double maxHcalIso  = m_gam_barrel_HcalIso_offset
                      + m_gam_barrel_HcalIso_slope * gamPt
                      + m_gam_barrel_HcalIso_rhoSlope * gamRho;

   if( endcap ) {
      maxTrackIso = m_gam_endcap_TrkIso_offset
                  + m_gam_endcap_TrkIso_slope * gamPt
                  + m_gam_endcap_TrkIso_rhoSlope * gamRho;

      maxEcalIso  = m_gam_endcap_EcalIso_offset
                  + m_gam_endcap_EcalIso_slope * gamPt
                  + m_gam_endcap_EcalIso_rhoSlope * gamRho;

      maxHcalIso  = m_gam_endcap_HcalIso_offset
                  + m_gam_endcap_HcalIso_slope * gamPt
                  + m_gam_endcap_HcalIso_rhoSlope * gamRho;
   }

   // New (uniform) naming convention in Skimmer.
   // TODO: Remove try-block once no old samples are used anymore.
   try {
      //Jurrasic ECAL Isolation
      if( gam->getUserRecord( "ID_ECALIso" ).toDouble() > maxEcalIso ) return false;
      //Tower-based HCAL Isolation
      if( gam->getUserRecord( "ID_HCALIso" ).toDouble() > maxHcalIso ) return false;
      //hollow cone track isolation
      if( gam->getUserRecord( "ID_TrkIso" ).toDouble() > maxTrackIso ) return false;
   } catch( std::runtime_error ) {
      // Jurrasic ECAL Isolation.
      if( gam->getUserRecord( "ECALIso" ).toDouble() > maxEcalIso ) return false;
      // Tower-based HCAL Isolation.
      if( gam->getUserRecord( "HCALIso" ).toDouble() > maxHcalIso ) return false;
      // Hollow cone track isolation.
      if( gam->getUserRecord( "TrkIso" ).toDouble() > maxTrackIso ) return false;
   }

   return true;
}


bool EventSelector::passCutBasedPhotonID2012( pxl::Particle const *gam,
                                              double const gamRho,
                                              bool const barrel,
                                              bool const endcap
                                              ) const {
   double const gamPt  = gam->getPt();
   double const abseta = fabs( gam->getEta() );

   double const chargedHadronEA = m_gam_EA.getEffectiveArea( abseta, EffectiveArea::chargedHadron );
   double const neutralHadronEA = m_gam_EA.getEffectiveArea( abseta, EffectiveArea::neutralHadron );
   double const photonEA        = m_gam_EA.getEffectiveArea( abseta, EffectiveArea::photon );

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

   if( eleVeto_require and gam->getUserRecord( "hasMatchedPromptElectron" ).asBool() ) return false;

   if( gam->getUserRecord( "HoverE2012" ).toDouble() > HoEm2012_max ) return false;

   // Correct the isolation variables accrding to:
   // https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonID2012#Effective_Areas_for_rho_correcti
   // (r20: 2013-01-10)
   double const chargedIsoCorr = max( gam->getUserRecord( "PFIso03ChargedHadron" ).toDouble() -
                                      gamRho * chargedHadronEA,
                                      0.0
                                      );
   double const neutralIsoCorr = max( gam->getUserRecord( "PFIso03NeutralHadron" ).toDouble() -
                                      gamRho * neutralHadronEA,
                                      0.0
                                      );
   double const photonIsoCorr = max( gam->getUserRecord( "PFIso03Photon" ).toDouble() -
                                     gamRho * photonEA,
                                     0.0
                                     );

   if( chargedIsoCorr > PFIsoChargedHadron_max ) return false;

   if( neutralIsoCorr > PFneutralHadronIso_max ) return false;

   if( photonIsoCorr > PFphotonIso_max ) return false;

   return true;
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------
//ATTENTION: jet-vector is changed!
void EventSelector::applyCutsOnJet( std::vector< pxl::Particle* > &jets, const bool &isRec ) {
   vector< pxl::Particle* > jetsAfterCuts;
   for( vector< Particle* >::const_iterator jet = jets.begin(); jet != jets.end(); ++jet ) {
      Particle *thisJet = *jet;
      bool acceptJet = passJet( thisJet, isRec );

      // TODO: Update to use new variable "isPFJet" implemented in Skimmer that
      // should be available in 5XY skimmed samples!
      // If any of the three is false, we don't count the jet as PF.
      thisJet->setUserRecord( "isPF", isRec and m_jet_isPF and acceptJet );

      // bJet?
      if( m_jet_bJets_use and acceptJet ) {
         if( isRec ) {
            if( thisJet->getUserRecord( m_jet_bJets_algo ).toDouble() > m_jet_bJets_discr_min ) {
               thisJet->setUserRecord( "bJetType", m_jet_bJets_algo );
            } else {
               thisJet->setUserRecord( "bJetType", "nonB" );
            }
         } else {
            if( abs( thisJet->getUserRecord( m_jet_bJets_genFlavourAlgo ).toInt32() ) == 5 ) {
               thisJet->setUserRecord( "bJetType", m_jet_bJets_gen_label );
            } else {
               thisJet->setUserRecord( "bJetType", "nonB" );
            }
         }
      }

      if( acceptJet ) jetsAfterCuts.push_back( thisJet );
      else            thisJet->owner()->remove( thisJet );
   }
   jets = jetsAfterCuts;
}


bool EventSelector::passJet( pxl::Particle *jet, const bool &isRec ) const {
   double const absEta = std::abs( jet->getEta() );
   if( absEta > m_jet_eta_max )
      return false;
   if( jet->getPt() < m_jet_pt_min )
      return false;
   if( isRec ) {
      if( m_jet_ID_use ) {
         if( not jet->getUserRecord( m_jet_ID_name ).asBool() ) return false;
      } else {
         // We do it ourselves!
         if( not jet->getUserRecord( "neutralHadronEnergyFraction" ).toDouble() < m_jet_nHadEFrac_max )
            return false;
         if( not jet->getUserRecord( "neutralEmEnergyFraction" ).toDouble() < m_jet_nEMEFrac_max )
            return false;
         // This variable is unnecessarily stored as a double! Not any more!!
         if( jet->getUserRecord( "nconstituents" ).asUInt32() < m_jet_numConstituents_min )
            return false;
         // Additional cuts if |eta|>2.4:
         if( absEta > 2.4 ) {
            if( not jet->getUserRecord( "chargedHadronEnergyFraction" ).toDouble() > m_jet_cHadEFrac_min )
               return false;
            if( not jet->getUserRecord( "chargedEmEnergyFraction" ).toDouble() < m_jet_cEMEFrac_max )
               return false;
            // This variable is unnecessarily stored as a double!
            if( jet->getUserRecord( "chargedMultiplicity" ).asUInt32() < m_jet_cMultiplicity_min )
               return false;
         }
      }
   } else {
      double const HadOverEm = jet->getUserRecord( "HadE" ).toDouble() / jet->getUserRecord( "EmE" ).toDouble();
      double const HadEFrac  = jet->getUserRecord( "HadE" ).toDouble() / jet->getE();

      if( HadOverEm < m_jet_gen_hadOverEm_min ) return false;
      if( HadEFrac  < m_jet_gen_hadEFrac_min )  return false;
   }

   return true;
}


void EventSelector::countParticles( pxl::EventView *EvtView,
                                    std::vector< pxl::Particle* > const &particles,
                                    std::string const &name,
                                    bool const &isRec
                                    ) const {
   std::string label = "Num";
   if( isRec )
      label += m_gen_rec_map.get( name ).RecName;
   else
      label += m_gen_rec_map.get( name ).GenName;

   EvtView->setUserRecord( label, particles.size() );
}


void EventSelector::countJets( pxl::EventView *EvtView, std::vector< pxl::Particle* > &jets, const bool &isRec ) {
   unsigned int numJet = 0;
   unsigned int numB = 0;
   if( m_jet_bJets_use ) {
      for( vector< Particle* >::const_iterator jet = jets.begin(); jet != jets.end(); ++jet ) {
         if( (*jet)->getUserRecord( "bJetType" ).asString() == "nonB" ) {
            ++numJet;
         } else {
            ++numB;
         }
      }
   } else {
      numJet = jets.size();
   }

   std::string labelJet  = "Num";
   std::string labelBJet = "Num";
   if( isRec ) {
      labelJet  += m_RecJetName;
      labelBJet += m_jet_bJets_algo;
   } else {
      labelJet  += m_GenJetName;
      labelBJet += m_jet_bJets_gen_label;
   }

   EvtView->setUserRecord( labelJet,  numJet );
   EvtView->setUserRecord( labelBJet, numB );
}


//--------------------Apply cuts on Particles-----------------------------------------------------------------

void EventSelector::applyCutsOnMET( std::vector< pxl::Particle* > &mets, const bool &isRec ) {
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
         quality_OK = ( !(*PV)->getUserRecord( "IsFake" ).asBool() and (*PV)->getUserRecord( "ndof" ).toInt32() >= m_PV_ndof_min );
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

   EvtView->setUserRecord( "NumPV", numPV );
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

   if( EvtView->getUserRecord( "Type" ).asString() == "Rec" ){
      //only HCAL noise ID cut on rec level in the moment
      //return true when empty to disable cut
      if( not m_runOnFastSim )
         if( m_hcal_noise_ID_use and EvtView->getUserRecord( m_hcal_noise_ID_name ).asBool() ) return false;

      // Use cut on track number?
      //does not work for miniAOD -- no tracks!
      if( m_tracks_use )
         if( EvtView->getUserRecord( "Num" + m_tracks_type ).asUInt32() > m_tracks_num_max ) return false;
   }

   //this makes no sence at all here!!!
   //if anythig why only electrons
   //for( std::vector< pxl::Particle* >::const_iterator ele = eles.begin(); ele != eles.end(); ++ele ){
      //for( std::vector< pxl::Particle* >::const_iterator met = mets.begin(); met != mets.end(); ++met ){
         //double delta_phi = fabs( (*ele)->getVector().deltaPhi( dynamic_cast< const pxl::Basic3Vector* >( &((*met)->getVector()) ) ) );
         //if( delta_phi < m_met_dphi_ele_min ) return false;
      //}
   //}

   //nothing went wrong, good event
   return true;
}


void EventSelector::checkOrder( std::vector< pxl::Particle* > const &particles
                                ) const {
   if( particles.size() < 2 ) return;
   std::vector< pxl::Particle* >::const_iterator part = particles.begin();
   double first_pt = (*part)->getPt();
   ++part;
   for( ; part != particles.end(); ++part ) {
      double const pt = (*part)->getPt();
      if( pt > first_pt ) {
         std::stringstream exc;
         exc << "[ERROR] (EventSelector): ";
         exc << "Unsorted particle no. " << part - particles.begin();
         exc << " of type: " << (*part)->getName() << "!" << std::endl;
         exc << "Full info of unsorted particle:" << std::endl;
         (*part)->print( 1, exc );
         exc << "pt of previous particle: " << first_pt << std::endl;
         exc << "pt of all particles: " << std::endl;

         std::vector< pxl::Particle* >::const_iterator part2 = particles.begin();
         for( part2 = particles.begin(); part2 != particles.end(); ++part2 ) {
            exc << "pt: " << (*part2)->getPt() << std::endl;
         }
         throw Tools::unsorted_error( exc.str() );
      } else {
         first_pt = pt;
      }
   }
}



//--------------------This is the main method to perform the selection-----------------------------------------
void EventSelector::performSelection(EventView* EvtView, const int& JES) {   //used with either GenEvtView or RecEvtView
   string process = EvtView->getUserRecord("Process");
   bool isRec = (EvtView->getUserRecord("Type").asString() == "Rec");
   if (JES == -1){
       process += "_JES_DOWN";
       EvtView->setUserRecord("Process", process);
    } else if(JES == 1){
       process += "_JES_UP";
       EvtView->setUserRecord("Process", process);
    }

   const bool filterAccept = passFilterSelection( EvtView, isRec );
   EvtView->setUserRecord( "filter_accept", filterAccept );

   const bool L1_accept = m_triggerSelector.passL1Trigger( EvtView, isRec );
   EvtView->setUserRecord( "L1_accept", L1_accept );
   if(!m_useTrigger){
       EvtView->setUserRecord( "L1_accept", true );
   }

   double eleRho = 0.0;
   double gamRho = 0.0;
   // rho is only available in Rec.
   if( isRec ) {

      eleRho = EvtView->getUserRecord( m_ele_rho_label );

      double rho25 = EvtView->getUserRecord( m_rho_use );
      // TODO: Remove this block once corrected.
      // In the current version of the Skimmer, the wrong number value is stored
      // in the rho25 variable. Until the correct values are available, the
      // "wrong" one will be corrected.
      // In future, the following variables will be available:
      // "rho": rho value with eta_max = 5.0
      // "rho25": rho value with eta_max = 2.5
      // "rho44": rho value with eta_max = 4.4
      try {
         EvtView->getUserRecord( "rho" );
      } catch( std::runtime_error &e ) {
         // The correction factor is purley empirical and extracted from a very
         // limited set of MC (DY) and SingleMu events. This is only a temporary
         // solution!
         rho25 *= 0.9;
      }

      // Always use rho25 for Vgamma2011PhotonID!
      if( m_gam_Vgamma2011PhotonID_use )
         gamRho = rho25;
      else {
         // The recommended 'rho' for CutBasedPhotonID2012 is
         // rho(eta_max = 4.4). But in an earlier version of the Skimmer, it was
         // saved in the variable 'rho' rather than 'rho44'. So make sure
         // rho(eta_max = 4.4) is always used here.
         try {
            gamRho = EvtView->getUserRecord( m_gam_rho_label );
         } catch( std::runtime_error ) {
            gamRho = EvtView->getUserRecord( m_rho_use );
         }
      }
   }

   // get all particles
   vector<pxl::Particle*> allparticles;
   EvtView->getObjectsOfType<pxl::Particle>(allparticles);
   pxl::sortParticles( allparticles );

   // No 'bJets' are filled because 'jets' is only used in 'varyJESMET', where
   // all jets are treated exactly the same way.
   vector< pxl::Particle* > muons,
                            eles,
                            taus,
                            gammas,
                            jets,
                            mets,
                            s3_particles;
   for (vector<pxl::Particle*>::const_iterator part = allparticles.begin(); part != allparticles.end(); ++part) {
      string name = (*part)->getName();
      // Only fill the collection if we want to use the particle!
      // If the collections are not filled, the particles are also ignored in
      // the event cleaning.
      if( isRec ) {
         if(      m_muo_use and name == m_RecMuoName ) muons.push_back( *part );
         else if( m_ele_use and name == m_RecEleName ) eles.push_back( *part );
         else if( m_tau_use and name == m_RecTauName ) taus.push_back( *part );
         else if( m_gam_use and name == m_RecGamName ) gammas.push_back( *part );
         else if( m_jet_use and name == m_RecJetName ) jets.push_back( *part );
         else if( m_met_use and name == m_RecMETName ) mets.push_back( *part );
         // There are no S3 particles in Rec.
      } else {
         if(      m_muo_use and name == m_GenMuoName ) muons.push_back( *part );
         else if( m_ele_use and name == m_GenEleName ) eles.push_back( *part );
         else if( m_tau_use and name == m_GenTauName ) taus.push_back( *part );
         else if( m_gam_use and name == m_GenGamName ) gammas.push_back( *part );
         else if( m_jet_use and name == m_GenJetName ) jets.push_back( *part );
         else if( m_met_use and name == m_GenMETName ) mets.push_back( *part );
         else if( m_gen_use and name == "S3" ) s3_particles.push_back(*part);
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
   applyCutsOnMuon( muons, isRec );
   applyCutsOnEle( eles, eleRho, isRec );
   applyCutsOnTau( taus, isRec );
   applyCutsOnGam( gammas, gamRho, isRec );
   //first vary JES and then check corrected jets to pass cuts
   varyJES(jets, JES, isRec);
   applyCutsOnJet( jets, isRec );     //distribution into jets and b-jets
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
   applyCutsOnMET( mets, isRec );

   //now store the number of particles of each type
   countParticles( EvtView, muons,  "Muo", isRec );
   countParticles( EvtView, eles,   "Ele", isRec );
   countParticles( EvtView, taus,   "Tau", isRec );
   countParticles( EvtView, gammas, "Gam", isRec );
   countParticles( EvtView, mets,   "MET", isRec );
   countJets( EvtView, jets, isRec );
   applyCutsOnVertex( EvtView, vertices, isRec );

   //check global effects, e.g. HCAL noise
   bool global_accept = applyGlobalEventCuts( EvtView, vertices, eles, mets );

   bool topo_accept = passEventTopology( muons, eles, taus, gammas, jets, mets );
   EvtView->setUserRecord( "topo_accept", topo_accept );

   if(m_useTrigger){
       // Check if there are any unprescaled single muon or single electron
       // triggers.
       bool const MuEaccept = m_triggerSelector.checkHLTMuEle( EvtView, isRec );

       if( MuEaccept ) {
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
          EvtView->setUserRecord( "Veto", vetoed );

          bool const HLT_accept = m_triggerSelector.passHLTrigger( isRec,
                                                                   muons,
                                                                   eles,
                                                                   taus,
                                                                   gammas,
                                                                   jets,
                                                                   mets,
                                                                   EvtView
                                                                   );
          EvtView->setUserRecord( "HLT_accept", HLT_accept );

          bool const triggerAccept = HLT_accept and L1_accept;
          EvtView->setUserRecord( "trigger_accept", triggerAccept );

          bool const non_topo_accept = global_accept && filterAccept && triggerAccept && !vetoed;
          EvtView->setUserRecord( "non_topo_accept", non_topo_accept );

          //event accepted after all cuts
          bool accepted = topo_accept && non_topo_accept;
          EvtView->setUserRecord( "accepted", accepted );
       } else {
          EvtView->setUserRecord( "HLT_accept", false );
          EvtView->setUserRecord( "Veto", false );
          EvtView->setUserRecord( "trigger_accept", false );
          EvtView->setUserRecord( "non_topo_accept", false );
          EvtView->setUserRecord( "accepted", false );
       }
   }else{
       EvtView->setUserRecord( "HLT_accept", true );
       EvtView->setUserRecord( "Veto", true );
       EvtView->setUserRecord( "trigger_accept", true );
       EvtView->setUserRecord( "non_topo_accept", true );
       EvtView->setUserRecord( "accepted", true );
   }

   // For gen: check if generator cuts are fulfilled.
   if( !isRec ) {
      EvtView->setUserRecord( "generator_accept", m_gen_accept.passGeneratorCuts( EvtView, s3_particles ) );
   }
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

// FIXME: This doesn't work anymore (due to the changes in the particle naming)!
// In my opinion, this should be somewhere else and also should also be called
// from outside (form main function or so).
// Feel free to reimplement and remove this part!
//--------------Dump all the stuff inside the EventView--------------------------------------------------------

//void EventSelector::dumpEventView(const EventView* EvtView) {

   //cout << endl << "   Starting EventViewDump: " << endl
                //<< "   ======================= " << endl << endl;
   //cout << "      in detail : " << " #Ele = " << EvtView->getUserRecord("NumEle")
                                //<< ", #Mu = " << EvtView->getUserRecord("NumMuon")
                                //<< ", #Gam = " << EvtView->getUserRecord("NumGamma")
            //<< ", #" << m_met_type << " = " << EvtView->getUserRecord( "Num" + m_met_type )
            //<< ", #" << m_jet_algo << " = " << EvtView->getUserRecord( "Num" + m_jet_algo )
            //<< ", #" << m_jet_bJets_algo  << " = " << EvtView->getUserRecord( "Num" + m_jet_bJets_algo )
            //<< ", #Vertices = " << EvtView->getUserRecord("NumVertices") << endl;
   //cout << "      Type      : " <<  EvtView->findUserRecord<string>("Type") << endl;
   //// loop over all particles:
   //vector<Particle*> particles;
   //EvtView->getObjectsOfType<Particle>(particles);
   //pxl::sortParticles( particles );
   //for (vector<Particle*>::const_iterator part = particles.begin(); part != particles.end(); ++part) {
      //cout << "      ";
      //(*part)->print(0);
   //}
   //if (EvtView->findUserRecord<string>("Type") == "Rec") {

      ////this record steers if event is globally accepted by trigger, depending on "OR" of the HLT-path selected
      //cout << endl << "      HLT accept?   : " << EvtView->getUserRecord("HLT_accept", false) << endl;
   //}
   ////this record steers if event is globally accepted by event topology criteria
   //cout << "      Topo accept?   : " << EvtView->getUserRecord("Topo_accept", false) << endl;
//}
