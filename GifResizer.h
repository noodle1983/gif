#ifndef GIFRESIZER_H
#define GIFRESIZER_H

#include "ImageLibStruct.h"
#include "GifHanderInterface.h"

namespace IMAGELIB
{
namespace GIFLIB
{

class GifResizer: public GifHanderInterface
{
public:
   GifResizer(const float thetRate,
      GifHanderInterface *theNextHandler)
         : outputRateM(thetRate)
         , GifHanderInterface(theNextHandler)
         , disposalMethodM(0)
         , frameIndexM(0){};
   
protected:  
   virtual int exec(gif_header_t &theGifStruct, string &theOutputBuffer){return 0;};  
   virtual int exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_glb_color_tbl_t &theGifStruct, string &theOutputBuffer){return 0;};
   virtual int exec(gif_graphic_ctrl_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_image_desc_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_lcl_color_tbl_t &theGifStruct, string &theOutputBuffer){return 0;};
   virtual int exec(gif_lzw_code_size_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_image_data_block_t &theGifStruct, string &theOutputBuffer){return 0;};
   virtual int exec(gif_image_data_ter_t &theGifStruct, string &theOutputBuffer){return 0;};
   virtual int exec(gif_data_sub_block_t &theGifStruct, string &theOutputBuffer){return 0;};
   virtual int exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer){return 0;};
   virtual int exec(string &theGifPlainData, string &theOutputBuffer);
   virtual int exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer);
   virtual int exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer){return 0;};
   virtual int exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer){return 0;};
   virtual int exec(gif_trailer_t &theGifStruct, string &theOutputBuffer){return 0;};

   unsigned char lzwCodeSizeM;
   unsigned char disposalMethodM;
   unsigned char tpColorIndexM;
   unsigned char bgColorIndexM;
   float outputRateM; //input /output, should > 1.0

   unsigned logicScreenWidthM;
   unsigned logicScreenHeightM;
   unsigned inputFrameLeftM;
   unsigned inputFrameTopM;
   unsigned inputFrameWidthM;
   unsigned inputFrameHeightM;
   unsigned outputFrameWidthM;
   unsigned outputFrameHeightM;
   long long outputFrameIndexM;
    
   unsigned frameIndexM;
   unsigned curXM, curYM;
   string inputFrameBufferM;
   
};


};//namespace GIFLIB
};//namespace IMAGELIB

#endif //GIFRESIZER_H
