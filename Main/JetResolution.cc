#include "JetResolution.hh"

#include <cmath>

#include "Tools/Tools.hh"
#include "Tools/PXL/PXL.hh"

JetResolution::JetResolution( Tools::MConfig const &config ) :
   m_jet_res_config( Tools::AbsolutePath( config.GetItem< std::string >( "Jet.Resolutions.Corr.File" ) ) ),

   m_abs_eta(  m_jet_res_config.GetItem< bool   >( "abs_eta" ) ),
   m_sigma_MC( m_jet_res_config.GetItem< double >( "sigma_MC" ) ),

   m_use_overflow(  false ),
   m_use_underflow( false ),

   m_ratios( initHisto() ),

   m_rand( 0 )
{
}


TH1D JetResolution::initHisto() {
   std::vector< double > eta_edges     = Tools::splitString< double >( m_jet_res_config.GetItem< std::string >( "eta_edges" ) );
   std::vector< double > data_MC_ratio = Tools::splitString< double >( m_jet_res_config.GetItem< std::string >( "data_MC_ratio" ) );

   std::sort( eta_edges.begin(), eta_edges.end() );

   int const num_eta    = eta_edges.size();
   int const num_values = data_MC_ratio.size();

   TH1D ratios( "data_MC_ratio", "Data/MC corr factor", num_eta - 1, &eta_edges.at( 0 ) );

   if( num_eta - num_values == 1 ) {
      for( int bin = 1; bin <= ratios.GetNbinsX(); ++bin ) {
         ratios.SetBinContent( bin, data_MC_ratio.at( bin - 1 ) );
      }
   } else if( num_eta == num_values ) {
      if( m_abs_eta ) {
         for( int bin = 1; bin <= ratios.GetNbinsX() + 1; ++bin ) {
            ratios.SetBinContent( bin, data_MC_ratio.at( bin - 1 ) );
         }
         m_use_overflow = true;
      } else {
         std::stringstream err;
         err << "In config file: '";
         err << m_jet_res_config.GetConfigFilePath();
         err << "': Not supported number of values for 'abs_eta = true'.";
         throw Tools::config_error( err.str() );
      }
   } else if( num_values - num_eta == 1 ) {
      if( not m_abs_eta ) {
         for( int bin = 0; bin <= ratios.GetNbinsX() + 1; ++bin ) {
            ratios.SetBinContent( bin, data_MC_ratio.at( bin - 1 ) );
         }
         m_use_overflow  = true;
         m_use_underflow = true;
      } else {
         std::stringstream err;
         err << "In config file: '";
         err << m_jet_res_config.GetConfigFilePath();
         err << "': Not supported number of values for 'abs_eta = false'.";
         throw Tools::config_error( err.str() );
      }
   } else {
      std::stringstream err;
      err << "In config file: '";
      err << m_jet_res_config.GetConfigFilePath();
      err << "': Not supported number of values.";
      throw Tools::config_error( err.str() );
   }

   return ratios;
}


double JetResolution::getScalingFactor( double const eta ) const {
   double const used_eta = m_abs_eta ? std::fabs( eta ) : eta;
   int const bin = m_ratios.FindFixBin( used_eta );

   if( not m_use_overflow and m_ratios.IsBinOverflow( bin ) ) {
      std::stringstream err;
      err << "(JetResolution): In getScalingFactor(...): ";
      err << "Unsupported value for eta: " << eta << ". ";
      err << "Please invesigate!";
      throw Tools::value_error( err.str() );
   }

   if( not m_use_underflow and m_ratios.IsBinUnderflow( bin ) ) {
      std::stringstream err;
      err << "(JetResolution): In getScalingFactor(...): ";
      err << "Unsupported value for eta: " << eta << ". ";
      err << "Please invesigate!";
      throw Tools::value_error( err.str() );
   }

   return m_ratios.GetBinContent( bin );
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
