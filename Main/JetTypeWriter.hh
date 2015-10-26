#ifndef JetTypeWriter_hh
#define JetTypeWriter_hh
#include "Pxl/Pxl/interface/pxl/hep.hh"
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Main/GenRecNameMap.hh"
#include "Tools/MConfig.hh"

///////////////////////////////////////////////////////
// Class used to write tagging information into jets //
///////////////////////////////////////////////////////

class JetTypeWriter {

	public:
	   JetTypeWriter ( const Tools::MConfig &cfg );

	   // Main method to write jet tag info to all jets
	   void writeJetTypes( pxl::EventView* EvtView );

	private:
      // Helper method to test if jet passed B jet criterion
      bool passBJetCriterion( pxl::Particle *jet, bool isRec ) const;
      std::vector<pxl::Particle*> getJetListFromEventView(pxl::EventView* EvtView) const;

      // Configuration Variables
      GenRecNameMap const	 m_gen_rec_map;			 // Map containing rec and gen names
      std::string const 	 m_recJetName;			    // Name of reconstructed jets
      std::string const 	 m_genJetName;			    // Name of generator jets

      // bJet specific
      std::string const 	 m_bJet_algo;			            // What criterion do we check?
      bool const            m_bJet_separate;                 // Flag if the name of the particle is changed from m_rec_map["Jet"] to m_rec_map["B"]
      float const           m_bJet_discriminatorThreshold;	// What threshold is used?
      std::string const 	 m_bJet_userRecordKey;		      // Which UserRecord do we write to?
      std::string const 	 m_bJet_gen_FlavourAlgo;		   // What generator information to use?
      std::string const 	 m_bJet_gen_label;		         // What label to apply to passes gen jets?
};
#endif
