#ifndef IZWCOMPRESSOR_H
#define IZWCOMPRESSOR_H

#include "ImageLibStruct.h"

namespace IMAGELIB
{
namespace GIFLIB
{
   
class IzwCompressor
{
public:
   IzwCompressor()
   {};
   enum{MAX_COLOR_STRING_LEN = 4097};

   Result init(const unsigned char theLzwCopdeSize);
   Result compress(const string &theInputData, string &theOutputData);
   Result writeEof(string &theOutputData);
   

private:
   
   struct TableItem
   {
      int16_t nextIndexM;
      int16_t rightIndexM;
      unsigned char colorM;
   };
   
   unsigned char lzwCodeSizeM;
   unsigned char curCodeSizeM;
   int16_t codeMaskM;
   int16_t clearCodeM;
   int16_t eofCodeM;
   int16_t nextCodeIndexM;
   uint16_t oldCodeM;
   uint16_t curCodeM;
   int16_t startTableSizeM; 
   TableItem stringTableM[MAX_COLOR_STRING_LEN];

   int isFirstTimeM;
   uint64_t holdingBitsM;
   char holdingLenM;
   uint16_t maxColorM;
   unsigned char curColorM;
};

};//namespace GIFLIB
};//namespace IMAGELIB

#endif //IZWCOMPRESSOR_H

