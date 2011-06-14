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
	         result = decode(gifStruct, theInputBuffer, decodeIndex, theOutputBuffer);
				if (DONE == result)
					stateM = PARSING_LOGICAL_SCREEN_DESCRIPTOR;
				break;
	      }
	      
			case PARSING_LOGICAL_SCREEN_DESCRIPTOR:
			{
				gif_lgc_scr_desc_t gifStruct;
				result = decode(gifStruct, theInputBuffer, decodeIndex, theOutputBuffer);
				if (DONE == result)
					stateM = PARSING_GLOBAL_COLOR_TABLE;
				break;
			}

		}// switch
	}
   return result;

}

template<typename GifStruct>
Result GifDataStreamDecoder::decode(GifStruct &theGifStruct, const string &theInputBuffer, int& theDecodeIndex, string &theOutputBuffer)
{
	Result result = decodeStruct((char *) &theGifStruct, sizeof(GifStruct), bufferM, theInputBuffer, theDecodeIndex);
   if (DONE != result)
		return result;
	
   if (0 != handlerM->handle(theGifStruct, theOutputBuffer))
   {
      return ERROR;
   }      
   return DONE;
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
	cout << "Logical Screen Descriptor:" << endl
		<< "\t lgc_scr_width:" << theLgcScrDesc.lgc_scr_width << endl
		<< "\t lgc_scr_height:" << theLgcScrDesc.lgc_scr_height << endl
		<< "\t bg_color_index:" << (int)theLgcScrDesc.bg_color_index << endl
		<< "\t pixel_aspect_ratio:" << (int)theLgcScrDesc.pixel_aspect_ratio << endl;
   return 0;
}

int GifDumper::exec(gif_glb_color_tbl_t &theGlbColorTbl, string &theOutputBuffer)
{
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

