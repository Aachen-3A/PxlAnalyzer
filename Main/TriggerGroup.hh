#ifndef TRIGGERGROUP
#define TRIGGERGROUP

#include <map>
#include <string>
#include <vector>

#include "Tools/MConfig.hh"

namespace pxl {
   class Particle;
}

class TriggerGroup {
   public:
      typedef std::vector< double > TriggerCuts;

      TriggerGroup( Tools::MConfig const &cfg, unsigned int const group_num );
      ~TriggerGroup() {}

      // Getters:
      std::string const &getName() const { return m_name; }
      std::string const &getTrigger() const { return m_trigger; }
      bool const &getRequire() const { return m_require; }
      bool const &getReject() const { return m_reject; }
      TriggerCuts const &getCuts( std::string const &particle ) const {
         return (*m_cuts_map.find( particle )).second;
      }
      int getNCuts( std::string const &particle ) const {
         return static_cast< int >( (*m_cuts_map.find( particle )).second.size() );
      }

      // Functions to be called from outside for event selection:

      // Check whether the given set of particles fulfills the topological
      // requirements of the given trigger group. I.e. do we have enough particles of
      // each type.
      bool checkTopology( int const numMuo,
                          int const numEle,
                          int const numTau,
                          int const numGam,
                          int const numJet,
                          int const numMET
                          ) const;
      // Check if there are as many particles (of each type) in the event as required
      // by the trigger(s) and check if they fulfill the pt requirements.
      bool passTriggerParticles( bool const isRec,
                                 std::vector< pxl::Particle* > const &muos,
                                 std::vector< pxl::Particle* > const &eles,
                                 std::vector< pxl::Particle* > const &taus,
                                 std::vector< pxl::Particle* > const &gams,
                                 std::vector< pxl::Particle* > const &jets,
                                 std::vector< pxl::Particle* > const &mets
                                 ) const;
      // Compute the minimal possible sumpt in each trigger group for
      // the given set of particles.
      double computeSumptMin( int numMuo,
                              int numEle,
                              int numTau,
                              int numGam,
                              int numJet,
                              int numMET,
                              bool const inclusive
                              ) const;

   private:
      // There are six types of particles that are considered at the moment. For each
      // trigger group, look if there is one or more pt cuts set for each of these
      // particles. If they are, these cuts will be used to select the event. The
      // set of these cuts also defines the topological selection of the events. I.e.
      // if there a two cuts for electrons in the config file, there must be at least
      // two electrons in the event to be accepted.
      // The topology selection is implicitly done by the number of cuts for each
      // particle.
      std::map< std::string, TriggerCuts > initCuts( Tools::MConfig const &cfg ) const;

      // Check the pt for all required particles.
      bool checkTriggerParticles( TriggerCuts const &cuts,
                                  std::vector< pxl::Particle* > const &particles
                                  ) const;

      // Member variables:
      std::string const m_prefix;
      std::string const m_name;
      std::string const m_trigger;
      bool const m_require;
      bool const m_reject;
      std::map< std::string, TriggerCuts > m_cuts_map;

      double const m_muoPtMin;
      double const m_elePtMin;
      double const m_tauPtMin;
      double const m_gamPtMin;
      double const m_jetPtMin;
      double const m_metPtMin;
};

#endif /*TRIGGERGROUP*/
