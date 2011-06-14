#ifndef GIFSTREAMCODEC_H
#define GIFSTREAMCODEC_H

#include <stdlib.h>
#include <string>
using namespace std;

#include "gif.h"
/**
 *The Grammar.
 *
 *<GIF Data Stream> ::=     Header <Logical Screen> <Data>* Trailer
 *
 *<Logical Screen> ::=      Logical Screen Descriptor [Global Color Table]
 *
 *<Data> ::=                <Graphic Block>  |
 *                          <Special-Purpose Block>
 *
 *<Graphic Block> ::=       [Graphic Control Extension] <Graphic-Rendering Block>
 *
 *<Graphic-Rendering Block> ::=  <Table-Based Image>  |
 *                               Plain Text Extension
 *
 *<Table-Based Image> ::=   Image Descriptor [Local Color Table] Image Data
 *
 *<Special-Purpose Block> ::=    Application Extension  |
 *                               Comment Extension
 *
 */

enum Result
{
  ERROR = -1,
  DONE = 0,
  PARTLY = 1,
  NEXT = 2
};

class GifHanderInterface
{
public:
   GifHanderInterface()
        :nextHandlerM(NULL){};
   GifHanderInterface(GifHanderInterface *theNextHandler)
        :nextHandlerM(theNextHandler){};
   virtual ~GifHanderInterface()
        {if (nextHandlerM) delete nextHandlerM;};
   virtual int exec(gif_header_t &theHeader, string &theOutputBuffer)=0;
    
protected:
   GifHanderInterface *nextHandlerM; 
};

class GifLogicalScreenDecoder
{
public:
   enum
   {
      PARSING_LOGICAL_SCREEN_DESC = 0,
      PARSING_GLOBAL_COLOR_TABLE = 1,
      PARSING_DONE
   };
   
   GifLogicalScreenDecoder(GifHanderInterface* theHandler, string &theBuffer)
      :handlerM(theHandler), 
      bufferM(theBuffer),
      stateM(PARSING_LOGICAL_SCREEN_DESC){};
   ~GifLogicalScreenDecoder()
   {};
   
   Result decode(const string &theInputBuffer, int& theDecodeIndex, string &theOutputBuffer);
   Result decodeLogicalScreenDesc(const string &theInputBuffer, int& theDecodeIndex, string &theOutputBuffer);
   
private:
   string &bufferM;
   
   int stateM;
   GifHanderInterface *handlerM; 
};


class GifDataStreamDecoder
{
public:
   enum
   {
      PARSING_HEADER = 0,
      PARSING_LOGICAL_SCREEN = 1,
      PARSING_DATA = 2,
      PARSING_TRAILER = 3,  
      PARSING_DONE
   };
   
   GifDataStreamDecoder(GifHanderInterface* theHandler)
    :handlerM(theHandler), 
    stateM(PARSING_HEADER),
    logicalScreenDecoderM(theHandler, bufferM){};
   ~GifDataStreamDecoder()
   {if (handlerM) delete handlerM;};
   
   Result decode(const string &theInputBuffer, string &theOutputBuffer);
   Result decodeHeader(const string &theInputBuffer, int& theDecodeIndex, string &theOutputBuffer);

private:
   string bufferM;
   
   int stateM;
   GifHanderInterface *handlerM; 

   GifLogicalScreenDecoder logicalScreenDecoderM;
};



class GifEncoder: public GifHanderInterface
{
public:
   GifEncoder(){};
   GifEncoder(GifHanderInterface *theNextHandler)
      :GifHanderInterface(theNextHandler){};
    
   virtual int exec(gif_header_t &theHeader, string &theOutputBuffer);   
};

#endif /* GIFSTREAMCODEC_H */
