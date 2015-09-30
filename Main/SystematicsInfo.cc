#include "SystematicsInfo.hh"
//--------------------Constructor-----------------------------------------------------------------

SystematicsInfo::SystematicsInfo(std::string particleType,
        std::string sysType,
        std::string funcKey,
        bool isDifferential,
        double constantShift ):
    eventViewPointers( {} ),
    m_isDifferential( isDifferential ),
    m_particleType( particleType ),
    m_sysType( sysType ),
    m_funcKey( funcKey ),
    m_constantShift( constantShift )
{}

//--------------------Destructor------------------------------------------------------------------

SystematicsInfo::~SystematicsInfo(){}
