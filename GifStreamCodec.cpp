#include "GifStreamCodec.h"
#include <iostream>
#include <string.h>
#include <assert.h>
using namespace std;
using namespace IMAGELIB::GIFLIB;

IMAGELIB::Result IzwDecompressor::init(const unsigned char theLzwCopdeSize)
{
   lzwCodeSizeM = theLzwCopdeSize;
   if (lzwCodeSizeM > 8)
      return IMAGELIB::ERROR;
   clear();

   for (int i = 0; i < startTableSizeM; i++)
   {
      stringTableM[i].preIndexM = -1;
      stringTableM[i].colorM = i;
   }

   isEndM = 0;
   holdingLenM = 0;
   holdingBitsM = 0;
   return IMAGELIB::DONE;
}

IMAGELIB::Result IzwDecompressor::decompress(const gif_image_data_block_t& theInputData, string &theOutputData)
{
   if (lzwCodeSizeM > 8)
      return IMAGELIB::ERROR;

   unsigned char inputLen = theInputData.block_size;
   unsigned char inputIndex = 0;
   
   while (!isEndM
		&&(inputIndex < inputLen || holdingLenM >= curCodeSizeM))
   {
      //read the theInputData to holdingBitsM, 
      //until it is large enough to get a code.
      readData(theInputData, inputIndex);

      //read curCodeM from holdingBitsM     
	   if (holdingLenM < curCodeSizeM)
	      return IMAGELIB::PARTLY;
      getACode();

      if (curCodeM == clearCodeM)
      {
         clear();
			continue;
      }else if (curCodeM == eofCodeM)
      {
         isEndM = 1;
      }else if (-1 == oldCodeM) //at the beginning
      {
         theOutputData.append((char*)&stringTableM[curCodeM].colorM, 1);
      }else if (curCodeM < nextCodeIndexM)//if found in string table
      {
         unsigned char codeString[4097] = {0};
         int16_t codeStringLen = 0;
         calcColorStringOfCode(curCodeM, codeString, codeStringLen);
         
         theOutputData.append((char*)codeString, codeStringLen);
         
         //update the string table
         expandStringTable(codeString[0], oldCodeM);
			
      }else //if curcode is not in the string table
      {
         unsigned char codeString[4097] = {0};
         int16_t codeStringLen = 0;
         calcColorStringOfCode(oldCodeM, codeString, codeStringLen);

         expandStringTable(codeString[0], oldCodeM);

         theOutputData.append((char*)codeString, codeStringLen);
         theOutputData.append((char*)&codeString[0], 1);
      }
      oldCodeM = curCodeM;
   }
   return (isEndM)? IMAGELIB::DONE : IMAGELIB::PARTLY;

}


/**
 * read the theInputData to holdingBitsM, until it is large enough to get a code.
 */
void IzwDecompressor::readData(const gif_image_data_block_t& theInputData, unsigned char &theInputIndex)
{
   while(theInputIndex < theInputData.block_size 
      && holdingLenM < curCodeSizeM)
   {
      unsigned char curChar = theInputData.data_value[theInputIndex++];
      holdingBitsM = holdingBitsM + (curChar << holdingLenM);
      holdingLenM += 8;
   }
}

/**
 * read curCodeM from holdingBitsM
 */
void IzwDecompressor::getACode()
{
   curCodeM = holdingBitsM & codeMaskM;
   holdingBitsM >>= curCodeSizeM;
   holdingLenM -= curCodeSizeM;
}

void IzwDecompressor::clear()
{
   curCodeSizeM = lzwCodeSizeM + 1;
   codeMaskM = (1 << curCodeSizeM) - 1;
   startTableSizeM = (1 << lzwCodeSizeM);
   clearCodeM = startTableSizeM;
   eofCodeM = clearCodeM + 1;
   nextCodeIndexM = eofCodeM + 1;
   oldCodeM = -1;
}

void IzwDecompressor::calcColorStringOfCode(const int16_t theCode, unsigned char* theColorString, int16_t &theStringLen)
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

void IzwDecompressor::expandStringTable(const unsigned char theColor, const int16_t thePreColorIndex)
{
   stringTableM[nextCodeIndexM].colorM = theColor;
   stringTableM[nextCodeIndexM].preIndexM = thePreColorIndex;

   //update the current code size
   if (nextCodeIndexM > codeMaskM -1 && codeMaskM != 4095)
   {
      curCodeSizeM++;
      codeMaskM = (1 << curCodeSizeM) - 1;
   }
   nextCodeIndexM++;
}

IMAGELIB::Result IzwCompressor::init(const unsigned char theLzwCopdeSize)
{
   lzwCodeSizeM = theLzwCopdeSize;
   if (lzwCodeSizeM > 8)
      return IMAGELIB::ERROR;

   isFirstTimeM = true;
   maxColorM = 1 << lzwCodeSizeM;
   for (int i = 0; i < maxColorM; i++)
   {
      stringTableM[i].nextIndexM = -1;
      stringTableM[i].rightIndexM = i + 1;
      stringTableM[i].colorM = i;
   }

   clearCodeM = maxColorM;
   eofCodeM = clearCodeM + 1;
   startTableSizeM = eofCodeM + 1;
   nextCodeIndexM = startTableSizeM;
   curCodeSizeM = lzwCodeSizeM + 1;

   holdingBitsM = clearCodeM;
   holdingLenM = curCodeSizeM;
   return IMAGELIB::DONE;
   
};

IMAGELIB::Result IzwCompressor::compress(const string &theInputData, string &theOutputData)
{
   if (lzwCodeSizeM > 8)
      return IMAGELIB::ERROR;

   if (theInputData.empty())
      return IMAGELIB::DONE;
      
   int inputIndex = 0;
   if (isFirstTimeM)
   {
      isFirstTimeM = 0;
      curColorM = theInputData[inputIndex++];
      curCodeM = curColorM;
      oldCodeM = curCodeM;
   }

   while (inputIndex < theInputData.length())
   {
      curColorM = theInputData[inputIndex++];
      oldCodeM = curCodeM;
      if (-1 == stringTableM[curCodeM].nextIndexM)
      {
         stringTableM[curCodeM].nextIndexM = nextCodeIndexM;
      }else
      {
         curCodeM = stringTableM[curCodeM].nextIndexM;
         while (stringTableM[curCodeM].colorM != curColorM
            && -1 != stringTableM[curCodeM].rightIndexM)
         {
            curCodeM = stringTableM[curCodeM].rightIndexM;
         }

         if (stringTableM[curCodeM].colorM != curColorM
            && -1 == stringTableM[curCodeM].rightIndexM)
         {
            stringTableM[curCodeM].rightIndexM = nextCodeIndexM;
         }else
         {
            continue;
         }
      }

      holdingBitsM = holdingBitsM + (oldCodeM<<holdingLenM);
      holdingLenM += curCodeSizeM;

      stringTableM[nextCodeIndexM].rightIndexM = -1;
      stringTableM[nextCodeIndexM].nextIndexM = -1;
      stringTableM[nextCodeIndexM].colorM = curColorM;
      curCodeM = curColorM;

      if (0 != (nextCodeIndexM >> curCodeSizeM)
            || nextCodeIndexM == 4093)
      {
         if (curCodeSizeM >=12)
         {
            holdingBitsM = holdingBitsM + (clearCodeM << holdingLenM);
            holdingLenM = holdingLenM + curCodeSizeM;
            for (int i = 0; i < maxColorM; i++)
               stringTableM[i].nextIndexM = -1;
            nextCodeIndexM = startTableSizeM - 1;
            curCodeSizeM = lzwCodeSizeM;
         }
         curCodeSizeM++;
      }
      nextCodeIndexM++;

      while (holdingLenM >= 8)
      {
         char color = holdingBitsM & 0xFF;
         theOutputData.append(1, color);
         holdingBitsM >>= 8;
         holdingLenM -= 8;
      }

   }
   return IMAGELIB::DONE;
}

IMAGELIB::Result IzwCompressor::writeEof(string &theOutputData)
{
   holdingBitsM = holdingBitsM + (curCodeM << holdingLenM);
   holdingLenM += curCodeSizeM;
   holdingBitsM = holdingBitsM + (eofCodeM << holdingLenM);
   holdingLenM += curCodeSizeM;

   while (holdingLenM > 0)
   {
      char color = holdingBitsM & 0xFF;
      theOutputData.append(1, color);
      holdingBitsM >>= 8;
      holdingLenM -= 8;
   }
   return IMAGELIB::DONE;
}

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
		if (stateM != PARSING_IMAGE_DATA_BLOCK && stateM != PARSING_IMAGE_DATA_TERMINATOR)
			cout << "state:" << stateM << endl;
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
         

         case PARSING_IMAGE_DATA_BLOCK:
            result = stateParsingImageDataBlock(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
            
         case PARSING_IMAGE_DATA_TERMINATOR:
            result = stateParsingImageDataTerminator(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

			/* Special-Purpose Block */
			case PARSING_APPLICATION_EXTENSION:
            result = stateParsingApplicationExtension(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
         
         case PARSING_COMMENT_EXTENSION:
            result = stateParsingCommentExtension(theInputBuffer, decodeIndex, theOutputBuffer);
				break;

			case PARSING_SUB_BLOCK_TER_SIZE:
            result = stateParsingSubBlockTerSize(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
			
			case PARSING_SUB_BLOCK_TER:
            result = stateParsingSubBlockTer(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
			
         case PARSING_TRAILER:
            result = stateParsingTrailer(theInputBuffer, decodeIndex, theOutputBuffer);
				break;
            
            
            
         case PARSING_PLAIN_TEXT_EXTENSION:
         {
            cout << "state:" << stateM << endl;
            assert(0);
            break;
         }
         
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
   stateM = PARSING_ERROR;
   return IMAGELIB::ERROR;

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
   
   stateM = PARSING_IMAGE_DATA_TERMINATOR;
   gifStruct.lzw_code_size = (gifStruct.lzw_code_size > 8)? 8 : gifStruct.lzw_code_size;
   izwDecompressor.init(gifStruct.lzw_code_size);
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
       stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }  
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingImageDataBlock(
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
   
   stateM = PARSING_IMAGE_DATA_TERMINATOR;
   
   string gifPlainData;
   gifPlainData.reserve(gifStruct.block_size * 2);
   
   izwDecompressor.decompress(gifStruct, gifPlainData);
   if (0 != handlerM->handle(gifPlainData, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }  
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingImageDataTerminator(
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
      stateM = PARSING_IMAGE_DATA_BLOCK;
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

   stateM = PARSING_SUB_BLOCK_TER_SIZE;
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

   stateM = PARSING_SUB_BLOCK_TER_SIZE;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      
   return result;
}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingSubBlockTerSize(
   const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_data_sub_block_ter_t gifStruct;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, 2, bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
   return result;

   if (0 != gifStruct.block_size)
   {
      bufferM.append((char*)&gifStruct, 2);
      stateM = PARSING_SUB_BLOCK_TER;
      return result;
   }
   
   stateM = CHECK_DATA_INTRODUCOR;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      
   return result;

}

IMAGELIB::Result 
GifDataStreamDecoder::stateParsingSubBlockTer(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   assert (bufferM.length() > 1);
   gif_data_sub_block_ter_t gifStruct;
   unsigned char size = (unsigned char)bufferM[0];
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, size + 2, bufferM, theInputBuffer, theDecodeIndex);
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
GifDataStreamDecoder::stateParsingTrailer(
const string &theInputBuffer
   , uint64_t &theDecodeIndex
   , string &theOutputBuffer)
{
   gif_trailer_t gifStruct;
   IMAGELIB::Result result = decodeStruct((char *) &gifStruct, sizeof(gif_trailer_t), bufferM, theInputBuffer, theDecodeIndex);
   if (IMAGELIB::DONE != result)
      return result;
   
   stateM = PARSING_DONE;
   if (0 != handlerM->handle(gifStruct, theOutputBuffer))
   {
      stateM = PARSING_ERROR;
      return IMAGELIB::ERROR;
   }
      
   return result;
}


int GifEncoder::exec(gif_header_t &theGifStruct, string &theOutputBuffer)  
{
   theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_header_t));   
   return 0;
}

int GifEncoder::exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_lgc_scr_desc_t));
   return 0;
}

int GifEncoder::exec(gif_glb_color_tbl_t &theGifStruct, string &theOutputBuffer)
{
   theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_color_triplet_t) * theGifStruct.size);
   return 0;
}

int GifEncoder::exec(gif_graphic_ctrl_ext_t &theGifStruct, string &theOutputBuffer)
{
   theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_graphic_ctrl_ext_t));
   return 0;
}

int GifEncoder::exec(gif_image_desc_t &theGifStruct, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_image_desc_t));
   return 0;
}

int GifEncoder::exec(gif_lcl_color_tbl_t &theGifStruct, string &theOutputBuffer)
{
   theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_color_triplet_t) * theGifStruct.size);
   return 0;
}

int GifEncoder::exec(gif_lzw_code_size_t &theGifStruct, string &theOutputBuffer)
{
   outputBufferM.clear();
   izwCompressor.init(theGifStruct.lzw_code_size);
   theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_lzw_code_size_t));
   return 0;
}

int GifEncoder::exec(gif_image_data_block_t &theGifStruct, string &theOutputBuffer)
{
   theOutputBuffer.append((const char*)&theGifStruct, theGifStruct.block_size + 1);
   return 0;
}

int GifEncoder::exec(gif_image_data_ter_t &theGifStruct, string &theOutputBuffer)
{
   izwCompressor.writeEof(outputBufferM);
   int bufferLen = outputBufferM.length();
   int buferIndex = 0;
   while ((bufferLen - buferIndex) >= 255)
   {
      gif_image_data_block_t imageData;
      imageData.block_size = 255;
      memcpy(imageData.data_value, outputBufferM.c_str() + buferIndex, 255);
      buferIndex += 255;
      exec(imageData, theOutputBuffer);
   }
   if ((bufferLen - buferIndex) > 0)
   {
      gif_image_data_block_t imageData;
      imageData.block_size = bufferLen - buferIndex;
      memcpy(imageData.data_value, outputBufferM.c_str() + buferIndex, bufferLen - buferIndex);
      exec(imageData, theOutputBuffer);
   }
   outputBufferM.clear();
   theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_image_data_ter_t));
   return 0;
}


int GifEncoder::exec(string &theGifPlainData, string &theOutputBuffer)
{
	izwCompressor.compress(theGifPlainData, outputBufferM);
   int bufferLen = outputBufferM.length();
   int buferIndex = 0;
   while ((bufferLen - buferIndex) >= 255)
   {
      gif_image_data_block_t imageData;
      imageData.block_size = 255;
      memcpy(imageData.data_value, outputBufferM.c_str() + buferIndex, 255);
      buferIndex += 255;
      exec(imageData, theOutputBuffer);
   }
   outputBufferM = outputBufferM.substr(buferIndex);
   
   return 0;
}

int GifEncoder::exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_plain_text_ext_t));
   return 0;
}

int GifEncoder::exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_appl_ext_t));
   return 0;
}

int GifEncoder::exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_comment_ext_t));
   return 0;
}

int GifEncoder::exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theGifStruct, theGifStruct.block_size + 2);
   return 0;
}

int GifEncoder::exec(gif_trailer_t &theGifStruct, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_trailer_t));
   return 0;
}

int GifResizer::exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer)
{
   assert(outputRateM > 1.0);
   theGifStruct.lgc_scr_width = (uint16_t)(theGifStruct.lgc_scr_width/outputRateM);
   theGifStruct.lgc_scr_height = (uint16_t)(theGifStruct.lgc_scr_height/outputRateM);
   return 0;
}

int GifResizer::exec(gif_image_desc_t &theGifStruct, string &theOutputBuffer)
{
   inputFrameWidthM = theGifStruct.image_width;
   inputFrameHeightM = theGifStruct.image_height;
   theGifStruct.image_left = (uint16_t)(theGifStruct.image_left/outputRateM);
   theGifStruct.image_top  = (uint16_t)(theGifStruct.image_top/outputRateM);
   theGifStruct.image_width = (uint16_t)(theGifStruct.image_width/outputRateM);
   theGifStruct.image_height = (uint16_t)(theGifStruct.image_height/outputRateM);
   outputFrameWidthM = theGifStruct.image_width;
   outputFrameHeightM = theGifStruct.image_height;
   return 0;
}

int GifResizer::exec(gif_lzw_code_size_t &theGifStruct, string &theOutputBuffer)
{
   lzwCodeSizeM = theGifStruct.lzw_code_size;
   //assert(8 == lzwCodeSizeM
   //   || 4 == lzwCodeSizeM
   //   || 2 == lzwCodeSizeM
   //   || 1 == lzwCodeSizeM);
   curXM = 0;
   curYM = 0;
   outputFrameIndexM = -1;
   return 0;
}

int GifResizer::exec(string &theGifPlainData, string &theOutputBuffer)
{
   string outputData;
   size_t outputLen = (size_t)(theGifPlainData.length()/outputRateM);
   outputData.reserve(outputLen + 1);

   for (int i = 0; i < theGifPlainData.length(); i++)
   {
      unsigned char curColor = theGifPlainData[i];

      //x * width + y
      unsigned outputX = (unsigned)(((float)curXM) * outputFrameHeightM / inputFrameHeightM);
      unsigned outputY = (unsigned)(((float)curYM) * outputFrameWidthM / inputFrameWidthM);
      curYM++;
      curXM += curYM / inputFrameWidthM;
      curYM = curYM % inputFrameWidthM;
      long long newOutputYIndex = outputX * outputFrameWidthM + outputY;
      if (newOutputYIndex <= outputFrameIndexM)
         continue;
      outputFrameIndexM = newOutputYIndex;
		outputData.append((char*)&curColor, 1); 
   }

   theGifPlainData = outputData;
   return 0;
}

int GifResizer::exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_header_t &theGifStruct, string &theOutputBuffer)
{
	cout << "output index:" << theOutputBuffer.length() << endl;
	cout << "header:" << endl
		<< "\t signature:" << string(theGifStruct.signature, sizeof (theGifStruct.signature)) << endl
		<< "\t version:" << string(theGifStruct.version, sizeof (theGifStruct.signature)) << endl;
   return 0;
}
  
int GifDumper::exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer)
{
	cout << "output index:" << theOutputBuffer.length() << endl;
   int flag = (int) theGifStruct.global_flag.global_color_table_flag;
   int size = 1 << (theGifStruct.global_flag.global_color_tbl_sz + 1);
	cout << "Logical Screen Descriptor:" << endl
		<< "\t lgc_scr_width:" << theGifStruct.lgc_scr_width << endl
		<< "\t lgc_scr_height:" << theGifStruct.lgc_scr_height << endl
		<< "\t global_color_table_flag:" << flag << endl
		<< "\t global_color_table_size:" << size << endl
		<< "\t bg_color_index:" << (int)theGifStruct.bg_color_index << endl
		<< "\t pixel_aspect_ratio:" << (int)theGifStruct.pixel_aspect_ratio << endl;
   return 0;
}

int GifDumper::exec(gif_glb_color_tbl_t &theGifStruct, string &theOutputBuffer)
{
	cout << "output index:" << theOutputBuffer.length() << endl;
   cout << "Global Color Table:" << endl
      << "\t ..." << endl;
   return 0;
}

int GifDumper::exec(gif_graphic_ctrl_ext_t &theGifStruct, string &theOutputBuffer)
{
	cout << "output index:" << theOutputBuffer.length() << endl;
   cout << "Graphic Control Extension:" << endl
      << "\t ext_introducer:" << (int)theGifStruct.ext_introducer << endl
      << "\t graphic_ctrl_label:" << (int)theGifStruct.graphic_ctrl_label << endl
      << "\t block_size:" << (int)theGifStruct.block_size << endl
         << "\t\t transparent_color_flag:" << (int)theGifStruct.flag.transparent_color_flag << endl
         << "\t\t user_input_flag:" << (int)theGifStruct.flag.user_input_flag << endl
         << "\t\t disposal_method:" << (int)theGifStruct.flag.disposal_method << endl
         << "\t\t reserved:" << (int)theGifStruct.flag.reserved << endl
      << "\t delay_time:" << theGifStruct.delay_time << endl
      << "\t transparent_color_index:" << (int)theGifStruct.transparent_color_index << endl
      << "\t block_terminator:" << (int)theGifStruct.block_terminator << endl;
   return 0;
}

int GifDumper::exec(gif_image_desc_t &theGifStruct, string &theOutputBuffer)
{
	cout << "output index:" << theOutputBuffer.length() << endl;
	cout << "Image Descriptor" << endl
		<< "\t image_sep:" << (int)theGifStruct.image_sep << endl
		<< "\t image_left:" << theGifStruct.image_left << endl
		<< "\t image_top:" << theGifStruct.image_top << endl
		<< "\t image_width:" << theGifStruct.image_width << endl
		<< "\t image_height:" << theGifStruct.image_height << endl
		<< "\t\t local_color_tbl_sz:" << (int)theGifStruct.local_flag.local_color_tbl_sz << endl    
		<< "\t\t reserved:" << (int)theGifStruct.local_flag.reserved << endl                        
		<< "\t\t sort_flag:" << (int)theGifStruct.local_flag.sort_flag << endl                      
		<< "\t\t interlace_flag:" << (int)theGifStruct.local_flag.interlace_flag << endl            
		<< "\t\t local_color_tbl_flag:" << (int)theGifStruct.local_flag.local_color_tbl_flag << endl;
   return 0;
}

int GifDumper::exec(gif_lcl_color_tbl_t &theGifStruct, string &theOutputBuffer)
{
   cout << "output index:" << theOutputBuffer.length() << endl;
   cout << "Local Color Table:" << endl
      << "\t ..." << endl;
   return 0;
}

int GifDumper::exec(gif_lzw_code_size_t &theGifStruct, string &theOutputBuffer)
{
   cout << "output index:" << theOutputBuffer.length() << endl;
	cout << "Image Data" << endl
      << "\t LZW code size:" << (int)theGifStruct.lzw_code_size << endl;
   return 0;
}

int GifDumper::exec(gif_image_data_block_t &theGifStruct, string &theOutputBuffer)
{
   //cout << "output index:" << theOutputBuffer.length() << endl;
	//cout << "\t Image data block size:" << (int)theGifStruct.block_size << endl;
	cout << ".";
	//cout << output.length();
   return 0;
}

int GifDumper::exec(gif_image_data_ter_t &theGifStruct, string &theOutputBuffer)
{
   cout << "\n\t Image data terminator:" << (int)theGifStruct.terminator<< endl;
   return 0;
}


int GifDumper::exec(string &theGifPlainData, string &theOutputBuffer)
{
	cout << "[" << theGifPlainData.length() << "]";
   return 0;
}

int GifDumper::exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer)
{
	cout << "output index:" << theOutputBuffer.length() << endl;
	cout << "Application Extension:" << endl
		<< "\t ext_introducer:" << (int)theGifStruct.ext_introducer << endl
		<< "\t plain_text_label:" << (int)theGifStruct.plain_text_label << endl
		<< "\t block_size:" << (int)theGifStruct.block_size << endl
		<< "\t identifier:" << string(theGifStruct.identifier, 8) << endl
		<< "\t appl_auth_code:" << string(theGifStruct.appl_auth_code, 3) << endl;
   return 0;
}

int GifDumper::exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer)
{
	cout << "output index:" << theOutputBuffer.length() << endl;
	cout << "\t data_sub_block and Terminator bytes:" << (theGifStruct.block_size + 2) << endl;
   return 0;
}

int GifDumper::exec(gif_trailer_t &theGifStruct, string &theOutputBuffer)
{
   cout << "output index:" << theOutputBuffer.length() << endl;
	cout << "Trailer" << endl;
   return 0;
}


