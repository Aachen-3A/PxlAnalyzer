#include "JetResolution.hh"

#include <cmath>

#include "Tools/Tools.hh"
#include "Tools/PXL/PXL.hh"

JetResolution::JetResolution( Tools::MConfig const &config ) :
   m_jet_res_config( Tools::AbsolutePath( config.GetItem< std::string >( "Jet.Resolutions.Corr.File" ) ) ),

   m_sigma_MC( m_jet_res_config.GetItem< double >( "sigma_MC" ) ),

   m_eta_corr_map( m_jet_res_config, "eta_edges", "data_MC_ratio", "abs_eta" ),

   m_rand( 0 )
{
}


double JetResolution::getScalingFactor( double const eta ) const {
   return m_eta_corr_map.getValue( eta );
}


double JetResolution::getJetPtCorrFactor( pxl::Particle const *recJet,
                                          pxl::Particle const *genJet
                                          ) {
   double const recJetPt  = recJet->getPt();
   double const recJetEta = recJet->getEta();

   double const scaling_factor = getScalingFactor( recJetEta );

   double jetCorrFactor = 1.0;

   // Found a match?
   if( genJet ) {
      double const genJetPt  = genJet->getPt();
      double const corrJetPt = std::max( 0.0, genJetPt +
                                              scaling_factor *
                                              ( recJetPt - genJetPt )
                                              );

      jetCorrFactor = corrJetPt / recJetPt;

   // If not, just smear with a Gaussian.
   } else {
      double const sigma = m_sigma_MC *
                           std::sqrt( scaling_factor * scaling_factor - 1.0 );
      double const corrJetPt = m_rand.Gaus( recJetPt, sigma );

      jetCorrFactor = corrJetPt / recJetPt;
   }

   // WARNING: 0 can be returned! Catch this case at the place this function is
   // used!
   return jetCorrFactor;
}
