PREFIX?=/usr/local
INCLUDE_DIR=$(PREFIX)/include
LIB_DIR=$(PREFIX)/lib

LIB=libcjson.a
HEADER=cjson.h

DISABLED_WARNINGS=switch
CFLAGS=-g -Wall -std=gnu11 -Ilibcollect $(patsubst %, -Wno-%, $(DISABLED_WARNINGS))
LDFLAGS=-Llibcollect
LDLIBS=-lcollect
YACC=lemon/lemon
LEX=flex

SOURCES=src/cjson.c src/lexer.l src/gram.y
COBJECTS=$(patsubst %.c, %.o, $(SOURCES))
LOBJECTS=$(patsubst %.l, %.o, $(COBJECTS))
OBJECTS=$(patsubst %.y, %.o, $(LOBJECTS))

.PHONY: all clean lemon libcollect getexternals

all: $(LIB)

$(LIB): $(OBJECTS) src/lexer.c libcollect
	ar -rcs $@ $(OBJECTS)

src/cjson.o: src/cjson.c src/lexer.c

src/lexer.c: src/lexer.l src/gram.c
	$(LEX) --header-file=src/lexer.h -o $@ $<

src/gram.c: src/gram.y lemon
	$(YACC) $<

clean:
	rm -f $(LIB) $(OBJECTS) src/lexer.c src/lexer.h src/gram.c src/gram.h src/gram.out

install:
	mkdir -p $(INCLUDE_DIR)
	mkdir -p $(LIB_DIR)
	install -m 444 $(LIB) $(LIB_DIR)/$(LIB)
	install -m 644 src/$(HEADER) $(INCLUDE_DIR)/$(HEADER)

uninstall:
	rm -f $(LIB_DIR)/$(LIB)
	rm -f $(INCLUDE_DIR)/$(HEADER)

libcollect:
	@- make -C libcollect

lemon:
	@- make -C lemon

getexternals:
	git submodule init
	git submodule update
