#!/usr/bin/make
#
# test - mkiocccentry test tools
#


SHELL= /bin/bash

TARGETS= utf8_test jnum_chk dyn_test


all: ${TARGETS}

utf8_test.o: utf8_test.c ../utf8_posix_map.h Makefile
	${CC} ${CFLAGS} -I.. utf8_test.c -c

utf8_test: utf8_test.o ../utf8_posix_map.o ../dbg ../util.o ../dyn_array.o Makefile
	${CC} ${CFLAGS} -I.. utf8_test.o ../utf8_posix_map.o ../dbg/dbg.o ../util.o ../dyn_array.o -o utf8_test

dyn_test.o: dyn_test.c Makefile
	${CC} ${CFLAGS} -I.. dyn_test.c -c

dyn_test: dyn_test.o ../dbg ../util.o ../dyn_array.o Makefile
	${CC} ${CFLAGS} -I.. dyn_test.o ../dbg/dbg.o ../util.o ../dyn_array.o -o dyn_test

jnum_test.o: jnum_test.c Makefile
	${CC} ${CFLAGS} -I.. jnum_test.c -c

jnum_chk.o: jnum_chk.c jnum_chk.h Makefile
	${CC} ${CFLAGS} -I.. jnum_chk.c -c

jnum_chk: jnum_chk.o ../dbg ../json_parse.o ../json_util.o ../util.o ../dyn_array.o jnum_test.o Makefile
	${CC} ${CFLAGS} -I.. jnum_chk.o ../dbg/dbg.o ../json_parse.o ../json_util.o ../util.o ../dyn_array.o \
				 jnum_test.o -lm -o jnum_chk

configure:
	@echo nothing to $@

clean:
	${RM} -f *.o

clobber: clean
	${RM} -f ${TARGETS}

install: all
	@echo nothing to $@
