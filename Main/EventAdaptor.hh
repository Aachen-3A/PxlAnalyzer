#ifndef EVENTADAPTOR
#define EVENTADAPTOR

#include "Main/GenRecNameMap.hh"
#include "Main/JetResolution.hh"

namespace pxl {
   class EventView;
   class Particle;
}

namespace Tools {
   class MConfig;
}

class EventAdaptor {
   public:
      typedef std::vector< pxl::Particle* > pxlParticles;
      EventAdaptor( Tools::MConfig const &cfg, unsigned int const debug = 1 );
      ~EventAdaptor() {}

      void applyCocktailMuons( pxl::EventView const *RecEvtView ) const;
      void applyJETMETSmearing( pxl::EventView const *GenEvtView,
                                pxl::EventView const *RecEvtView,
                                std::string const linkName
                                );

   private:
      void adaptMuon( pxl::Particle *muon ) const;

      unsigned int const m_debug;

      GenRecNameMap const m_gen_rec_map;

      JetResolution m_jet_res;

      bool const m_muo_useCocktail;
      bool const m_jet_res_corr_use;

      std::string const m_muo_RecName;
      std::string const m_jet_RecName;
      std::string const m_met_RecName;
};

#endif /*EVENTADAPTOR*/
