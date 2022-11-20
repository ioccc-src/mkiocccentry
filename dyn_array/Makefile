#!/usr/bin/make
#
# dyn_array - dynamic array facility
#
# Copyright (c) 2014,2015,2022 by Landon Curt Noll.  All Rights Reserved.
# 
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby granted,
# provided that the above copyright, this permission notice and text
# this comment, and the disclaimer below appear in all of the following:
# 
#        supporting documentation
#        source copies
#        source works derived from this source
#        binaries derived from this source or from derived source
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
CFLAGS= -O3 -g3 -pedantic -Wall
#CFLAGS= -O3 -g3 -pedantic -Wall -Werror
CP= cp
RM= rm
GREP= grep
CAT= cat

DESTDIR= /usr/local/bin

TARGETS= dyn_array.o

.PHONY: all

all: ${TARGETS}
	@:

dyn_array.o: dyn_array.c dyn_array.h Makefile
	${CC} ${CFLAGS} dyn_array.c -c

configure:
	@echo nothing to $@

clean:
	@:

clobber: clean
	${RM} -f ${TARGETS}

install: all
	@echo nothing to $@
