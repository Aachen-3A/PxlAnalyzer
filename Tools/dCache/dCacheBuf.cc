#include "dCacheBuf.hh"

#include <iostream>
#include <algorithm>
#include <string.h>

#include "dcap.h"


using namespace std;

dCacheBuf::dCacheBuf() : streambuf() {
   //get one megabyte of buffer space
   bufsize = 1048576;
   //take one permille as push back buffer, but at least 10 characters
   pbsize = max( bufsize / 1000, 10 );
   buffer = new char[ bufsize+pbsize ];
   
   //we don't have a file open yet
   file = 0;
   readsize = 0;

   //set all pointer to the start of the buffer, behind the push back area
   setg( buffer+pbsize, buffer+pbsize, buffer+pbsize );
}


dCacheBuf::~dCacheBuf(){
   //close file, if still open
   close();
   //get rid of the buffer
   delete[] buffer;
}



dCacheBuf * dCacheBuf::open( const char *name ){
   //fail if we already got a file open
   if( is_open() ){
      return 0;
   }

   //open a file read only
   file = dc_open( name, O_RDONLY );
   //check if it worked
   if( file > 0 ){
      //looks good, so return this
      return this;
   } else {
      //clean up if we failed
      close();
      return 0;
   }
}


dCacheBuf * dCacheBuf::close(){
   //clean up
   readsize = 0;
   //if we have a file open, close it
   if( is_open() ){
      //try to close it
      if( dc_close( file ) == 0 ){
         //closing successful, clean up and return a success
         file = 0;
         return this;
      } else {
         //closing failed, so clean up and return a failure
         file = 0;
         return 0;
      }
   } else {
      //nothing to close, so reset file in case of an error, then return a failure
      file = 0;
      return 0;
   }
}



int dCacheBuf::underflow(){
   if( gptr() < egptr() ){
      cout << "dCacheBuf: Underflow called, but the (get) buffer pointer is still inside of the buffer. This shouldn't happen" << endl;
      return traits_type::to_int_type(*gptr());
   }

   //compute the number of characters for the push back area
   //it can't be larger than the push back area
   //and also not larger than the number of characters we've already read
   streamsize numPutback = min( pbsize, gptr() - eback());

   //move the numPutback characters before the current pointer position into the push back area
   memmove( buffer+pbsize-numPutback, gptr()-numPutback, numPutback );

   //try to read data, at max bufSize characters
   //write into the buffer behind the push back area
   streamsize num = dc_read( file, buffer+pbsize, bufsize );
   //in case we got nothing or something went wront: return EOF
   if( num <= 0 ){
      return traits_type::eof();
   }

   //reading worked, reset the pointers
   setg( buffer+pbsize-numPutback, //beginning of the push back area
         buffer+pbsize, //end of the push back area
         buffer+pbsize+num ); //end of the push back area plus the number of character we've read

   //increase the number of characters we've already read
   readsize += num;

   //return the next character
   return traits_type::to_int_type(*gptr());
}


streamsize dCacheBuf::showmanyc(){
   //jump to the end of the file to get the current file size
   streamsize filesize = dc_lseek( file, 0, SEEK_END );
   //jumo back to where we came from
   dc_lseek( file, readsize, SEEK_SET );
   //return remaining characters
   return filesize-readsize;
}
