#ifndef TRIGGERGROUP
#define TRIGGERGROUP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Tools/MConfig.hh"

namespace pxl {
   class EventView;
   class Particle;
}

class TriggerGroup {
   public:
      typedef std::vector< double > TriggerCuts;
      typedef std::map< std::string, TriggerCuts > TriggerCutsCollection;
      typedef std::set< std::string > Triggers;
      typedef std::map< std::string, bool > TriggerResults;
      typedef std::map< std::string, double > PtMap;

      TriggerGroup( Tools::MConfig const &cfg,
                    std::string const &triggerPrefix,
                    std::string const &groupName
                    );
      ~TriggerGroup() {}

      // Getters:
      std::string const &getName() const { return m_name; }
      Triggers const &getTriggers() const { return m_triggers; }
      bool const &getRequire() const { return m_require; }
      bool const &getReject() const { return m_reject; }

      // Check if there are any pt-cuts set for the given particle type and
      // store it in the given variable.
      bool const getCuts( std::string const &particleType, TriggerCuts &cuts ) const {
         TriggerCutsCollection::const_iterator found = m_cuts_map.find( particleType );

         if( found != m_cuts_map.end() ) {
            cuts = (*found).second;
            return true;
         } else
            return false;
      }

      // Get the number of pt-cuts for the given particle type.
      // If no cuts are set return -1.
      // This is used for topological selection.
      int getNCuts( std::string const &particleType ) const {
         TriggerCutsCollection::const_iterator found = m_cuts_map.find( particleType );

         if( found != m_cuts_map.end() ) {
            return static_cast< int >( (*found).second.size() );
         } else
            return -1;
      }

      // Functions to be called from outside for event selection:

      TriggerResults getTriggerResults( pxl::EventView const *evtView ) const;

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
      // Compute the minimal possible sumpt in this trigger group for
      // the given set of particles.
      double getSumptMin( int const numMuo,
                          int const numEle,
                          int const numTau,
                          int const numGam,
                          int const numJet,
                          int const numMET,
                          bool const inclusive
                          ) const;
      double getMETMin();

   private:
      // Each trigger group (in the config file) can define one oder more
      // triggers. All of these triggers must be of the "same type", e.g.
      // trigger group 1 could define several SingleMu triggers and accordingly
      // one set of pt-cuts for this trigger.
      Triggers initTriggers( Tools::MConfig const &cfg ) const;

      // Store all the pt-cuts for non-triggering particles in a map to have
      // easier access later. They are used to compute the smallest possible
      // sum(pt) for the given topology.
      PtMap initPtMin( Tools::MConfig const &cfg ) const;

      // There are six types of particles that are considered at the moment. For each
      // trigger group, look if there is one or more pt cuts set for each of these
      // particles. If they are, these cuts will be used to select the event. The
      // set of these cuts also defines the topological selection of the events. I.e.
      // if there a two cuts for electrons in the config file, there must be at least
      // two electrons in the event to be accepted.
      // The topology selection is implicitly done by the number of cuts for each
      // particle.
      TriggerCutsCollection initCuts( Tools::MConfig const &cfg ) const;

      // Check the pt for all required particles.
      bool checkTriggerParticles( std::string const &particleType,
                                  std::vector< pxl::Particle* > const &particles
                                  ) const;

      // Calculate the minium possible sum(pt) for all the given particles.
      double sumPtMinForParticles( std::string const &particleType,
                                   bool const &inclusive,
                                   int numParticles
                                   ) const;

      // Member variables:
      std::string const m_triggerPrefix;
      std::string const m_prefix;
      std::string const m_name;
      Triggers const m_triggers;
      bool const m_require;
      bool const m_reject;
      TriggerCutsCollection const m_cuts_map;
      PtMap const m_ptMin_map;
};

#endif /*TRIGGERGROUP*/
