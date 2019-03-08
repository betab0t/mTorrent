PROG = mTorrent
CC = g++
CPPFLAGS = -g -Wall --std=c++14
OBJS = main.o bencode.o

$(PROG) : $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS)

main.o : bencode.h
	$(CC) $(CPPFLAGS) -c main.cpp

bencode.o : bencode.h
	$(CC) $(CPPFLAGS) -c bencode.cpp

clean:
	rm $(PROG) $(OBJS)