#ifndef pxl_Sort_hh
#define pxl_Sort_hh

#include <algorithm>
#include <vector>

//#include "Tools/PXL/PXL.hh"
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"


namespace pxl {
   //inline to avoid stupid compiler errors
   inline bool compPart( pxl::Particle *first, pxl::Particle *second ) {
      //there might be something weird going on when doing a direct comparision, so store the values in variables first
      double pt1 = first->getPt();
      double pt2 = second->getPt();
      return pt1 > pt2;
   }

   inline void sortParticles( std::vector< pxl::Particle* > &vec ) {
      std::sort( vec.begin(), vec.end(), compPart );
   }
}

#endif
