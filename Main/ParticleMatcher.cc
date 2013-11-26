#include "ParticleMatcher.hh"

#include "Tools/MConfig.hh"

using namespace std;
using namespace pxl;

ParticleMatcher::ParticleMatcher( Tools::MConfig const &cfg,
                                  int const debug
                                  ) :
   m_DeltaR_Particles( cfg.GetItem< double >( "Matching.DeltaR.particles" ) ),
   m_DeltaR_MET(       cfg.GetItem< double >( "Matching.DeltaR.met" ) ),
   m_DeltaPtoPt(       cfg.GetItem< double >( "Matching.DeltaPtOverPt" ) ),
   m_DeltaCharge(      cfg.GetItem< double >( "Matching.DeltaCharge" ) ),

   m_jet_bJets_use(       cfg.GetItem< bool   >( "Jet.BJets.use" ) ),
   m_jet_bJets_algo(      cfg.GetItem< string >( "Jet.BJets.Algo" ) ),
   m_jet_bJets_gen_label( cfg.GetItem< string >( "Jet.BJets.Gen.Label" ) ),

   m_gen_rec_map( cfg ),

   m_debug( debug )
{}

// ------------ matching Method ------------

void ParticleMatcher::matchObjects( EventView const *GenEvtView,
                                    EventView const *RecEvtView,
                                    std::string const defaultLinkName,
                                    bool const customMatch
                                    ) const {
   // containers to keep the filtered gen/rec particles
   vector< Particle* > gen_particles;
   vector< Particle* > rec_particles;

   for( GenRecNameMap::const_iterator objType = m_gen_rec_map.begin(); objType != m_gen_rec_map.end(); ++objType ) {
      gen_particles.clear();
      rec_particles.clear();

      // Choose name filter criterion
      ParticlePtEtaNameCriterion const critRec( (*objType).second.RecName );
      ParticlePtEtaNameCriterion const critGen( (*objType).second.GenName );

      ParticleFilter::apply( RecEvtView->getObjectOwner(), rec_particles, critRec );
      ParticleFilter::apply( GenEvtView->getObjectOwner(), gen_particles, critGen );

      makeMatching( gen_particles, rec_particles, "Match", "hctaM", defaultLinkName );
   }

   // jet-subtype-matching:
   if( m_jet_bJets_use ) {
      // Get all Gen b-jets.
      gen_particles.clear();
      JetSubtypeCriterion const critBJetGen( m_gen_rec_map.get( "Jet" ).GenName, m_jet_bJets_gen_label );
      ParticleFilter::apply( GenEvtView->getObjectOwner(),
                             gen_particles,
                             critBJetGen
                             );

      // Get all Rec b-jets.
      rec_particles.clear();
      JetSubtypeCriterion const critBJetRec( m_gen_rec_map.get( "Jet" ).RecName, m_jet_bJets_algo );
      ParticleFilter::apply( RecEvtView->getObjectOwner(),
                             rec_particles,
                             critBJetRec
                             );

      makeMatching( gen_particles,
                    rec_particles,
                    "bJet-Match",
                    "bJet-hctaM",
                    "bJet-priv-gen-rec"
                    );

      // nonBJet(rec)-nonBjet(gen)-matching:
      gen_particles.clear();
      JetSubtypeCriterion const critJetGen( m_gen_rec_map.get( "Jet" ).GenName, "nonB" );
      ParticleFilter::apply( GenEvtView->getObjectOwner(),
                             gen_particles,
                             critJetGen
                             );

      rec_particles.clear();
      JetSubtypeCriterion const critJetRec( m_gen_rec_map.get( "Jet" ).RecName, "nonB" );
      ParticleFilter::apply( RecEvtView->getObjectOwner(),
                             rec_particles,
                             critJetRec
                             );

      makeMatching( gen_particles,
                    rec_particles,
                    "nonBJet-Match",
                    "nonBJet-hctaM",
                    "nonBJet-priv-gen-rec"
                    );
   }

   if( customMatch ) {
      //Make matching for estimation of fake rate for gammas
      rec_particles.clear();
      ParticlePtEtaNameCriterion const critGamRec( m_gen_rec_map.get( "Gam" ).RecName );
      ParticleFilter::apply( RecEvtView->getObjectOwner(),
                             rec_particles,
                             critGamRec
                             );

      gen_particles.clear();
      ParticlePtEtaNameCriterion const critEleGen( m_gen_rec_map.get( "Ele" ).GenName );
      ParticleFilter::apply( GenEvtView->getObjectOwner(),
                             gen_particles,
                             critEleGen
                             );

      makeMatching( gen_particles,
                    rec_particles,
                    "MatchGammaEle",
                    "hctaMGammaEle",
                    "priv-genEle-recGamma"
                    );

      gen_particles.clear();
      ParticlePtEtaNameCriterion const critJetGen( m_gen_rec_map.get( "Jet" ).GenName );
      ParticleFilter::apply( GenEvtView->getObjectOwner(),
                             gen_particles,
                             critJetGen
                             );

      makeMatching( gen_particles,
                    rec_particles,
                    "MatchGamma" + m_gen_rec_map.get( "Jet" ).RecName,
                    "hctaMGamma" + m_gen_rec_map.get( "Jet" ).RecName,
                    "priv-gen"   + m_gen_rec_map.get( "Jet" ).RecName + "-recGamma"
                    );

      //match SIM converted photons to GEN photons
      gen_particles.clear();
      ParticlePtEtaNameCriterion const critGamGen( m_gen_rec_map.get( "Gam" ).GenName );
      ParticleFilter::apply( GenEvtView->getObjectOwner(),
                             gen_particles,
                             critGamGen
                             );

      // Use Rec particles container here.
      rec_particles.clear();
      ParticlePtEtaNameCriterion const critSIMConv( "SIMConvGamma" );
      ParticleFilter::apply( GenEvtView->getObjectOwner(),
                             rec_particles,
                             critSIMConv
                             );

      makeMatching( gen_particles,
                    rec_particles,
                    "MatchGammaSIM",
                    "hctaMGammaSIM",
                    "priv-genGamma-SIMGammaConv"
                    );

      //match SIM converted photons to REC photons
      rec_particles.clear();
      ParticleFilter::apply( RecEvtView->getObjectOwner(),
                             rec_particles,
                             critGamRec
                             );

      gen_particles.clear();
      ParticleFilter::apply( GenEvtView->getObjectOwner(),
                             gen_particles,
                             critSIMConv
                             );

      makeMatching( gen_particles,
                    rec_particles,
                    "MatchGammaRecSIM",
                    "hctaMGammaRecSIM",
                    "priv-recGamma-SIMGammaConv"
                    );
   }
}

// ------------ implementation of the matching Gen <--> Rec ------------



void ParticleMatcher::makeMatching(std::vector<Particle*>& gen_particles,
                                   std::vector<Particle*>& rec_particles,
                                   const string& Match,
                                   const string& hctaM,
                                   const string& linkname
                                   ) const {
   // First set for Gen all Matches to -1 and reset bools:
   for (std::vector<Particle*>::iterator gen_iter = gen_particles.begin(); gen_iter != gen_particles.end(); gen_iter++) {
      (*gen_iter)->setUserRecord<int>(Match, -1);
      (*gen_iter)->setUserRecord<bool>(hctaM, false);
   }
   // same for Rec
   for (std::vector<Particle*>::iterator rec_iter = rec_particles.begin(); rec_iter != rec_particles.end(); rec_iter++) {
      (*rec_iter)->setUserRecord<int>(Match, -1);
      (*rec_iter)->setUserRecord<bool>(hctaM, false);
   }
   unsigned int num_gen = gen_particles.size();
   unsigned int num_rec = rec_particles.size();

   // we need at least one Gen and one Rec to perform matching!
   if (num_gen > 0 && num_rec > 0) {
      unsigned int col = 0;
      unsigned int row = 0;
      std::string particle;

      if( m_debug > 1 ) cout << "Found " << num_gen << " Gen Objects and " << num_rec << " Rec Objects" << endl;

      TMatrixT<double> DistanzMatrix(num_gen, num_rec);
      TMatrixT<double> DeltaPtoPtMatrix(num_gen, num_rec);
      TMatrixT<double> DeltaChargeMatrix(num_gen, num_rec);

      for (std::vector<Particle*>::iterator gen_iter = gen_particles.begin(); gen_iter != gen_particles.end(); gen_iter++) {
         col = 0;
         for (std::vector<Particle*>::iterator rec_iter = rec_particles.begin(); rec_iter != rec_particles.end(); rec_iter++) {
            // Calculate the distance
            if( m_debug > 0 ) {
               cout << "Gen: " << (*gen_iter)->print(0);
               cout << "Rec: " << (*rec_iter)->print(0);
               cout << "Distance: " << (*gen_iter)->getVector().deltaR(&((*rec_iter)->getVector())) << endl;
            }
            DistanzMatrix(row,col) = (*gen_iter)->getVector().deltaR(&((*rec_iter)->getVector()));
            DeltaPtoPtMatrix(row,col) = fabs(((*rec_iter)->getVector().getPt() / (*gen_iter)->getVector().getPt()) - 1);
            DeltaChargeMatrix(row,col) = fabs( ((*rec_iter)->getCharge()) - ((*gen_iter)->getCharge()) );
            col++;
         }
         row++;
      }

      if( m_debug > 0 ) DistanzMatrix.Print();

      //define value in dR used as matching criterion
      double DeltaRMatching = m_DeltaR_Particles;
      //define value in DeltaPtoPt used as matching criterion
      double DeltaPtoPtMatching = m_DeltaPtoPt;
      // def value in Delta Charge used as matching criterion
      double DeltaChargeMatching = m_DeltaCharge;

      particle = (gen_particles.front())->getName();
      if( particle == m_gen_rec_map.get( "MET" ).GenName ) DeltaRMatching = m_DeltaR_MET;

      // go through every row and pushback index of Rec with smallest Distance
      for (unsigned int irow = 0; irow < num_gen; irow++) {
         int matched = SmallestRowElement(&DistanzMatrix, &DeltaPtoPtMatrix, &DeltaChargeMatrix, irow, DeltaRMatching, DeltaChargeMatching, DeltaPtoPtMatching);
         gen_particles[irow]->setUserRecord<int>(Match, matched);
         if( m_debug > 0 ) cout << "GenObject " << irow << " is matched with " << matched << endl;

         if (matched != -1){
            gen_particles[ irow ]->setUserRecord< int >( "Charge"+Match, DeltaChargeMatrix( irow, matched ) );
            //redundant information with softlink, should replace the UserRecords after testing
            gen_particles[irow]->linkSoft(rec_particles[matched], linkname);

            rec_particles[matched]->setUserRecord<bool>(hctaM, true);
            if( m_debug > 0 ) cout << "RecObject " << matched << " has matching Gen " << endl;
         }
      }

      for (unsigned int icol = 0; icol < num_rec; icol++) {
         //define value in dR which defines matching
         int matched = SmallestColumnElement(&DistanzMatrix, &DeltaPtoPtMatrix, &DeltaChargeMatrix, icol, DeltaRMatching, DeltaChargeMatching, DeltaPtoPtMatching);
         rec_particles[icol]->setUserRecord<int>(Match, matched);
         if( m_debug > 0 ) cout << "RecObject " << icol << " is matched with " << matched << endl;

         if (matched != -1) {
            rec_particles[ icol ]->setUserRecord< int >( "Charge"+Match, DeltaChargeMatrix( matched, icol ) );
            //redundant information with softlink, should replace the UserRecords after testing
            rec_particles[icol]->linkSoft(gen_particles[matched], linkname);
            gen_particles[matched]->setUserRecord<bool>(hctaM, true);
            if( m_debug > 0 ) cout << "GenObject " << matched << " has matching Rec " << endl;
         }
      }
   }
}

// ---------------------- Helper Method ------------------------------

int ParticleMatcher::SmallestRowElement(TMatrixT<double>* matrixDR,
                                        TMatrixT<double>* matrixDp,
                                        TMatrixT<double>* matrixDC,
                                        const unsigned int& row,
                                        const double& DeltaRMatching,
                                        const double& DeltaChargeMatching,
                                        const double& DeltaPtoPtMatching
                                        ) const {

   // loop over row and return index of smallest element
   double elementDR = (*matrixDR)(row, 0);
   double elementDp = (*matrixDp)(row, 0);
   double elementDC = (*matrixDC)(row, 0);
   int index = 0;
   for (int i = 1; i < matrixDR->GetNcols(); i++) {
      if ((*matrixDR)(row, i) < elementDR) {
         elementDR = (*matrixDR)(row,i);
         elementDp = (*matrixDp)(row,i);
         elementDC = (*matrixDC)(row,i);
         index = i;
      }
   }
   if ((elementDR > DeltaRMatching) || (elementDp > DeltaPtoPtMatching) || ( elementDC > DeltaChargeMatching)) index = -1;
   return index;
}

// ---------------------- Helper Method ------------------------------

int ParticleMatcher::SmallestColumnElement(TMatrixT<double>* matrixDR,
                                           TMatrixT<double>* matrixDp,
                                           TMatrixT<double>* matrixDC,
                                           const unsigned int& col,
                                           const double& DeltaRMatching,
                                           const double& DeltaChargeMatching,
                                           const double& DeltaPtoPtMatching
                                           ) const {

   // loop over row and return index of smallest element
   double elementDR = (*matrixDR)(0, col);
   double elementDp = (*matrixDp)(0, col);
   double elementDC = (*matrixDC)(0, col);
   int index = 0;
   for (int i = 1; i < matrixDR->GetNrows(); i++) {
      if ((*matrixDR)(i, col) < elementDR) {
         elementDR = (*matrixDR)(i,col);
         elementDp = (*matrixDp)(i,col);
         elementDC = (*matrixDC)(i,col);
         index = i;
      }
   }
   if ((elementDR > DeltaRMatching) || (elementDp > DeltaPtoPtMatching) || ( elementDC > DeltaChargeMatching)) index = -1;
   return index;
}
