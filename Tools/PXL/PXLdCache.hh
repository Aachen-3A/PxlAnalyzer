#ifndef PXLdCache_hh
#define PXLdCache_hh

#include <string>

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
