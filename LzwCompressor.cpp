#include "LzwCompressor.h"
#include <string.h>
#include <assert.h>
using namespace std;
using namespace IMAGELIB::GIFLIB;

IMAGELIB::Result LzwCompressor::init(const unsigned char theLzwCopdeSize)
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

IMAGELIB::Result LzwCompressor::compress(const string &theInputData, string &theOutputData)
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

IMAGELIB::Result LzwCompressor::writeEof(string &theOutputData)
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

