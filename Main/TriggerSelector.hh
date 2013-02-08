#ifndef TRIGGERSELECTOR
#define TRIGGERSELECTOR

#include <list>
#include <string>
#include <vector>

#include "Tools/MConfig.hh"
#include "Tools/Tools.hh"

#include "TriggerGroup.hh"

namespace pxl {
   class EventView;
   class Particle;
}

class TriggerSelector {
   public:
      typedef std::list< TriggerGroup > TriggerGroupCollection;

      explicit TriggerSelector( Tools::MConfig const &cfg );
      ~TriggerSelector() {}

      // Functions to be called from outside for event selection:

      // Check if any of the required triggers (in the config file) fired in this event.
      bool passHLTrigger( bool const isRec,
                          std::vector< pxl::Particle* > const &muos,
                          std::vector< pxl::Particle* > const &eles,
                          std::vector< pxl::Particle* > const &taus,
                          std::vector< pxl::Particle* > const &gams,
                          std::vector< pxl::Particle* > const &jets,
                          std::vector< pxl::Particle* > const &mets,
                          pxl::EventView *evtView
                          ) const;

      // Check L1 technical bits.
      bool passL1Trigger( pxl::EventView const *evtView, bool const isRec ) const;

      // Returns true, if a "reject" trigger has fired AND if the event topology fits.
      bool checkVeto( bool const isRec,
                      std::vector< pxl::Particle* > const &muos,
                      std::vector< pxl::Particle* > const &eles,
                      std::vector< pxl::Particle* > const &taus,
                      std::vector< pxl::Particle* > const &gams,
                      std::vector< pxl::Particle* > const &jets,
                      std::vector< pxl::Particle* > const &mets,
                      pxl::EventView const *evtView
                      ) const;

      // Check if there are any unprescaled single muon or single electron
      // triggers.
      bool checkHLTMuEle( pxl::EventView const *evtView, bool const isRec ) const;

      // Check if the given set of particles fulfills any of the possible required
      // trigger topologies.
      bool passEventTopology( int const numMuo,
                              int const numEle,
                              int const numTau,
                              int const numGam,
                              int const numJet,
                              int const numMET
                              ) const;

      // Getters:

      // Compute all possible minimal sumpt values for the given set of particles with
      // the given trigger groups (from the config file) and pt cuts.
      double getSumptMin( int const numMuo,
                          int const numEle,
                          int const numTau,
                          int const numGam,
                          int const numJet,
                          int const numMET,
                          bool const inclusive
                          ) const;

      // Compute the minimal possible value for MET with the given trigger groups and
      // pt cuts.
      double getMETMin( int const numMuo,
                        int const numEle,
                        int const numTau,
                        int const numGam,
                        int const numJet,
                        int const numMET,
                        bool const inclusive
                        ) const;

      std::string const &getTriggerPrefix() const { return m_triggerPrefix; }
      TriggerGroupCollection const &getTriggerGroups() const { return m_triggerGroups; }

   private:
      // Get the trigger groups from the config file.
      TriggerGroupCollection initTriggerGroups( Tools::MConfig const &cfg ) const;
      bool anyTriggerFired( TriggerGroup::TriggerResults const &triggerResults ) const;

      // Member variables:
      bool const m_runOnData;
      bool const m_ignoreL1;
      bool const m_ignoreHLT;
      std::string const m_triggerPrefix;
      TriggerGroupCollection const m_triggerGroups;

      double const m_metPtMin;
};

#endif /*TRIGGERSELECTOR*/
