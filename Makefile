CC=cc
CFLAGS=-Wall --std=c99 --pedantic -g -O0
LDFLAGS=
PARTS=alloc rb-tree seq set sha1
SOURCES=$(PARTS:=.c)
OBJECTS=$(PARTS:=.o)
TESTS=$(addprefix tests/, $(PARTS:=-test))
TESTS_SOURCES=$(TESTS:=.c)

all: libutils

libutils: $(OBJECTS) 
	echo $(PARTS)
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(TESTS)

dist:
	tar cfv libutils.tar $(SOURCES) $(SOURCES:.c=.h)

distclean:
	rm -rf libutils.tar

check: $(TESTS)

tests/cmockery.o: tests/cmockery.c
	$(CC) $(CFLAGS) -Itests -w $< -o $@

$(TESTS): tests/cmockery.o $(OBJECTS)
	$(CC) $(CFLAGS) -I. -Itests $(@:=.c) $(OBJECTS) $< -o $@
