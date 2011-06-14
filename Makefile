
CCFLAGS=-I.

SRC=main.cpp GifStreamCodec.cpp

OBJS=$(patsubst %.cpp, %.o, $(SRC) )

all:$(OBJS)
	g++ -o test $(OBJS)

%.o: %.cpp
	g++ -g -c $< -o $@ $(CCFLAGS) 

clean:
	rm -rf *.o
