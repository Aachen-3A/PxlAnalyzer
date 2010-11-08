#ifndef pxl_Sort_hh
#define pxl_Sort_hh

#include <algorithm>
#include <vector>

#include "Tools/PXL/PXL.hh"


namespace pxl {
   //inline to avoid stupid compiler errors
   inline bool compPart( pxl::Particle *first, pxl::Particle *second ) {
      return first->getPt() > second->getPt();
   }

   inline void sortParticles( std::vector< pxl::Particle* > &vec ) {
      std::sort( vec.begin(), vec.end(), compPart );
   }
}

#endif
