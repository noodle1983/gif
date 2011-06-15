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
      theBuffer.append(theInputBuffer.c_str() + theDecodeIndex, inputLen);
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
	Result result = DONE;
	while(DONE == result && PARSING_DONE != stateM)
	{
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
            {
					stateM = PARSING_ERROR;
               return ERROR;
            }
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
               stateM = CHECK_DATA_INTRODUCOR;
            }
            if (0 != handlerM->handle(gifStruct, theOutputBuffer))
            {
					stateM = PARSING_ERROR;
               return ERROR;
            }
				break;
			}

         case PARSING_GLOBAL_COLOR_TABLE:
         {
            gif_glb_color_tbl_t gifStruct;
            gifStruct.size = globalTableSize;
				result = decodeStruct((char *) &gifStruct, sizeof(gif_color_triplet_t) * globalTableSize, bufferM, theInputBuffer, decodeIndex);
				if (DONE != result)
               break;
            
            stateM = CHECK_DATA_INTRODUCOR;
            if (0 != handlerM->handle(gifStruct, theOutputBuffer))
            {
					stateM = PARSING_ERROR;
               return ERROR;
            }
				break;

         }

         case CHECK_DATA_INTRODUCOR:
         {
            assert(theInputBuffer.length() >= decodeIndex);
            if (theInputBuffer.length() == decodeIndex){
               result = PARTLY;
               break;
            }

            
           const unsigned char introducer = theInputBuffer[decodeIndex];
           if (0x21 == introducer)
            {
               stateM = CHECK_DATA_EXTENSION_LABEL;
               bufferM.append((char *)&introducer, 1);
               decodeIndex++;
               result = DONE;
               break;
            }
            if (0x2C == introducer)
            {
               stateM = PARSING_IMAGE_DESCRIPTOR;
               result = DONE;
               break;
            }
            if (0x3B == introducer)
            {
               stateM = PARSING_TRAILER;
               result = DONE;
               break;
            }
            stateM = PARSING_ERROR;
            return ERROR;

         }

         case CHECK_DATA_EXTENSION_LABEL:
         {
            assert(theInputBuffer.length() >= decodeIndex);
            if (theInputBuffer.length() == decodeIndex){
               result = PARTLY;
               break;
            }

            /* Graphic Block */
            const unsigned char label = theInputBuffer[decodeIndex];
            if (0xF9 == label)
            {
               stateM = PARSING_GRAPHIC_CONTROL_EXTENSION;
               result = DONE;
               break;
            }
            if (0x01 == label)
            {
               stateM = PARSING_PLAIN_TEXT_EXTENSION;
               result = DONE;
               break;
            }
            if (0xFF == label)
            {
               stateM = PARSING_APPLICATION_EXTENSION;
               result = DONE;
               break;
            }
            if (0xFE == label)
            {
               stateM = PARSING_COMMENT_EXTENSION;
               result = DONE;
               break;
            }
				stateM = PARSING_ERROR;
            return ERROR;

         }
         
         /* Graphic Block */
         case PARSING_GRAPHIC_CONTROL_EXTENSION:
         {
            gif_graphic_ctrl_ext_t gifStruct;
				result = decodeStruct((char *) &gifStruct, sizeof(gif_graphic_ctrl_ext_t), bufferM, theInputBuffer, decodeIndex);
				if (DONE != result)
               break;
            
            stateM = CHECK_DATA_INTRODUCOR;
            if (0 != handlerM->handle(gifStruct, theOutputBuffer))
            {
            	stateM = PARSING_ERROR;
               return ERROR;
            }
               
				break;

         }

			/* Special-Purpose Block */
			case PARSING_APPLICATION_EXTENSION:
			{
				gif_appl_ext_t gifStruct;
				result = decodeStruct((char *) &gifStruct, sizeof(gif_appl_ext_t), bufferM, theInputBuffer, decodeIndex);
				if (DONE != result)
               break;
            
            stateM = CHECK_DATA_INTRODUCOR;
            if (0 != handlerM->handle(gifStruct, theOutputBuffer))
            {
            	stateM = PARSING_ERROR;
               return ERROR;
            }
               
				break;
			}

			case PARSING_ERROR:
			default:
			{
				stateM = PARSING_ERROR;
				return ERROR;
			}

		}// switch
	}
	if (ERROR == result)
	{
		stateM = PARSING_ERROR;
	}
   return result;

}

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



int GifDumper::exec(gif_header_t &theGifStruct, string &theOutputBuffer)
{
	cout << "header:" << endl
		<< "\t signature:" << string(theGifStruct.signature, sizeof (theGifStruct.signature)) << endl
		<< "\t version:" << string(theGifStruct.version, sizeof (theGifStruct.signature)) << endl;
   return 0;
}
  
int GifDumper::exec(gif_lgc_scr_desc_t &theGifStruct, string &theOutputBuffer)
{
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
   cout << "Logical Screen Descriptor:" << endl
      << "\t ..." << endl;
   return 0;
}

int GifDumper::exec(gif_graphic_ctrl_ext_t &theGifStruct, string &theOutputBuffer)
{
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
   return 0;
}

int GifDumper::exec(gif_plain_text_ext_t &theGifStruct, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_appl_ext_t &theGifStruct, string &theOutputBuffer)
{
	cout << "Application Extension:" << endl
		<< "\t ext_introducer:" << (int)theGifStruct.ext_introducer << endl
		<< "\t plain_text_lable:" << (int)theGifStruct.plain_text_lable << endl
		<< "\t block_size:" << (int)theGifStruct.block_size << endl
		<< "\t identifier:" << string(theGifStruct.identifier, 8) << endl
		<< "\t appl_auth_code:" << string(theGifStruct.appl_auth_code, 3) << endl;
   return 0;
}

int GifDumper::exec(gif_comment_ext_t &theGifStruct, string &theOutputBuffer)
{
   return 0;
}

int GifDumper::exec(gif_trailer_t &theGifStruct, string &theOutputBuffer)
{
   return 0;
}

