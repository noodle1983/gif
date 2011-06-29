#ifndef GIFSTREAMCODEC_H
#define GIFSTREAMCODEC_H

#include <stdlib.h>
#include <string>
using namespace std;

#include "gif.h"
#include "GifHanderInterface.h"
#include "LzwDecompressor.h"
#include "ImageLibStruct.h"
#include "LzwCompressor.h"
#include "GifEncoder.h"
#include "GifResizer.h"
#include "GifDumper.h"

namespace IMAGELIB
{
namespace GIFLIB
{
   
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
 * Which is 
 * *<GIF Data Stream> ::=     Header 
 *       Logical Screen Descriptor [Global Color Table]
 *          {[Graphic Control Extension] {{Image Descriptor [Local Color Table] Image Data}  |Plain Text Extension}
 *          | Application Extension  
 *          | Comment Extension}}* 
 *       Trailer
 */
class GifDataStreamDecoder
{
public:
   enum
   {
      PARSING_HEADER = 0,
      PARSING_LOGICAL_SCREEN_DESCRIPTOR,
      PARSING_GLOBAL_COLOR_TABLE,

      CHECK_DATA_INTRODUCOR = 1000,
      CHECK_DATA_EXTENSION_LABEL,
      
      /* Graphic Block */
      PARSING_GRAPHIC_CONTROL_EXTENSION = 2000,

      PARSING_IMAGE_DESCRIPTOR = 2100,
      PARSING_LOCAL_COLOR_TABLE,
      
      PARSING_IMAGE_DATA_LZW_CODE_SIZE,
      PARSING_IMAGE_DATA_SUB_BLOCK,
      PARSING_IMAGE_DATA_SUB_BLOCK_TERMINATOR,
      
      PARSING_PLAIN_TEXT_EXTENSION = 2200,

      /* Special-Purpose Block */
      PARSING_APPLICATION_EXTENSION = 3000,
      PARSING_COMMENT_EXTENSION,
         
      PARSING_TRAILER = 4000,  

      PARSING_DATA_SUB_BLOCK = 5000,
      PARSING_DATA_SUB_BLOCK_TERMINATOR,
      
      PARSING_DONE = 6000,
      PARSING_ERROR
   };
   
   GifDataStreamDecoder(GifHanderInterface* theHandler)
    :handlerM(theHandler), 
    stateM(PARSING_HEADER),
    globalTableSizeM(0),
    localTableSizeM(0){};
   ~GifDataStreamDecoder()
   {if (handlerM) delete handlerM;};
   
   Result decode(const string &theInputBuffer, string &theOutputBuffer);

private:
   IMAGELIB::Result 
   stateParsingHeader(
      const string &theInputBuffer
      , uint64_t &theDecodeIndex
      , string &theOutputBuffer);
   
   IMAGELIB::Result 
   stateParsingLogicalScreenDescriptor(
      const string &theInputBuffer
      , uint64_t &theDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingGlobalColorTable(
      const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateCheckDataIntroducor(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);
      
   IMAGELIB::Result 
   stateCheckDataExtensionLabel(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   /* Graphic Block */
   IMAGELIB::Result 
   stateParsingGraphicControlExtension(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);
      
   IMAGELIB::Result 
   stateParsingImageDescriptor(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingLocalColorTable(
      const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingImageDataLzwCodeSize(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingImageDataSubBlock(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingImageDataSubTerminator(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);
   
   IMAGELIB::Result 
   stateParsingDataSubBlock(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingDataSubTerminator(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingApplicationExtension(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingCommentExtension(
      const string &theInputBuffer
      , uint64_t &theDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingTrailer(
   const string &theInputBuffer
      , uint64_t &thetheDecodeIndex
      , string &theOutputBuffer);

   IMAGELIB::Result 
   stateParsingPlainTextExtension(
   const string &theInputBuffer
      , uint64_t &theDecodeIndex
      , string &theOutputBuffer);

   
   string bufferM;
   
   int stateM;
   GifHanderInterface *handlerM; 
   LzwDecompressor lzwDecompressor;

   size_t globalTableSizeM;
   size_t localTableSizeM;
};

};//namespace GIFLIB
};//namespace IMAGELIB

#endif /* GIFSTREAMCODEC_H */
