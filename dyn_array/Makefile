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


#############
# utilities #
#############

CC= cc
RM= rm
SHELL= bash


#######################
# Makefile parameters #
#######################

# linker options
#
LDFLAGS=

# This repo supports c11 and later.
#
# NOTE: The use of -std=gnu11 is because there are a few older systems
#	in late 2021 that do not have compilers that (yet) support gnu17.
#	While there may be even more out of date systems that do not
#	support gnu11, we have to draw the line somewhere.
#
#	--------------------------------------------------
#
#	^^ the line is above :-)
#
# XXX - ###################################################################### - XXX #
# XXX - In 2024 we will will support only c17 so C_STD will become c-std=gnu17 - XXX #
# XXX - ###################################################################### - XXX #
#
C_STD= -std=gnu11
#C_STD= -std=gnu17

# optimization and debug level
#
#COPT= -O3 -g3	# XXX - this will be the production release value - XXX #
COPT= -O0 -g

# Compiler warnings
#
#WARN_FLAGS= -Wall -Wextra	# XXX - this will be the production release value - XXX #
WARN_FLAGS= -Wall -Wextra -Werror

# how to compile
#
# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
CFLAGS= ${C_STD} ${COPT} -pedantic ${WARN_FLAGS} ${LDFLAGS}

# where and what to install
#
MAN1_DIR = /usr/local/share/man/man1
MAN8_DIR = /usr/local/share/man/man8
MAN3_DIR = /usr/local/share/man/man3
DESTDIR= /usr/local/bin
TARGETS= dyn_array.o


######################################
# all - default rule - must be first #
######################################

all: ${TARGETS}
	@:

.PHONY: all

dyn_array.o: dyn_array.c dyn_array.h Makefile
	${CC} ${CFLAGS} dyn_array.c -c


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	@:

clobber: clean
	${RM} -f ${TARGETS}

install: all
	@echo nothing to $@
