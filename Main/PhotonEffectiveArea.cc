#include "PhotonEffectiveArea.hh"

#include <cmath>
#include <string>
#include <sstream>

#include "Tools/Tools.hh"

PhotonEffectiveArea::PhotonEffectiveArea( Tools::MConfig const &config ) :
   m_EA_config( Tools::AbsolutePath( config.GetItem< std::string >( "Gamma.EffectiveArea.File" ) ) ),

   m_eta_EAchargedHadrons_map( m_EA_config, "eta_edges", "EA_charged_hadrons", "abs_eta" ),
   m_eta_EAneutralHadrons_map( m_EA_config, "eta_edges", "EA_neutral_hadrons", "abs_eta" ),
   m_eta_EAphotons_map(        m_EA_config, "eta_edges", "EA_photons", "abs_eta" )
{
}

double PhotonEffectiveArea::getEffectiveArea( double const eta,
                                              unsigned int const type
                                              ) const {
   if( type == chargedHadron ) return m_eta_EAchargedHadrons_map.getValue( eta );
   if( type == neutralHadron ) return m_eta_EAneutralHadrons_map.getValue( eta );
   if( type == photon        ) return m_eta_EAphotons_map.getValue( eta );

   std::stringstream err;
   err << "[ERROR] (PhotonEffectiveArea): Not supported type = "<< type << "!" << std::endl;
   throw Tools::value_error( err.str() );
}