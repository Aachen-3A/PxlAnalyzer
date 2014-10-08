#include "Systematics.hh"
#include <vector>
#include "Tools/PXL/Sort.hh"


//--------------------Constructor-----------------------------------------------------------------

Systematics::Systematics(const Tools::MConfig &cfg, unsigned int const debug):
   // read uncertainties from config
   m_ratioEleBarrel(cfg.GetItem< double      >( "Syst.Ele.Scale.Barrel" ) ),
   m_ratioEleEndcap(cfg.GetItem< double      >( "Syst.Ele.Scale.Endcap" ) ),
   m_ratioMuo(      cfg.GetItem< double      >( "Syst.Muo.Scale" ) ),
   m_ratioTau(      cfg.GetItem< double      >( "Syst.Tau.Scale" ) ),
   // read lepton names from config
   m_TauType(       cfg.GetItem< std::string >( "Tau.Type.Rec" ) ),
   m_JetType(       cfg.GetItem< std::string >( "Jet.Type.Rec" ) ),
   m_METType(       cfg.GetItem< std::string >( "MET.Type.Rec" ) ),

   m_debug(debug)
{
}



//--------------------Destructor------------------------------------------------------------------

Systematics::~Systematics(){
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
   return;
}



void Systematics::shiftMuoAndMET(std::string const shiftType){
   if(!checkshift(shiftType)) return;

   // shiftType == "Resolution"
   if(shiftType == "Resolution"){
      // TODO: not implemented yet!
      std::cout << "shift type 'Resolution' not implemented yet" << std::endl;
      return;
   }

   //else shiftType == "Scale"

   double ratio = m_ratioMuo;
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
      //use the high pt definition x% per TeV
      ratio_up   = MuonList.at(i)->getPt()*(1.+ratio)/1000.;
      shiftParticle(EventViewMuonUp,   MuonList.at(i), ratio_up,   dPx_up,   dPy_up);
      ratio_down = MuonList.at(i)->getPt()*(1.-ratio)/1000.;
      shiftParticle(EventViewMuonDown, MuonList.at(i), ratio_down, dPx_down, dPy_down);
   }

   shiftMET(dPx_up, dPx_down, dPy_up, dPy_down);
   METListUp.clear();
   METListDown.clear();

   return;
}



void Systematics::shiftEleAndMET(std::string const shiftType){
   if(!checkshift(shiftType)) return;

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

   // create new EventViews inside the event
   pxl::EventView* EventViewEleUp   = m_event->create< pxl::EventView >();
   pxl::EventView* EventViewEleDown = m_event->create< pxl::EventView >();
   m_event->setIndex(std::string("Ele") + "_syst" + shiftType + "Up",   EventViewEleUp);
   m_event->setIndex(std::string("Ele") + "_syst" + shiftType + "Down", EventViewEleDown);

   fillMETLists(EventViewEleUp, EventViewEleDown);

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
      shiftParticle(EventViewEleUp,   EleList.at(i), 1. + ratio, dPx_up,   dPy_up);
      shiftParticle(EventViewEleDown, EleList.at(i), 1. - ratio, dPx_down, dPy_down);
   }

   shiftMET(dPx_up, dPx_down, dPy_up, dPy_down);
   METListUp.clear();
   METListDown.clear();

   return;
}



void Systematics::shiftTauAndMET(std::string const shiftType){
   if(!checkshift(shiftType)) return;

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

   // create new EventViews inside the event
   pxl::EventView* EventViewTauUp   = m_event->create< pxl::EventView >();
   pxl::EventView* EventViewTauDown = m_event->create< pxl::EventView >();
   m_event->setIndex(std::string("Tau") + "_syst" + shiftType + "Up",   EventViewTauUp);
   m_event->setIndex(std::string("Tau") + "_syst" + shiftType + "Down", EventViewTauDown);

   fillMETLists(EventViewTauUp, EventViewTauDown);

   // add shifted particles to these EventViews
   for(unsigned int i = 0; i < TauList.size(); i++){
      shiftParticle(EventViewTauUp,   TauList.at(i), 1. + ratio, dPx_up,   dPy_up);
      shiftParticle(EventViewTauDown, TauList.at(i), 1. - ratio, dPx_down, dPy_down);
   }

   shiftMET(dPx_up, dPx_down, dPy_up, dPy_down);
   METListUp.clear();
   METListDown.clear();

   return;
}



void shiftJetAndMET(std::string const shiftType){/*
   if(!checkshift(shiftType)) return;
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

   // create Copys of the original Event View and modify the JES
   pxl::EventView *GenEvtView_JES_UP = m_event.getObjectOwner().create< pxl::EventView >( GenEvtView );
   event.setIndex( "Gen_JES_UP", GenEvtView_JES_UP );
   pxl::EventView *RecEvtView_JES_UP = m_event.getObjectOwner().create< pxl::EventView >( RecEvtView );
   event.setIndex( "Rec_JES_UP", RecEvtView_JES_UP );
   pxl::EventView *GenEvtView_JES_DOWN = m_event.getObjectOwner().create< pxl::EventView >( GenEvtView );
   event.setIndex( "Gen_JES_DOWN", GenEvtView_JES_DOWN );
   pxl::EventView *RecEvtView_JES_DOWN = m_event.getObjectOwner().create< pxl::EventView >( RecEvtView );
   event.setIndex( "Rec_JES_DOWN", RecEvtView_JES_DOWN );

*/}



void Systematics::shiftMETUnclustered(std::string const shiftType){
   if(!checkshift(shiftType)) return;

   // shiftType == "Resolution"
   if(shiftType == "Resolution"){
      // TODO: not implemented yet!
      std::cout << "shift type 'Resolution' not implemented yet" << std::endl;
      return;
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

