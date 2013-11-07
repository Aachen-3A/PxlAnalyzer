#ifndef PDFTOOL
#define PDFTOOL

#include <string>
#include <vector>

#include "LHAPDF/LHAPDF.h"

#include "Tools/PXL/PXL.hh"
#include "Main/PDFInfo.hh"

namespace Tools {
   class MConfig;
}

namespace pdf {

class PDFTool {
   public:
      typedef std::vector< std::string > vstring;
      typedef std::vector< unsigned int > vuint;
      typedef std::vector< LHAPDF::PDF* > PDFSets;
      PDFTool( Tools::MConfig const &config, unsigned int const debug = 1 );
      ~PDFTool() {}

   // Delete old and write new PDF weights into the pxl::Event.
   void setPDFWeights( pxl::Event &event ) const;
   pdf::PDFInfo const &getPDFInfo() const { return m_pdfInfo; }

   private:
      PDFTool::PDFSets initCTEQPDFs( Tools::MConfig const &config,
                                     std::string const &PDFNames = "PDF.CTEQ.Names"
                                     );

      PDFTool::PDFSets initMSTWPDFs( Tools::MConfig const &config,
                                     std::string const &PDFNames = "PDF.MSTW.Names"
                                     );

      PDFTool::PDFSets initNNPDFPDFs( Tools::MConfig const &config,
                                      std::string const &PDFNames = "PDF.NNPDF.Names",
                                      std::string const &PDFNumbers = "PDF.NNPDF.Nums"
                                      );

      // PDFInfo stores information about the read PDF sets.
      PDFInfo m_pdfInfo;

      bool const m_debug;
      std::string const m_pdfPath;
      LHAPDF::PDF const *m_pdfProd;
      PDFSets const m_pdfSetsCTEQ;
      PDFSets const m_pdfSetsMSTW;
      PDFSets const m_pdfSetsNNPDF;
};

}

#endif /*PDFTOOL*/
