
CCFLAGS=-I.

SRC= \
    GifDataStreamDecoder.cpp \
    GifDumper.cpp \
    GifEncoder.cpp \
    GifResizer.cpp \
    LzwCompressor.cpp \
    LzwDecompressor.cpp \
    main.cpp 

OBJS=$(patsubst %.cpp, %.o, $(SRC) )

all:$(OBJS)
	g++ -o test $(OBJS)

%.o: %.cpp
	g++ -g -c $< -o $@ $(CCFLAGS) 

clean:
	rm -rf *.o
