#include "JetTypeWriter.hh"


// Constructor
JetTypeWriter::JetTypeWriter ( const Tools::MConfig &cfg ) :
	m_gen_rec_map( cfg ),
	m_recJetName( m_gen_rec_map.get( "Jet" ).RecName ),
	m_genJetName( m_gen_rec_map.get( "Jet" ).GenName ),

	m_bJet_algo(                  cfg.GetItem< std::string   >( "Jet.BJets.Algo" ) ),
    m_bJet_separate( cfg.GetItem< bool >( "Jet.BJets.separate" ) ),
	m_bJet_discriminatorThreshold( 	cfg.GetItem< float > ("Jet.BJets.Discr.min") ),
	// FIXME: In principle this should not be hardcoded
   // instead one should get the userRecordKey from config
   // Need to change this in ALL places where bJetType is retrieved
	m_bJet_userRecordKey("bJetType"),
   //m_bJet_userRecordKey(		cfg.GetItem< std::string   >( "Jet.BJets.UserRecordKey" ) ),
	m_bJet_gen_FlavourAlgo( cfg.GetItem< std::string >( "Jet.BJets.genFlavourAlgo" ) ),
	m_bJet_gen_label(      cfg.GetItem< std::string >( "Jet.BJets.Gen.Label" ) )
{
}

void JetTypeWriter::writeJetTypes( pxl::EventView* EvtView ) {
	const std::string type = "Type";
	bool isRec = (EvtView->getUserRecord( "Type" ) == "Rec");
	bool passed;

	// Get vector of jets in event
	std::vector< pxl::Particle* > jetList = getJetListFromEventView( EvtView );

	// Loop over jets
	for(std::vector< pxl::Particle*>::iterator jet = jetList.begin(); jet != jetList.end();	jet++) {
		// Check if criterion is fulfilled
		passed = passBJetCriterion ( (*jet), isRec );

		// Write user record accordingly
		if(passed && isRec ) {
			if(m_bJet_separate) (*jet)->setName( m_gen_rec_map.get("b").RecName );
			(*jet)->setUserRecord(m_bJet_userRecordKey, m_bJet_algo );
		} else if(passed) {
            (*jet)->setName( m_gen_rec_map.get("b").GenName  );
			if(m_bJet_separate) (*jet)->setUserRecord(m_bJet_algo, m_gen_rec_map.get("b").GenName );
		} else {
			(*jet)->setUserRecord(m_bJet_userRecordKey,"nonB");
		}
	}
}

bool JetTypeWriter::passBJetCriterion( pxl::Particle* jet, bool isRec) const {
	bool passed = false;
	if(isRec) {
		float rec_discriminator	= jet->getUserRecord( m_bJet_algo );
		passed			= rec_discriminator > m_bJet_discriminatorThreshold;
	} else {
		int gen_discriminator 	= jet->getUserRecord( m_bJet_gen_FlavourAlgo );
		// PDGID b quark = 5
		passed 			= (abs(gen_discriminator) == 5);
	}
	return 	passed;
}

std::vector< pxl::Particle* > JetTypeWriter::getJetListFromEventView( pxl::EventView* EvtView ) const {

	// Get all particles from event
	std::vector<pxl::Particle*> allparticles;
	EvtView->getObjectsOfType<pxl::Particle>(allparticles);

	// Create empty jet vector and fill
	std::vector<pxl::Particle*> jets;
	for (std::vector<pxl::Particle*>::const_iterator part = allparticles.begin(); part != allparticles.end(); ++part) {
		std::string name = (*part)->getName();
		// Do not care if rec or gen
		if( (name == m_recJetName) || (name == m_genJetName)) {
			jets.push_back(*part);
		}
	}

	return jets;
}
