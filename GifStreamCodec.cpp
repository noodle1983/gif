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
         result = decodeHeader(theInputBuffer, decodeIndex, theOutputBuffer);
			if (DONE == result)
				stateM = PARSING_LOGICAL_SCREEN;
			break;
	   case PARSING_LOGICAL_SCREEN:
			result = logicalScreenDecoderM.decode(theInputBuffer, decodeIndex, theOutputBuffer);
			if (DONE == result)
				stateM = PARSING_DATA;
			break;

		}
	}
   return result;

}



Result GifDataStreamDecoder::decodeHeader(const string &theInputBuffer, int& theDecodeIndex, string &theOutputBuffer)
{
   gif_header_t header;
	Result result = decodeStruct((char *) &header, sizeof(gif_header_t), bufferM, theInputBuffer, theDecodeIndex);
   if (DONE != result)
		return result;
	
   if (0 != handlerM->exec(header, theOutputBuffer))
   {
      return ERROR;
   }      
   return DONE;

}

Result GifLogicalScreenDecoder::decode(const string &theInputBuffer, int& theDecodeIndex, string &theOutputBuffer)
{
	Result result = DONE;
   switch(stateM)
	{
      case PARSING_LOGICAL_SCREEN_DESC:
			result = decodeLogicalScreenDesc(theInputBuffer, decodeIndex, theOutputBuffer);
			if (DONE == result)
				stateM = PARSING_GLOBAL_COLOR_TABLE;
			break;
         //return decodeLogicalScreenDesc(theInputBuffer, theInputLen, theOutputBuffer);
         
   }
   return result;
}



int GifEncoder::exec(gif_header_t &theHeader, string &theOutputBuffer)  
{
   theOutputBuffer.append((const char*)&theHeader, sizeof(gif_header_t));   
   cout << "header:" <<string((const char*)&theHeader, sizeof(gif_header_t))<< endl; 
   return 0;
}
