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
      std::cout << "FATAL: MUSIC_BASE not set!" << std::endl;
      output = "";
   }
   return output;
}



void Tools::splitString( std::vector< std::string > &result, const std::string &input, const std::string sep, bool ignoreEmpty ){
   result.clear();

   if( ignoreEmpty && input.empty() ) return;

   std::string::size_type pos = input.find( sep );
   std::string::size_type pos_old = 0;

   size_t sep_len = sep.size();

   while( std::string::npos != pos ){
      size_t len = pos - pos_old;

      if( ! ignoreEmpty || len != 0 ){
         result.push_back( input.substr( pos_old, len ) );
      }
      pos_old = pos + sep_len;

      pos = input.find( sep, pos_old );
   }

   if( ! ignoreEmpty || pos_old != input.size() ){
      result.push_back( input.substr( pos_old ) );
   }
}
