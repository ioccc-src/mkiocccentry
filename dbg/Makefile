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
CFLAGS= -O3 -g3 -pedantic -Wall
#CFLAGS= -O3 -g3 -pedantic -Wall -Werror
CP= cp
RM= rm
GREP= grep
CAT= cat

DESTDIR= /usr/local/bin

TARGETS= dbg.o dbg_test dbg_example

.PHONY: all

all: ${TARGETS}
	@:

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
