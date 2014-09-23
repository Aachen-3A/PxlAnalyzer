#include "PDFTool.hh"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Tools/MConfig.hh"
#include "Tools/Tools.hh"

using namespace pdf;

PDFTool::PDFTool( Tools::MConfig const &config, unsigned int const debug ) :
   m_pdfInfo(),
   m_debug( debug ),
   // LHAPATH is an environment variable that is set, when you source
   // '/cvmfs/cms.cern.ch/cmsset_default.sh' and call 'cmsenv'.
   // If it is set correctly, LHAPDF will find the files containing the PDF sets.
   m_pdfPath( getenv( "LHAPATH" ) ),
   m_pdfProd( LHAPDF::mkPDF( config.GetItem< std::string >( "PDF.Prod.Name" ),
                             0
                             ) ),
   m_pdfSetsCTEQ( initCTEQPDFs( config ) ),
   m_pdfSetsMSTW( initMSTWPDFs( config ) ),
   m_pdfSetsNNPDF( initNNPDFPDFs( config ) )
{
   // Set the init flag in PDFInfo. This is important when merging later!
   m_pdfInfo.init = true;

   if( debug > 1 ) {
      std::stringstream info;
      info << "[DEBUG] (PDFTool): ";
      info << "Found " << m_pdfInfo.n_cteq  << " CTEQ PDF sets."  << std::endl;
      info << "Found " << m_pdfInfo.n_mstw  << " MSTW PDF sets."  << std::endl;
      info << "Found " << m_pdfInfo.n_nnpdf << " NNPDF PDF sets." << std::endl;
      info << "Found " << m_pdfInfo.n_alpha_cteq;
      info << " CTEQ alpha_s PDF sets." << std::endl;
      info << "Found " << m_pdfInfo.n_alpha_mstw;
      info << " MSTW alpha_s PDF sets." << std::endl;

      std::cerr << info.str();
   }
}


PDFTool::PDFSets PDFTool::initCTEQPDFs( Tools::MConfig const &config,
                                        std::string const &PDFNames
                                        ) {
   LHAPDF::setPDFPath( m_pdfPath );

   // Get PDF set names from config.
   vstring const PDFSetNames = Tools::splitString< std::string >(
                                  config.GetItem< std::string >( PDFNames ),
                                  true
                                  );

   // For CTEQ (CT10), the first name should point to the actual PDFs sets, the
   // second to the alpha_s variations.

   if( PDFSetNames.size() != 2 ) {
      std::stringstream err;
      err << "[ERROR] (PDFTool): ";
      err << "Only supported number of CTEQ (CT10) PDF set names is exactly 2.";
      throw Tools::config_error( err.str() );
   }

   PDFSets CTEQpdfs( LHAPDF::mkPDFs( PDFSetNames.at( 0 ) ) );

   m_pdfInfo.n_cteq = CTEQpdfs.size();

   // In the second PDFset, number 4 is the alpha_s down, number 6 is the
   // alpha_s up variation.
   CTEQpdfs.push_back( LHAPDF::mkPDF( PDFSetNames.at( 1 ), 4 ) );
   CTEQpdfs.push_back( LHAPDF::mkPDF( PDFSetNames.at( 1 ), 6 ) );

   m_pdfInfo.n_alpha_cteq = CTEQpdfs.size() - m_pdfInfo.n_cteq;

   return CTEQpdfs;
}


PDFTool::PDFSets PDFTool::initMSTWPDFs( Tools::MConfig const &config,
                                        std::string const &PDFNames
                                        ) {
   LHAPDF::setPDFPath( m_pdfPath );

   // Get PDF set names from config.
   vstring const PDFSetNames = Tools::splitString< std::string >(
                                  config.GetItem< std::string >( PDFNames ),
                                  true
                                  );

   // For MSTW, the first name should point to the actual PDFs sets, the second
   // to the alpha_s variation up, the third to the alpha_s variation down!

   if( PDFSetNames.size() != 3 ) {
      std::stringstream err;
      err << "[ERROR] (PDFTool): ";
      err << "Only supported number of MSTW PDF set names is exactly 3.";
      throw Tools::config_error( err.str() );
   }

   PDFSets MSTWpdfs( LHAPDF::mkPDFs( PDFSetNames.at( 0 ) ) );

   m_pdfInfo.n_mstw = MSTWpdfs.size();

   // In the second PDFset, number 0 is the alpha_s variation up.
   MSTWpdfs.push_back( LHAPDF::mkPDF( PDFSetNames.at( 1 ), 0 ) );

   // In the third PDFset, number 0 is the alpha_s variation down.
   MSTWpdfs.push_back( LHAPDF::mkPDF( PDFSetNames.at( 2 ), 0 ) );

   m_pdfInfo.n_alpha_mstw = MSTWpdfs.size() - m_pdfInfo.n_mstw;

   return MSTWpdfs;
}


PDFTool::PDFSets PDFTool::initNNPDFPDFs( Tools::MConfig const &config,
                                         std::string const &PDFNames,
                                         std::string const &PDFNumbers
                                         ) {
   LHAPDF::setPDFPath( m_pdfPath );

   // Get PDF set names from config.
   vstring const PDFSetNames = Tools::splitString< std::string >(
                                  config.GetItem< std::string >( PDFNames ),
                                  true
                                  );

   // For NNPDF we need the distribution of alpha_s varied PDFSets.

   vuint const PDFSetNums( Tools::splitString< unsigned int >(
                              config.GetItem< std::string >( PDFNumbers )
                              ) );

   if( PDFSetNames.size() != PDFSetNums.size() ) {
      std::stringstream err;
      err << "[ERROR] (PDFTool): ";
      err << "Number of 'Nums' must be equal to the number of PDF set 'Names' ";
      err << "for NNPDF!";
      throw Tools::config_error( err.str() );
   }

   PDFSets NNPDFpdfs;
   // We know there will be ~50-100 PDFSets to store.
   NNPDFpdfs.reserve( 100 );

   // Loop over PDFSets and number:
   vstring::const_iterator PDFSet = PDFSetNames.begin();
   vuint::const_iterator NumPDFs  = PDFSetNums.begin();

   for( ; PDFSet != PDFSetNames.end(); ++PDFSet, ++NumPDFs ) {
      // Always take the first n PDF replicas, where n should be Gaussian
      // distributed around the central alpha_s value.
      // (This is random and reproducible: http://xkcd.com/221)
      for( unsigned int n = 0; n < *NumPDFs; ++n ) {
         NNPDFpdfs.push_back( LHAPDF::mkPDF( *PDFSet, n ) );
      }
   }

   m_pdfInfo.n_nnpdf = NNPDFpdfs.size();

   return NNPDFpdfs;
}


void PDFTool::setPDFWeights( pxl::Event &event ) const {
   pxl::EventView *GenEvtView =
      event.getObjectOwner().findObject< pxl::EventView >( "Gen" );

   pxl::EventView *RecEvtView =
      event.getObjectOwner().findObject< pxl::EventView >( "Rec" );

   // Remove the old weights.
   // TODO: Once the new reweighting (i.e. this here) is established, the
   // calculation of weights should be removed from the Skimmer!
   // Ugly hard-coded stuff (we had 41 PDF sets in the past).
   try{
      for( unsigned int i = 1; i < 41; ++i ) {
         std::stringstream sstream;
         sstream << "w" << i;
         std::string const str_i = sstream.str();
         GenEvtView->eraseUserRecord( str_i );
         RecEvtView->eraseUserRecord( str_i );
      }
   } catch( std::runtime_error ) {
   }
   
   
   float const Q  = GenEvtView->getUserRecord( "Q" );
   float const x1 = GenEvtView->getUserRecord( "x1" );
   float const x2 = GenEvtView->getUserRecord( "x2" );
   int const f1   = GenEvtView->getUserRecord( "f1" );
   int const f2   = GenEvtView->getUserRecord( "f2" );
   float const prodWeight = m_pdfProd->xfxQ( f1, x1, Q ) *
                            m_pdfProd->xfxQ( f2, x2, Q );

   // Get the weight for every loaded PDFSet, write it into the event!
   std::string const w( "w" );
   unsigned int count = 1;

   // Only three kinds of PDF sets at the moment, so treat them one by one.
   PDFSets::const_iterator PDFSet;
   for( PDFSet = m_pdfSetsCTEQ.begin(); PDFSet != m_pdfSetsCTEQ.end(); ++PDFSet ) {
      // Compute the PDF weight for this event.
      float const pdfWeight = (*PDFSet)->xfxQ( f1, x1, Q ) *
                              (*PDFSet)->xfxQ( f2, x2, Q );

      // Divide the new weight by the weight from the PDF the event was produced
      // with.
      float const weight = pdfWeight/prodWeight;

      std::stringstream stream;
      stream << "w" << count;
      event.setUserRecord( stream.str(), weight );
      ++count;
   }

   for( PDFSet = m_pdfSetsMSTW.begin(); PDFSet != m_pdfSetsMSTW.end(); ++PDFSet ) {
      // Compute the PDF weight for this event.
      float const pdfWeight = (*PDFSet)->xfxQ( f1, x1, Q ) *
                              (*PDFSet)->xfxQ( f2, x2, Q );

      // Divide the new weight by the weight from the PDF the event was produced
      // with.
      float const weight = pdfWeight/prodWeight;

      std::stringstream stream;
      stream << "w" << count;
      event.setUserRecord( stream.str(), weight );
      ++count;

   }

   for( PDFSet = m_pdfSetsNNPDF.begin(); PDFSet != m_pdfSetsNNPDF.end(); ++PDFSet ) {
      // Compute the PDF weight for this event.
      float const pdfWeight = (*PDFSet)->xfxQ( f1, x1, Q ) *
                              (*PDFSet)->xfxQ( f2, x2, Q );

      // Divide the new weight by the weight from the PDF the event was produced
      // with.
      float const weight = pdfWeight/prodWeight;

      std::stringstream stream;
      stream << "w" << count;
      event.setUserRecord( stream.str(), weight );
      ++count;
   }
}
