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


#############
# utilities #
#############

CAT= cat
CC= cc
CP= cp
GREP= grep
RM= rm
SHELL= bash


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
MAN1_DIR= /usr/local/share/man/man1
MAN8_DIR= /usr/local/share/man/man8
MAN3_DIR= /usr/local/share/man/man3
DESTDIR= /usr/local/bin
TARGETS= dbg.o dbg_test dbg_example
SH_TARGETS=

# man pages
#
# We explicitly define the man page targets for more than three reasons:
#
# (0) Currently not all targets have man pages.
# (1) Some of the targets that have man pages are not actually in the TARGETS
#     variable. TEST_TARGETS, for example, has utf8_test, which means
#     utf8_test.1 would be left out.
# (2) Even when all targets have man pages if another target is added without
#     adding a man page make install would fail and even if make install is not
#     that likely to be used we still don't want it to fail in the case it
#     actually is used.
# (3) Along the lines of (2) there are some files that will have man pages
#     (run_bison.sh and run_flex.sh for two examples) that are not targets at
#     all but still important parts of the repo so these would be skipped as
#     well if we directly referred to TARGETS.
#
MAN1_TARGETS= 
MAN3_TARGETS= dbg.3
MAN8_TARGETS=
MAN_TARGETS= ${MAN1_TARGETS} ${MAN3_TARGETS} ${MAN8_TARGETS}
HTML_MAN_TARGETS= $(patsubst %,%.html,$(MAN_TARGETS))
# This is a simpler way to do:
#
#   MAN1PAGES= $(patsubst %,%.1,$(MAN1_TARGETS))
#   MAN3PAGES= $(patsubst %,%.3,$(MAN3_TARGETS))
#   MAN8PAGES= $(patsubst %,%.8,$(MAN8_TARGETS))
#
MAN1PAGES= $(MAN1_TARGETS:=.1)
MAN3PAGES= $(MAN3_TARGETS:=.3)
MAN8PAGES= $(MAN8_TARGETS:=.8)
MANPAGES= ${MAN1PAGES} ${MAN3PAGES} ${MAN8PAGES}


######################################
# all - default rule - must be first #
######################################

all: ${TARGETS}
	@:

.PHONY: all test configure clean clobber install

dbg.o: dbg.c dbg.h Makefile
	${CC} ${CFLAGS} dbg.c -c

dbg_test.c: dbg.c Makefile
	${RM} -f $@
	${CP} -v -f dbg.c $@

dbg_test.o: dbg_test.c dbg.h Makefile
	${CC} ${CFLAGS} -DDBG_TEST dbg_test.c -c

dbg_test: dbg_test.o Makefile
	${CC} ${CFLAGS} dbg_test.o -o $@

dbg_example.o:  dbg_example.c dbg.h Makefile
	${CC} ${CFLAGS} dbg_example.c -c

dbg_example: dbg_example.o dbg.o Makefile
	${CC} ${CFLAGS} dbg_example.o dbg.o -o $@


###################################################################
# repo tools - rules for those who maintain the mkiocccentry repo #
###################################################################

test: dbg_test Makefile
	${RM} -f dbg_test.out
	@echo "RUNNING: dbg_test"
	@echo "./dbg_test -e 2 foo bar baz >dbg_test.out 2>&1"
	@-./dbg_test -v 1 -e 2 foo bar baz > dbg_test.out 2>&1; \
	  status="$$?"; \
	  if [[ $$status -ne 5 ]]; then \
	    echo "exit status of dbg_test: $$status != 5"; \
	    exit 21; \
	  else \
	      ${GREP} -q '^ERROR\[5\]: main: simulated error, foo: foo bar: bar: errno\[2\]: No such file or directory$$' dbg_test.out; \
	      status="$$?"; \
	      if [[ $$status -ne 0 ]]; then \
		echo "ERROR: did not find the correct dbg_test error message" 1>&2; \
		echo "ERROR: beginning dbg_test.out contents" 1>&2; \
		${CAT} dbg_test.out 1>&2; \
		echo "ERROR: dbg_test.out contents complete" 1>&2; \
		exit 22; \
	      else \
		echo "PASSED: dbg_test"; \
	      fi; \
	  fi


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	${RM} -f dbg.o dbg_example.o
	${RM} -f dbg.out

clobber: clean
	${RM} -f ${TARGETS}
	${RM} -f dbg_test.c dbg_test.o dbg_test.out

install: all
	@echo nothing to $@
