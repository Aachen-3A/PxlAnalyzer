#ifndef PXLdCache_hh
#define PXLdCache_hh

#include <string>
#include <iostream>

#include "Tools/PXL/PXL.hh"
#include "Tools/dCache/idCacheStream.hh"

namespace pxl {
   class dCacheInputFile : public InputHandler {   
   public: 
      dCacheInputFile() :
         InputHandler(),
         stream(),
         reader( stream, ChunkReader::nonSeekable ) {}
      dCacheInputFile( const char *filename ) :
         InputHandler(),
         stream( filename ),
         reader( stream, ChunkReader::nonSeekable ) {}

      virtual void open(const string &filename){
         //reset and close everything that might be open
         close();
         stream.open( filename.c_str() );
         if( !stream.good() ) {
            if( stream.eof() ) std::cerr << "dCache file opened, but EOF! File empty? File: " << filename << std::endl;
            else throw dCache_error( "Failed to open file: "+filename );
         }
      }
      virtual void close(){
         stream.close();
         reset();
      }

      virtual ChunkReader &getChunkReader(){
         return reader;
      }

   private:
      idCacheStream stream;
      ChunkReader reader;
   };
}

#endif // PXLdCache_hh
