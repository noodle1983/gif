#include "GifDataStreamDecoder.h"
#include <iostream>
#include <string.h>
#include <assert.h>
using namespace std;
using namespace IMAGELIB::GIFLIB;

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

int GifDumper::exec(gif_data_sub_block_t &theGifStruct, string &theOutputBuffer)
{
	cout << ".";
   return 0;
}

int GifDumper::exec(gif_data_sub_block_ter_t &theGifStruct, string &theOutputBuffer)
{
   cout << "\n\t Data Sub-blocks terminator:" << (int)theGifStruct.terminator<< endl;
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
   cout << "output index:" << theOutputBuffer.length() << endl;
	cout << "Comment Extension:" << endl
		<< "\t ext_introducer:" << (int)theGifStruct.ext_introducer << endl
		<< "\t comment_label:" << (int)theGifStruct.comment_label << endl;
   return 0;
}

int GifDumper::exec(gif_trailer_t &theGifStruct, string &theOutputBuffer)
{
   cout << "output index:" << theOutputBuffer.length() << endl;
	cout << "Trailer" << endl;
   return 0;
}

