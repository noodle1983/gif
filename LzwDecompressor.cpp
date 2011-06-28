#include "LzwDecompressor.h"
#include <string.h>
#include <assert.h>
using namespace std;
using namespace IMAGELIB::GIFLIB;

IMAGELIB::Result LzwDecompressor::init(const unsigned char theLzwCopdeSize)
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

IMAGELIB::Result LzwDecompressor::decompress(const gif_image_data_block_t& theInputData, string &theOutputData)
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
void LzwDecompressor::readData(const gif_image_data_block_t& theInputData, unsigned char &theInputIndex)
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
void LzwDecompressor::getACode()
{
   curCodeM = holdingBitsM & codeMaskM;
   holdingBitsM >>= curCodeSizeM;
   holdingLenM -= curCodeSizeM;
}

void LzwDecompressor::clear()
{
   curCodeSizeM = lzwCodeSizeM + 1;
   codeMaskM = (1 << curCodeSizeM) - 1;
   startTableSizeM = (1 << lzwCodeSizeM);
   clearCodeM = startTableSizeM;
   eofCodeM = clearCodeM + 1;
   nextCodeIndexM = eofCodeM + 1;
   oldCodeM = -1;
}

void LzwDecompressor::calcColorStringOfCode(const int16_t theCode, unsigned char* theColorString, int16_t &theStringLen)
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

void LzwDecompressor::expandStringTable(const unsigned char theColor, const int16_t thePreColorIndex)
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
