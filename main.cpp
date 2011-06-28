#include <assert.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;


#include "GifDataStreamDecoder.h"
//#include "PlainData.h"
//#include "CompressedData.h"
using namespace IMAGELIB;
using namespace IMAGELIB::GIFLIB;


void testDecoder(const string& theInputFileName)
{
	char buf[512];
	ifstream in;
	in.open (theInputFileName.c_str(), ios::binary );
	if (!in.is_open())
	{
		cout << "can't open file:" << theInputFileName << endl;
		return;
	}
   cout << "input file:" << theInputFileName << endl;
    
	GifDataStreamDecoder gif_data(
      new GifResizer(2.7, 
      //new GifDumper(
      new GifEncoder));//);
   string output;

	int totalSize = 0;
	while (!in.eof()){
		in.read(buf, 512);
		Result result = gif_data.decode(string(buf, in.gcount()), output);
      
		//cout << "handle " << in.gcount() << "B" << endl;
      //if (!output.empty()){
      //   cout << "total output size:" << output.length() << endl;
      //}
		if (result == ERROR)
		{
         cout << "decode error!" << endl;
			return;
		}
	}
   in.close();

	string outputFileName = string("tiny_") + theInputFileName;
   ofstream out;
   out.open (outputFileName.c_str(), ofstream::binary );
   out.write(output.c_str(), output.length());
   out.close();

}

void testLzwCompress()
{
	unsigned char inputArray[] = 
		{0, 2, 1, 0, 0, 1, 2, 0, 0, 2, 1, 1, 2, 2, 0, 0};
	const char rightOutput[] = 
		{0x84, 0x02, 0x10, 0x92, 0xb7, 
		 0x0c, 0x0a};
	string input((char*)inputArray, sizeof(inputArray));
	string output;
	LzwCompressor compressor;
	compressor.init(2);
	compressor.compress(input, output);
	compressor.writeEof(output);
	for (int i = 0; i < output.length(); i++)
	{
		assert(output[i] == rightOutput[i]);
	}
}
/*
void testLzwCompressLargeData()
{
   cout << "==================================================================================" << endl;
   cout << "--------------------------testLzwCompressLargeData--------------------------------" << endl;
   cout << "total output data should be " << sizeof(COMPRESSED_DATA)  << endl; 
	string input(GIF_PLAIN_DATA, sizeof(GIF_PLAIN_DATA));
	string output;
	LzwCompressor compressor;
	compressor.init(8);
	compressor.compress(input, output);
	compressor.writeEof(output);
	for (int i = 0; i < output.length(); i++)
	{
		if (output[i] != COMPRESSED_DATA[i]){
			cout << "unmatch data at " << i << endl;
			cout << "expect: 0x" << std::hex << ((int)COMPRESSED_DATA[i]&0xFF) 
              << ", actually: 0x" << std::hex << ((int)output[i]&0xFF)<< endl;
		}
		assert(output[i] == COMPRESSED_DATA[i]);
	}
   cout << "==================================================================================" << endl;
}
*/


void testLzwDecompress()
{
	string output;
	gif_image_data_block_t input = {7, 
		{0x84, 0x02, 0x10, 0x92, 0xb7, 0x0c, 0x0a}};
	const unsigned char rightOutput[] = 
		{0, 2, 1, 0, 0, 1, 2, 0, 0, 2, 1, 1, 2, 2, 0, 0};
	LzwDecompressor decompressor;
	decompressor.init(2);
	decompressor.decompress(input, output);
	for (int i = 0; i < output.length(); i++)
	{
		assert(output[i] == rightOutput[i]);
	}
	
}

/*
void testLzwDecompressLargeData()
{
	string output;
	gif_image_data_block_t input;
	LzwDecompressor decompressor;
	decompressor.init(8);
	int totalIndex = 0;
	int totalInputLen = sizeof(COMPRESSED_DATA);
	int totalInputIndex = 0;
	while(totalInputLen > 0){
		int inputLen = (totalInputLen>255)?255:totalInputLen;
		input.block_size = inputLen;
		memcpy(input.data_value, COMPRESSED_DATA + totalInputIndex, 255);
		totalInputIndex += inputLen;
		totalInputLen -= inputLen;

		output.clear();
		decompressor.decompress(input, output);
		for (int i = 0; i < output.length(); i++)
		{
			cout << totalIndex << endl;
			assert(output[i] == GIF_PLAIN_DATA[totalIndex++]);
		}
	}
	
}
*/

int main(int argc, char* argv[])
{
	//testLzwCompress();
	//testLzwDecompress();
	//testLzwDecompressLargeData();
	//testLzwCompressLargeData();
	if (argc < 2)
	{
		cout << "usage: gifresizer.exe filename1 filename2 ..." << endl;
		return 0;
	}
	for (int i = 1; i < argc; i++)
	{
		testDecoder(string(argv[i]));
	}
	return 0;
}

