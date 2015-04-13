
#include "Main/EventSelector.hh"
#include "Main/PDFTool.hh"

#include <exception>
#include <sstream>
#include <stdexcept>

#include "Tools/Tools.hh"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "boost/program_options.hpp"
namespace po = boost::program_options;
using std::string;
class MUSiCComposer : public AnalysisComposer{

public:
    AnalysisComposer();
    // Destructor
    ~AnalysisComposer() {;};
    //~ po::options_description addCmdArguments( argstream &as );
    po::options_description getCmdArguments( );
    pxl::AnalysisFork addForkObjects ( const Tools::MConfig &config,
                                       string outputDirectory,
                                       pdf::PDFInfo const &pdfInfo,
                                       EventSelector &selector,
                                       const bool debug);
    void endAnalysis();

private:
    string m_analysisName;
    bool runOnData;
};
