#ifndef Systematics_hh
#define Systematics_hh

#include <string>
#include <iostream>
#include <functional>
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"
#include "Tools/MConfig.hh"
#include "JetResolution.hh"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#pragma GCC diagnostic pop
#include "SystematicsInfo.hh"
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
   void createShiftedViews();


private:
   // variables
   double const m_ratioEleBarrel, m_ratioEleEndcap, m_scaleMuo, m_resMuo, m_ratioTau;
   std::string const m_TauType, m_JetType, m_METType;

   // To access the JEC uncertainties from file.
   // New recipe:
   // https://twiki.cern.ch/twiki/bin/view/CMS/JECUncertaintySources?rev=19#Code_example
   std::string const m_jecType;
   JetCorrectorParameters const m_jecPara;
   JetCorrectionUncertainty m_jecUnc;
   JetResolution m_jetRes;

   // method map for function calls by string
   // Method name tags should be constructed following the sheme:
   // ParticleType_ShiftType e.g. Ele_Scale
   std::map<std::string, std::function<void()>>  systFuncMap ;

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

   pxl::EventView* m_GenEvtView;

   TRandom3* rand;
   //~ std::vector< std::reference_wrapper<SystematicsInfo> > m_activeSystematics;
   std::vector< SystematicsInfo* > m_activeSystematics;
   SystematicsInfo* m_activeSystematic;
   // methods
   void shiftMuoAndMET(std::string const shiftType);
   void shiftEleAndMET(std::string const shiftType);
   void shiftTauAndMET(std::string const shiftType);
   void shiftJetAndMET(std::string const shiftType);
   void shiftMETUnclustered(std::string const shiftType);
   bool inline checkshift(std::string const shiftType) const;
   void createEventViews(std::string prefix, pxl::EventView** evup, pxl::EventView** evdown);
   void fillMETLists(pxl::EventView* evup, pxl::EventView* evdown);
   void shiftParticle(pxl::EventView* eview, pxl::Particle* const part , double const& ratio, double& dPx, double& dPy/*, bool const up*/);
   void shiftMET(double const dPx_up, double const dPx_down, double const dPy_up, double const dPy_down);
};
#endif /*Systematics_hh*/
