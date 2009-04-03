#include "Tools.hh"

#include <string>
#include <cstdlib>
#include <iostream>

std::string Tools::musicAbsPath( std::string relPath ){
   if( relPath.substr(0,1) == "/" ) return relPath;
   std::string output;
   char *pPath = std::getenv( "MUSIC_BASE" );
   if( pPath != NULL ){
      output = std::string( pPath ) + "/" + relPath;
   } else {
      std::cout << "FATAL: MUSIC_PATH not set!" << std::endl;
      output = "";
   }
   return output;
}
