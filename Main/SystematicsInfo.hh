#ifndef SystematicsInfo_hh
#define SystematicsInfo_hh

#include <string>
#include <iostream>
#include <vector>
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"
class SystematicsInfo {

public:
   SystematicsInfo( std::string particleType,
      std::string sysType,
      std::string funcKey,
      bool isCorrelated=false  );
   ~SystematicsInfo();
   //~ std::vector< std::string > eventViewIndices;
   std::vector< pxl::EventView* > eventViewPointers;
   bool m_isCorrelated;
   std::string m_particleType;
   std::string m_sysType;
   // the key of the shifting function
   //in the systematics class function map
   std::string m_funcKey;
};
#endif /*SystematicsInfo_hh*/
