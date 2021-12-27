#!/usr/bin/env make
#
# iocccsize - IOCCC Source Size Tool
#
# This IOCCC size tool source file.  See below for the VERSION string.
#
# "You are not expected to understand this" :-)
#
# Public Domain 1992, 2015, 2018, 2019 by Anthony Howe.  All rights released.
# With IOCCC minor mods in 2019,2021 by chongo (Landon Curt Noll) ^oo^

.POSIX :

GIT= git
RM= rm

O := .o
E :=

.SUFFIXES :
.SUFFIXES : .c .h .i $O $E

########################################################################
### No further configuration beyond this point.
########################################################################

top_srcdir	:= ..
PROJ 		:= iocccsize
TAR_I		:= -T
CFLAGS		:= -g -std=c11 -Wall -Wextra -Wno-char-subscripts -pedantic
CFLAGS89	:= -g -std=c89 -Wall -Wextra -Wno-char-subscripts -pedantic
CPPFLAGS	:=
LIBS		:=

.c.i:
	${CC} -E ${CFLAGS} ${CPPFLAGS} $*.c >$*.i

.c$E:
	${CC} ${CFLAGS} ${CPPFLAGS} -o $*$E $*.c

#######################################################################

all: build
	@true

build: ${PROJ}

clean:
	${RM} -f ${PROJ}.i ${PROJ}$O *.stackdump *.core
	${RM} -rf ${PROJ}.dSYM

clobber: distclean

distclean: clean
	${RM} -fr ${PROJ}$E test a.out
	${RM} -fr decom a.out iocccsize.dSYM

test: ./${PROJ}-test.sh ${PROJ}
	./${PROJ}-test.sh -v

unittest: test
	./${PROJ}-test.sh -v

version:
	${GIT} describe --tags --abbrev=0
