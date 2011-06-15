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
 * Which is 
 * *<GIF Data Stream> ::=     Header 
 *       Logical Screen Descriptor [Global Color Table]
 *          {[Graphic Control Extension] {{Image Descriptor [Local Color Table] Image Data}  |Plain Text Extension}
 *          | Application Extension  
 *          | Comment Extension}}* 
 *       Trailer
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
   template<typename GifStruct>
   int handle(GifStruct &theStruct, string &theOutputBuffer)
   {
      if (0 != exec(theStruct, theOutputBuffer))
         return -1;
      if (nextHandlerM)
         nextHandlerM->handle(theStruct, theOutputBuffer);
      return 0;
   }

   
protected:
   virtual int exec(gif_header_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_glb_color_tbl_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_graphic_ctrl_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_image_desc_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer) = 0;
   virtual int exec(gif_trailer_t &theGifStruct, string &theOutputBuffer)=0;
   GifHanderInterface *nextHandlerM; 
};

class GifDataStreamDecoder
{
public:
   enum
   {
      PARSING_HEADER = 0,
      PARSING_LOGICAL_SCREEN_DESCRIPTOR,
      PARSING_GLOBAL_COLOR_TABLE,

      CHECK_DATA_INTRODUCOR = 100,
      CHECK_DATA_EXTENSION_LABEL,
      
         /* Graphic Block */
         PARSING_GRAPHIC_CONTROL_EXTENSION = 200,
         PARSING_IMAGE_DESCRIPTOR,
         PARSING_LOCAL_COLOR_TABLE,
         PARSING_IMAGE_DATA,
         PARSING_PLAIN_TEXT_EXTENSION,

         /* Special-Purpose Block */
         PARSING_APPLICATION_EXTENSION = 300,
         PARSING_COMMENT_EXTENSION,
         
         PARSING_TRAILER = 400,  

         PARSING_SUB_BLOCK_TER_SIZE = 500,
         PARSING_SUB_BLOCK_TER,
         
      PARSING_DONE = 600,
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
   string bufferM;
   
   int stateM;
   GifHanderInterface *handlerM; 

   size_t globalTableSizeM;
   size_t localTableSizeM;
};



class GifEncoder: public GifHanderInterface
{
public:
   GifEncoder(){};
   GifEncoder(GifHanderInterface *theNextHandler)
      :GifHanderInterface(theNextHandler){};
   
protected:   
   virtual int exec(gif_header_t &theGifStruct, string &theOutputBuffer);  
   virtual int exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_glb_color_tbl_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_graphic_ctrl_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_image_desc_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_trailer_t &theGifStruct, string &theOutputBuffer);
};

class GifDumper: public GifHanderInterface
{
public:
   GifDumper(){};
   GifDumper(GifHanderInterface *theNextHandler)
      :GifHanderInterface(theNextHandler){};
   
protected:      
   virtual int exec(gif_header_t &theGifStruct, string &theOutputBuffer);  
   virtual int exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_glb_color_tbl_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_graphic_ctrl_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_image_desc_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_trailer_t &theGifStruct, string &theOutputBuffer);
};

#endif /* GIFSTREAMCODEC_H */
