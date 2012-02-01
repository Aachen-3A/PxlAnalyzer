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
   m_runOnData(      cfg.GetItem< bool >( "General.RunOnData" ) ),
   m_useCharge(      cfg.GetItem< bool >( "General.UseCharge" ) ),
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

   // Triggers:
   m_ignoreL1(       cfg.GetItem< bool   >( "Trigger.IgnoreL1" ) ),
   m_ignoreHLT(      cfg.GetItem< bool   >( "Trigger.IgnoreHL" ) ),
   m_trigger_prefix( cfg.GetItem< string >( "Trigger.Name" ) + "_" ),
   m_trigger_groups( getTriggerGroups( cfg ) ),

   // Electrons:
   m_ele_num_min(                        cfg.GetItem< int    >( "Ele.N.min" ) ),
   m_ele_trigger_pt_min(                 cfg.GetItem< double >( "Ele.Trigger.pt.min" ) ),

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
   m_muo_num_min(                    cfg.GetItem< int    >( "Muon.N.min" ) ),
   m_muo_trigger_pt_min(             cfg.GetItem< double >( "Muon.Trigger.pt.min" ) ),

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
   m_tau_num_min( cfg.GetItem< int    >( "Tau.Nmin" ) ),
   m_tau_pt_min(  cfg.GetItem< double >( "Tau.pt.min" ) ),
   m_tau_eta_max( cfg.GetItem< double >( "Tau.Eta.max" ) ),
   //Get Tau-Discriminators and save them
   m_tau_discriminators( Tools::splitString< string >( cfg.GetItem< string >( "Tau.Discriminators" ), true ) ),

   // Photons:
   m_gam_num_min(                  cfg.GetItem< int    >( "Gamma.N.min" ) ),
   m_gam_trigger_pt_min(           cfg.GetItem< double >( "Gamma.Trigger.pt.min" ) ),

   m_gam_pt_min(                   cfg.GetItem< double >( "Gamma.pt.min" ) ),
   m_gam_eta_barrel_max(           cfg.GetItem< double >( "Gamma.Eta.Barrel.max" ) ),
   m_gam_eta_endcap_min(           cfg.GetItem< double >( "Gamma.Eta.Endcap.min" ) ),
   m_gam_eta_endcap_max(           cfg.GetItem< double >( "Gamma.Eta.Endcap.max" ) ),
   m_gam_useEndcap(                cfg.GetItem< bool   >( "Gamma.UseEndcap" ) ),
   m_gam_useConverted(             cfg.GetItem< bool   >( "Gamma.UseConverted" ) ),
   m_gam_useSeedVeto(              cfg.GetItem< bool   >( "Gamma.UseSeedVeto" ) ),
   m_gam_rejectOutOfTime(          cfg.GetItem< bool   >( "Gamma.RejectOutOfTime" ) ),
   m_gam_HoEm_max(                 cfg.GetItem< double >( "Gamma.HoEm.max" ) ),
   m_gam_corrFactor_max(           cfg.GetItem< double >( "Gamma.CorrFactor.max" ) ),
   // Barrel:
   m_gam_barrel_sigmaIetaIeta_min( cfg.GetItem< double >( "Gamma.Barrel.SigmaIetaIeta.min" ) ),
   m_gam_barrel_sigmaIetaIeta_max( cfg.GetItem< double >( "Gamma.Barrel.SigmaIetaIeta.max" ) ),
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
   m_gam_endcap_sigmaIetaIeta_max( cfg.GetItem< double >( "Gamma.Endcap.SigmaIetaIeta.max" ) ),
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
   m_jet_num_min(              cfg.GetItem< int    >( "Jet.N.min" ) ),
   m_jet_trigger_pt_min(       cfg.GetItem< double >( "Jet.Trigger.pt.min" ) ),

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
   m_met_num_min(        cfg.GetItem< int    >( "MET.N.min" ) ),
   m_met_trigger_pt_min( cfg.GetItem< double >( "MET.Trigger.pt.min" ) ),

   m_met_type(           cfg.GetItem< string >( "MET.Type" ) ),
   m_met_pt_min(         cfg.GetItem< double >( "MET.pt.min" ) ),
   m_met_dphi_ele_min(   cfg.GetItem< double >( "MET.dPhi.Ele.min" ) ),

   // HCAL noise:
   m_hcal_noise_ID_use(  cfg.GetItem< bool   >( "HCAL.Noise.ID.use" ) ),
   m_hcal_noise_ID_name( cfg.GetItem< string >( "HCAL.Noise.ID.name" ) ),

   // If all cuts are disabled, we don't need to cut.
   m_no_topo_cut( m_muo_num_min < 0 and m_ele_num_min < 0 and m_gam_num_min < 0 and m_jet_num_min < 0 and m_met_num_min < 0 ),

   // To access the JEC uncertainties from file.
   m_jecUnc( Tools::ExpandPath( cfg.GetItem< string >( "Jet.Error.JESFile" ) ) ),

   // Particle Matcher and Duplicate Remover.
   m_matcher(),
   m_duplicate(),

   m_rho25( 0.0 )
{
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


std::vector< trigger_info > EventSelector::getTriggerGroups( const Tools::MConfig &config ) const {
   vector< trigger_info > trigger_groups;

   const int num_trigger_groups = config.GetItem< int >( "Trigger.Groups" );
   for( int i = 1; i <= num_trigger_groups; ++i ) {
      const string prefix = "Trigger." + Tools::toString( i ) + ".";
      trigger_info one_group;
      one_group.name    = config.GetItem< string >( prefix + "Name" );
      one_group.require = config.GetItem< bool   >( prefix + "Require" );
      one_group.reject  = config.GetItem< bool   >( prefix + "Reject" );

      Tools::splitString( one_group.triggers, config.GetItem< string >( prefix + "Triggers"  ), ",", true );
      trigger_groups.push_back( one_group );
   }

   return trigger_groups;
}


//--------------------Function that checks for trigger is called witin applyCutOnParticles !!!-----------------------------------------------------------------

bool EventSelector::passTriggerSelection( EventView *EvtView,
                                          bool const isRec,
                                          std::vector< pxl::Particle* > const &muons,
                                          std::vector< pxl::Particle* > const &eles,
                                          std::vector< pxl::Particle* > const &taus,
                                          std::vector< pxl::Particle* > const &gammas,
                                          std::vector< pxl::Particle* > const &jets,
                                          std::vector< pxl::Particle* > const &mets
                                          ) {
   //check HLT
   bool HLTaccept = passHLTrigger( EvtView, isRec );
   EvtView->setUserRecord< bool >( "HLT_accept", HLTaccept );

   //check L1 technical bits
   bool L1_accept = passL1Trigger( EvtView, isRec );
   EvtView->setUserRecord< bool >( "L1_accept", L1_accept );

   bool trigger_particle_accept = passTriggerParticles( isRec, muons, eles, taus, gammas, jets, mets );
   EvtView->setUserRecord< bool >( "trigger_particle_accept", trigger_particle_accept );

   //global trigger accept
   bool triggerAccept = HLTaccept && L1_accept && trigger_particle_accept;
   EvtView->setUserRecord< bool >( "trigger_accept", triggerAccept );

   return triggerAccept;
}

//--------------------Check L1 technical bits-----------------------------------------
bool EventSelector::passL1Trigger( pxl::EventView* EvtView, const bool isRec ) {
   //ignore L1
   if( m_ignoreL1 ) return true;

   //no "L1 simulation" on Gen level
   if( !isRec ) return true;

   //get the bits
   bool b0 = EvtView->findUserRecord< bool >( m_trigger_prefix+"L1_0" );

   //bit 0 is not always properly simulated, but it doesn't make sense in MC anyway
   if( !m_runOnData ) b0 = true;

   return b0;
}




bool EventSelector::passHLTrigger( EventView *EvtView, const bool isRec ){
   if( m_ignoreHLT ) {
      //accept all
      return true;
   } else if( isRec ) {
      //accept all events if no triggers are configured
      if( m_trigger_groups.size() == 0 ) return true;

      //stores if all required triggers fired
      bool required_accept = true;
      bool any_accept = false;

      //loop trigger groups
      for( std::vector< trigger_info >::const_iterator group = m_trigger_groups.begin();
           group != m_trigger_groups.end();
           ++group ) {
         bool accepted = false;
         //loop over trigger names
         for( std::vector< std::string >::const_iterator trigger = group->triggers.begin();
              trigger != group->triggers.end();
              ++trigger ) {
            //check this trigger
            if( EvtView->findUserRecord<bool>( m_trigger_prefix+*trigger ) ){
               accepted = true;
               break; //if we found one fired trigger, we don't care for the rest
            }
         }
         //store the result
         EvtView->setUserRecord< bool >( "HLTAccept_"+group->name, accepted );
         //check what we found
         if( accepted ) {
            any_accept = true;
         } else if( group->require ) {
            //a required trigger group was false
            required_accept = false;
         }
      }

      //we're good if any trigger fired and no required group is missing
      return any_accept && required_accept;
   } else {
      // in case of Gen HLT is true by definition
      return true;
   }
}



bool EventSelector::passTriggerParticles( const bool isRec,
                                          const std::vector< pxl::Particle* > &muons,
                                          const std::vector< pxl::Particle* > &eles,
                                          const std::vector< pxl::Particle* > &taus,
                                          const std::vector< pxl::Particle* > &gammas,
                                          const std::vector< pxl::Particle* > &jets,
                                          const std::vector< pxl::Particle* > &mets
                                          ) const {
   //no need to check this on gen level
   if( !isRec ) return true;

   for( vector< Particle* >::const_iterator tau = taus.begin(); tau != taus.end(); ++tau ) {
      if( ( m_no_topo_cut || m_tau_num_min >= 0 )
          && (*tau)->getPt() > m_tau_pt_min
          ) return true;
   }

   for( vector< Particle* >::const_iterator muon = muons.begin(); muon != muons.end(); ++muon ) {
      if( ( m_no_topo_cut || m_muo_num_min >= 0 )
          && (*muon)->getPt() > m_muo_trigger_pt_min
          ) return true;
   }
   for( vector< Particle* >::const_iterator ele = eles.begin(); ele != eles.end(); ++ele ) {
      if( ( m_no_topo_cut || m_ele_num_min >= 0 )
          && (*ele)->getPt() > m_ele_trigger_pt_min
          ) return true;
   }
   for( vector< Particle* >::const_iterator gamma = gammas.begin(); gamma != gammas.end(); ++gamma ) {
      if( ( m_no_topo_cut || m_gam_num_min >= 0 )
          && (*gamma)->getPt() > m_gam_trigger_pt_min
          ) return true;
   }
   for( vector< Particle* >::const_iterator jet = jets.begin(); jet != jets.end(); ++jet ) {
      if( ( m_no_topo_cut || m_jet_num_min >= 0 )
          && (*jet)->getPt() > m_jet_trigger_pt_min
          ) return true;
   }
   for( vector< Particle* >::const_iterator met = mets.begin(); met != mets.end(); ++met ) {
      if( ( m_no_topo_cut || m_met_num_min >= 0 )
          && (*met)->getPt() > m_met_trigger_pt_min
          ) return true;
   }


   //no good particle, so fail the cut
   return false;
}



bool EventSelector::checkVeto( pxl::EventView* EvtView, const bool isRec ){
   if( m_ignoreHLT ) {
      // triggers ignored
      return false;
   } else if( isRec ) {
      //loop over groups
      for( std::vector< trigger_info >::const_iterator group = m_trigger_groups.begin();
           group != m_trigger_groups.end();
           ++group ) {
         //and veto if any rejected trigger group has fired
         if( group->reject && EvtView->findUserRecord< bool >( "HLTAccept_"+group->name ) ) return true;
      }

      //no rejected group fired, so we're good
      return false;
   } else {
      //no veto for gen
      return false;
   }
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
         if( gam_sigma_ieta_ieta > m_gam_endcap_sigmaIetaIeta_max ) return false;

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

      //Hadronic / electromagnetic energy fraction
      if( gam->findUserRecord< double >( "HoEm" ) > m_gam_HoEm_max ) return false;

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

      //track (pixel seed) veto
      if( m_gam_useSeedVeto and gam->findUserRecord< bool >( "HasSeed" ) ) return false;

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


//--------------------Apply cuts on Topology-----------------------------------------------------------------
bool EventSelector::applyCutsOnTopology( std::vector< pxl::Particle* > const &muons,
                                         std::vector< pxl::Particle* > const &eles,
                                         std::vector< pxl::Particle* > const &taus,
                                         std::vector< pxl::Particle* > const &gammas,
                                         std::vector< pxl::Particle* > const &jets,
                                         std::vector< pxl::Particle* > const &mets
                                         ) {
   int numMuon = muons.size();
   int numEle = eles.size();
   int numTau = taus.size();
   int numGamma = gammas.size();
   int numJet = jets.size();
   int numMET = mets.size();

   return checkTopology( numMuon, numEle, numTau, numGamma, numJet, numMET );
}


bool EventSelector::checkTopology( const int muons, const int eles, const int taus, const int gammas, const int jets, const int METs ) {
   if( m_no_topo_cut ) return true;

   if( m_muo_num_min >= 0 && muons  >= m_muo_num_min ) return true;
   if( m_ele_num_min >= 0 && eles   >= m_ele_num_min ) return true;
   if( m_tau_num_min >= 0 && taus   >= m_tau_num_min ) return true;
   if( m_gam_num_min >= 0 && gammas >= m_gam_num_min ) return true;
   if( m_jet_num_min >= 0 && jets   >= m_jet_num_min ) return true;
   if( m_met_num_min >= 0 && METs   >= m_met_num_min ) return true;

   //no cut passed, so return failure
   return false;
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

   // rho is only available in Rec.
   // It defaults to 0!
   if( isRec ) m_rho25 = EvtView->findUserRecord< double >( "rho25" );

   // get all particles
   vector<pxl::Particle*> allparticles;
   EvtView->getObjectsOfType<pxl::Particle>(allparticles);
   pxl::sortParticles( allparticles );

   vector< pxl::Particle* > muons, eles, taus, gammas, jets, mets, doc_particles;     // no 'bJets' because jets is only filled into varyJESMET, where all jets are treated exactly the same way
   for (vector<pxl::Particle*>::const_iterator part = allparticles.begin(); part != allparticles.end(); ++part) {
      string name = (*part)->getName();
      if( name == "Muon") muons.push_back(*part);
      else if( name == "Ele") eles.push_back(*part);
      else if( name == "Tau" ) taus.push_back( *part );
      else if( name == "Gamma") gammas.push_back(*part);
      else if( name == m_jet_algo ) jets.push_back( *part );
      else if( name == m_met_type ) mets.push_back( *part );
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
   if( !m_ignoreOverlaps ) m_duplicate.removeOverlap( muons, eles, taus, gammas, jets, isRec );

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

   bool topo_accept = applyCutsOnTopology( muons, eles, taus, gammas, jets, mets );
   EvtView->setUserRecord< bool >( "topo_accept", topo_accept );

   //for gen: check the binning value
   if( !isRec ) {
      EvtView->setUserRecord< bool >( "binning_accept", applyGeneratorCuts( EvtView, doc_particles ) );
   }

   //check global effects, e.g. HCAL noise
   bool global_accept = applyGlobalEventCuts( EvtView, vertices, eles, mets );

   // check for Trigger
   // this record steers if event is globally accepted by trigger, depending on "OR" of the HLT-path selected
   bool triggerAccept = passTriggerSelection( EvtView, isRec, muons, eles, taus, gammas, jets, mets );

   //check if the events must be vetoed
   bool vetoed = checkVeto( EvtView, isRec );
   EvtView->setUserRecord< bool >( "Veto", vetoed );

   bool filterAccept = passFilterSelection( EvtView, isRec );
   EvtView->setUserRecord< bool >( "filter_accept", filterAccept );

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
      cout << "      Trigger: " << m_trigger_prefix << endl;
      for( std::vector< trigger_info >::const_iterator group = m_trigger_groups.begin();
           group != m_trigger_groups.end();
           ++group ) {
         if( EvtView->findUserRecord< bool >( "HLTAccept_"+group->name ) ) {
            cout << "Group: " << group->name << endl;
            for( std::vector< std::string >::const_iterator trigger = group->triggers.begin();
                 trigger != group->triggers.end();
                 ++trigger ) {
               //check this trigger
               if( EvtView->findUserRecord<bool>( m_trigger_prefix+*trigger ) ){
                  cout << *trigger << "";
               }
            }
         }
      }

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
