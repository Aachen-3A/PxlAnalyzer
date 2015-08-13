#include "Tools.hh"

#include <string>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <random>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

std::string Tools::musicAbsPath( std::string relPath ){
   if( relPath.substr(0,1) == "/" ) return relPath;
   std::string output;
   char *pPath = std::getenv( "MUSIC_BASE" );
   if( pPath != NULL ){
      output = std::string( pPath ) + "/" + relPath;
   } else {
      std::cout << "FATAL: MUSIC_BASE not set!" << std::endl;
      output = "";
   }
   return output;
}


std::string Tools::removeComment( std::string line, char const commentChar ) {
   if( line.empty() ) return line;

   std::string::size_type pos = line.find_first_of( commentChar );
   if( pos != std::string::npos ) line.erase( pos );

   boost::trim( line );

   return line;
}

std::string Tools::random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

std::vector< std::string > Tools::getParticleTypeAbbreviations() {
    std::vector< std::string > partList;
    partList.push_back("Ele");
    partList.push_back("Muon");
    partList.push_back("Tau");
    partList.push_back("Gamma");
    partList.push_back("Jet");
    partList.push_back("MET");
    return partList;
}
