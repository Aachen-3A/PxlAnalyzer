#ifndef MUSIC_TOOLS
#define MUSIC_TOOLS

#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <typeinfo>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "TSystem.h" // for ExpandPathName

namespace Tools {
   //returns the abolute path to file given with a path relative to MUSIC_BASE
   //returns the given path if it is already absolute (starts with a /)
   std::string musicAbsPath( std::string relPath );

   //return everything you can << into an ostream as a string
   template< class T > std::string toString( T &input ) {
      std::stringstream out;
      out << input;
      return out.str();
   }

   //convert a string into anything you can >> from an istream (this is what boost::lexical_cast basically does)
   template< class T > T inline fromString( const std::string &valuestring ) {
      T value = T();

      if( !valuestring.empty() ) value = boost::lexical_cast< T >( valuestring );

      return value;
   }

   //if the return type is 'string', than you don't have to convert it to 'string' any more
   template<> std::string inline fromString< std::string >( const std::string &input ) { return input; }

   //splits the input string at each occurence of sep and puts the parts into the result vector
   //if ignore empty is not set, the output vector will contain default values for repeated separators
   template< class T > void splitString( std::vector< T > &result, const std::string &input, const std::string sep = ",", bool ignoreEmpty = false ) {
      result.clear();
      if( ignoreEmpty && input.empty() ) return;

      std::string::size_type pos = input.find( sep );
      std::string::size_type pos_old = 0;

      size_t sep_len = sep.size();

      while( std::string::npos != pos ){
         size_t len = pos - pos_old;

         if( len != 0 ) {
            std::string substring = input.substr( pos_old, len );
            substring = boost::trim_copy( substring );
            T temp = T();
            if( !substring.empty() ) temp = fromString< T >( substring );
            result.push_back( temp );
         } else if( !ignoreEmpty ) {
            result.push_back( T() );
         }

         pos_old = pos + sep_len;

         pos = input.find( sep, pos_old );
      }

      if( pos_old != input.size() ) {
         std::string substring = input.substr( pos_old );
         substring = boost::trim_copy( substring );
         T temp = T();
         if( !substring.empty() ) temp = fromString< T >( substring );
         result.push_back( temp );
      } else if( !ignoreEmpty ) {
         result.push_back( T() );
      }
   }

   // this is just a shortcut to the ROOT function
   inline std::string ExpandPath( const std::string &path ) { return ( std::string )gSystem->ExpandPathName( path.c_str() ); }
}

#endif /*MUSIC_TOOLS*/
