#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "GifStreamCodec.h"
using namespace IMAGELIB;
using namespace IMAGELIB::GIFLIB;


void testDecoder()
{
	char buf[512];
	ifstream in;
	in.open ("test.gif", ios::binary );
    
	GifDataStreamDecoder gif_data(new GifDumper(new GifEncoder));
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
			return;
	}
   in.close();
   
   ofstream out;
   out.open ("out.gif", ofstream::binary );
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
	IzwCompressor compressor;
	compressor.init(2);
	compressor.compress(input, output);
	compressor.writeEof(output);
	for (int i = 0; i < output.length(); i++)
	{
		assert(output[i] == rightOutput[i]);
	}
}

void testLzwDecompress()
{
	string output;
	gif_image_data_block_t input = {7, 
		{0x84, 0x02, 0x10, 0x92, 0xb7, 0x0c, 0x0a}};
	const unsigned char rightOutput[] = 
		{0, 2, 1, 0, 0, 1, 2, 0, 0, 2, 1, 1, 2, 2, 0, 0};
	IzwDecompressor decompressor;
	decompressor.init(2);
	decompressor.decompress(input, output);
	for (int i = 0; i < output.length(); i++)
	{
		assert(output[i] == rightOutput[i]);
	}
	
}

int main()
{
	testLzwCompress();
	testLzwDecompress();
	testDecoder();
	return 0;
}

