#ifndef GIFSTREAMCODEC_H
#define GIFSTREAMCODEC_H

#include <stdlib.h>
#include <string>
using namespace std;

#include "gif.h"


namespace IMAGELIB
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

enum Result
{
  ERROR = -1,
  DONE = 0,
  PARTLY = 1
};

namespace GIFLIB
{
   
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
   virtual int exec(gif_lcl_color_tbl_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_lzw_code_size_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_image_data_block_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_image_data_ter_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer) = 0;
   virtual int exec(gif_trailer_t &theGifStruct, string &theOutputBuffer)=0;
   GifHanderInterface *nextHandlerM; 
};

class IzwDecompressor
{
public:
   IzwDecompressor()
   {};
   enum{MAX_COLOR_STRING_LEN = 4097};

   void init(const unsigned char theLzwCopdeSize)
   {
      lzwCodeSizeM = theLzwCopdeSize;
      if (lzwCodeSizeM > 8)
         return;
      clear();
      outputOffsetM = 0;

      for (int i = 0; i < startTableSizeM; i++)
      {
         stringTableM[i].preIndexM = -1;
         stringTableM[i].colorM = i;
      }

      isEnd = 0;
      holdingLenM = 0;
      holdingBitsM = 0;
      
   };
   Result decompressor(const gif_image_data_block_t& theInputData, string &theOutputData)
   {
      if (lzwCodeSizeM > 8)
         return ERROR;

      unsigned char inputLen = theInputData.block_size;
      unsigned char inputIndex = 0;
      
      while ((inputIndex < inputLen) && !isEnd)
      {
         //read the theInputData to holdingBitsM, 
         //until it is large enough to get a code.
         readData(theInputData, inputIndex);

         //read curCodeM from holdingBitsM
         getACode();

         if (curCodeM == clearCodeM)
         {
            clear();
         }else if (curCodeM == eofCodeM)
         {
            isEnd = 1;
         }else if (-1 == oldCodeM) //at the beginning
         {
            assert(curCodeM < clearCodeM);
            theOutputData.append((char*)&stringTableM[curCodeM].colorM, 1);
         }else if (curCodeM < nextCodeIndexM)//if found in string table
         {
            //calc the string of stringTableM[curCodeM]
            unsigned char codeString[4097] = {0};
            int16_t codeStringLen = 0;
            calcColorStringOfCode(curCodeM, codeString, codeStringLen);
            
            theOutputData.append((char*)codeString, codeStringLen);
            
            //update the string table
            expandStringTable(codeString[0], oldCodeM);
         }else //if curcode is not in the string table
         {
            //calc the string(reversed) of stringTableM[oldCodeM]
            unsigned char codeString[4097] = {0};
            int16_t codeStringLen = 0;
            calcColorStringOfCode(oldCodeM, codeString, codeStringLen);

            expandStringTable(codeString[0], oldCodeM);

            theOutputData.append((char*)codeString, codeStringLen);
            theOutputData.append((char*)&codeString[0], 1);
         }
         oldCodeM = curCodeM;
      }
      return (isEnd)? DONE : PARTLY;

   }

private:
   /**
    * read the theInputData to holdingBitsM, until it is large enough to get a code.
    */
   void readData(const gif_image_data_block_t& theInputData, unsigned char &theInputIndex)
   {
      while(theInputIndex < theInputData.block_size 
         && holdingLenM < curCodeSizeM)
      {
         char curChar = theInputData.data_value[theInputIndex++];
         holdingBitsM = holdingBitsM + (curChar << holdingLenM);
         holdingLenM += 8;
      }
   }

   /**
    * read curCodeM from holdingBitsM
    */
   void getACode()
   {
      if (holdingLenM < curCodeSizeM)
         return;
      curCodeM = holdingBitsM & codeMaskM;
      holdingBitsM >>= curCodeSizeM;
      holdingLenM -= curCodeSizeM;
   }

   void clear()
   {
      curCodeSizeM = lzwCodeSizeM + 1;
      codeMaskM = (1 << curCodeSizeM) - 1;
      startTableSizeM = (1 << lzwCodeSizeM);
      clearCodeM = startTableSizeM;
      eofCodeM = clearCodeM + 1;
      nextCodeIndexM = eofCodeM + 1;
      oldCodeM = -1;
   }

   void calcColorStringOfCode(const int16_t theCode, unsigned char* theColorString, int16_t &theStringLen)
   {
      theStringLen = 0;
      int16_t root = theCode;
      while(1)
      {
         theColorString[theStringLen++] = stringTableM[root].colorM;
         if (-1 == stringTableM[root].preIndexM)
            break;
         root = stringTableM[root].preIndexM;
      }
      
      //reverse
      int first = 0;
      int last = theStringLen;
      while((first!=last)&&(first!=--last))
      {
         unsigned char tmp = theColorString[first];
         theColorString[first] = theColorString[last];
         theColorString[last] = tmp;
         first++;
      }
   }

   void expandStringTable(const unsigned char theColor, const int16_t thePreColorIndex)
   {
      stringTableM[nextCodeIndexM].colorM = theColor;
      stringTableM[nextCodeIndexM].preIndexM = thePreColorIndex;

      //update the current code size
      if (nextCodeIndexM == codeMaskM && codeMaskM != 4095)
      {
         curCodeSizeM++;
         codeMaskM = (1 << curCodeSizeM) - 1;
      }
      nextCodeIndexM++;
   }
   
   struct TableItem
   {
      int16_t preIndexM;
      unsigned char colorM;
   };
   
   unsigned char lzwCodeSizeM;
   unsigned char curCodeSizeM;
   int16_t codeMaskM;
   int16_t clearCodeM;
   int16_t eofCodeM;
   int16_t nextCodeIndexM;
   int16_t oldCodeM;
   int16_t curCodeM;
   int16_t startTableSizeM;
   int64_t outputOffsetM;
   TableItem stringTableM[4096];

   int isEnd;
   int16_t holdingBitsM;
   unsigned char holdingLenM;
   
   
   
};

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
      PARSING_IMAGE_DATA_BLOCK,
      PARSING_IMAGE_DATA_TERMINATOR,
      
      PARSING_PLAIN_TEXT_EXTENSION = 2200,

      /* Special-Purpose Block */
      PARSING_APPLICATION_EXTENSION = 3000,
      PARSING_COMMENT_EXTENSION,
         
      PARSING_TRAILER = 4000,  

      PARSING_SUB_BLOCK_TER_SIZE = 5000,
      PARSING_SUB_BLOCK_TER,
         
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
   virtual int exec(gif_lcl_color_tbl_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_lzw_code_size_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_image_data_block_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_image_data_ter_t &theGifStruct, string &theOutputBuffer);
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
   virtual int exec(gif_lcl_color_tbl_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_lzw_code_size_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_image_data_block_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_image_data_ter_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_trailer_t &theGifStruct, string &theOutputBuffer);
};
};//namespace GIFLIB
};//namespace IMAGELIB

#endif /* GIFSTREAMCODEC_H */
