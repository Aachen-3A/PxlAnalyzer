#include <streambuf>
#include <stdexcept>
#include <string>

class dCache_error : public std::runtime_error {
public:
   explicit dCache_error( const std::string &what_arg ) : runtime_error( what_arg ) {}
};


class dCacheBuf : public std::streambuf {
public:
   dCacheBuf();
   ~dCacheBuf();
   
   //tell us, if there is a file already open
   bool is_open(){ return file > 0; }
   //open the file denoted by name
   //name must something that's understood by dCache (precisely by dc_open() )
   //returns the this pointer if successful, 0 otherwise
   dCacheBuf * open( const char *name );
   dCacheBuf * open( const std::string &name ) { return open( name.c_str() ); }
   //close file, if there is one
   //returns the this pointer, if the file was successfully closed, 0 otherwise
   //(closing a non-opened file is a failure, too
   dCacheBuf * close();

protected:
   //how many chars are left in the file
   std::streamsize showmanyc();
   //called when the buffer runs low, refills the buffer
   int underflow();

private:
   //size of the buffer
   std::streamsize bufsize;
   //size of push back buffer
   std::streamsize pbsize;
   //buffer to hold data
   char *buffer;
   //name of the last successfully opened file
   std::string filename;
   //file descriptor
   int file;
   //number of characters already read
   std::streamsize readsize;
};
