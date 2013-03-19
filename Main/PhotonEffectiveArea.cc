#include "PhotonEffectiveArea.hh"

#include <cmath>
#include <string>
#include <sstream>

#include "Tools/Tools.hh"

PhotonEffectiveArea::PhotonEffectiveArea( Tools::MConfig const &config ) :
   m_EA_config( Tools::AbsolutePath( config.GetItem< std::string >( "Gamma.EffectiveArea.File" ) ) ),

   m_abs_eta( m_EA_config.GetItem< bool >( "abs_eta" ) ),
   m_EAs( initEAs() )
{
}

PhotonEffectiveArea::EAs const PhotonEffectiveArea::initEAs() const {
   EAs effective_areas;

   std::vector< double > eta_edges = Tools::splitString< double >( m_EA_config.GetItem< std::string >( "eta_edges" ) );

   std::vector< double > EA_chargedHadrons = Tools::splitString< double >( m_EA_config.GetItem< std::string >( "EA_charged_hadrons" ) );
   std::vector< double > EA_neutralHadrons = Tools::splitString< double >( m_EA_config.GetItem< std::string >( "EA_neutral_hadrons" ) );
   std::vector< double > EA_photons        = Tools::splitString< double >( m_EA_config.GetItem< std::string >( "EA_photons" ) );

   if( EA_chargedHadrons.size() != EA_neutralHadrons.size() or
       EA_neutralHadrons.size() != EA_photons.size() ) {
      std::stringstream err;
      err << "In config file: '";
      err << m_EA_config.GetConfigFilePath();
      err << "': Number of EA values do not match.";
      throw Tools::config_error( err.str() );
   }

   if( eta_edges.size() != EA_chargedHadrons.size() ) {
      std::stringstream err;
      err << "In config file: '";
      err << m_EA_config.GetConfigFilePath();
      err << "': Number of eta values does not match with number of EA values.";
      throw Tools::config_error( err.str() );
   }

   std::vector< double >::const_iterator eta     = eta_edges.begin();
   std::vector< double >::const_iterator charged = EA_chargedHadrons.begin();
   std::vector< double >::const_iterator neutral = EA_neutralHadrons.begin();
   std::vector< double >::const_iterator photon  = EA_photons.begin();

   for( ; eta != eta_edges.end(); ++eta, ++charged, ++neutral, ++photon ) {
      std::vector< double > row;
      row.push_back( *charged );
      row.push_back( *neutral );
      row.push_back( *photon );
      effective_areas[ *eta ] = row;
   }

   return effective_areas;
}


double PhotonEffectiveArea::getEffectiveArea( double const &eta, unsigned int const &type ) const {
   double const use_eta = m_abs_eta ? fabs( eta ) : eta;

   EAs::const_iterator upper;
   upper = m_EAs.lower_bound( use_eta );

   if( upper == m_EAs.begin() ) {
      std::stringstream err;
      err << "In config file: '";
      err << m_EA_config.GetConfigFilePath();
      err << "': No EAs found for eta = ";
      err << eta;
      err << "!";
      throw Tools::config_error( err.str() );
   }

   // upper_bound always gives you the "next" entry. So, decrease the iterator.
   upper--;

   return (*upper).second.at( type );
}
