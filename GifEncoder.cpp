#include "GifEncoder.h"
#include <string.h>
#include <assert.h>
using namespace std;
using namespace IMAGELIB::GIFLIB;

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
   lzwCompressor.init(theGifStruct.lzw_code_size);
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
   lzwCompressor.writeEof(outputBufferM);
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


int GifEncoder::exec(gif_data_sub_block_t &theGifStruct, string &theOutputBuffer)
{
   theOutputBuffer.append((const char*)&theGifStruct, theGifStruct.block_size + 1);
   return 0;
}

int GifEncoder::exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer)
{
   theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_image_data_ter_t));
   return 0;
}

int GifEncoder::exec(string &theGifPlainData, string &theOutputBuffer)
{
	lzwCompressor.compress(theGifPlainData, outputBufferM);
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

int GifEncoder::exec(gif_trailer_t &theGifStruct, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theGifStruct, sizeof(gif_trailer_t));
   return 0;
}

