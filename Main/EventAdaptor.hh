#ifndef EVENTADAPTOR
#define EVENTADAPTOR

#include "Main/GenRecNameMap.hh"

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

   private:
      void adaptMuon( pxl::Particle *muon ) const;

      unsigned int const m_debug;

      GenRecNameMap const m_gen_rec_map;

      bool const m_muo_useCocktail;

      std::string const m_muo_RecName;
};

#endif /*EVENTADAPTOR*/
