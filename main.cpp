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
	GifDataStream gif_data;
	
	while (!in.eof()){
		in.read(buf, 512);
		gif_data.decode(buf, in.gcount());
		cout << "handle " << in.gcount() << "B" << endl;
	}

	return 0;
}

