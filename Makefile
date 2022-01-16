#!/usr/bin/env make
#
# iocccsize - IOCCC Source Size Tool
#
# This IOCCC size tool source file.  See below for the VERSION string.
#
# "You are not expected to understand this" :-)
#
# Public Domain 1992, 2015, 2018, 2019 by Anthony Howe.  All rights released.
# With IOCCC minor mods in 2019,2021,2022 by chongo (Landon Curt Noll) ^oo^

# utilities
#
CC= cc
RM= rm

# compile and link options
#
CFLAGS= -O3 -g3 -Wall -Wextra -pedantic
LFLAGS=

# where to install
#
DESTDIR= /usr/local/bin

all: iocccsize

# rules, not file targets
#
.PHONY: all configure clean clobber install test

iocccsize: iocccsize.c Makefile
	${CC} ${CFLAGS} iocccsize.c ${LDFLAGS} -o $@

clean:
	${RM} -f iocccsize.o
	${RM} -rf iocccsize.dSYM

clobber: clean
	${RM} -f iocccsize

install: all
	${INSTALL} -m 0555 iocccsize ${DESTDIR}

test: ./iocccsize-test.sh iocccsize Makefile
	./iocccsize-test.sh -v
