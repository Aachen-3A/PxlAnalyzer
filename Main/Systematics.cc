#include "Systematics.hh"
#include <vector>
#include "Tools/PXL/Sort.hh"
#include "TRandom3.h"


//--------------------Constructor-----------------------------------------------------------------

Systematics::Systematics(const Tools::MConfig &cfg, unsigned int const debug):
   // read uncertainties from config
   m_ratioEleBarrel(cfg.GetItem< double      >( "Ele.Syst.Scale.Barrel" ) ),
   m_ratioEleEndcap(cfg.GetItem< double      >( "Ele.Syst.Scale.Endcap" ) ),
   m_scaleMuo(      cfg.GetItem< double      >( "Muon.Syst.Scale" ) ),
   m_resMuo(        cfg.GetItem< double      >( "Muon.Syst.Res" ) ),
   m_ratioTau(      cfg.GetItem< double      >( "Tau.Syst.Scale" ) ),
   // read lepton names from config
   m_TauType(       cfg.GetItem< std::string >( "Tau.Type.Rec" ) ),
   m_JetType(       cfg.GetItem< std::string >( "Jet.Type.Rec" ) ),
   m_METType(       cfg.GetItem< std::string >( "MET.Type.Rec" ) ),

   // To access the JEC uncertainties from file.
   m_jecType( Tools::ExpandPath( cfg.GetItem< std::string >( "Jet.Error.JESType" ) ) ),
   m_jecPara( Tools::ExpandPath( cfg.GetItem< std::string >( "Jet.Error.JESFile" ) ), m_jecType ),
   m_jecUnc( m_jecPara ),

   m_jetRes( cfg ),

   m_debug(debug)


{
    rand = new TRandom3();
}



//--------------------Destructor------------------------------------------------------------------

Systematics::~Systematics(){
    delete rand;
}



//------------
//public------
//------------

// get all particles from the event and put them into vectors for later use
void Systematics::init(pxl::Event* event){
   m_event = event;
   m_eventView = m_event->getObjectOwner().findObject< pxl::EventView >( "Rec" );
   if (m_eventView == 0) {
      std::cout << "No EventView 'Rec' found in event:" << m_event->toString() << std::endl;
      return;
   }

   // clear lists from previous event
   MuonList.clear();
   EleList.clear();
   TauList.clear();
   JetList.clear();
   METList.clear();
   UnclusteredEnUp.clear();
   UnclusteredEnDown.clear();

   // get all particles
   std::vector< pxl::Particle* > AllParticles;
   m_eventView->getObjectsOfType< pxl::Particle >( AllParticles );
   pxl::sortParticles( AllParticles );
   // push them into the corresponding vectors
   for( std::vector< pxl::Particle* >::const_iterator part_it = AllParticles.begin(); part_it != AllParticles.end(); ++part_it ) {
      pxl::Particle *part = *part_it;
      std::string Name = part->getName();
      // Only fill the collection if we want to use the particle!
      if(      Name == "Muon")     MuonList.push_back( part );
      else if( Name == "Ele" )     EleList.push_back( part );
      else if( Name == m_TauType ) TauList.push_back( part );
      else if( Name == m_JetType ) JetList.push_back( part );
      else if( Name == m_METType ) METList.push_back( part );


   }

   m_GenEvtView = m_event->getObjectOwner().findObject< pxl::EventView >( "Gen" );
   std::vector< pxl::Particle* > GenParticles;
   m_GenEvtView->getObjectsOfType< pxl::Particle >( GenParticles );
   for( std::vector< pxl::Particle* >::const_iterator part_it = GenParticles.begin(); part_it != GenParticles.end(); ++part_it ) {
      pxl::Particle *part = *part_it;
      std::string Name = part->getName();
      // Only fill the collection if we want to use the particle!
      //copy already shifted MET from Event:
      if( Name == m_METType+"uncert_10") UnclusteredEnUp.push_back( part );
      else if( Name == m_METType+"uncert_11") UnclusteredEnDown.push_back( part );
  }




   return;
}



void Systematics::shiftMuoAndMET(std::string const shiftType){
   if(!checkshift(shiftType)) return;

   bool do_resolution = false;

   if(shiftType == "Resolution"){
      do_resolution = true;
   }

   double sca_ratio = m_scaleMuo;
   double res_ratio = m_resMuo;
   double ratio_up, ratio_down;
   double dPx_up=0;
   double dPy_up=0;
   double dPx_down=0;
   double dPy_down=0;

   // create new EventViews inside the event
   pxl::EventView* EventViewMuonUp   = m_event->getObjectOwner().create< pxl::EventView >();
   pxl::EventView* EventViewMuonDown = m_event->getObjectOwner().create< pxl::EventView >();
   m_event->setIndex(std::string("Muon") + "_syst" + shiftType + "Up",   EventViewMuonUp);
   m_event->setIndex(std::string("Muon") + "_syst" + shiftType + "Down", EventViewMuonDown);

   fillMETLists(EventViewMuonUp, EventViewMuonDown);

   // add shifted particles to these EventViews
   for(unsigned int i = 0; i < MuonList.size(); i++){
      if(do_resolution){
         /// muon momentum resolution
         //double resolution_ratio = rand->Gaus(0,muon_resolution_histo -> FindBin(MuonList.at(i)->getPt()) * res_ratio);
         double resolution_ratio = rand->Gaus(0,0.1 * res_ratio);
         ratio_up   = 1 + fabs(resolution_ratio);
         ratio_down = 1 - fabs(resolution_ratio);
      }else{
         /// muon momentum scale (5% per TeV from https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonReferenceResolution)
         ratio_up   = 1 + (sca_ratio * MuonList.at(i)->getPt()/1000.);
         ratio_down = 1 - (sca_ratio * MuonList.at(i)->getPt()/1000.);
      }
      /// use 1/ratio for scaling to scale 1/pT
      shiftParticle(EventViewMuonUp,   MuonList.at(i), 1./ratio_up,   dPx_up,   dPy_up);
      shiftParticle(EventViewMuonDown, MuonList.at(i), 1./ratio_down, dPx_down, dPy_down);
   }

   shiftMET(dPx_up, dPx_down, dPy_up, dPy_down);
   METListUp.clear();
   METListDown.clear();

   return;
}



void Systematics::shiftEleAndMET(std::string const shiftType){
   checkshift(shiftType);

   // shiftType == "Resolution"
   if(shiftType == "Resolution"){
      // TODO: not implemented yet!
      std::cout << "shift type 'Resolution' not implemented yet" << std::endl;
      return;
   }

   //else shiftType == "Scale"

   double ratio_barrel = m_ratioEleBarrel;
   double ratio_endcap = m_ratioEleEndcap;
   double ratio;
   double dPx_up=0;
   double dPy_up=0;
   double dPx_down=0;
   double dPy_down=0;
   std::string prefix = std::string("Ele_syst") + shiftType;
   pxl::EventView* evup   = 0;
   pxl::EventView* evdown = 0;

   createEventViews(prefix, &evup, &evdown);
   fillMETLists(evup, evdown);

   // add shifted particles to these EventViews
   for(unsigned int i = 0; i < EleList.size(); i++){
      // "isBarrel" and "isEndcap" only set for reconstructed data skimmed by MUSiCSkimmer_miniAOD.cc
      if(EleList.at(i)->getUserRecord("isBarrel")) {
         ratio = ratio_barrel;
      } else if(EleList.at(i)->getUserRecord("isEndcap")) {
         ratio = ratio_endcap;
      } else {
         throw std::runtime_error( "Systematics.cc: electrons must be in either endcap or barrel and have an id" );
      }
      shiftParticle(evup,   EleList.at(i), 1. + ratio, dPx_up,   dPy_up);
      shiftParticle(evdown, EleList.at(i), 1. - ratio, dPx_down, dPy_down);
   }

   shiftMET(dPx_up, dPx_down, dPy_up, dPy_down);
   METListUp.clear();
   METListDown.clear();

   return;
}



void Systematics::shiftTauAndMET(std::string const shiftType){
   checkshift(shiftType);

   // shiftType == "Resolution"
   if(shiftType == "Resolution"){
      // TODO: not implemented yet!
      std::cout << "shift type 'Resolution' not implemented yet" << std::endl;
      return;
   }

   //else shiftType == "Scale"

   double ratio = m_ratioTau;
   double dPx_up=0;
   double dPy_up=0;
   double dPx_down=0;
   double dPy_down=0;
   std::string prefix = std::string("Tau_syst") + shiftType;
   pxl::EventView* evup   = 0;
   pxl::EventView* evdown = 0;

   createEventViews(prefix, &evup, &evdown);
   fillMETLists(evup, evdown);

   // add shifted particles to these EventViews
   for(unsigned int i = 0; i < TauList.size(); i++){
      shiftParticle(evup,   TauList.at(i), 1. + ratio, dPx_up,   dPy_up);
      shiftParticle(evdown, TauList.at(i), 1. - ratio, dPx_down, dPy_down);
   }

   shiftMET(dPx_up, dPx_down, dPy_up, dPy_down);
   METListUp.clear();
   METListDown.clear();

   return;
}



void Systematics::shiftJetAndMET(std::string const shiftType){

   bool do_resolution = false;

   if(shiftType == "Resolution"){
      do_resolution = true;
   }

   double ratio =0;
   double ratio_up =0;
   double ratio_down =0;
   double dPx_up=0;
   double dPy_up=0;
   double dPx_down=0;
   double dPy_down=0;
   std::string prefix = std::string("Jet_syst") + shiftType;
   pxl::EventView* evup   = 0;
   pxl::EventView* evdown = 0;

   createEventViews(prefix, &evup, &evdown);
   fillMETLists(evup, evdown);

   // add shifted particles to these EventViews
   for(unsigned int i = 0; i < JetList.size(); i++){
      if(do_resolution){
         if(JetList.at(i)->getSoftRelations().hasType("priv-gen-rec")){
            pxl::Particle *genPart =  dynamic_cast< pxl::Particle* >(JetList.at(i)->getSoftRelations().getFirst (m_GenEvtView->getObjectOwner(), "priv-gen-rec"));
            ratio_up = m_jetRes.getJetPtCorrFactor(JetList.at(i),genPart,m_GenEvtView->getUserRecord( "NumVerticesPUTrue" ).toDouble(),1);
            ratio_down = m_jetRes.getJetPtCorrFactor(JetList.at(i),genPart,m_GenEvtView->getUserRecord( "NumVerticesPUTrue" ).toDouble(),-1);
         }else{
            ratio_up = m_jetRes.getJetPtCorrFactor(JetList.at(i),0,m_GenEvtView->getUserRecord( "NumVerticesPUTrue" ).toDouble(),1);
            ratio_down = m_jetRes.getJetPtCorrFactor(JetList.at(i),0,m_GenEvtView->getUserRecord( "NumVerticesPUTrue" ).toDouble(),-1);
         }
         // fot the resolution there is no up and down
         shiftParticle(evup,   JetList.at(i), ratio_up, dPx_up,   dPy_up);
         shiftParticle(evdown, JetList.at(i), ratio_down, dPx_down, dPy_down);
      }else{
         //The uncertainty is a function of eta and the (corrected) p_t of a jet.
         m_jecUnc.setJetEta( JetList.at(i)->getEta() );
         m_jecUnc.setJetPt( JetList.at(i)->getPt() );
         ratio = m_jecUnc.getUncertainty( true ) ;
         shiftParticle(evup,   JetList.at(i), 1. + ratio, dPx_up,   dPy_up);
         shiftParticle(evdown, JetList.at(i), 1. - ratio, dPx_down, dPy_down);
      }

   }

   shiftMET(dPx_up, dPx_down, dPy_up, dPy_down);
   METListUp.clear();
   METListDown.clear();

   return;



    /*
   checkshift(shiftType);
   if( not m_jet_res_corr_use ) {
      if( m_debug > 0 ) {
         std::stringstream warn;
         warn << "[WARNING] (EventAdaptor): " << std::endl;
         warn << "Using 'applyJETMETSmearing(...)', but config file says: ";
         warn << "'Jet.Resolutions.Corr.use = " << m_jet_res_corr_use << "'";
         warn << std::endl << std::endl;

         std::cerr << warn.str();
      }
   }

   // shiftType == "Resolution"
   if(shiftType == "Resolution"){
      // TODO: not implemented yet!
      std::cout << "shift type 'Resolution' not implemented yet" << std::endl;
      return;
   }

   //else shiftType == "Scale"



*/}



void Systematics::shiftMETUnclustered(std::string const shiftType){
   checkshift(shiftType);

   // shiftType == "Resolution"
   if(shiftType == "Resolution"){
      // TODO: not implemented yet!
      std::cout << "shift type 'Resolution' not implemented yet" << std::endl;
      return;
   }

   //from MUSiCSkimmer_miniAOD.cc:
   //Get systmetShifts:
   //enum   METUncertainty {
     //JetEnUp =0, JetEnDown =1, JetResUp =2, JetResDown =3,
     //MuonEnUp =4, MuonEnDown =5, ElectronEnUp =6, ElectronEnDown =7,
     //TauEnUp =8, TauEnDown =9, UnclusteredEnUp =10, UnclusteredEnDown =11,
     //METUncertaintySize =12
   //}


   std::string find   = m_METType+"uncert_";
   std::string prefix = m_METType + "_syst"+shiftType;
   pxl::EventView* evup   = 0;
   pxl::EventView* evdown = 0;
   pxl::Particle*  part   = 0;

   createEventViews(prefix, &evup, &evdown);
   // create previously copied uncert MET

   for(unsigned int i = 0; i < UnclusteredEnUp.size(); i++){
       part = evup->getObjectOwner().create< pxl::Particle >(UnclusteredEnUp.at(i));
       part->setName(m_METType);
   }
   for(unsigned int i = 0; i < UnclusteredEnDown.size(); i++){
       part = evdown->getObjectOwner().create< pxl::Particle >(UnclusteredEnDown.at(i));
       part->setName(m_METType);
   }
   return;
}



//------------
//private-----
//------------

bool inline Systematics::checkshift(std::string const shiftType) const {
   // check if given shiftType is supported
   if(not (shiftType == "Resolution") && not (shiftType == "Scale")){
      std::cout << "Systematics.cc: only accepted shift types at the moment are 'Resolution' and 'Scale'" << std::endl;
      return false;
   }
   return true;
}



void Systematics::createEventViews(std::string prefix, pxl::EventView** evup, pxl::EventView** evdown) {
   bool success;

   // create new EventViews inside the event
   (*evup)   = m_event->getObjectOwner().create< pxl::EventView >();
   (*evdown) = m_event->getObjectOwner().create< pxl::EventView >();
   if((*evup)==0 || (*evdown)==0){
      throw std::runtime_error("Systematics.cc: creating an event view failed!");
   }
   success = m_event->getObjectOwner().setIndexEntry(prefix + "Up",   (*evup));
   (*evup)->setName(prefix + "Up");
   if(!success){
      std::string message = "Systematics.cc: setIndex for event view" + prefix + "Up" + " failed!";
      throw std::runtime_error(message);
   }
   success = m_event->getObjectOwner().setIndexEntry(prefix + "Down", (*evdown));
   (*evdown)->setName(prefix + "Down");
   if(!success){
      std::string message = "Systematics.cc: setIndex for event view" + prefix + "Down" + " failed!";
      throw std::runtime_error(message);
   }
   return;
}



void Systematics::fillMETLists(pxl::EventView* evup, pxl::EventView* evdown){
   // create new MET
   pxl::Particle* shiftedMET;
   for(unsigned int i = 0; i < METList.size(); i++){
      shiftedMET = evup->getObjectOwner().create< pxl::Particle >(METList.at(i));
      METListUp.push_back(shiftedMET);
      shiftedMET = evdown->getObjectOwner().create< pxl::Particle >(METList.at(i));
      METListDown.push_back(shiftedMET);
   }
   return;
}



void Systematics::shiftParticle(pxl::EventView* eview, pxl::Particle* const part , double const& ratio, double& dPx, double& dPy){
   //create a copy of the original particle
   pxl::Particle* shiftedParticle = eview->getObjectOwner().create< pxl::Particle >(part);
   //add the shifted part up
   dPx+=shiftedParticle->getPx()*(ratio-1);
   dPy+=shiftedParticle->getPy()*(ratio-1);
   //WARNING change the particle content for the particle in the new event view
   shiftedParticle->setP4(ratio*shiftedParticle->getPx(),
                          ratio*shiftedParticle->getPy(),
                          ratio*shiftedParticle->getPz(),
                          ratio*shiftedParticle->getE());
   return;
}



// change according MET
void Systematics::shiftMET(double const dPx_up, double const dPx_down, double const dPy_up, double const dPy_down){
   pxl::Particle* shiftedMET;
   double Px, Py, E;
   for(unsigned int i_met = 0; i_met < METListUp.size(); i_met++){
      //up
      shiftedMET= METListUp.at(i_met);
      Px = shiftedMET->getPx()-dPx_up;
      Py = shiftedMET->getPy()-dPy_up;
      E  = std::sqrt(Px*Px + Py*Py);
      shiftedMET->setP4(Px, Py, 0., E);
      //down
      shiftedMET= METListDown.at(i_met);
      Px = shiftedMET->getPx()-dPx_down;
      Py = shiftedMET->getPy()-dPy_down;
      E  = std::sqrt(Px*Px + Py*Py);
      shiftedMET->setP4(Px, Py, 0., E);
   }
   return;
}
