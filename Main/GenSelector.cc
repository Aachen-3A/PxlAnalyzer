#include "GenSelector.hh"

#include <string>

#include "Tools/Tools.hh"

GenSelector::GenSelector( Tools::MConfig const &cfg ) :
   // Cut on generator binning.
   m_binningValue_max( cfg.GetItem< double >( "Generator.BinningValue.max" ) ),

   // Cuts on invariant mass of resonance particle.
   m_mass_min(           cfg.GetItem< double >( "Generator.Mass.min", 0 ) ),
   m_mass_max(           cfg.GetItem< double >( "Generator.Mass.max", 0 ) ),
   m_mass_IDs(       Tools::splitString< int >( cfg.GetItem< std::string >( "Generator.Mass.IDs" ), true ) ),
   m_mass_MotherIDs( Tools::splitString< int >( cfg.GetItem< std::string >( "Generator.Mass.mothers" ), true ) ),

   // Cuts on transverse momentum of resonance particle.
   m_pt_min(           cfg.GetItem< double >( "Generator.pt.min", 0 ) ),
   m_pt_max(           cfg.GetItem< double >( "Generator.pt.max", 0 ) ),
   m_pt_IDs(       Tools::splitString< int >( cfg.GetItem< std::string >( "Generator.pt.IDs" ), true ) ),
   m_pt_MotherIDs( Tools::splitString< int >( cfg.GetItem< std::string >( "Generator.pt.mothers" ), true ) )

{
}


bool GenSelector::passGeneratorCuts( pxl::EventView const *EvtView,
                                     pxlParticles const &s3_particles
                                     ) const {
   bool accept_binning = passBinningCuts( EvtView );
   bool accept_mass    = passMassCuts( s3_particles );
   bool accept_pt      = passTransverseMomentumCuts( s3_particles );

   return accept_binning and accept_mass and accept_pt;
}


bool GenSelector::passBinningCuts( pxl::EventView const *EvtView ) const {
   // check binning value
   if( m_binningValue_max > 0 and
       EvtView->getUserRecord( "binScale" ).toDouble() > m_binningValue_max
       ) {
      return false;
   }
   return true;
}


bool GenSelector::passMassCuts( pxlParticles const &s3_particles ) const {

   // Check if a generator invariant mass cut is being applied.
   if( m_mass_min <= 0 and m_mass_max <= 0 ) return true;

   // Store only S3 particles needed for applying generator invariant mass cuts.
   pxlParticles s3_particlesSelected;

   for( pxlParticles::const_iterator part = s3_particles.begin(); part != s3_particles.end(); ++part ) {
      if( ( m_mass_IDs.size() == 0
            or std::find( m_mass_IDs.begin(), m_mass_IDs.end(), (*part)->getUserRecord( "id" ).toInt32() ) != m_mass_IDs.end()
            ) and
          ( m_mass_MotherIDs.size() == 0
            or std::find( m_mass_MotherIDs.begin(), m_mass_MotherIDs.end(), (*part)->getUserRecord( "mother_id" ).toInt32() ) != m_mass_MotherIDs.end()
            ) ) {
         s3_particlesSelected.push_back( *part );
      }
   }

   CheckNumberOfParticles( s3_particlesSelected );

   pxl::LorentzVector sum;
   sum += s3_particlesSelected.at( 0 )->getVector();
   sum += s3_particlesSelected.at( 1 )->getVector();

   if( m_mass_min <= 0 and m_mass_max > 0 ) {
      return sum.getMass() <= m_mass_max;
   }
   else if( m_mass_min > 0 and m_mass_max <= 0 ) {
      return sum.getMass() >= m_mass_min;
   }
   else {   // m_mass_min > 0 and m_mass_max > 0
      return ( sum.getMass() >= m_mass_min && sum.getMass() <= m_mass_max );
   }
}


bool GenSelector::passTransverseMomentumCuts( pxlParticles const &s3_particles ) const {

   // Check if a generator pt cut is being applied.
   if( m_pt_min <= 0 and m_pt_max <= 0 ) return true;

   // Store only S3 particles needed for applying generator pt cuts.
   pxlParticles s3_particlesSelected;

   for( pxlParticles::const_iterator part = s3_particles.begin(); part != s3_particles.end(); ++part ) {
      if( ( m_pt_IDs.size() == 0
            or std::find( m_pt_IDs.begin(), m_pt_IDs.end(), (*part)->getUserRecord( "id" ).toInt32() ) != m_pt_IDs.end()
            ) and
          ( m_pt_MotherIDs.size() == 0
            or std::find( m_pt_MotherIDs.begin(), m_pt_MotherIDs.end(), (*part)->getUserRecord( "mother_id" ).toInt32() ) != m_pt_MotherIDs.end()
            ) ) {
         s3_particlesSelected.push_back( *part );
      }
   }

   CheckNumberOfParticles( s3_particlesSelected );

   pxl::LorentzVector sum;
   sum += s3_particlesSelected.at( 0 )->getVector();
   sum += s3_particlesSelected.at( 1 )->getVector();

   if( m_pt_min <= 0 and m_pt_max > 0 ) {
      return sum.getPt() <= m_pt_max;
   }
   else if( m_pt_min > 0 and m_pt_max <= 0 ) {
      return sum.getPt() >= m_pt_min;
   }
   else {   // m_pt_min > 0 and m_pt_max > 0
      return ( sum.getPt() >= m_pt_min && sum.getPt() <= m_pt_max );
   }
}


bool GenSelector::CheckNumberOfParticles( pxlParticles const &s3_particlesSelected ) const {

   if( s3_particlesSelected.size() < 2 ) {
      for( pxlParticles::const_iterator part = s3_particlesSelected.begin(); part != s3_particlesSelected.end(); ++part ) {
         std::cerr << "ID=" << ( *part )->getUserRecord( "id" ).toInt32() << " mother=" << (*part)->getUserRecord( "mother_id" ).toInt32() << std::endl;
      }
      throw std::length_error( "Can't build resonance particle with less than 2 particles." );
   }
   else if( s3_particlesSelected.size() > 3 ) {
      for( pxlParticles::const_iterator part = s3_particlesSelected.begin(); part != s3_particlesSelected.end(); ++part ) {
         std::cerr << "ID=" << ( *part )->getUserRecord( "id" ).toInt32() << " mother=" << (*part)->getUserRecord( "mother_id" ).toInt32() << std::endl;
      }
      throw std::length_error( "Can't build resonance particle with more than 2 particles." );
   }
   return true;
}
