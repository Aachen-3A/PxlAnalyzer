#ifndef PHOTONEFFECTIVEAREA
#define PHOTONEFFECTIVEAREA

#include "Tools/MConfig.hh"

class PhotonEffectiveArea {
   typedef std::map< double, std::vector< double > > EAs;
   public:
      PhotonEffectiveArea( Tools::MConfig const &config );
      ~PhotonEffectiveArea() {}

      double getEffectiveArea( double const &eta, unsigned int const &type ) const;

      enum EffArea { chargedHadron = 0,
                     neutralHadron = 1,
                     photon = 2
                     };

   private:
      EAs const initEAs() const;
      Tools::MConfig const m_EA_config;

      bool const m_abs_eta;

      EAs const m_EAs;
};

#endif /*PHOTONEFFECTIVEAREA*/
