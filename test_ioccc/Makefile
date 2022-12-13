#!/usr/bin/make
#
# test - mkiocccentry test tools
#
# Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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


#############
# utilities #
#############

# suggestion: List utility filenames, not paths.
#	      Do not list shell builtin (echo, cd, ...) tools.
#	      Keep the list in alphabetical order.
#

AWK= awk
BASENAME= basename
CAT= cat
CC= cc
CHECKNR = checknr
CMP = cmp
CP= cp
CTAGS= ctags
CUT = cut
DIFF= diff
FMT= fmt
GIT= git
GREP= grep
HEAD= head
INSTALL= install
MAKE= make
MAN2HTML= man2html
MAN= man
MKTEMP= mktemp
MV= mv
PICKY= picky
RM= rm
RPL= rpl
RSYNC= rsync
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
TEE= tee
TR= tr
TRUE= true


#######################
# Makefile parameters #
#######################

# linker options
#
LDFLAGS=

# C source standards being used
#
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
# TODO - ###################################################################### - TODO #
# TODO - In 2023 we will will support only c17 so C_STD will become c-std=gnu17 - TODO #
# TODO - ###################################################################### - TODO #
#
C_STD= -std=gnu11
#C_STD= -std=gnu17

# optimization and debug level
#
#COPT= -O3 -g3	# TODO - this will be the production release value - TODO #
COPT= -O0 -g

# Compiler warnings
#
#WARN_FLAGS= -Wall -Wextra	# TODO - this will be the production release value - TODO #
WARN_FLAGS= -Wall -Wextra -Werror


# how to compile
#
# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
CFLAGS= ${C_STD} ${COPT} -pedantic ${WARN_FLAGS} ${LDFLAGS}


# NOTE: If you use ASAN, set this environment var:
#	ASAN_OPTIONS="detect_stack_use_after_return=1"
#
#CFLAGS= ${C_STD} -O0 -g -pedantic ${WARN_FLAGS} ${LDFLAGS} -fsanitize=address -fno-omit-frame-pointer

# NOTE: For valgrind, run with:
#
#	valgrind --leak-check=yes --track-origins=yes --leak-resolution=high --read-var-info=yes \
#           --leak-check=full --show-leak-kinds=all ./mkiocccentry ...
#
# NOTE: Replace mkiocccentry with whichever tool you want to test and the ...
# with the arguments and options you want.

# where and what to install
#
DESTDIR= /usr/local/bin
TARGETS= utf8_test
SH_TARGETS=


######################################
# all - default rule - must be first #
######################################

all: ${TARGETS}

.PHONY: all test configure clean clobber install

utf8_test.o: utf8_test.c ../utf8_posix_map.h Makefile
	${CC} ${CFLAGS} -I.. utf8_test.c -c

utf8_test: utf8_test.o ../utf8_posix_map.o ../dbg/dbg.a ../util.o ../dyn_array/dyn_array.a Makefile
	${CC} ${CFLAGS} -I.. utf8_test.o ../utf8_posix_map.o ../dbg/dbg.a ../util.o ../dyn_array/dyn_array.a -o utf8_test

ioccc_test.sh: ../dbg/dbg_test ../dyn_array/dyn_test
	@:


###################################################################
# repo tools - rules for those who maintain the mkiocccentry repo #
###################################################################

test: all ioccc_test.sh
	 ./ioccc_test.sh


#############################################################
# rules that invoke rules in Makefiles in other directories #
#############################################################

../util.o: ../Makefile
	${MAKE} -C .. util.o

../utf8_posix_map.h: ../Makefile
	${MAKE} -C .. utf8_posix_map.h

../utf8_posix_map.o: ../Makefile
	${MAKE} -C .. utf8_posix_map.o

../dbg/dbg.a: ../dbg/Makefile
	${MAKE} -C ../dbg dbg.a

../dbg/dbg_test: ../dbg/Makefile
	${MAKE} -C ../dbg dbg_test

../dyn_array/dyn_array.a: ../dyn_array/Makefile
	${MAKE} -C ../dyn_array dyn_array.a

../dyn_array/dyn_test: ../dyn_array/Makefile
	${MAKE} -C ../dyn_array dyn_test


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	${RM} -f *.o

clobber: clean
	${RM} -f ${TARGETS}

install: all
	@echo nothing to $@
