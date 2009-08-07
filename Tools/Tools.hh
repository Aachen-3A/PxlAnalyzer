#ifndef MUSIC_TOOLS
#define MUSIC_TOOLS

#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

namespace Tools{
   //returns the abolute path to file given with a path relative to MUSIC_BASE
   //returns the given path if it is already absolute (starts with a /)
   std::string musicAbsPath( std::string relPath );

   //splits the input string at each occurence of sep and puts the parts into the result vector
   //if ignore empty is set, the output vector will contain no empty strings
   void splitString( std::vector< std::string > &result, const std::string &input, const std::string sep=",", bool ignoreEmpty = false );

   //return everything you can << into an ostream as a string
   template< class T > std::string toString( T &input ){
      std::stringstream out;
      out << input;
      return out.str();
   }
}

#endif
