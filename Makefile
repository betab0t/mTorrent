PROG = mTorrent
CC = g++
CPPFLAGS = -g -Wall --std=c++14
LIBS = -lssl -lcrypto -lcurl -lnsl
OBJS = main.o bencode.o tracker.o

$(PROG) : $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)

main.o : bencode.h
	$(CC) $(CPPFLAGS) -c main.cpp

bencode.o : bencode.h
	$(CC) $(CPPFLAGS) -c bencode.cpp

tracker.o : tracker.h
	$(CC) $(CPPFLAGS) -c tracker.cpp

clean:
	rm $(PROG) $(OBJS)