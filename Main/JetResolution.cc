#include "JetResolution.hh"

#include <cmath>

#include "Tools/Tools.hh"
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

JetResolution::JetResolution( Tools::MConfig const &config ) :
    m_jet_res_config( Tools::AbsolutePath( config.GetItem< std::string >( "Jet.Resolutions.Corr.File" ) ) ),

    //m_sigma_MC( m_jet_res_config.GetItem< double >( "sigma_MC" ) ),

    m_eta_corr_map( m_jet_res_config, "eta_edges", "data_MC_ratio", "abs_eta" ),

    m_rand( 0 ),

    CorParr(Tools::AbsolutePath( config.GetItem< std::string >( "Jet.Resolutions.Unmatched.File" ) ))
{
    mFunc = new TFormula("function",((CorParr.definitions()).formula()).c_str());
}


double JetResolution::getScalingFactor( double const eta ) const {
    return m_eta_corr_map.getValue( eta );
}


double JetResolution::getSigmaMC( double const pt, double const eta, double const pu ) const {
    std::vector<float> fx,fy;

    //Number of parameters
    unsigned N=2;

    if(fabs(eta)>5.){
        fx.push_back(5.); // Jet Eta
    }else{
        fx.push_back(fabs(eta)); // Jet Eta
    }
    fy.push_back(pt);
    fy.push_back(pu);

    //not implemented yet nor binned in pileup
    int bin = CorParr.binIndex(fx);

    const std::vector<float>& par = CorParr.record(bin).parameters();
    for(unsigned int i=2*N;i<par.size();i++){
        mFunc->SetParameter(i-2*N,par[i]);
    }

    float x[4];
    std::vector<float> tmp;
    for(unsigned i=0;i<N;i++)
    {
        //std::cout<<fy[i]<<"   "<<par[2*i]<<"  "<< par[2*i+1]<<std::endl;
        x[i] = (fy[i] < par[2*i]) ? par[2*i] : (fy[i] > par[2*i+1]) ? par[2*i+1] : fy[i];
        tmp.push_back(x[i]);
    }

    double result = mFunc->Eval(x[0],x[1]);
    //std::cout<<x[0]<<"  "<<x[1]<<"  "<<result*pt<<std::endl;
    return result*pt;
}


double JetResolution::getJetPtCorrFactor( pxl::Particle const *recJet,
                                            pxl::Particle const *genJet,
                                            double truthpu
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

        m_sigma_MC = getSigmaMC(recJetPt,recJetEta,truthpu);

        double const sigma = m_sigma_MC *
                                    std::sqrt( scaling_factor * scaling_factor - 1.0 );
        double const corrJetPt = m_rand.Gaus( recJetPt, sigma );

        jetCorrFactor = corrJetPt / recJetPt;
    }

    // WARNING: 0 can be returned! Catch this case at the place this function is
    // used!
    return jetCorrFactor;
}
