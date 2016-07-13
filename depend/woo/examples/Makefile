include ../Makefile.in

INC_PATH=-I../include
LIB_LINK=-L../lib -lwoo -lpthread -lstdc++
ALL_BIN=binaryserver scgiserver binaryclient

all:$(ALL_BIN)

scgiserver : scgiserver.o
	$(CC) $(CFLAGS) -o $@ $^  $(LIB_LINK)

binaryserver : binaryserver.o
	$(CC) $(CFLAGS) -o $@ $^  $(LIB_LINK)

binaryclient : binaryclient.o
	$(CC) $(CFLAGS) -o $@ $^  $(LIB_LINK)


%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH)

clean:
	rm $(ALL_BIN) *.o
