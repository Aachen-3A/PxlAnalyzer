#ifndef MUSIC_TOOLS
#define MUSIC_TOOLS

#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <typeinfo>

namespace Tools{
   //returns the abolute path to file given with a path relative to MUSIC_BASE
   //returns the given path if it is already absolute (starts with a /)
   std::string musicAbsPath( std::string relPath );

   //return everything you can << into an ostream as a string
   template< class T > std::string toString( T &input ){
      std::stringstream out;
      out << input;
      return out.str();
   }

   //convert a string into anything you can >> from an istream
   template< class T > T fromString( const std::string &input ){
      std::istringstream in( input );

      T res;

      in >> res;
      if( in.fail() ) throw std::invalid_argument( "Cannot convert '"+input+"' to type '"+typeid( T ).name()+"'" );

      in >> std::ws;
      if( !in.eof() ) throw std::invalid_argument( "Characters left after converting '"+input+"' to type '"+typeid( T ).name()+"'" );

      return res;
   }



   //splits the input string at each occurence of sep and puts the parts into the result vector
   //if ignore empty is not set, the output vector will contain default values for repeated separators
   template< class T > void splitString( std::vector< T > &result, const std::string &input, const std::string sep=",", bool ignoreEmpty = false ) {
      result.clear();

      if( ignoreEmpty && input.empty() ) return;

      std::string::size_type pos = input.find( sep );
      std::string::size_type pos_old = 0;

      size_t sep_len = sep.size();

      while( std::string::npos != pos ){
         size_t len = pos - pos_old;

         if( len != 0 ) {
            T temp = fromString< T >( input.substr( pos_old, len ) );
            result.push_back( temp );
         } else if( !ignoreEmpty ) {
            result.push_back( T() );
         }

         pos_old = pos + sep_len;

         pos = input.find( sep, pos_old );
      }

      if( pos_old != input.size() ) {
         T temp = fromString< T >( input.substr( pos_old ) );
         result.push_back( temp );
      } else if( !ignoreEmpty ) {
         result.push_back( T() );
      }
   }
}

#endif
