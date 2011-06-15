#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "GifStreamCodec.h"

int main()
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
      if (!output.empty()){
         cout << "total output size:" << output.length() << endl;
      }
		if (result != PARTLY)
			return 0;
	}

	return 0;
}

