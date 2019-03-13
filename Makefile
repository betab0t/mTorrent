PROG = mTorrent
CC = g++
CPPFLAGS = -g -Wall --std=c++14
OBJS = main.o bencode.o tracker.o utils.o

$(PROG) : $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS)

main.o : bencode.h
	$(CC) $(CPPFLAGS) -c main.cpp

bencode.o : bencode.h
	$(CC) $(CPPFLAGS) -c bencode.cpp

tracker.o : tracker.h
	$(CC) $(CPPFLAGS) -c tracker.cpp

utils.o : utils.h
	$(CC) $(CPPFLAGS) -c utils.cpp

clean:
	rm $(PROG) $(OBJS)