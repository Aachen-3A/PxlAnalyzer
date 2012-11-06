#include "ParticleMatcher.hh"

using namespace std;
using namespace pxl;

// ------------ matching Method ------------

void ParticleMatcher::matchObjects(EventView* GenView,
                                   EventView* RecView,
                                   const std::vector<std::string>& _JetAlgos,
                                   const std::string& _bJetAlgo,
                                   const std::string& _METType,
                                   const bool& regardingBJets,
                                   const bool& CustomMatch) {

   // match all particles of each Particle types without regarding b-jets:
   // FIXME: Make code more generic! Generate a list of all Particle types
   std::vector<std::string> typeList;
   typeList.push_back("Muon");
   typeList.push_back("Ele");
   typeList.push_back("Gamma");
   typeList.push_back(_METType);
   int count_label = 0;
   for (std::vector<std::string>::const_iterator jet_label = _JetAlgos.begin(); jet_label != _JetAlgos.end(); ++jet_label) {
      typeList.push_back(_JetAlgos[count_label]);
      count_label++;
   }

   // containers to keep the filtered gen/rec particles
   vector<Particle*> gen_particles;
   vector<Particle*> rec_particles;
   for (std::vector<std::string>::const_iterator partType = typeList.begin(); partType != typeList.end(); ++partType) {
      // Choose name filter criterion
      gen_particles.clear();
      rec_particles.clear();
      ParticlePtEtaNameCriterion crit(*partType);
      ParticleFilter::apply( GenView->getObjectOwner(), gen_particles, crit );
      ParticleFilter::apply( RecView->getObjectOwner(), rec_particles, crit );
      makeMatching(gen_particles, rec_particles, _METType, "Match", "hctaM", "priv-gen-rec");
   }

   // want to match something else? Feel free to do it here!
   if(CustomMatch == true){

      // jet-subtype-matching:
      if (regardingBJets) {
         for (std::vector<std::string>::const_iterator jet_label = _JetAlgos.begin(); jet_label != _JetAlgos.end(); ++jet_label) {
            // bJet(rec)-bjet(gen)-matching:
            gen_particles.clear();
            rec_particles.clear();
            ParticleFilter::apply( GenView->getObjectOwner(), gen_particles, JetSubtypeCriterion(*jet_label, "genBJet") );
            ParticleFilter::apply( RecView->getObjectOwner(), rec_particles, JetSubtypeCriterion(*jet_label, _bJetAlgo) );
            makeMatching(gen_particles, rec_particles, _METType, "bJet-Match", "bJet-hctaM", "bJet-priv-gen-rec");

            // nonBJet(rec)-nonBjet(gen)-matching:
            gen_particles.clear();
            rec_particles.clear();
            ParticleFilter::apply( GenView->getObjectOwner(), gen_particles, JetSubtypeCriterion(*jet_label, "nonB") );
            ParticleFilter::apply( RecView->getObjectOwner(), rec_particles, JetSubtypeCriterion(*jet_label, "nonB") );
            makeMatching(gen_particles, rec_particles, _METType, "nonBJet-Match", "nonBJet-hctaM", "nonBJet-priv-gen-rec");
         }
      }

      //Make matching for estimation of fake rate for gammas
      rec_particles.clear();
      ParticlePtEtaNameCriterion critGamma("Gamma");
      ParticleFilter::apply( RecView->getObjectOwner(), rec_particles, critGamma );

      gen_particles.clear();
      ParticlePtEtaNameCriterion critEle("Ele");
      ParticleFilter::apply( GenView->getObjectOwner(), gen_particles, critEle );
      makeMatching(gen_particles, rec_particles, _METType, "MatchGammaEle", "hctaMGammaEle", "priv-genEle-recGamma");

      count_label = 0;

      for (std::vector<std::string>::const_iterator jet_label = _JetAlgos.begin(); jet_label != _JetAlgos.end(); ++jet_label) {
         gen_particles.clear();
         ParticlePtEtaNameCriterion critJet(_JetAlgos[count_label]);
         ParticleFilter::apply( GenView->getObjectOwner(), gen_particles, critJet );
         makeMatching(gen_particles, rec_particles, _METType, "MatchGamma"+_JetAlgos[count_label] , "hctaMGamma"+_JetAlgos[count_label], "priv-gen"+_JetAlgos[count_label]+"-recGamma");
         count_label++;
      }

      //match SIM converted photons to GEN photons
      gen_particles.clear();
      ParticlePtEtaNameCriterion critGammaGen("Gamma");
      ParticleFilter::apply( GenView->getObjectOwner(), gen_particles, critGammaGen );
      rec_particles.clear();
      ParticlePtEtaNameCriterion critSIMConv("SIMConvGamma");
      ParticleFilter::apply( GenView->getObjectOwner(), rec_particles, critSIMConv );
      makeMatching(gen_particles, rec_particles, _METType, "MatchGammaSIM", "hctaMGammaSIM", "priv-genGamma-SIMGammaConv");

      //match SIM converted photons to REC photons
      rec_particles.clear();
      ParticlePtEtaNameCriterion critGammaRec("Gamma");
      ParticleFilter::apply( RecView->getObjectOwner(), rec_particles, critGammaRec );
      gen_particles.clear();
      ParticleFilter::apply( GenView->getObjectOwner(), gen_particles, critSIMConv );
      makeMatching(gen_particles, rec_particles, _METType, "MatchGammaRecSIM", "hctaMGammaRecSIM", "priv-recGamma-SIMGammaConv");


   }
}

// ------------ implementation of the matching Gen <--> Rec ------------



void ParticleMatcher::makeMatching(std::vector<Particle*>& gen_particles,
                                   std::vector<Particle*>& rec_particles,
                                   const string& _METType,
                                   const string& Match,
                                   const string& hctaM,
                                   const string& linkname) {
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

      if (_fDebug > 1) cout << "Found " << num_gen << " Gen Objects and " << num_rec << " Rec Objects" << endl;

      TMatrixT<double> DistanzMatrix(num_gen, num_rec);
      TMatrixT<double> DeltaPtoPtMatrix(num_gen, num_rec);
      TMatrixT<double> DeltaChargeMatrix(num_gen, num_rec);

      for (std::vector<Particle*>::iterator gen_iter = gen_particles.begin(); gen_iter != gen_particles.end(); gen_iter++) {
         col = 0;
         for (std::vector<Particle*>::iterator rec_iter = rec_particles.begin(); rec_iter != rec_particles.end(); rec_iter++) {
            // Calculate the distance
            if (_fDebug > 0) {
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

      if (_fDebug > 0) DistanzMatrix.Print();

      //define value in dR used as matching criterion
      double DeltaRMatching = _DeltaR_Particles;
      //define value in DeltaPtoPt used as matching criterion
      double DeltaPtoPtMatching = _DeltaPtoPt;
      // def value in Delta Charge used as matching criterion
      double DeltaChargeMatching = _DeltaCharge;

      particle = (gen_particles.front())->getName();
      if (particle == _METType) DeltaRMatching = _DeltaR_MET;

      // go through every row and pushback index of Rec with smallest Distance
      for (unsigned int irow = 0; irow < num_gen; irow++) {
         int matched = SmallestRowElement(&DistanzMatrix, &DeltaPtoPtMatrix, &DeltaChargeMatrix, irow, DeltaRMatching, DeltaChargeMatching, DeltaPtoPtMatching);
         gen_particles[irow]->setUserRecord<int>(Match, matched);
         if (_fDebug > 0) cout << "GenObject " << irow << " is matched with " << matched << endl;

         if (matched != -1){
            gen_particles[ irow ]->setUserRecord< int >( "Charge"+Match, DeltaChargeMatrix( irow, matched ) );
            //redundant information with softlink, should replace the UserRecords after testing
            gen_particles[irow]->linkSoft(rec_particles[matched], linkname);

            rec_particles[matched]->setUserRecord<bool>(hctaM, true);
            if (_fDebug > 0) cout << "RecObject " << matched << " has matching Gen " << endl;
         }
      }

      for (unsigned int icol = 0; icol < num_rec; icol++) {
         //define value in dR which defines matching
         int matched = SmallestColumnElement(&DistanzMatrix, &DeltaPtoPtMatrix, &DeltaChargeMatrix, icol, DeltaRMatching, DeltaChargeMatching, DeltaPtoPtMatching);
         rec_particles[icol]->setUserRecord<int>(Match, matched);
         if (_fDebug > 0) cout << "RecObject " << icol << " is matched with " << matched << endl;

         if (matched != -1) {
            rec_particles[ icol ]->setUserRecord< int >( "Charge"+Match, DeltaChargeMatrix( matched, icol ) );
            //redundant information with softlink, should replace the UserRecords after testing
            rec_particles[icol]->linkSoft(gen_particles[matched], linkname);
            gen_particles[matched]->setUserRecord<bool>(hctaM, true);
            if (_fDebug > 0) cout << "GenObject " << matched << " has matching Rec " << endl;
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
                                        const double& DeltaPtoPtMatching) {

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
                                           const double& DeltaPtoPtMatching) {

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
