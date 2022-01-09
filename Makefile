#!/usr/bin/make
#
# mkiocccentry - make an ioccc entry
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


SHELL= /bin/bash

CC= cc
#CFLAGS= -O3 -g3 --pedantic -Wall
CFLAGS= -O3 -g3 --pedantic -Wall -Werror
## If you use clang ASAN, set this environment var: ASAN_OPTIONS="detect_stack_use_after_return=1"
#CFLAGS= -O3 -g3 --pedantic -Wall -Werror -fsanitize=address -fno-omit-frame-pointer
#CFLAGS= -O3 -g3 --pedantic -Wall -Werror -DDEBUG_LINT
#CFLAGS= -O0 -g --pedantic -Wall -Werror
RM= rm
INSTALL= install

DESTDIR= /usr/local/bin

TARGETS= mkiocccentry

all: ${TARGETS}

mkiocccentry: mkiocccentry.c
	${CC} ${CFLAGS} mkiocccentry.c -o $@

configure:
	@echo nothing to configure

clean quick_clean quick_distclean distclean:
	${RM} -f mkiocccentry.o
	${RM} -rf mkiocccentry.dSYM

clobber quick_clobber: clean
	${RM} -r mkiocccentry

install: all
	${INSTALL} -m 0555 ${TARGETS} ${DESTDIR}
