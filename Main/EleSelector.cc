#include "EleSelector.hh"





//--------------------Constructor-----------------------------------------------------------------

EleSelector::EleSelector( const Tools::MConfig &cfg ):

   // general:
   m_ele_ID_use(         cfg.GetItem< std::string >( "Ele.ID.use" ) ),
   m_ele_pt_min(         cfg.GetItem< double      >( "Ele.pt.min" ) ),
   m_ele_eta_barrel_max( cfg.GetItem< double      >( "Ele.eta.Barrel.max" ) ),
   m_ele_eta_endcap_min( cfg.GetItem< double      >( "Ele.eta.Endcap.min" ) ),
   m_ele_eta_endcap_max( cfg.GetItem< double      >( "Ele.eta.Endcap.max" ) ),
   m_ele_invertIso(      cfg.GetItem< bool        >( "Ele.InvertIsolation" ) ),

   // CutBasedID (Tight):
   //m_ele_cbid_use( cfg.GetItem< bool >( "Ele.CBID.use" ) ),
   // lowEta: |eta| < 1.0
   m_ele_cbid_lowEta_EoP_min( cfg.GetItem< double >( "Ele.CBID.lowEta.EoverPin.min" ) ), // Only for 2011!
   m_ele_cbid_fBrem_min(      cfg.GetItem< double >( "Ele.CBID.fBrem.min" ) ), // Only for 2011!
   // Barrel values:
   m_ele_cbid_barrel_DEtaIn_max(              cfg.GetItem< double >( "Ele.CBID.Barrel.DEtaIn.max" ) ),
   m_ele_cbid_barrel_DPhiIn_max(              cfg.GetItem< double >( "Ele.CBID.Barrel.DPhiIn.max" ) ),
   m_ele_cbid_barrel_sigmaIetaIeta_max(       cfg.GetItem< double >( "Ele.CBID.Barrel.sigmaIetaIeta.max" ) ),
   m_ele_cbid_barrel_HoE_max(                 cfg.GetItem< double >( "Ele.CBID.Barrel.HoE.max" ) ),
   m_ele_cbid_barrel_Dxy_max(                 cfg.GetItem< double >( "Ele.CBID.Barrel.Dxy.max" ) ),
   m_ele_cbid_barrel_Dz_max(                  cfg.GetItem< double >( "Ele.CBID.Barrel.Dz.max" ) ),
   m_ele_cbid_barrel_RelInvEpDiff_max(        cfg.GetItem< double >( "Ele.CBID.Barrel.RelInvEpDiff.max" ) ),
   m_ele_cbid_barrel_PFIsoRel_max(            cfg.GetItem< double >( "Ele.CBID.Barrel.PFIsoRel.max" ) ),
   m_ele_cbid_barrel_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.CBID.Barrel.NInnerLayerLostHits.max" ) ),
   m_ele_cbid_barrel_Conversion_reject(       cfg.GetItem< bool   >( "Ele.CBID.Barrel.Conversion.reject" ) ),
   // Endcap values:
   m_ele_cbid_endcap_DEtaIn_max(              cfg.GetItem< double >( "Ele.CBID.Endcap.DEtaIn.max" ) ),
   m_ele_cbid_endcap_DPhiIn_max(              cfg.GetItem< double >( "Ele.CBID.Endcap.DPhiIn.max" ) ),
   m_ele_cbid_endcap_sigmaIetaIeta_max(       cfg.GetItem< double >( "Ele.CBID.Endcap.sigmaIetaIeta.max" ) ),
   m_ele_cbid_endcap_HoE_max(                 cfg.GetItem< double >( "Ele.CBID.Endcap.HoE.max" ) ),
   m_ele_cbid_endcap_Dxy_max(                 cfg.GetItem< double >( "Ele.CBID.Endcap.Dxy.max" ) ),
   m_ele_cbid_endcap_Dz_max(                  cfg.GetItem< double >( "Ele.CBID.Endcap.Dz.max" ) ),
   m_ele_cbid_endcap_RelInvEpDiff_max(        cfg.GetItem< double >( "Ele.CBID.Endcap.RelInvEpDiff.max" ) ),
   m_ele_cbid_endcap_PFIsoRel_max(            cfg.GetItem< double >( "Ele.CBID.Endcap.PFIsoRel.max" ) ),
   m_ele_cbid_endcap_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.CBID.Endcap.NInnerLayerLostHits.max" ) ),
   m_ele_cbid_endcap_Conversion_reject(       cfg.GetItem< bool   >( "Ele.CBID.Endcap.Conversion.reject" ) ),

   // HEEP ID v4.1:
   //m_ele_heepid_use(             cfg.GetItem< bool   >( "Ele.HEEPID.use" ) ),
   m_ele_heepid_EoP_max(           cfg.GetItem< double >( "Ele.HEEPID.EoP.max" ) ),
   m_ele_heepid_requireEcalDriven( cfg.GetItem< bool   >( "Ele.HEEPID.RequireEcalDriven" ) ),
   m_ele_heepid_rejectOutOfTime(   cfg.GetItem< bool   >( "Ele.HEEPID.RejectOutOfTime" ) ),
   // Barrel values:
   m_ele_heepid_barrel_deltaEta_max(            cfg.GetItem< double >( "Ele.HEEPID.Barrel.DEta.max" ) ),
   m_ele_heepid_barrel_deltaPhi_max(            cfg.GetItem< double >( "Ele.HEEPID.Barrel.DPhi.max" ) ),
   m_ele_heepid_barrel_HoEM_max(                cfg.GetItem< double >( "Ele.HEEPID.Barrel.HoEM.max" ) ),
   m_ele_heepid_barrel_trackiso_max(            cfg.GetItem< double >( "Ele.HEEPID.Barrel.TrkIso.max" ) ),
   m_ele_heepid_barrel_HcalD1_offset(           cfg.GetItem< double >( "Ele.HEEPID.Barrel.HcalD1.Offset" ) ),
   m_ele_heepid_barrel_HcalD1_slope(            cfg.GetItem< double >( "Ele.HEEPID.Barrel.HcalD1.Slope" ) ),
   m_ele_heepid_barrel_HcalD1_rhoSlope(         cfg.GetItem< double >( "Ele.HEEPID.Barrel.HcalD1.RhoSlope" ) ),
   m_ele_heepid_barrel_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.HEEPID.Barrel.NInnerLayerLostHits.max" ) ),
   m_ele_heepid_barrel_dxy_max(                 cfg.GetItem< double >( "Ele.HEEPID.Barrel.dxy.max" ) ),
   m_ele_heepid_barrel_e1x5_min(                cfg.GetItem< double >( "Ele.HEEPID.Barrel.E1x5.min" ) ),
   m_ele_heepid_barrel_e2x5_min(                cfg.GetItem< double >( "Ele.HEEPID.Barrel.E2x5.min" ) ),
   // Endcap values:
   m_ele_heepid_endcap_deltaEta_max(            cfg.GetItem< double >( "Ele.HEEPID.Endcap.DEta.max" ) ),
   m_ele_heepid_endcap_deltaPhi_max(            cfg.GetItem< double >( "Ele.HEEPID.Endcap.DPhi.max" ) ),
   m_ele_heepid_endcap_HoEM_max(                cfg.GetItem< double >( "Ele.HEEPID.Endcap.HoEM.max" ) ),
   m_ele_heepid_endcap_trackiso_max(            cfg.GetItem< double >( "Ele.HEEPID.Endcap.TrkIso.max" ) ),
   m_ele_heepid_endcap_HcalD1_offset(           cfg.GetItem< double >( "Ele.HEEPID.Endcap.HcalD1.Offset" ) ),
   m_ele_heepid_endcap_HcalD1_slope(            cfg.GetItem< double >( "Ele.HEEPID.Endcap.HcalD1.Slope" ) ),
   m_ele_heepid_endcap_HcalD1_rhoSlope(         cfg.GetItem< double >( "Ele.HEEPID.Endcap.HcalD1.RhoSlope" ) ),
   m_ele_heepid_endcap_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.HEEPID.Endcap.NInnerLayerLostHits.max" ) ),
   m_ele_heepid_endcap_dxy_max(                 cfg.GetItem< double >( "Ele.HEEPID.Endcap.dxy.max" ) ),
   m_ele_heepid_endcap_sigmaIetaIeta_max(       cfg.GetItem< double >( "Ele.HEEPID.Endcap.SigmaIetaIeta.max" ) )
{
}





//--------------------Destructor------------------------------------------------------------------

EleSelector::~EleSelector(){
}





bool EleSelector::passEle( pxl::Particle *ele, double const eleRho, bool const &isRec ) const {
   double const elePt = ele->getPt();
   // Updated transverse energy in Skimmer for HEEP selection (Supercluster based transverse energy).
   // TODO: Remove try-block once everything is reskimmed.
   double eleEt = ele->getEt();
   if( m_ele_ID_use == "HEEP") {
       try {
          eleEt = ele->getUserRecord( "SCEt" );
       } catch( std::runtime_error ) {
          // Do nothing, simply use the Et from the pxl::Particle.
       }
   }

   // Transverse energy cut.
   if( elePt < m_ele_pt_min ) return false;

   // eta
   double const abseta = isRec ? fabs( ele->getUserRecord( "SCeta" ).toDouble() ) : fabs( ele->getEta() );

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
      //stringstream warning;
      //warning << "WARNING: In passEle(...): ";
      //warning << "At this point ";
      //if( isRec ) warning << "(Rec) ";
      //else        warning << "(Gen) ";
      //warning << "electron should be in barrel or endcap. But:" << endl;
      //warning << "eta = " << abseta << endl;
      //warning << "pt  = " << elePt << endl;
      //warning << "Et  = " << eleEt << endl;
      //warning << "Ignoring this particle!" << endl;
      //cerr << warning.str();

      return false;
   }

   if( isRec ) {
      if( m_ele_ID_use == "CB" ) {
         bool const pass = passCBID( ele, elePt, abseta, barrel, endcap, eleRho );
         if( not pass )
            return false;
      } else if( m_ele_ID_use == "HEEP") {
         bool const pass = passHEEPID( ele, eleEt, barrel, endcap, eleRho );
         if( not pass )
            return false;
      } else {
          //stringstream error;
          //error << "In config file: ";
          //error << "'" << cfg.GetConfigFilePath() << "': ";
          //error << "'Ele.ID.use' must be either 'CB' or 'HEEP'!";
          //throw Tools::config_error( error.str() );
          return false;
      }
      
   } else {
      //gen ele cuts
      if( barrel ) {
         //ele in barrel
         bool iso_failed = false;
         if( m_ele_ID_use == "HEEP" )
            iso_failed = ele->getUserRecord( "GenIso" ).toDouble() > m_ele_heepid_barrel_trackiso_max;
         if( m_ele_ID_use == "CB" )
            iso_failed = ele->getUserRecord( "GenIso" ).toDouble() / elePt > m_ele_cbid_barrel_PFIsoRel_max;

         //turn around for iso-inversion
         if( m_ele_invertIso )
            iso_failed = not iso_failed;
         //now test
         if( iso_failed )
            return false;
      }

      //gen ele in endcap
      if( endcap ) {
         bool iso_failed = false;
         if(  m_ele_ID_use == "HEEP" )
            iso_failed = ele->getUserRecord( "GenIso" ).toDouble() > m_ele_heepid_barrel_trackiso_max;
         if( m_ele_ID_use == "CB" )
            iso_failed = ele->getUserRecord( "GenIso" ).toDouble() / elePt > m_ele_cbid_barrel_PFIsoRel_max;

         //turn around for iso-inversion
         if( m_ele_invertIso )
            iso_failed = not iso_failed;
         //now test
         if( iso_failed )
            return false;
      }
   }

   //no cut failed
   return true;
}





// Return true, if the electron passes the CutBasedID.
// See also:
// https://twiki.cern.ch/twiki/bin/view/CMS/EgammaCutBasedIdentification?rev=30
bool EleSelector::passCBID( pxl::Particle const *ele,
                              double const elePt,
                              double const eleAbsEta,
                              bool const eleBarrel,
                              bool const eleEndcap,
                              double const eleRho
                              ) const {
   // Retrieve each variable and IMMEDIATELY check if it passes the cut!

   double const DEtaIn = ele->getUserRecord( "DEtaSCVtx" );
   if( eleBarrel and DEtaIn > m_ele_cbid_barrel_DEtaIn_max )
      return false;
   if( eleEndcap and DEtaIn > m_ele_cbid_endcap_DEtaIn_max )
      return false;

   double const DPhiIn = ele->getUserRecord( "DPhiSCVtx" );
   if( eleBarrel and DPhiIn > m_ele_cbid_barrel_DPhiIn_max )
      return false;
   if( eleEndcap and DPhiIn > m_ele_cbid_endcap_DPhiIn_max )
      return false;

   double const sigmaIetaIeta = ele->getUserRecord( "sigmaIetaIeta" );
   if( eleBarrel and sigmaIetaIeta > m_ele_cbid_barrel_sigmaIetaIeta_max )
      return false;
   if( eleEndcap and sigmaIetaIeta > m_ele_cbid_endcap_sigmaIetaIeta_max )
      return false;

   double const HoE = ele->getUserRecord( "HoEm" );
   if( eleBarrel and HoE > m_ele_cbid_barrel_HoE_max )
      return false;
   if( eleEndcap and HoE > m_ele_cbid_endcap_HoE_max )
      return false;

   double const Dxy = ele->getUserRecord( "Dxy" );
   if( eleBarrel and Dxy > m_ele_cbid_barrel_Dxy_max )
      return false;
   if( eleEndcap and Dxy > m_ele_cbid_endcap_Dxy_max )
      return false;

   double const Dz = ele->getUserRecord( "Dz" );
   if( eleBarrel and Dz > m_ele_cbid_barrel_Dz_max )
      return false;
   if( eleEndcap and Dz > m_ele_cbid_endcap_Dz_max )
      return false;

   double const Energy = ele->getE();
   double const EoP = ele->getUserRecord( "EoP" );
   // p_in, the same as 'pat::Electron::trackMomentumAtVtx().p()'
   double const pIn = Energy / EoP;
   double const relInvEpDiff = std::abs( 1.0 / Energy - 1.0/ pIn );
   if( eleBarrel and relInvEpDiff > m_ele_cbid_barrel_RelInvEpDiff_max )
      return false;
   if( eleEndcap and relInvEpDiff > m_ele_cbid_endcap_RelInvEpDiff_max )
      return false;

   // check for isolation
   if( not passCBID_Isolation(ele, eleRho , eleBarrel, eleEndcap) )
      return false;

   double const NinnerLayerLostHits = ele->getUserRecord( "NinnerLayerLostHits" );
   if( eleBarrel and NinnerLayerLostHits > m_ele_cbid_barrel_NInnerLayerLostHits_max )
      return false;
   if( eleEndcap and NinnerLayerLostHits > m_ele_cbid_endcap_NInnerLayerLostHits_max )
      return false;

   //double const PFIso03PUCorrected = ele->getUserRecord( "PFIso03PUCorrected" );

   double const hasConversion = ele->getUserRecord( "hasMatchedConversion" );
   if( eleBarrel and m_ele_cbid_barrel_Conversion_reject and hasConversion )
      return true;
   if( eleEndcap and m_ele_cbid_endcap_Conversion_reject and hasConversion )
      return true;

   // NOTE: fbrem and E/pIn cuts only for 2011 analysis!
   // See also:
   // https://twiki.cern.ch/twiki/bin/view/CMS/EgammaCutBasedIdentification?rev=30#E_p_and_fbrem_based_tight_ID_201

   double const fBrem = ele->getUserRecord( "fbrem" );
   // It is OK that fBrem is too small, if we are in the lowEta region and the
   // EoP is large enough!
   bool const passfBrem = fBrem > m_ele_cbid_fBrem_min or
                          ( eleAbsEta < 1.0 and EoP > m_ele_cbid_lowEta_EoP_min );
   if( not passfBrem )
      return false;

   // All cuts passed!
   return true;
}





// This function returns true, if the given electron passes the HEEP Selection.
// See also:
// https://twiki.cern.ch/twiki/bin/view/CMS/HEEPElectronID?rev=65
bool EleSelector::passHEEPID( pxl::Particle const *ele,
                                double const eleEt,
                                bool const eleBarrel,
                                bool const eleEndcap,
                                double const eleRho
                                ) const {
   // Require electron to be ECAL driven?
   if( m_ele_heepid_requireEcalDriven and
       not ele->getUserRecord( "ecalDriven" )
       ) return false;

   if( ele->getUserRecord( "EoP" ).toDouble() > m_ele_heepid_EoP_max )
      return false;

   // These variables are checked in the barrel as well as in the endcaps.
   double const ele_absDeltaEta = fabs( ele->getUserRecord( "DEtaSCVtx" ).toDouble() );
   double const ele_absDeltaPhi = fabs( ele->getUserRecord( "DPhiSCVtx" ).toDouble() );
   double const ele_HoEM        = ele->getUserRecord( "HoEm" );
   double const ele_TrkIso      = ele->getUserRecord( "TrkIso03" );
   double const ele_ECALIso     = ele->getUserRecord( "ECALIso03" );
   double const ele_HCALIso     = ele->getUserRecord( "HCALIso03d1" );
   double const ele_CaloIso     = ele_ECALIso + ele_HCALIso;


   // TODO: Remove this construct when FA11 or older samples are not used anymore.
   // (Typo in skimmer already fixed. UserRecord: NinnerLayerLostHits.)
   int ele_innerLayerLostHits;
   try{
      ele_innerLayerLostHits = ele->getUserRecord( "NMissingHits: " );
   } catch( std::runtime_error ) {
      ele_innerLayerLostHits = ele->getUserRecord( "NinnerLayerLostHits" );
   } catch( ... ) {
      throw;
   }

   if( m_ele_heepid_rejectOutOfTime and
       ele->getUserRecord_def( "recoFlag",0 ).toInt32() == 2
       ) return false;

   //ele in barrel
   if( eleBarrel ) {
      //delta eta between SC and track
      if( ele_absDeltaEta > m_ele_heepid_barrel_deltaEta_max )
         return false;

      //delta phi between SC and track
      if( ele_absDeltaPhi > m_ele_heepid_barrel_deltaPhi_max )
         return false;

      //hadronic over EM
      if( ele_HoEM > m_ele_heepid_barrel_HoEM_max )
         return false;
      //shower shape
      double const e5x5 = ele->getUserRecord( "e5x5" );
      double const e1x5 = ele->getUserRecord( "e1x5" );
      double const e2x5 = ele->getUserRecord( "e2x5" );

      if( e1x5/e5x5 < m_ele_heepid_barrel_e1x5_min and
          e2x5/e5x5 < m_ele_heepid_barrel_e2x5_min
          ) return false;

      //Isolation
      bool iso_ok = true;
      //HCAL iso depth 1
      double const maxIso = m_ele_heepid_barrel_HcalD1_offset +
                            m_ele_heepid_barrel_HcalD1_slope * eleEt +
                            m_ele_heepid_barrel_HcalD1_rhoSlope * eleRho;

      if( iso_ok and ele_CaloIso > maxIso ) iso_ok = false;
      //Track iso
      if( iso_ok and ele_TrkIso > m_ele_heepid_barrel_trackiso_max ) iso_ok = false;

      //turn around for iso-inversion
      if( m_ele_invertIso ) iso_ok = not iso_ok;
      //now test
      if( !iso_ok ) return false;


      if( ele_innerLayerLostHits > m_ele_heepid_barrel_NInnerLayerLostHits_max )
         return false;

      if( ele->getUserRecord( "Dxy" ).toDouble() > m_ele_heepid_barrel_dxy_max )
         return false;
   }

   //ele in endcap
   if( eleEndcap ) {
      //delta eta between SC and track
      if( ele_absDeltaEta > m_ele_heepid_endcap_deltaEta_max )
         return false;

      //delta phi between SC and track
      if( ele_absDeltaPhi > m_ele_heepid_endcap_deltaPhi_max )
         return false;

      //hadronic over EM
      if( ele_HoEM > m_ele_heepid_endcap_HoEM_max )
         return false;

      //sigma iEta-iEta
      if( ele->getUserRecord( "sigmaIetaIeta" ).toDouble() > m_ele_heepid_endcap_sigmaIetaIeta_max )
         return false;

      //Isolation
      bool iso_ok = true;
      //HCAL iso depth 1
      double maxIso = m_ele_heepid_endcap_HcalD1_offset +
                      m_ele_heepid_endcap_HcalD1_rhoSlope * eleRho;

      //add a slope for high energy electrons
      if( eleEt > 50.0 ) maxIso += m_ele_heepid_endcap_HcalD1_slope * ( eleEt - 50.0 );
      //now test
      if( iso_ok and ele_CaloIso > maxIso )
         iso_ok = false;
      //Track iso
      if( iso_ok and ele_TrkIso > m_ele_heepid_endcap_trackiso_max )
         iso_ok = false;
      //turn around for iso-inversion
      if( m_ele_invertIso )
         iso_ok = not iso_ok;
      //now test
      if( not iso_ok )
         return false;

      if( ele_innerLayerLostHits > m_ele_heepid_endcap_NInnerLayerLostHits_max )
         return false;

      if( ele->getUserRecord( "Dxy" ).toDouble() > m_ele_heepid_endcap_dxy_max )
         return false;
   }

   return true;
}





bool EleSelector::passCBID_Isolation( pxl::Particle const *ele, double const &eleRho, bool const &eleBarrel, bool const &eleEndcap ) const {
   double const effArea = ele->getUserRecord( "EffectiveArea" );
   double const pfIsoCH = ele->getUserRecord( "PFIso03ChargedHadron" );
   double const pfIsoNH = ele->getUserRecord( "PFIso03NeutralHadron" );
   double const pfIsoPH = ele->getUserRecord( "PFIso03Photon" );
   double const pfIsoPUCorrected = pfIsoCH + std::max( 0.0, ( pfIsoPH + pfIsoNH ) - effArea * eleRho );

   bool iso_ok = true;
   if( eleBarrel and pfIsoPUCorrected/ele->getPt() > m_ele_cbid_barrel_PFIsoRel_max )
      iso_ok = false;
   if( eleEndcap and pfIsoPUCorrected/ele->getPt() > m_ele_cbid_endcap_PFIsoRel_max )
      iso_ok = false;

   // Turn around for Iso-inversion.
   if( m_ele_invertIso ) iso_ok = not iso_ok;
   // Now check.
   if( not iso_ok )
      return false;
      
   return true;
}
