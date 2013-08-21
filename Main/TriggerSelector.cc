#include "TriggerSelector.hh"

#include "Tools/PXL/PXL.hh"

using std::string;

TriggerSelector::TriggerSelector( Tools::MConfig const &cfg ) :
   m_runOnData(     cfg.GetItem< bool   >( "General.RunOnData" ) ),
   m_ignoreL1(      cfg.GetItem< bool   >( "Trigger.IgnoreL1" ) ),
   m_ignoreHLT(     cfg.GetItem< bool   >( "Trigger.IgnoreHL" ) ),
   m_triggerPrefix( cfg.GetItem< string >( "Trigger.Name" ) + "_" ),   // This will be "HLT" in general.

   m_triggerGroups( initTriggerGroups( cfg ) ),

   m_metPtMin( cfg.GetItem< double >( "MET.pt.min" ) )
{
}


TriggerSelector::TriggerGroupCollection TriggerSelector::initTriggerGroups( Tools::MConfig const &cfg ) const {
   TriggerGroupCollection triggerGroups;
   if( not m_ignoreHLT ) {
      // Which trigger groups shall be considered?
      std::vector< string > const groupNames = Tools::splitString< string >( cfg.GetItem< string >( "Trigger.Groups" ), true );


      std::vector< string >::const_iterator groupName;
      for( groupName = groupNames.begin(); groupName != groupNames.end(); ++groupName ) {
         // TriggerGroup knows what to do with the config.
         TriggerGroup one_group( cfg, m_triggerPrefix, *groupName );
         triggerGroups.push_back( one_group );
      }
   }

   return triggerGroups;
}


bool TriggerSelector::passHLTrigger( bool const isRec,
                                     std::vector< pxl::Particle* > const &muos,
                                     std::vector< pxl::Particle* > const &eles,
                                     std::vector< pxl::Particle* > const &taus,
                                     std::vector< pxl::Particle* > const &gams,
                                     std::vector< pxl::Particle* > const &jets,
                                     std::vector< pxl::Particle* > const &mets,
                                     pxl::EventView *evtView
                                     ) const {
   // Do we care about HLTs?
   if( m_ignoreHLT ) return true;

   // In case of "Gen" there is no HLT.
   if( not isRec ) return true;

   // Accept all events if no triggers are configured.
   if( m_triggerGroups.size() == 0 ) return true;

   // Store if any trigger of any group fired.
   bool any_group_accepted = false;

   // Store if in all required groups at least one trigger fired.
   bool all_required_groups_accepted = true;

   // For each trigger group, check if any of the triggers has fired and if the event
   // topology fulfills the trigger requirements.
   for( TriggerGroupCollection::const_iterator group = m_triggerGroups.begin(); group != m_triggerGroups.end(); ++group ) {
      TriggerGroup const &this_group = *group;

      bool const group_required = this_group.getRequire();

      TriggerGroup::TriggerResults const triggerResults = this_group.getTriggerResults( evtView );

      bool const any_trigger_fired = anyTriggerFired( triggerResults );

      // Do not call passTriggerParticles if none of the triggers in this trigger group fired anyway.
      bool const trigger_particle_accepted = any_trigger_fired ? this_group.passTriggerParticles( isRec, muos, eles, taus, gams, jets, mets ) : false;
      evtView->setUserRecord< bool >( "trigger_particle_accept", trigger_particle_accepted );

      TriggerGroup::TriggerResults::const_iterator triggerResult;
      for( triggerResult = triggerResults.begin(); triggerResult != triggerResults.end(); ++triggerResult ) {
         // The trigger group is accepted if the trigger has fired and we have the
         // right particles in the event.
         evtView->setUserRecord< bool >( "HLTAccept_" + (*triggerResult).first, (*triggerResult).second and trigger_particle_accepted );
      }

      if( any_trigger_fired and trigger_particle_accepted ) any_group_accepted = true;
      if( group_required and not ( any_trigger_fired and trigger_particle_accepted ) ) all_required_groups_accepted = false;
   }

   // If any required group didn't fire, do not accept the event.
   // If no group is required, any fired trigger is fine.
   return all_required_groups_accepted and any_group_accepted;
}


// TODO: Do we really need this?
bool TriggerSelector::passL1Trigger( pxl::EventView const *evtView, bool const isRec ) const {
   // Ignore L1?
   if( m_ignoreL1 ) return true;

   // No "L1 simulation" on Gen level.
   if( not isRec ) return true;

   // Get the bits.
   bool b0 = evtView->findUserRecord< bool >( m_triggerPrefix + "L1_0" );

   // Bit 0 is not always properly simulated, but it doesn't make sense in MC anyway.
   if( not m_runOnData ) b0 = true;

   return b0;
}


bool TriggerSelector::passEventTopology( int const numMuo,
                                         int const numEle,
                                         int const numTau,
                                         int const numGam,
                                         int const numJet,
                                         int const numMET
                                         ) const {
   bool any_accepted = false;

   // Check topology for all given trigger groups.
   for( TriggerGroupCollection::const_iterator group = m_triggerGroups.begin(); group != m_triggerGroups.end(); ++group ) {
      if( (*group).checkTopology( numMuo, numEle, numTau, numGam, numJet, numMET ) ) any_accepted = true;
      // No need to proceed once we found one.
      if( any_accepted ) break;
   }

   return any_accepted;
}


bool TriggerSelector::checkVeto( bool const isRec,
                                 std::vector< pxl::Particle* > const &muos,
                                 std::vector< pxl::Particle* > const &eles,
                                 std::vector< pxl::Particle* > const &taus,
                                 std::vector< pxl::Particle* > const &gams,
                                 std::vector< pxl::Particle* > const &jets,
                                 std::vector< pxl::Particle* > const &mets,
                                 pxl::EventView const *evtView
                                 ) const {
   // If triggers ignored, no veto.
   if( m_ignoreHLT ) return false;

   // No veto for "Gen".
   if( not isRec ) return false;

   // Veto no events if no triggers are configured.
   if( m_triggerGroups.size() == 0 ) return false;

   // Store if any trigger group causes a veto.
   bool any_veto = false;

   // Loop over groups...
   for( TriggerGroupCollection::const_iterator group = m_triggerGroups.begin(); group != m_triggerGroups.end(); ++group ) {
      // If we found one, no need to proceed.
      if( any_veto ) break;

      TriggerGroup const &this_group = *group;

      bool const trigger_reject = this_group.getReject();

      if( not trigger_reject ) continue;  // Nothing to do here...

      TriggerGroup::TriggerResults const triggerResults = this_group.getTriggerResults( evtView );
      // Did any trigger in this group fire?
      bool const any_trigger_fired = anyTriggerFired( triggerResults );

      // Do not call passTriggerParticles if none of the triggers in this trigger group fired anyway.
      bool const trigger_particle_accepted = any_trigger_fired ? this_group.passTriggerParticles( isRec, muos, eles, taus, gams, jets, mets ) : false;

      // Reject the event if the trigger fired.
      if( trigger_reject and any_trigger_fired and trigger_particle_accepted ) any_veto = true;
   }

   return any_veto;
}


double TriggerSelector::getSumptMin( int const numMuo,
                                     int const numEle,
                                     int const numTau,
                                     int const numGam,
                                     int const numJet,
                                     int const numMET,
                                     bool const inclusive
                                     ) const {
   // Empty+X class!
   if( numMuo == 0 and
       numEle == 0 and
       numTau == 0 and
       numGam == 0 and
       numJet == 0 and
       numMET == 0 ) return 0.0;

   std::vector< double > sumpts;

   TriggerGroupCollection::const_iterator group;
   for( group = m_triggerGroups.begin(); group != m_triggerGroups.end(); ++group ) {
      TriggerGroup const &this_group = *group;
      bool compute_sumpt = false;

      if( inclusive ) {
         compute_sumpt = true;
      } else {
         // If we look at an exclusive class, we have to check the topology first,
         // to see which TriggerGroup applys here.
         if( this_group.checkTopology( numMuo, numEle, numTau, numGam, numJet, numMET ) ) {
            compute_sumpt = true;
         }
      }

      if( compute_sumpt ) sumpts.push_back( this_group.getSumptMin( numMuo,
                                                                    numEle,
                                                                    numTau,
                                                                    numGam,
                                                                    numJet,
                                                                    numMET,
                                                                    inclusive
                                                                    ) );
   }

   std::sort( sumpts.begin(), sumpts.end() );

   // Return the smallest value.
   return sumpts.at( 0 );
}


double TriggerSelector::getMETMin() const {
   std::vector< double > mets;

   TriggerGroupCollection::const_iterator group;
   for( group = m_triggerGroups.begin(); group != m_triggerGroups.end(); ++group ) {
      TriggerGroup this_group = *group;
      if( this_group.getNCuts( "MET" ) > 0 ) {
         mets.push_back( this_group.getMETMin() );
      }
   }

   mets.push_back( m_metPtMin );

   std::sort( mets.begin(), mets. end() );

   if( not mets.size() > 0 ) return 0.0;

   // Return the smallest value.
   return mets.at( 0 );
}


bool TriggerSelector::anyTriggerFired( TriggerGroup::TriggerResults const &triggerResults ) const {
   bool any_trigger_fired = false;

   TriggerGroup::TriggerResults::const_iterator triggerResult;
   for( triggerResult = triggerResults.begin(); triggerResult != triggerResults.end(); ++triggerResult ) {
      if( (*triggerResult).second == 1 ) any_trigger_fired = true;
      // No need to continue once we found one.
      if( any_trigger_fired ) break;
   }

   return any_trigger_fired;
}
