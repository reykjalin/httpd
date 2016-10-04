CC = gcc
CPPFLAGS =
CFLAGS = -std=c11 -D_XOPEN_SOURCE=700 -O2 -Wall -Wextra -Wformat=2
LDFLAGS = `pkg-config --cflags --libs glib-2.0`
LOADLIBES =
LDLIBS = requestParser.o responseBuilder.o utility.o logger.o

.DEFAULT: all
.PHONY: all
all: httpd

httpd: obj
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) httpd.c -o serv

obj:
	$(CC) $(CFLAGS) $(LDFLAGS) -c requestParser.c -o requestParser.o
	$(CC) $(CFLAGS) $(LDFLAGS) -c responseBuilder.c -o responseBuilder.o
	$(CC) $(CFLAGS) $(LDFLAGS) -c utility.c -o utility.o
	$(CC) $(CFLAGS) $(LDFLAGS) -c logger.c -o logger.o

clean:
	rm -f *.o

distclean: clean
	rm -f httpd
