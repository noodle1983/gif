#ifndef GIFDUMPER_H
#define GIFDUMPER_H

#include "ImageLibStruct.h"

namespace IMAGELIB
{
namespace GIFLIB
{

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
   virtual int exec(string &theGifPlainData, string &theOutputBuffer);
};

};//namespace GIFLIB
};//namespace IMAGELIB

#endif //GIFDUMPER_H

