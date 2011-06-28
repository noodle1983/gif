#ifndef IZWDECOMPRESSOR_H
#define IZWDECOMPRESSOR_H

#include "ImageLibStruct.h"

namespace IMAGELIB
{
namespace GIFLIB
{

class IzwDecompressor
{
public:
   IzwDecompressor()
   {};
   enum{MAX_COLOR_STRING_LEN = 4097};

   Result init(const unsigned char theLzwCopdeSize);
   Result decompress(const gif_image_data_block_t& theInputData, string &theOutputData);

private:
   /**
    * read the theInputData to holdingBitsM, until it is large enough to get a code.
    */
   void readData(const gif_image_data_block_t& theInputData, unsigned char &theInputIndex);

   /**
    * read curCodeM from holdingBitsM
    */
   void getACode();

   void clear();

   void calcColorStringOfCode(const int16_t theCode, unsigned char* theColorString, int16_t &theStringLen);

   void expandStringTable(const unsigned char theColor, const int16_t thePreColorIndex);
   
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
   TableItem stringTableM[MAX_COLOR_STRING_LEN];

   int isEndM;
   uint32_t holdingBitsM;
   unsigned char holdingLenM;
};

};//namespace GIFLIB
};//namespace IMAGELIB

#endif //IZWDECOMPRESSOR_H
