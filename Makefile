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

####
# dbg - example of how to use usage(), dbg(), warn(), err(), vfprintf_usage(), etc.
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
####


# utilities
#
CC= cc
RM= rm
CP= cp
INSTALL= install
GREP= grep
AWK= awk
TR= tr
SED= sed

# how to compile
#
CFLAGS= -D_XOPEN_SOURCE -O3 -g3 -pedantic -Wall -Wextra

# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
#CFLAGS= -O3 -g3 -pedantic -Wall -Wextra -Werror

# NOTE: There are some things clang -Weverything warns about that are not relevant
# 	and this for the -Weverything case, we exclude several directives
#
#CFLAGS= -O3 -g3 -pedantic -Wall -Wextra -Werror -Weverything \
#     -Wno-poison-system-directories -Wno-unreachable-code-break -Wno-padded

# NOTE: If you use ASAN, set this environment var:
#	ASAN_OPTIONS="detect_stack_use_after_return=1"
#
#CFLAGS= -O0 -g -pedantic -Wall -Wextra -Werror -fsanitize=address -fno-omit-frame-pointer

# NOTE: For valgrind, run with:
#	valgrind --leak-check=yes --track-origins=yes --leak-resolution=high --read-var-info=yes
#
#CFLAGS= -O0 -g -pedantic -Wall -Wextra -Werror

# where and what to install
#
DESTDIR= /usr/local/bin
TARGETS= mkiocccentry iocccsize dbg_test limit_ioccc.sh
TEST_TARGETS= dbg_test

all: ${TARGETS} ${TEST_TARGETS}

# rules, not file targets
#
.PHONY: all configure clean clobber install test

rule_count.c: iocccsize.c
	${RM} -f $@
	${CP} -f $? $@

rule_count.o: rule_count.c limit_ioccc.h Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE rule_count.c -c

mkiocccentry: mkiocccentry.c limit_ioccc.h rule_count.o dbg.o Makefile
	${CC} ${CFLAGS} mkiocccentry.c rule_count.o dbg.o -o $@

iocccsize: iocccsize.c limit_ioccc.h Makefile
	${CC} ${CFLAGS} iocccsize.c -o $@

dbg.o: dbg.c dbg.h Makefile
	${CC} ${CFLAGS} dbg.c -c

dbg_test: dbg.c dbg.h Makefile
	${CC} ${CFLAGS} -DDBG_TEST dbg.c -o $@

limit_ioccc.sh: limit_ioccc.h
	${RM} -f $@
	${GREP} -E '^#define (RULE_|MAX_)' limit_ioccc.h | \
	    ${AWK} '{print $$2 "=\"" $$3 "\"" ;}' | ${TR} -d '[a-z]()' | \
	    ${SED} -e 's/"_/"/' -e 's/^/export /' > $@

configure:
	@echo nothing to configure

clean:
	${RM} -f mkiocccentry.o iocccsize.o rule_count.o dbg_test.o dbg.o
	${RM} -rf mkiocccentry.dSYM iocccsize.dSYM dbg_test.dSYM

clobber: clean
	${RM} -f ${TARGETS} ${TEST_TARGETS}
	${RM} -f rule_count.c answers.txt
	${RM} -rf test-iocccsize test-src test-work

install: all
	${INSTALL} -m 0555 ${TARGETS} ${DESTDIR}

test: ./iocccsize-test.sh iocccsize dbg_test mkiocccentry ./mkiocccentry-test.sh Makefile
	@echo "RUNNING: iocccsize-test.sh"
	./iocccsize-test.sh -v
	@echo "PASSED: iocccsize-test.sh"
	@echo "This next test is supposed fail with the error: FATAL[5]: main: simulated error, ..."
	@echo "RUNNING: dbg_test"
	-./dbg_test -v 1 -e 12 work_dir iocccsize_path
	@echo "PASSED: dbg_test"
	@echo "RUNNING: mkiocccentry-test.sh"
	./mkiocccentry-test.sh
	@echo "PASSED: mkiocccentry-test.sh"
