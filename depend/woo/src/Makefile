include ../Makefile.in
LIB=../lib/libwoo.a
all:$(LIB)

$(LIB):tcpserver.o log.o scgi.o config.o binaryclient.o
	test -d ../lib || mkdir ../lib
	ar -rc $@ $^
	ranlib $@
	
%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ -I../include/

clean:
	rm $(LIB) *.o
