#include "SystematicsInfo.hh"
//--------------------Constructor-----------------------------------------------------------------

SystematicsInfo::SystematicsInfo(std::string particleType,
        std::string sysType,
        std::string funcKey,
        bool isCorrelated ):
    eventViewPointers( {} ),
    m_isCorrelated( isCorrelated ),
    m_particleType( particleType ),
    m_sysType( sysType ),
    m_funcKey( funcKey )

{}

//--------------------Destructor------------------------------------------------------------------

SystematicsInfo::~SystematicsInfo(){}
