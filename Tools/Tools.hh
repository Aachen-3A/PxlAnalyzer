#ifndef MUSIC_TOOLS
#define MUSIC_TOOLS

#include <string>
#include <cstdlib>

namespace Tools{
   //returns the abolute path to file given with a path relative to MUSIC_BASE
   //returns the given path if it is already absolute (starts with a /)
   std::string musicAbsPath( std::string relPath );
}

#endif
