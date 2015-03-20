#ifndef Systematics_hh
#define Systematics_hh

#include <string>
#include <iostream>
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"
#include "Tools/MConfig.hh"

/*
 * written by Michael Margos
 * (michael.margos@rwth-aachen.de)
 */

class TRandom3;

class Systematics {
public:
   Systematics(const Tools::MConfig &cfg, unsigned int const debug);
   ~Systematics();

   void init(pxl::Event* event);

   void shiftMuoAndMET(std::string const shiftType);
   void shiftEleAndMET(std::string const shiftType);
   void shiftTauAndMET(std::string const shiftType);
   void shiftJetAndMET(std::string const shiftType);
   void shiftMETUnclustered(std::string const shiftType);

private:
   // variables
   double const m_ratioEleBarrel, m_ratioEleEndcap, m_scaleMuo, m_resMuo, m_ratioTau;
   std::string const m_TauType, m_JetType, m_METType;
   unsigned int const m_debug;

   pxl::Event*      m_event;
   pxl::EventView* m_eventView;
   std::vector< pxl::Particle* > EleList;
   std::vector< pxl::Particle* > MuonList;
   std::vector< pxl::Particle* > TauList;
   std::vector< pxl::Particle* > JetList;
   std::vector< pxl::Particle* > METList;
   std::vector< pxl::Particle* > METListUp;
   std::vector< pxl::Particle* > METListDown;
   std::vector< pxl::Particle* > UnclusteredEnUp;
   std::vector< pxl::Particle* > UnclusteredEnDown;

   pxl::EventView* m_GeneventView;

    TRandom3* rand;

   // methods
   bool inline checkshift(std::string const shiftType) const;
   void createEventViews(std::string prefix, pxl::EventView** evup, pxl::EventView** evdown);
   void fillMETLists(pxl::EventView* evup, pxl::EventView* evdown);
   void shiftParticle(pxl::EventView* eview, pxl::Particle* const part , double const& ratio, double& dPx, double& dPy/*, bool const up*/);
   void shiftMET(double const dPx_up, double const dPx_down, double const dPy_up, double const dPy_down);
};
#endif /*Systematics_hh*/
