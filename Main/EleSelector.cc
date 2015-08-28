#include "EleSelector.hh"
#include <sstream>



//--------------------Constructor-----------------------------------------------------------------

EleSelector::EleSelector( const Tools::MConfig &cfg ):

   // general:
   m_ele_id_type(         cfg.GetItem< std::string >( "Ele.ID.Type" , "HEEP") ),
   m_ele_id_ptswitch(     cfg.GetItem< double >( "Ele.ID.switchpt" , 100) ),
   m_ele_pt_min(         cfg.GetItem< double      >( "Ele.pt.min" ) ),
   m_ele_eta_barrel_max( cfg.GetItem< double      >( "Ele.eta.Barrel.max" ) ),
   m_ele_eta_endcap_min( cfg.GetItem< double      >( "Ele.eta.Endcap.min" ) ),
   m_ele_eta_endcap_max( cfg.GetItem< double      >( "Ele.eta.Endcap.max" ) ),
   m_ele_invertIso(      cfg.GetItem< bool        >( "Ele.InvertIsolation" ) ),

   // CutBasedID (Tight):
   //m_ele_cbid_use( cfg.GetItem< bool >( "Ele.CBID.use" ) ),
   // lowEta: |eta| < 1.0
   m_ele_cbid_usebool(       cfg.GetItem< bool   >( "Ele.CBID.usebool" , 1 ) ),
   m_ele_cbid_boolname(         cfg.GetItem< std::string >( "Ele.CBID.boolname" , "DefaultBoolname") ),
   m_ele_cbid_lowEta_EoP_min( cfg.GetItem< double >( "Ele.CBID.lowEta.EoverPin.min" , 0. ) ), // Only for 2011!
   m_ele_cbid_fBrem_min(      cfg.GetItem< double >( "Ele.CBID.fBrem.min" , 0. ) ), // Only for 2011!
   // Barrel values:
   m_ele_cbid_barrel_DEtaIn_max(              cfg.GetItem< double >( "Ele.CBID.Barrel.DEtaIn.max" , 0.004) ),
   m_ele_cbid_barrel_DPhiIn_max(              cfg.GetItem< double >( "Ele.CBID.Barrel.DPhiIn.max" , 0.03) ),
   m_ele_cbid_barrel_sigmaIetaIeta_max(       cfg.GetItem< double >( "Ele.CBID.Barrel.sigmaIetaIeta.max" , 0.01) ),
   m_ele_cbid_barrel_HoE_max(                 cfg.GetItem< double >( "Ele.CBID.Barrel.HoE.max" , 0.12 ) ),
   m_ele_cbid_barrel_Dxy_max(                 cfg.GetItem< double >( "Ele.CBID.Barrel.Dxy.max" , 0.12 ) ),
   m_ele_cbid_barrel_Dz_max(                  cfg.GetItem< double >( "Ele.CBID.Barrel.Dz.max" , 0.1 ) ),
   m_ele_cbid_barrel_RelInvEpDiff_max(        cfg.GetItem< double >( "Ele.CBID.Barrel.RelInvEpDiff.max" , 0.05 ) ),
   m_ele_cbid_barrel_PFIsoRel_max(            cfg.GetItem< double >( "Ele.CBID.Barrel.PFIsoRel.max" , 0.1 ) ),
   m_ele_cbid_barrel_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.CBID.Barrel.NInnerLayerLostHits.max" , 0 ) ),
   m_ele_cbid_barrel_Conversion_reject(       cfg.GetItem< bool   >( "Ele.CBID.Barrel.Conversion.reject" , 1 ) ),
   // Endcap values:
   m_ele_cbid_endcap_DEtaIn_max(              cfg.GetItem< double >( "Ele.CBID.Endcap.DEtaIn.max" , 0.005 ) ),
   m_ele_cbid_endcap_DPhiIn_max(              cfg.GetItem< double >( "Ele.CBID.Endcap.DPhiIn.max" , 0.02 ) ),
   m_ele_cbid_endcap_sigmaIetaIeta_max(       cfg.GetItem< double >( "Ele.CBID.Endcap.sigmaIetaIeta.max" , 0.03 ) ),
   m_ele_cbid_endcap_HoE_max(                 cfg.GetItem< double >( "Ele.CBID.Endcap.HoE.max" , 0.1 ) ),
   m_ele_cbid_endcap_Dxy_max(                 cfg.GetItem< double >( "Ele.CBID.Endcap.Dxy.max" , 0.02 ) ),
   m_ele_cbid_endcap_Dz_max(                  cfg.GetItem< double >( "Ele.CBID.Endcap.Dz.max" , 0.1 ) ),
   m_ele_cbid_endcap_RelInvEpDiff_max(        cfg.GetItem< double >( "Ele.CBID.Endcap.RelInvEpDiff.max" , 0.05 ) ),
   m_ele_cbid_endcap_PFIsoRel_max(            cfg.GetItem< double >( "Ele.CBID.Endcap.PFIsoRel.max" , 0.1 ) ),
   m_ele_cbid_endcap_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.CBID.Endcap.NInnerLayerLostHits.max" , 0 ) ),
   m_ele_cbid_endcap_Conversion_reject(       cfg.GetItem< bool   >( "Ele.CBID.Endcap.Conversion.reject" , 1 ) ),

   // HEEP ID v4.1:
   //m_ele_heepid_use(             cfg.GetItem< bool   >( "Ele.HEEPID.use" ) ),
   m_ele_heepid_usebool(       cfg.GetItem< bool   >( "Ele.HEEPID.usebool" , 1 ) ),
   m_ele_heepid_boolname(         cfg.GetItem< std::string >( "Ele.HEEPID.boolname" , "DefaultBoolname") ),
   m_ele_heepid_EoP_max(           cfg.GetItem< double >( "Ele.HEEPID.EoP.max" , 10 ) ),
   m_ele_heepid_requireEcalDriven( cfg.GetItem< bool   >( "Ele.HEEPID.RequireEcalDriven" , 1 ) ),
   m_ele_heepid_rejectOutOfTime(   cfg.GetItem< bool   >( "Ele.HEEPID.RejectOutOfTime" , 1 ) ),
   // Barrel values:
   m_ele_heepid_barrel_deltaEta_max(            cfg.GetItem< double >( "Ele.HEEPID.Barrel.DEta.max" , 0.05 ) ),
   m_ele_heepid_barrel_deltaPhi_max(            cfg.GetItem< double >( "Ele.HEEPID.Barrel.DPhi.max" , 0.06) ),
   m_ele_heepid_barrel_HoEM_max(                cfg.GetItem< double >( "Ele.HEEPID.Barrel.HoEM.max" , 0.05) ),
   m_ele_heepid_barrel_HoEM_slope(              cfg.GetItem< double >( "Ele.HEEPID.Barrel.HoEM.Slope" , 1) ),
   m_ele_heepid_barrel_trackiso_max(            cfg.GetItem< double >( "Ele.HEEPID.Barrel.TrkIso.max" , 5 ) ),
   m_ele_heepid_barrel_HcalD1_offset(           cfg.GetItem< double >( "Ele.HEEPID.Barrel.HcalD1.Offset" , 2 ) ),
   m_ele_heepid_barrel_HcalD1_slope(            cfg.GetItem< double >( "Ele.HEEPID.Barrel.HcalD1.Slope" , 0.03) ),
   m_ele_heepid_barrel_HcalD1_rhoSlope(         cfg.GetItem< double >( "Ele.HEEPID.Barrel.HcalD1.RhoSlope" , 0.28) ),
   m_ele_heepid_barrel_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.HEEPID.Barrel.NInnerLayerLostHits.max" , 1 ) ),
   m_ele_heepid_barrel_dxy_max(                 cfg.GetItem< double >( "Ele.HEEPID.Barrel.dxy.max" , 0.02 ) ),
   m_ele_heepid_barrel_e1x5_min(                cfg.GetItem< double >( "Ele.HEEPID.Barrel.E1x5.min" , 0.83 ) ),
   m_ele_heepid_barrel_e2x5_min(                cfg.GetItem< double >( "Ele.HEEPID.Barrel.E2x5.min" , 0.94 ) ),
   // Endcap values:
   m_ele_heepid_endcap_deltaEta_max(            cfg.GetItem< double >( "Ele.HEEPID.Endcap.DEta.max" , 0.007 ) ),
   m_ele_heepid_endcap_deltaPhi_max(            cfg.GetItem< double >( "Ele.HEEPID.Endcap.DPhi.max" , 0.06 ) ),
   m_ele_heepid_endcap_HoEM_max(                cfg.GetItem< double >( "Ele.HEEPID.Endcap.HoEM.max" , 0.05) ),
   m_ele_heepid_endcap_HoEM_slope(              cfg.GetItem< double >( "Ele.HEEPID.Endcap.HoEM.Slope" , 5) ),
   m_ele_heepid_endcap_trackiso_max(            cfg.GetItem< double >( "Ele.HEEPID.Endcap.TrkIso.max" , 5 ) ),
   m_ele_heepid_endcap_HcalD1_offset(           cfg.GetItem< double >( "Ele.HEEPID.Endcap.HcalD1.Offset" , 2.5 ) ),
   m_ele_heepid_endcap_HcalD1_slope(            cfg.GetItem< double >( "Ele.HEEPID.Endcap.HcalD1.Slope" , 0.03 ) ),
   m_ele_heepid_endcap_HcalD1_rhoSlope(         cfg.GetItem< double >( "Ele.HEEPID.Endcap.HcalD1.RhoSlope" , 0.28 ) ),
   m_ele_heepid_endcap_NInnerLayerLostHits_max( cfg.GetItem< int    >( "Ele.HEEPID.Endcap.NInnerLayerLostHits.max" , 1 ) ),
   m_ele_heepid_endcap_dxy_max(                 cfg.GetItem< double >( "Ele.HEEPID.Endcap.dxy.max" , 0.05 ) ),
   m_ele_heepid_endcap_sigmaIetaIeta_max(       cfg.GetItem< double >( "Ele.HEEPID.Endcap.SigmaIetaIeta.max" , 0.03) ),
   m_ele_EA( cfg , "Ele" )
{
   m_useAlternative=false;
}





//--------------------Destructor------------------------------------------------------------------

EleSelector::~EleSelector(){
}





int EleSelector::passEle( pxl::Particle *ele, double const eleRho, bool const &isRec ) const {
   bool passKin=true;
   bool passlooseID=true;
   bool passID=true;
   bool passIso=true;

   double const elePt = ele->getPt();
   //some times nan:
   if(elePt!=elePt){
      //the code is fucked up for this case just return with no id value
      //Waring if the numbering changes this has to be changed as well
      return 5;
   }

   // Updated transverse energy in Skimmer for HEEP selection (Supercluster based transverse energy).
   // TODO: Remove try-block once everything is reskimmed.
   double eleEt = ele->getEt();
   if( m_ele_id_type == "HEEP" or  (m_ele_id_type == "switch" and  elePt>m_ele_id_ptswitch) ) {
       try {
          eleEt = ele->getUserRecord( "SCEt" );
       } catch( std::runtime_error ) {
          // Do nothing, simply use the Et from the pxl::Particle.
       }
   }

   // Transverse energy cut.
   if( elePt < m_ele_pt_min ) passKin=false;

   // eta
   double const abseta = isRec ? fabs( ele->getUserRecord( "SCeta" ).toDouble() ) : fabs( ele->getEta() );

   //out of endcap
   if( abseta > m_ele_eta_endcap_max ) passKin=false;
   //between endcap and barrel
   if( abseta < m_ele_eta_endcap_min and abseta > m_ele_eta_barrel_max ) passKin=false;
   //ele in barrel?
   bool const barrel = abseta <= m_ele_eta_barrel_max;
   //ele in endcap?
   bool const endcap = abseta >= m_ele_eta_endcap_min and abseta <= m_ele_eta_endcap_max;

   if( barrel and endcap ) throw Tools::value_error( "In passEle(...): Electron cannot be in both barrel and endcap!" );
   if( not barrel and not endcap ) {
      //std::stringstream warning;
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
      passKin=false;
   }
   if( isRec ) {
      if( m_ele_id_type == "CB" or  (m_ele_id_type == "switch" and  elePt<=m_ele_id_ptswitch)) {
         //check ID
         try{
            if( not passlooseCBID( ele, elePt, abseta, barrel, endcap ) ){
               passlooseID=false;
               passID=false;
            }else if( not passtightCBID( ele, elePt, abseta, barrel, endcap ) ){
               passID=false;
            }
         }catch(std::runtime_error &e) {
            std::cout <<"CB ele "<< e.what() << '\n';
            m_useAlternative=true;
            m_alternativeUserVariables["full5x5_sigmaIetaIeta"]="sigmaIetaIeta";
            if (ele->hasUserRecord("hasMatchedConversion")){
               m_alternativeUserVariables["passConversionVeto"]="hasMatchedConversion";
            }else{
               m_alternativeUserVariables["passConversionVeto"]="passConversionVeto";
            }
            if( not passlooseCBID( ele, elePt, abseta, barrel, endcap ) ){
               passlooseID=false;
               passID=false;
            }else if( not passtightCBID( ele, elePt, abseta, barrel, endcap ) ){
               passID=false;
            }
         }

         // check for isolation
         if( not passCBID_Isolation(ele, eleRho , barrel, endcap) ){
            passIso=false;
         }
      } else if( m_ele_id_type == "HEEP" or  (m_ele_id_type == "switch" and  elePt>m_ele_id_ptswitch)) {

         try{
            //if( not passHEEPID( ele, eleEt, barrel, endcap ) ){
            if( not passlooseHEEPID( ele, eleEt, barrel, endcap ) ){
               passlooseID=false;
               passID=false;
            }else if (not passtightHEEPID( ele, eleEt, barrel, endcap ) ){
               passID=false;
            }
         }catch(std::runtime_error &e) {
            std::cout <<"HEEP ele "<< e.what() << '\n';
            m_useAlternative=true;
            m_alternativeUserVariables["full5x5_sigmaIetaIeta"]="sigmaIetaIeta";
            m_alternativeUserVariables["full5x5_e5x5"]="e5x5";
            m_alternativeUserVariables["full5x5_e2x5Max"]="e2x5";
            m_alternativeUserVariables["full5x5_e1x5"]="e1x5";
            if (ele->hasUserRecord("E")){
               m_alternativeUserVariables["SCE"]="E";
            }else{
               m_alternativeUserVariables["SCE"]="SCE";
            }
            if( not passlooseHEEPID( ele, eleEt, barrel, endcap ) ){
               passlooseID=false;
               passID=false;
            }else if (not passtightHEEPID( ele, eleEt, barrel, endcap ) ){
               passID=false;
            }
         }

         if(not passHEEP_Isolation(ele, eleEt, barrel, endcap, eleRho )){
            passIso=false;
         }
      } else {
          std::stringstream error;
          std::cout<<m_ele_id_type  <<"  "<<  m_ele_id_type <<"  "<<  ele->getEt()<<std::endl;
          error << "'Ele.ID.use' must be either 'CB' or 'HEEP'! The value is "<<m_ele_id_type;
          throw Tools::config_error( error.str() );
          passID=false;
      }

   } else {
      //gen ele cuts
      if( barrel ) {
         //ele in barrel
         bool iso_failed = false;
         if( m_ele_id_type == "HEEP" )
            iso_failed = ele->getUserRecord( "GenIso" ).toDouble() > m_ele_heepid_barrel_trackiso_max;
         if( m_ele_id_type == "CB" )
            iso_failed = ele->getUserRecord( "GenIso" ).toDouble() / elePt > m_ele_cbid_barrel_PFIsoRel_max;

         //turn around for iso-inversion
         if( m_ele_invertIso )
            iso_failed = not iso_failed;
         //now test
         if( iso_failed )
            passID=false;
      }

      //gen ele in endcap
      if( endcap ) {
         bool iso_failed = false;
         if(  m_ele_id_type == "HEEP" )
            iso_failed = ele->getUserRecord( "GenIso" ).toDouble() > m_ele_heepid_barrel_trackiso_max;
         if( m_ele_id_type == "CB" )
            iso_failed = ele->getUserRecord( "GenIso" ).toDouble() / elePt > m_ele_cbid_barrel_PFIsoRel_max;

         //turn around for iso-inversion
         if( m_ele_invertIso )
            iso_failed = not iso_failed;
         //now test
         if( iso_failed )
            passID=false;
      }
   }

   //no cut failed
   if(passKin && passID && passIso) return 0;
   else if (passKin && passlooseID && !passIso) return 5;
   else if (passKin && passlooseID && passIso) return 6;
   //give a hint what failed
   else if (passKin && passID && !passIso) return 1;
   else if (passKin && !passID && passIso) return 2;
   else if (passKin && !passID && !passIso) return 3;
   else if (!passKin && passID && passIso) return 4;
   return 7;
}





// Return true, if the electron passes the CutBasedID.
// See also:
// https://twiki.cern.ch/twiki/bin/view/CMS/EgammaCutBasedIdentification?rev=30
bool EleSelector::passlooseCBID( pxl::Particle const *ele,
                              double const elePt,
                              double const eleAbsEta,
                              bool const eleBarrel,
                              bool const eleEndcap
                              ) const {
   // First check if we want to use only id flags from miniaod or
   // reperform cuts
   if( m_ele_cbid_usebool ){
      if(ele->hasUserRecord( m_ele_cbid_boolname ) ){
         return ele->getUserRecord( m_ele_cbid_boolname );
      }else{
         std::cerr << "Error: You are tring to select Ele with CBID"<< std::endl;
         std::cerr << "But no user record is found for boolname: "<< m_ele_cbid_boolname <<std::endl;
         exit(1);
      }
    }

   double sigmaIetaIeta=0;
   if(!m_useAlternative){
      sigmaIetaIeta = ele->getUserRecord( "full5x5_sigmaIetaIeta" );
   }else{
      sigmaIetaIeta = ele->getUserRecord( m_alternativeUserVariables["full5x5_sigmaIetaIeta"] );
   }
   if( eleBarrel and sigmaIetaIeta > m_ele_cbid_barrel_sigmaIetaIeta_max )
      return false;
   if( eleEndcap and sigmaIetaIeta > m_ele_cbid_endcap_sigmaIetaIeta_max )
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


   double const NinnerLayerLostHits = ele->getUserRecord( "NinnerLayerLostHits" );
   if( eleBarrel and NinnerLayerLostHits > m_ele_cbid_barrel_NInnerLayerLostHits_max )
      return false;
   if( eleEndcap and NinnerLayerLostHits > m_ele_cbid_endcap_NInnerLayerLostHits_max )
      return false;

   double hasConversion=0;
   if(!m_useAlternative){
      hasConversion = ele->getUserRecord( "passConversionVeto" );
   }else{
      hasConversion = ele->getUserRecord(  m_alternativeUserVariables["passConversionVeto"] );
   }
   if( eleBarrel and m_ele_cbid_barrel_Conversion_reject and hasConversion )
      return true;
   if( eleEndcap and m_ele_cbid_endcap_Conversion_reject and hasConversion )
      return true;

   // All cuts passed!
   return true;
}

// Return true, if the electron passes the CutBasedID.
// See also:
// https://twiki.cern.ch/twiki/bin/view/CMS/EgammaCutBasedIdentification?rev=30
bool EleSelector::passtightCBID( pxl::Particle const *ele,
                              double const elePt,
                              double const eleAbsEta,
                              bool const eleBarrel,
                              bool const eleEndcap
                              ) const {
   // First check if we want to use only id flags from miniaod or
   // reperform cuts
   if( m_ele_cbid_usebool ){
      if(ele->hasUserRecord( m_ele_cbid_boolname ) ){
         return ele->getUserRecord( m_ele_cbid_boolname );
      }else{
         std::cerr << "Error: You are tring to select Ele with CBID"<< std::endl;
         std::cerr << "But no user record is found for boolname: "<< m_ele_cbid_boolname <<std::endl;
         exit(1);
      }
    }

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
   double sigmaIetaIeta=0;
   if(!m_useAlternative){
      sigmaIetaIeta = ele->getUserRecord( "full5x5_sigmaIetaIeta" );
   }else{
      sigmaIetaIeta = ele->getUserRecord( m_alternativeUserVariables["full5x5_sigmaIetaIeta"] );
   }
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

   double const NinnerLayerLostHits = ele->getUserRecord( "NinnerLayerLostHits" );
   if( eleBarrel and NinnerLayerLostHits > m_ele_cbid_barrel_NInnerLayerLostHits_max )
      return false;
   if( eleEndcap and NinnerLayerLostHits > m_ele_cbid_endcap_NInnerLayerLostHits_max )
      return false;


   double hasConversion=0;
   if(!m_useAlternative){
      hasConversion = ele->getUserRecord( "passConversionVeto" );
   }else{
      hasConversion = ele->getUserRecord(  m_alternativeUserVariables["passConversionVeto"] );
   }
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
bool EleSelector::passlooseHEEPID( pxl::Particle const *ele,
                                double const eleEt,
                                bool const eleBarrel,
                                bool const eleEndcap
                                ) const {

   // First check if we want to use only id flags from miniaod or
   // reperform cuts
   if( m_ele_heepid_usebool ){
      if(ele->hasUserRecord( m_ele_heepid_boolname ) ){
         return ele->getUserRecord( m_ele_heepid_boolname );
      }else{
         std::cerr << "Error: You are tring to select Ele with HEEPID"<< std::endl;
         std::cerr << "But no user record is found for boolname: "<< m_ele_heepid_boolname <<std::endl;
         exit(1);
      }
    }

   // Require electron to be ECAL driven?
   if( m_ele_heepid_requireEcalDriven and
       not ele->getUserRecord( "ecalDriven" )
       ) return false;

   if( ele->getUserRecord( "EoP" ).toDouble() > m_ele_heepid_EoP_max )
      return false;

   // These variables are checked in the barrel as well as in the endcaps.
   double const ele_absDeltaEta = fabs( ele->getUserRecord( "DEtaSCVtx" ).toDouble() );
   //double const ele_absDeltaPhi = fabs( ele->getUserRecord( "DPhiSCVtx" ).toDouble() );
   double const ele_HoEM        = ele->getUserRecord( "HoEm" );

   double ele_E=0;
   if(!m_useAlternative){
         ele_E = ele->getUserRecord( "SCE" );
   }else{
      ele_E= ele->getUserRecord( m_alternativeUserVariables["SCE"] );
   }

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
       ele->getUserRecord_def( "recoFlag",0 ).toUInt32() == 2
       ) return false;

   //ele in barrel
   if( eleBarrel ) {
      //delta eta between SC and track
      if( ele_absDeltaEta > m_ele_heepid_barrel_deltaEta_max )
         return false;

      //avoid division by zero in hadronic over EM
      if( ele_E == 0 )
         return false;

      //hadronic over EM
      if( ele_HoEM > (m_ele_heepid_barrel_HoEM_slope / ele_E + m_ele_heepid_barrel_HoEM_max) )
         return false;

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

      //avoid division by zero in hadronic over EM
      if( ele_E == 0 )
         return false;

      //hadronic over EM
      if( ele_HoEM > (m_ele_heepid_endcap_HoEM_slope/ele_E + m_ele_heepid_endcap_HoEM_max) )
         return false;

      if( ele_innerLayerLostHits > m_ele_heepid_endcap_NInnerLayerLostHits_max )
         return false;

      if( ele->getUserRecord( "Dxy" ).toDouble() > m_ele_heepid_endcap_dxy_max )
         return false;
   }
   return true;
}


// This function returns true, if the given electron passes the HEEP Selection.
// See also:
// https://twiki.cern.ch/twiki/bin/view/CMS/HEEPElectronID?rev=65
bool EleSelector::passtightHEEPID( pxl::Particle const *ele,
                                double const eleEt,
                                bool const eleBarrel,
                                bool const eleEndcap
                                ) const {

   // First check if we want to use only id flags from miniaod or
   // reperform cuts
   if( m_ele_heepid_usebool ){
      if(ele->hasUserRecord( m_ele_heepid_boolname ) ){
         return ele->getUserRecord( m_ele_heepid_boolname );
      }else{
         std::cerr << "Error: You are tring to select Ele with HEEPID"<< std::endl;
         std::cerr << "But no user record is found for boolname: "<< m_ele_heepid_boolname <<std::endl;
         exit(1);
      }
    }

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

   double ele_E=0;
   if(!m_useAlternative){
         ele_E = ele->getUserRecord( "SCE" );
   }else{
      ele_E= ele->getUserRecord( m_alternativeUserVariables["SCE"] );
   }



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
       ele->getUserRecord_def( "recoFlag",0 ).toUInt32() == 2
       ) return false;

   //ele in barrel
   if( eleBarrel ) {
      //delta eta between SC and track
      if( ele_absDeltaEta > m_ele_heepid_barrel_deltaEta_max )
         return false;

      //delta phi between SC and track
      if( ele_absDeltaPhi > m_ele_heepid_barrel_deltaPhi_max )
         return false;

      //avoid division by zero in hadronic over EM
      if( ele_E == 0 )
         return false;

      //hadronic over EM
      if( ele_HoEM > (m_ele_heepid_barrel_HoEM_slope / ele_E + m_ele_heepid_barrel_HoEM_max) )
         return false;
      //shower shape
      double e5x5=0;
      double e1x5=0;
      double e2x5=0;
      if(!m_useAlternative){
         e5x5 = ele->getUserRecord( "full5x5_e5x5" );
         e1x5 = ele->getUserRecord( "full5x5_e1x5" );
         e2x5 = ele->getUserRecord( "full5x5_e2x5Max" );
      }else{
         e5x5 = ele->getUserRecord( m_alternativeUserVariables["full5x5_e5x5"] );
         e1x5 = ele->getUserRecord( m_alternativeUserVariables["full5x5_e1x5"] );
         e2x5 = ele->getUserRecord( m_alternativeUserVariables["full5x5_e2x5Max"] );
      }


      if( e1x5/e5x5 < m_ele_heepid_barrel_e1x5_min and
          e2x5/e5x5 < m_ele_heepid_barrel_e2x5_min
          ) return false;

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

      //avoid division by zero in hadronic over EM
      if( ele_E == 0 )
         return false;

      //hadronic over EM
      if( ele_HoEM > (m_ele_heepid_endcap_HoEM_slope/ele_E + m_ele_heepid_endcap_HoEM_max) )
         return false;

      //sigma iEta-iEta
      if(!m_useAlternative){
         if( ele->getUserRecord( "full5x5_sigmaIetaIeta" ).toDouble() > m_ele_heepid_endcap_sigmaIetaIeta_max )
            return false;
      }else{
         if( ele->getUserRecord( m_alternativeUserVariables["full5x5_sigmaIetaIeta"] ).toDouble() > m_ele_heepid_endcap_sigmaIetaIeta_max )
            return false;
      }


      if( ele_innerLayerLostHits > m_ele_heepid_endcap_NInnerLayerLostHits_max )
         return false;

      if( ele->getUserRecord( "Dxy" ).toDouble() > m_ele_heepid_endcap_dxy_max )
         return false;
   }
   return true;
}


bool EleSelector::passHEEP_Isolation(pxl::Particle const *ele, double const eleEt, bool const eleBarrel, bool const eleEndcap, double const eleRho) const {

   double const ele_TrkIso      = ele->getUserRecord( "TrkIso03" );
   double const ele_ECALIso     = ele->getUserRecord( "ECALIso03" );
   double const ele_HCALIso     = ele->getUserRecord( "HCALIso03d1" );
   double const ele_CaloIso     = ele_ECALIso + ele_HCALIso;
   //ele in barrel
   if( eleBarrel ) {
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
   }
   //ele in endcap
   else if( eleEndcap ) {
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

   }else{
      return false;
   }
   return true;
}





bool EleSelector::passCBID_Isolation( pxl::Particle const *ele, double const &eleRho, bool const &eleBarrel, bool const &eleEndcap ) const {
   double  effArea=0;
   double  pfIsoCH=0;
   double  pfIsoNH=0;
   double  pfIsoPH=0;
   //double  pfIsoPU=0;
   //std::cout<<ele->hasUserRecord("EffectiveArea")<<std::endl;

   if (ele->hasUserRecord("EffectiveArea")){
      effArea = ele->getUserRecord( "EffectiveArea" );
      pfIsoCH = ele->getUserRecord( "PFIso03ChargedHadron" );
      pfIsoNH = ele->getUserRecord( "PFIso03NeutralHadron" );
      pfIsoPH = ele->getUserRecord( "PFIso03Photon" );
   }else{
      effArea = m_ele_EA.getEffectiveArea( fabs(ele->getEta()), EffectiveArea::chargedHadron );
      pfIsoCH = ele->getUserRecord( "chargedHadronIso" );
      pfIsoNH = ele->getUserRecord( "neutralHadronIso" );
      pfIsoPH = ele->getUserRecord( "photonIso" );
      //pfIsoPU = ele->getUserRecord( "puChargedHadronIso" );
   }



   double pfIsoPUCorrected = pfIsoCH + std::max( 0.0, ( pfIsoPH + pfIsoNH ) - effArea * eleRho );

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
