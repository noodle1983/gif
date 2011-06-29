#include "GifDataStreamDecoder.h"
#include <iostream>
#include <string.h>
#include <assert.h>
using namespace std;
using namespace IMAGELIB::GIFLIB;

IMAGELIB::Result decodeStruct(
	char* theImageStruct, const int theStructLen, 
	string &theBuffer, const string &theInputBuffer,
	uint64_t& theDecodeIndex)
{
   int bufferLen = theBuffer.length();
	int inputLen = theInputBuffer.length() - theDecodeIndex;
   if (inputLen + bufferLen < theStructLen)
   {
      theBuffer.append(theInputBuffer.c_str() + theDecodeIndex, inputLen);
      bufferLen += inputLen; 
		theDecodeIndex += inputLen;
      return IMAGELIB::PARTLY;
   }else
   {
      memcpy(theImageStruct, theBuffer.c_str(), bufferLen);
	   memcpy(theImageStruct + bufferLen, theInputBuffer.c_str() + theDecodeIndex, theStructLen - bufferLen);
	   theBuffer.clear();
		theDecodeIndex += theStructLen - bufferLen;
		return IMAGELIB::DONE;
   }
   return IMAGELIB::ERROR;
}

IMAGELIB::Result 
GifDataStreamDecoder::decode(const string &theInputBuffer, string &theOutputBuffer)
{
	uint64_t decodeIndex = 0;
	IMAGELIB::Result result = IMAGELIB::DONE;
	while(IMAGELIB::DONE == result && PARSING_DONE != stateM)
	{
		//if (stateM != PARSING_DATA_SUB_BLOCK && stateM != PARSING_DATA_SUB_BLOCK_TERMINATOR)
		//	cout << "state:" << stateM << endl;
	   switch(stateM)
		{
	      case PARSING_HEADER:
            result = stateParsingHeader(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
	      
			case PARSING_LOGICAL_SCREEN_DESCRIPTOR:
				result = stateParsingLogicalScreenDescriptor(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

         case PARSING_GLOBAL_COLOR_TABLE:
            result = stateParsingGlobalColorTable(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

         case CHECK_DATA_INTRODUCOR:
            result = stateCheckDataIntroducor(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

         case CHECK_DATA_EXTENSION_LABEL:
            result = stateCheckDataExtensionLabel(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
         
         /* Graphic Block */
         case PARSING_GRAPHIC_CONTROL_EXTENSION:
            result = stateParsingGraphicControlExtension(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
         
			case PARSING_IMAGE_DESCRIPTOR:
            result = stateParsingImageDescriptor(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

         case PARSING_LOCAL_COLOR_TABLE:
            result = stateParsingLocalColorTable(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
            
         case PARSING_IMAGE_DATA_LZW_CODE_SIZE:
            result = stateParsingImageDataLzwCodeSize(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

         case PARSING_IMAGE_DATA_SUB_BLOCK:
            result = stateParsingImageDataSubBlock(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
            
         case PARSING_IMAGE_DATA_SUB_BLOCK_TERMINATOR:
            result = stateParsingImageDataSubTerminator(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

         case PARSING_DATA_SUB_BLOCK:
            result = stateParsingDataSubBlock(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
            
         case PARSING_DATA_SUB_BLOCK_TERMINATOR:
            result = stateParsingDataSubTerminator(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

			/* Special-Purpose Block */
			case PARSING_APPLICATION_EXTENSION:
            result = stateParsingApplicationExtension(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
         
         case PARSING_COMMENT_EXTENSION:
            result = stateParsingCommentExtension(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
			
         case PARSING_TRAILER:
            result = stateParsingTrailer(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
            
            
         //not been tested.
         case PARSING_PLAIN_TEXT_EXTENSION:
            result = stateParsingPlainTextExtension(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
            
			case PARSING_ERROR:
			default:
			{
				stateM = PARSING_ERROR;
				return IMAGELIB::ERROR;
			}

		}// switch
	}
	if (IMAGELIB::ERROR == result)
	{
		stateM = PARSING_ERROR;
	}
   return result;

}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingHeader(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_header_t gifStruct;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_header_t), bufferM, theInputBuffer, theDecodeIndex);
	if (IMAGELIB::DONE != result)
      return result;
   
	stateM = PARSING_LOGICAL_SCREEN_DESCRIPTOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
		stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
   return IMAGELIB::DONE;
}


IMAGELIB::Result 
GifDataStreamDecoder::stateParsingLogicalScreenDescriptor(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_lgc_scr_desc_t gifStruct;
	IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_lgc_scr_desc_t), bufferM, theInputBuffer, theDecodeIndex);
	if (IMAGELIB::DONE != result)
      return result;

   if (gifStruct.global_flag.global_color_table_flag)
   {
		stateM = PARSING_GLOBAL_COLOR_TABLE;
      globalTableSizeM = 1 << (gifStruct.global_flag.global_color_tbl_sz + 1);
   }else{
      stateM = CHECK_DATA_INTRODUCOR;
      globalTableSizeM = 0;
   }
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
		stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
   return IMAGELIB::DONE;

}


IMAGELIB::Result 
GifDataStreamDecoder::stateParsingGlobalColorTable(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_glb_color_tbl_t gifStruct;
   gifStruct.size = globalTableSizeM;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_color_triplet_t) * globalTableSizeM, bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;
   
   stateM = CHECK_DATA_INTRODUCOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
   return IMAGELIB::DONE;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateCheckDataIntroducor(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   assert(theInputBuffer.length() >= theDecodeIndex);
   IMAGELIB::Result result;
   if (theInputBuffer.length() == theDecodeIndex){
      result = IMAGELIB::PARTLY;
      return result;
   }

   
  const unsigned char introducer = theInputBuffer[theDecodeIndex];
  if (0x21 == introducer)
   {
      stateM = CHECK_DATA_EXTENSION_LABEL;
      bufferM.append((char *)&introducer, 1);
      theDecodeIndex++;
      result = IMAGELIB::DONE;
      return result;
   }
   if (0x2C == introducer)
   {
      stateM = PARSING_IMAGE_DESCRIPTOR;
      result = IMAGELIB::DONE;
      return result;
   }
   if (0x3B == introducer)
   {
      stateM = PARSING_TRAILER;
      result = IMAGELIB::DONE;
      return result;
   }
   //cout << "introducer:" << (((int)introducer) & 0xFF) << endl;
   //stateM = PARSING_ERROR;
   //end the stream
   stateM = PARSING_TRAILER;
   result = IMAGELIB::DONE;
   return result;

}

IMAGELIB::Result 
GifDataStreamDecoder::stateCheckDataExtensionLabel(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   assert(theInputBuffer.length() >= theDecodeIndex);
   IMAGELIB::Result result;
   if (theInputBuffer.length() == theDecodeIndex){
      result = IMAGELIB::PARTLY;
      return result;
   }

   /* Graphic Block */
   const unsigned char label = theInputBuffer[theDecodeIndex];
   if (0xF9 == label)
   {
      stateM = PARSING_GRAPHIC_CONTROL_EXTENSION;
      result = IMAGELIB::DONE;
      return result;
   }
   if (0x01 == label)
   {
      stateM = PARSING_PLAIN_TEXT_EXTENSION;
      result = IMAGELIB::DONE;
      return result;
   }
   if (0xFF == label)
   {
      stateM = PARSING_APPLICATION_EXTENSION;
      result = IMAGELIB::DONE;
      return result;
   }
   if (0xFE == label)
   {
      stateM = PARSING_COMMENT_EXTENSION;
      result = IMAGELIB::DONE;
      return result;
   }
      stateM = PARSING_ERROR;
   return IMAGELIB::ERROR;

}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingGraphicControlExtension(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_graphic_ctrl_ext_t gifStruct;
   IMAGELIB::Result result;
   result = decodeStruct((char *) &gifStruct, sizeof(gif_graphic_ctrl_ext_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
   return result;
   
   stateM = CHECK_DATA_INTRODUCOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }  
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingImageDescriptor(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_image_desc_t gifStruct;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_image_desc_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;

   if (gifStruct.local_flag.local_color_tbl_flag)
   {
      stateM = PARSING_LOCAL_COLOR_TABLE;
      localTableSizeM = 1 << (gifStruct.local_flag.local_color_tbl_sz + 1);
   }else
   {
      stateM = PARSING_IMAGE_DATA_LZW_CODE_SIZE;
      localTableSizeM = 0;
   }
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
       stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingLocalColorTable(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_lcl_color_tbl_t gifStruct;
   gifStruct.size = localTableSizeM;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_color_triplet_t) * localTableSizeM, bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;
   
   stateM = PARSING_IMAGE_DATA_LZW_CODE_SIZE;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
   return IMAGELIB::DONE;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingImageDataLzwCodeSize(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_lzw_code_size_t gifStruct;
   IMAGELIB::Result  result = decodeStruct((char *) &gifStruct, sizeof(gif_lzw_code_size_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;
   
   stateM = PARSING_IMAGE_DATA_SUB_BLOCK_TERMINATOR;
   gifStruct.lzw_code_size = (gifStruct.lzw_code_size > 8)? 8 : gifStruct.lzw_code_size;
   lzwDecompressor.init(gifStruct.lzw_code_size);
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
       stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }  
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingImageDataSubBlock(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   assert (bufferM.length() > 0);
   gif_image_data_block_t gifStruct;
   unsigned char size = bufferM[0];
   IMAGELIB::Result  result = decodeStruct((char *) &gifStruct, size + 1, bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;
   
   stateM = PARSING_IMAGE_DATA_SUB_BLOCK_TERMINATOR;
   
   string gifPlainData;
   gifPlainData.reserve(gifStruct.block_size * 2);
   
   lzwDecompressor.decompress(gifStruct, gifPlainData);
   if (0 != handlerM->handle(gifPlainData, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }  
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingImageDataSubTerminator(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_image_data_ter_t gifStruct;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_image_data_ter_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;

   if (0 != gifStruct.terminator){
      bufferM.append((char*)&gifStruct, sizeof(gif_image_data_ter_t));
      stateM = PARSING_IMAGE_DATA_SUB_BLOCK;
      return result;
   }
   stateM = CHECK_DATA_INTRODUCOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
       stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      return result;
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingDataSubBlock(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   assert (bufferM.length() > 0);
   gif_data_sub_block_t gifStruct;
   unsigned char size = bufferM[0];
   IMAGELIB::Result  result = decodeStruct((char *) &gifStruct, size + 1, bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;
   
   stateM = PARSING_DATA_SUB_BLOCK_TERMINATOR;
   
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }  
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingDataSubTerminator(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_data_sub_block_ter_t gifStruct;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_image_data_ter_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;

   if (0 != gifStruct.terminator){
      bufferM.append((char*)&gifStruct, sizeof(gif_image_data_ter_t));
      stateM = PARSING_DATA_SUB_BLOCK;
      return result;
   }
   stateM = CHECK_DATA_INTRODUCOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
       stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      return result;
   return result;
}

/* Special-Purpose Block */
IMAGELIB::Result 
GifDataStreamDecoder::stateParsingApplicationExtension(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_appl_ext_t gifStruct;
   IMAGELIB::Result  result = decodeStruct((char *) &gifStruct, sizeof(gif_appl_ext_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;

   stateM = PARSING_DATA_SUB_BLOCK_TERMINATOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingCommentExtension(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_comment_ext_t gifStruct;
   IMAGELIB::Result  result = decodeStruct((char *) &gifStruct, sizeof(gif_comment_ext_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;

   stateM = PARSING_DATA_SUB_BLOCK_TERMINATOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingTrailer(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_trailer_t gifStruct;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_trailer_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;

   gifStruct.trailer = 0x3B;
   stateM = PARSING_DONE;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingPlainTextExtension(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_plain_text_ext_t gifStruct;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_plain_text_ext_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;
   
   stateM = PARSING_DATA_SUB_BLOCK_TERMINATOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      
   return result;
}






