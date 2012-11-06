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
   m_LumiWeights.weight3D_init( 1 );
}


void ReWeighter::ReWeightEvent( const pxl::Event &event ) {
   pxl::EventView *GenEvtView = event.getObjectOwner().findObject< pxl::EventView >( "Gen" );

   // Disable generator weights.
   if( not m_useGenWeights ) GenEvtView->setUserRecord< double >( "Weight", 1.0 );

   if( m_usePileUpReWeighting ) {
      const int numVerticesPU       = GenEvtView->findUserRecord< int >( "NumVerticesPU" );
      const int numVerticesPULastBX = GenEvtView->findUserRecord< int >( "NumVerticesPULastBX" );
      const int numVerticesPUNextBX = GenEvtView->findUserRecord< int >( "NumVerticesPUNextBX" );

      const double pileupWeight = m_LumiWeights.weight3D( numVerticesPULastBX, numVerticesPU, numVerticesPUNextBX );

      GenEvtView->setUserRecord< double >( "PUWeight", pileupWeight );
   }
}
