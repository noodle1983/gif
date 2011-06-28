#include "GifResizer.h"
#include <string.h>
#include <assert.h>
using namespace std;
using namespace IMAGELIB::GIFLIB;

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
   theGifStruct.text_grid_top = (uint16_t)(theGifStruct.text_grid_top/outputRateM);
   theGifStruct.text_grid_left = (uint16_t)(theGifStruct.text_grid_left/outputRateM);
   theGifStruct.text_grid_width = (uint16_t)(theGifStruct.text_grid_width/outputRateM);
   theGifStruct.text_grid_height = (uint16_t)(theGifStruct.text_grid_height/outputRateM);
   theGifStruct.char_cell_width = (unsigned char)(theGifStruct.char_cell_width/outputRateM);
   theGifStruct.char_cell_height = (unsigned char)(theGifStruct.char_cell_height/outputRateM);   
   return 0;
}

