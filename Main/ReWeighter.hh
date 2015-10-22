#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

namespace Tools {
   class MConfig;
}

// CMSSW include:
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"


class ReWeighter {
public:
   ReWeighter( const Tools::MConfig &cutconfig  );
   ~ReWeighter() {}

   void ReWeightEvent( pxl::Event* event );

private:
   edm::LumiReWeighting m_LumiWeights;
   const bool m_useGenWeights;
   const bool m_useREcoVertices;
   const bool m_usePileUpReWeighting;
};
