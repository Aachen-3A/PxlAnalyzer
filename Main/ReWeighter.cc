#include "ReWeighter.hh"

#include <string>

#include "Tools/Tools.hh"
#include "Tools/MConfig.hh"


ReWeighter::ReWeighter( const Tools::MConfig &cutconfig  ) :
   m_LumiWeights( Tools::ExpandPath( cutconfig.GetItem< std::string >( "Pileup.GenHistFile" ) ),
                  Tools::ExpandPath( cutconfig.GetItem< std::string >( "Pileup.DataHistFile" ) ),
                  cutconfig.GetItem< std::string >( "Pileup.GenHistName", "pileup" ),
                  cutconfig.GetItem< std::string >( "Pileup.DataHistName", "pileup" )
                  ),
   m_useGenWeights( cutconfig.GetItem< bool >( "General.UseGeneratorWeights" ) ),
   m_usePileUpReWeighting( cutconfig.GetItem< bool >( "Pileup.UsePileupReWeighting" ) )
{
}


void ReWeighter::ReWeightEvent( const pxl::Event &event ) {
   pxl::EventView *GenEvtView = event.getObjectOwner().findObject< pxl::EventView >( "Gen" );

   // Disable generator weights.
   if( not m_useGenWeights ) GenEvtView->setUserRecord< double >( "Weight", 1.0 );

   if( m_usePileUpReWeighting ) {
      float const numVerticesPUTrue = GenEvtView->findUserRecord< float >( "NumVerticesPUTrue" );

      double const pileupWeight = m_LumiWeights.weight( numVerticesPUTrue );

      GenEvtView->setUserRecord< double >( "PUWeight", pileupWeight );
   }
}
