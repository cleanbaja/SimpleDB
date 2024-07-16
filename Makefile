CFLAGS = -Og -g -pipe -march=native -isystem .

LIB_SRC = common/lib.c
LIB_OBJ = ${LIB_SRC:.c=.o}

all: test-smdb

.c.o:
	${CC} -c ${CFLAGS} -o $@ $<

${LIB_OBJ}: smdb.h

libsmdb.a: ${LIB_OBJ}
	ar rcs $@ $<

test-smdb: libsmdb.a smdb.h test.c
	${CC} ${CFLAGS} -o $@ test.c libsmdb.a

clean:
	rm -f *.o common/*.o libsmdb.a test-smdb

.PHONY: all clean