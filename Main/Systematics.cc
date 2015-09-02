#include "Systematics.hh"
#include <vector>
#include "Tools/PXL/Sort.hh"
#include "TRandom3.h"
#include "Tools/Tools.hh"

//--------------------Constructor-----------------------------------------------------------------

Systematics::Systematics(const Tools::MConfig &cfg, unsigned int const debug):
   m_activeSystematics( {} ),
   // read uncertainties from config
   m_full(cfg.GetItem< bool                  >( "General.Syst.fullview" ) ),
   m_emptyShift(cfg.GetItem< bool            >( "General.Syst.emptyShift" ) ),
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
   systFuncMap (
         {{ "Ele_Scale", std::bind(&Systematics::shiftEleAndMET, this,"Scale") },
         //~ {{ "Ele_Resolution", std::bind(&Systematics::shiftEleAndMET,this, "Resolution") },
         { "Muon_Resolution", std::bind(&Systematics::shiftMuoAndMET, this,"Resolution") },
         { "Muon_Scale", std::bind(&Systematics::shiftMuoAndMET, this, "Scale") },
         //~ {{ "Tau_Resolution", std::bind(&Systematics::shiftTauAndMET, this, "Resolution") },
         { "Tau_Scale", std::bind(&Systematics::shiftTauAndMET, this, "Scale") },
         { "Jet_Resolution", std::bind(&Systematics::shiftJetAndMET, this, "Resolution") },
         { "Jet_Scale", std::bind(&Systematics::shiftJetAndMET, this, "Scale") },
         //~ {{ "MET_Resolution", std::bind(&Systematics::shiftMETUnclustered, "Resolution") },
         { "MET_Scale", std::bind(&Systematics::shiftMETUnclustered, this, "Scale") }
         }),

   m_debug(debug)


{
   rand = new TRandom3();

   std::vector< std::string > availableFunctions;
   for(auto entry : systFuncMap ) availableFunctions.push_back( entry.first );
   // read in which systematics should be evaluated
   // loop all considered objects (Muo, Ele ...)
   for(std::string partType : Tools::getParticleTypeAbbreviations()){
      //add all selected systematic types
      auto systTypes = Tools::splitString< std::string >(
            cfg.GetItem< std::string >( partType + ".Syst.Types"),
            true
      );
      for(std::string systType : systTypes){
         std::string funcKey = partType + "_"+ systType;
         bool isAvailable = (std::find(availableFunctions.begin(),
                                       availableFunctions.end(),
                                        funcKey) != availableFunctions.end());
         if(isAvailable){
            SystematicsInfo *thisSystematic =
               new SystematicsInfo( partType, systType, funcKey, false);
            m_activeSystematics.push_back( thisSystematic );
         }else{
            std::cout << "Systematic type " << systType
                      <<" is not available for " << partType<<std::endl;
            exit(1);
         }
      }
   }
}



//--------------------Destructor------------------------------------------------------------------

Systematics::~Systematics(){
    delete rand;
}



//------------
//public------
//------------

void Systematics::createShiftedViews(){
   for(auto syst : m_activeSystematics){
      m_activeSystematic = syst;
      systFuncMap[ syst->m_funcKey ] ();
   }
}

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

   // clear shifted EventViews from last event
   for(auto syst : m_activeSystematics){
      syst->eventViewPointers.clear();
   }

   // get all particles
   std::vector< pxl::Particle* > AllParticles;
   m_eventView->getObjectsOfType< pxl::Particle >( AllParticles );
   pxl::sortParticles( AllParticles );
   std::string persistent_id;
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
       // mark both entries with same persistent ID ( standard ID changes when cloning views=
       persistent_id = Tools::random_string( 16 );
       part->setUserRecord("persistent_id", persistent_id );

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
      persistent_id = Tools::random_string( 16 );
      part->setUserRecord("persistent_id", persistent_id );
  }

   return;
}



void Systematics::shiftMuoAndMET(std::string const shiftType){
   // Do nothing if no muon in event
   if ( not m_emptyShift and MuonList.size() == 0 ) return;

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

   pxl::EventView* EventViewMuonUp   = 0;
   pxl::EventView* EventViewMuonDown = 0;
   std::string prefix = std::string("Muon_syst") + shiftType;
   createEventViews(prefix, &EventViewMuonUp, &EventViewMuonDown);
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
    // Do nothing if no electron in event
   if ( not m_emptyShift and EleList.size() == 0 ) return;

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
   // Do nothing if no electron in event
   if ( not m_emptyShift and TauList.size() == 0 ) return;

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
   // Do nothing if no jet in event
   if ( not m_emptyShift and JetList.size() == 0 ) return;

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

void Systematics::createEventViews(std::string prefix, pxl::EventView** evup, pxl::EventView** evdown) {
   bool success;

   // create new EventViews inside the event using deep copy and drop all objects
   // this preserves user record for the event view itself.
   // Shifted objects will be added in further systematics computation
   (*evup)   = m_event->getObjectOwner().create< pxl::EventView >( m_eventView );
   (*evup)->clearObjects();
   (*evdown) = m_event->getObjectOwner().create< pxl::EventView >( m_eventView );
   (*evdown)->clearObjects();
   if((*evup)==0 || (*evdown)==0){
      throw std::runtime_error("Systematics.cc: creating an event view failed!");
   }
   success = m_event->getObjectOwner().setIndexEntry(prefix + "Up",   (*evup));
   (*evup)->setName(prefix + "Up");
   m_activeSystematic->eventViewPointers.push_back( *evup );
   if(!success){
      std::string message = "Systematics.cc: setIndex for event view" + prefix + "Up" + " failed!";
      throw std::runtime_error(message);
   }
   success = m_event->getObjectOwner().setIndexEntry(prefix + "Down", (*evdown));
   (*evdown)->setName(prefix + "Down");
   m_activeSystematic->eventViewPointers.push_back( *evdown );
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

// This function creates a full event view with the full selected event (baseview))
// content from a sys shift event view which contains only the shifted view
void Systematics::createFullViews(  pxl::EventView* baseEvtView ){
    bool success = 0;
    if( baseEvtView == 0 ) baseEvtView = m_eventView;
    // Loop all active systematics
    for(auto& systInfo : m_activeSystematics){
        // replace shifted views wit views where unshifted selected
        // particles are also stored in the view
        for( auto& systEvtView : systInfo->eventViewPointers){
            // create a deep copy of the original view
            pxl::EventView* fullShiftedEvtView   = m_event->getObjectOwner().create< pxl::EventView >( baseEvtView );
            // correct the name of the new view to math the view to be replaced
            std::string shiftedName = systEvtView->getName();

            std::vector< pxl::Particle* > AllParticles;
            fullShiftedEvtView->getObjectsOfType< pxl::Particle >( AllParticles );

            std::vector< pxl::Particle* > AllParticlessShifted;
            systEvtView->getObjectsOfType< pxl::Particle >( AllParticlessShifted );
            // search and replace shifted particle by persistent id
            for( auto & part : AllParticles){
                // only particles related to a shifted particle have a persistent ID
                // and need to be replaced.
                for( auto& partShifted : AllParticlessShifted){
                    if( part->hasUserRecord("persistent_id") &&
                        partShifted->getUserRecord("persistent_id") == part->getUserRecord("persistent_id") ){
                        // delte original part and replace it with shifted one
                        //~ delete part;
                        fullShiftedEvtView->removeObject(part);
                        fullShiftedEvtView->getObjectOwner().create< pxl::Particle >(partShifted);
                    }
                }
            }
            m_event->removeObject( systEvtView );
            success = m_event->getObjectOwner().setIndexEntry( shiftedName,   fullShiftedEvtView);
            fullShiftedEvtView->setName(shiftedName);
            if(!success){
              std::string message = "Systematics.cc: setIndex for event view" + shiftedName + " failed!";
              throw std::runtime_error(message);
            }
            // replace old shifted event view with new one
            systEvtView = fullShiftedEvtView;
        }
    }
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
