#include "Tools.hh"

#include <string>
#include <cstdlib>
#include <iostream>

#include <boost/algorithm/string.hpp>

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