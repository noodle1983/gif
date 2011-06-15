#include "GifStreamCodec.h"
#include <iostream>
using namespace std;

Result decodeStruct(
	char* theImageStruct, const int theStructLen, 
	string &theBuffer, const string &theInputBuffer,
	int& theDecodeIndex)
{
   int bufferLen = theBuffer.length();
	int inputLen = theInputBuffer.length() - theDecodeIndex;
   if (inputLen + bufferLen < theStructLen)
   {
      theBuffer += theInputBuffer;
      bufferLen += inputLen; 
		theDecodeIndex += inputLen;
      return PARTLY;
   }else
   {
      memcpy(theImageStruct, theBuffer.c_str(), bufferLen);
	   memcpy(theImageStruct + bufferLen, theInputBuffer.c_str() + theDecodeIndex, theStructLen - bufferLen);
	   theBuffer.clear();
		theDecodeIndex += theStructLen - bufferLen;
		return DONE;
   }
   return ERROR;
}

Result 
GifDataStreamDecoder::decode(const string &theInputBuffer, string &theOutputBuffer)
{
	int decodeIndex = 0;
	int detectDeadloop = decodeIndex + 1;
	Result result = DONE;
	while(DONE == result && PARSING_DONE != stateM)
	{
		assert(detectDeadloop != decodeIndex);
		detectDeadloop = decodeIndex;
	   switch(stateM)
		{
	      case PARSING_HEADER:
			{
			   gif_header_t gifStruct;
	         result = decodeStruct((char *) &gifStruct, sizeof(gif_header_t), bufferM, theInputBuffer, decodeIndex);
				if (DONE != result)
               break;
            
				stateM = PARSING_LOGICAL_SCREEN_DESCRIPTOR;
            if (0 != handlerM->handle(gifStruct, theOutputBuffer))
               return ERROR;
				break;
	      }
	      
			case PARSING_LOGICAL_SCREEN_DESCRIPTOR:
			{
				gif_lgc_scr_desc_t gifStruct;
				result = decodeStruct((char *) &gifStruct, sizeof(gif_lgc_scr_desc_t), bufferM, theInputBuffer, decodeIndex);
				if (DONE != result)
               break;

            if (gifStruct.global_flag.global_color_table_flag)
            {
   				stateM = PARSING_GLOBAL_COLOR_TABLE;
               globalTableSize = 1 << (gifStruct.global_flag.global_color_tbl_sz + 1);
            }else{
               stateM = PARSING_GRAPHIC_CONTROL_EXTENSION;
            }
            if (0 != handlerM->handle(gifStruct, theOutputBuffer))
               return ERROR;
				break;
			}

         case PARSING_GLOBAL_COLOR_TABLE:
         {
            gif_glb_color_tbl_t gifStruct;
            gifStruct.size = globalTableSize;
				result = decodeStruct((char *) &gifStruct, sizeof(gif_color_triplet_t) * globalTableSize, bufferM, theInputBuffer, decodeIndex);
				if (DONE != result)
               break;
            
            stateM = PARSING_GRAPHIC_CONTROL_EXTENSION;
            if (0 != handlerM->handle(gifStruct, theOutputBuffer))
               return ERROR;
				break;

         }

         /* Graphic Block */
         case PARSING_GRAPHIC_CONTROL_EXTENSION:
         {

         }

		}// switch
	}
   return result;

}

int GifEncoder::exec(gif_header_t &theHeader, string &theOutputBuffer)  
{
   theOutputBuffer.append((const char*)&theHeader, sizeof(gif_header_t));   
   return 0;
}

int GifEncoder::exec(gif_lgc_scr_desc_t &theLgcScrDesc, string &theOutputBuffer)
{
	theOutputBuffer.append((const char*)&theLgcScrDesc, sizeof(gif_lgc_scr_desc_t));
   return 0;
}

int GifEncoder::exec(gif_glb_color_tbl_t &theGlbColorTbl, string &theOutputBuffer)
{
   theOutputBuffer.append((const char*)&theGlbColorTbl, sizeof(gif_color_triplet_t) * theGlbColorTbl.size);
   return 0;
}

int GifEncoder::exec(gif_graphic_ctrl_ext_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifEncoder::exec(gif_image_desc_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifEncoder::exec(gif_plain_text_ext_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifEncoder::exec(gif_appl_ext_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifEncoder::exec(gif_comment_ext_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifEncoder::exec(gif_trailer_t &theHeader, string &theOutputBuffer)
{
   return 0;
}



int GifDumper::exec(gif_header_t &theHeader, string &theOutputBuffer)
{
	cout << "header:" << endl
		<< "\t signature:" << string(theHeader.signature, sizeof (theHeader.signature)) << endl
		<< "\t version:" << string(theHeader.version, sizeof (theHeader.signature)) << endl;
   return 0;
}
  
int GifDumper::exec(gif_lgc_scr_desc_t &theLgcScrDesc, string &theOutputBuffer)
{
   int flag = (int) theLgcScrDesc.global_flag.global_color_table_flag;
   int size = 1 << (theLgcScrDesc.global_flag.global_color_tbl_sz + 1);
	cout << "Logical Screen Descriptor:" << endl
		<< "\t lgc_scr_width:" << theLgcScrDesc.lgc_scr_width << endl
		<< "\t lgc_scr_height:" << theLgcScrDesc.lgc_scr_height << endl
		<< "\t global_color_table_flag:" << flag << endl
		<< "\t global_color_table_size:" << size << endl
		<< "\t bg_color_index:" << (int)theLgcScrDesc.bg_color_index << endl
		<< "\t pixel_aspect_ratio:" << (int)theLgcScrDesc.pixel_aspect_ratio << endl;
   return 0;
}

int GifDumper::exec(gif_glb_color_tbl_t &theGlbColorTbl, string &theOutputBuffer)
{
   cout << "Logical Screen Descriptor:" << endl
      << "\t ..." << endl;
   return 0;
}

int GifDumper::exec(gif_graphic_ctrl_ext_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_image_desc_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_plain_text_ext_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_appl_ext_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_comment_ext_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_trailer_t &theHeader, string &theOutputBuffer)
{
   return 0;
}

