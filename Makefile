#!/usr/bin/env make
#
# mkiocccentry and iocccsize - make an IOCCC entry AND IOCCC Rule 2b size tool
#
# For mkiocccentry:
#
# Copyright (c) 2021,2022 by Landon Curt Noll.  All Rights Reserved.
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby granted,
# provided that the above copyright, this permission notice and text
# this comment, and the disclaimer below appear in all of the following:
#
#       supporting documentation
#       source copies
#       source works derived from this source
#       binaries derived from this source or from derived source
#
# LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
# EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
# USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#
# chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# Share and enjoy! :-)
####

####
# For iocccsize:
#
# This IOCCC size tool source file.  See below for the VERSION string.
#
# "You are not expected to understand this" :-)
#
# Public Domain 1992, 2015, 2018, 2019 by Anthony Howe.  All rights released.
# With IOCCC minor mods in 2019,2021,2022 by chongo (Landon Curt Noll) ^oo^
####


# utilities
#
CC= cc
RM= rm
CP= cp
INSTALL= install

# how to compile
#
CFLAGS= -O3 -g3 --pedantic -Wall -Wextra
#CFLAGS= -O3 -g3 --pedantic -Wall -Wextra -Werror
#CFLAGS= -O3 -g3 --pedantic -Wall -Wextra -Werror -DDEBUG_LINT
## If you use ASAN, set this environment var: ASAN_OPTIONS="detect_stack_use_after_return=1"
#CFLAGS= -O0 -g --pedantic -Wall -Wextra -Werror -fsanitize=address -fno-omit-frame-pointer
# For valgrind, run with: valgrind --leak-check=yes --track-origins=yes --leak-resolution=high --read-var-info=yes
#CFLAGS= -O0 -g --pedantic -Wall -Wextra -Werror

# where and what to install
#
DESTDIR= /usr/local/bin
TARGETS= mkiocccentry iocccsize

all: ${TARGETS}

# rules, not file targets
#
.PHONY: all configure clean clobber install test

rule_count.c: iocccsize.c
	${RM} -f $@
	${CP} -f $? $@

rule_count.o: rule_count.c limit_ioccc.h Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE rule_count.c -c

mkiocccentry: mkiocccentry.c limit_ioccc.h rule_count.o Makefile
	${CC} ${CFLAGS} mkiocccentry.c rule_count.o -o $@

iocccsize: iocccsize.c limit_ioccc.h Makefile
	${CC} ${CFLAGS} iocccsize.c -o $@

configure:
	@echo nothing to configure

clean:
	${RM} -f mkiocccentry.o iocccsize.o rule_count.o
	${RM} -rf mkiocccentry.dSYM iocccsize.dSYM

clobber: clean
	${RM} -f ${TARGETS} rule_count.c
	${RM} -rf test-iocccsize

install: all
	${INSTALL} -m 0555 ${TARGETS} ${DESTDIR}

test: ./iocccsize-test.sh iocccsize Makefile
	./iocccsize-test.sh -v
