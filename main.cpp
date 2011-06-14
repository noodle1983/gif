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
    
	GifDataStreamDecoder gif_data(new GifEncoder);
   string output;
	
	while (!in.eof()){
		in.read(buf, 512);
		gif_data.decode(buf, in.gcount(), output);
      
		//cout << "handle " << in.gcount() << "B" << endl;
      if (!output.empty()){
         cout << "[output:" << output << "]" << endl;
         output.clear();
      }
	}

	return 0;
}

