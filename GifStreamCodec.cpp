#include "GifStreamCodec.h"
#include <iostream>
using namespace std;


template<typename ImageStruct>
void compileBufferAndInput(ImageStruct &theImageStruct, 
   string &theBuffer, 
   const char* theInputBuffer, const size_t theInputLen)
{
   size_t bufferLen = theBuffer.length();
   memcpy(&theImageStruct, theBuffer.c_str(), bufferLen);
   memcpy(((char*)&theImageStruct) + bufferLen, theInputBuffer, sizeof(gif_header_t) - bufferLen);
   theBuffer.clear();
}

result 
GifDataStreamDecoder::decode(const char* theInputBuffer, size_t theInputLen, string &theOutputBuffer)
{
   switch(stateM)
	{
      case PARSING_HEADER:
         return decodeHeader(theInputBuffer, theInputLen, theOutputBuffer);

	}
   return ERROR;

}



result GifDataStreamDecoder::decodeHeader(const char* theInputBuffer, size_t theInputLen, string &theOutputBuffer)
{
   size_t bufferLen = bufferM.length();
   if (theInputLen + bufferLen < sizeof(gif_header_t))
   {
      bufferM.append(theInputBuffer, theInputLen);
      bufferLen += theInputLen; 
      return PARTLY;
   }else
   {
      gif_header_t header;
      compileBufferAndInput(header, bufferM, theInputBuffer, theInputLen);
      if (0 != handlerM->exec(header, theOutputBuffer))
      {
         return ERROR;
      }

      //decode next
      int leftLen = theInputLen + bufferLen - sizeof(gif_header_t);
      const char* leftIn = theInputBuffer + bufferLen - sizeof(gif_header_t);    
      stateM = PARSING_LOGICAL_SCREEN;      
      return decode(leftIn, leftLen, theOutputBuffer);
   }
   return ERROR;

}

result GifLogicalScreenDecoder::decode(const char* theInputBuffer, size_t theInputLen, string &theOutputBuffer)
{
   switch(stateM)
	{
      case PARSING_LOGICAL_SCREEN_DESC:
         return decodeLogicalScreenDesc(theInputBuffer, theInputLen, theOutputBuffer);
         
   }
   return ERROR;
}

result GifLogicalScreenDecoder::decodeLogicalScreenDesc(const char* theInputBuffer, size_t theInputLen, string &theOutputBuffer)
{
   size_t bufferLen = bufferM.length();
   if (theInputLen + bufferLen < sizeof(gif_header_t))
   {
      bufferM.append(theInputBuffer, theInputLen);
      bufferLen += theInputLen; 
      return PARTLY;
   }else
   {
      gif_header_t header;
      compileBufferAndInput(header, bufferM, theInputBuffer, theInputLen);
      if (0 != handlerM->exec(header, theOutputBuffer))
      {
         return ERROR;
      }

      //decode next
      int leftLen = theInputLen + bufferLen - sizeof(gif_header_t);
      const char* leftIn = theInputBuffer + bufferLen - sizeof(gif_header_t);    
      stateM = PARSING_LOGICAL_SCREEN;      
      return decode(leftIn, leftLen, theOutputBuffer);
   }
   return ERROR;

}

int GifEncoder::exec(gif_header_t &theHeader, string &theOutputBuffer)  
{
   theOutputBuffer.append((const char*)&theHeader, sizeof(gif_header_t));   
   cout << "header:" <<string((const char*)&theHeader, sizeof(gif_header_t))<< endl; 
   return 0;
}
