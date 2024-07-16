CFLAGS = -Og -g -pipe -march=native -isystem .

LIB_SRC = common/lib.c
LIB_OBJ = ${LIB_SRC:.c=.o}
SERVER_SRC = common/server.c
SERVER_OBJ = ${SERVER_SRC:.c=.o}

all: smdb-test smdb-server

.c.o:
	${CC} -c ${CFLAGS} -o $@ $<

${LIB_OBJ}: smdb.h

libsmdb.a: ${LIB_OBJ}
	ar rcs $@ $<

smdb-server: ${SERVER_OBJ}
	${CC} ${CFLAGS} -o $@ $<

smdb-test: libsmdb.a smdb.h test.c
	${CC} ${CFLAGS} -o $@ test.c libsmdb.a

clean:
	rm -f *.o common/*.o libsmdb.a smdb-test smdb-server

.PHONY: all clean