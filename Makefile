#!/usr/bin/make
#
# dbg - example of how to use usage(), dbg(), warn(), err()
#
# Copyright (c) 1989,1997,2018-2022 by Landon Curt Noll.  All Rights Reserved.
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
CFLAGS= -O3 -g3 --pedantic -Wall -Werror -DDBG_TEST
#CFLAGS= -O3 -g3 --pedantic -Wall -Werror -DDBG_TEST -DDBG_LINT
RM= rm

DESTDIR= /usr/local/bin

TARGETS= dbg_test

all: ${TARGETS}

dbg_test: dbg.c dbg.h Makefile
	${CC} ${CFLAGS} dbg.c -o $@

configure:
	@echo nothing to $@

clean:
	${RM} -f dbg.o dbg_test.o
	${RM} -rf dbg_test.dSYM

clobber: clean
	${RM} -f ${TARGETS}

install: all
	@echo nothing to $@
