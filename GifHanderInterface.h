#ifndef GIFHANDERINTERFACE_H
#define GIFHANDERINTERFACE_H

#include <string>
using namespace std;

#include "gif.h"

namespace IMAGELIB
{
namespace GIFLIB
{
   
class GifHanderInterface
{
public:
   GifHanderInterface()
        :nextHandlerM(NULL){};
   GifHanderInterface(GifHanderInterface *theNextHandler)
        :nextHandlerM(theNextHandler){};
   virtual ~GifHanderInterface()
        {if (nextHandlerM) delete nextHandlerM;};
   template<typename GifStruct>
   int handle(GifStruct &theStruct, string &theOutputBuffer)
   {
      if (0 != exec(theStruct, theOutputBuffer))
         return -1;
      if (nextHandlerM)
         return nextHandlerM->handle(theStruct, theOutputBuffer);
      return 0;
   }

   
protected:
   virtual int exec(gif_header_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_glb_color_tbl_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_graphic_ctrl_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_image_desc_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_lcl_color_tbl_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_lzw_code_size_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_image_data_block_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_image_data_ter_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer) = 0;
   virtual int exec(gif_trailer_t &theGifStruct, string &theOutputBuffer)=0;
   virtual int exec(string &theGifPlainData, string &theOutputBuffer)=0;
   GifHanderInterface *nextHandlerM; 
};

};//namespace GIFLIB
};//namespace IMAGELIB
#endif
