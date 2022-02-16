#!/usr/bin/env make
#
# mkiocccentry and related tools - how to form an IOCCC entry
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
# Public Domain 1992, 2015, 2018, 2019, 2021 by Anthony Howe.  All rights released.
# With IOCCC mods in 2019-2022 by chongo (Landon Curt Noll) ^oo^
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
RPL= rpl
TRUE= true

# C source standards being used
#
# NOTE: feature test macros are required for some systems: e.g., CentOS
#
STD_SRC= -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE -std=c99

# optimization and debug level
#
COPT= -O3 -g3
#COPT= -O0 -g

# how to compile
#
CFLAGS= ${STD_SRC} ${COPT} -pedantic -Wall -Wextra
#CFLAGS= ${STD_SRC} ${COPT} -pedantic -Wall -Wextra -Werror

# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
#CFLAGS= ${STD_SRC} ${COPT} -pedantic -Wall -Wextra -Werror

# NOTE: There are some things clang -Weverything warns about that are not relevant
# 	and this for the -Weverything case, we exclude several directives
#
#CFLAGS= ${STD_SRC} ${COPT} -pedantic -Wall -Wextra -Werror -Weverything \
#     -Wno-poison-system-directories -Wno-unreachable-code-break -Wno-padded

# NOTE: If you use ASAN, set this environment var:
#	ASAN_OPTIONS="detect_stack_use_after_return=1"
#
#CFLAGS= ${STD_SRC} -O0 -g -pedantic -Wall -Wextra -Werror -fsanitize=address -fno-omit-frame-pointer

# NOTE: For valgrind, run with:
#	valgrind --leak-check=yes --track-origins=yes --leak-resolution=high --read-var-info=yes
#
#CFLAGS= ${STD_SRC} -O0 -g -pedantic -Wall -Wextra -Werror

# where and what to install
#
MANDIR = /usr/local/share/man/man1
DESTDIR= /usr/local/bin
TARGETS= mkiocccentry iocccsize dbg_test limit_ioccc.sh fnamchk txzchk jauthchk jinfochk \
	jstrencode jstrdecode
MANPAGES = mkiocccentry.1 txzchk.1 fnamchk.1 iocccsize.1
TEST_TARGETS= dbg_test
OBJFILES = dbg.o util.o mkiocccentry.o iocccsize.o fnamchk.o txzchk.o jauthchk.o jinfochk.o \
	json.o jstrencode.o jstrdecode.o rule_count.o
SRCFILES = $(patsubst %.o,%.c,$(OBJFILES))
DSYMDIRS = $(patsubst %,%.dSYM,$(TARGETS))

all: ${TARGETS} ${TEST_TARGETS}

# rules, not file targets
#
.PHONY: all configure clean clobber install test reset_min_timestamp

rule_count.c: iocccsize.c
	${RM} -f $@
	${CP} -f $? $@

rule_count.o: rule_count.c Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE rule_count.c -c

mkiocccentry: mkiocccentry.c mkiocccentry.h rule_count.o dbg.o util.o json.o Makefile
	${CC} ${CFLAGS} mkiocccentry.c rule_count.o dbg.o util.o json.o -o $@

iocccsize: iocccsize.c Makefile
	${CC} ${CFLAGS} -DIOCCCSIZE_STANDALONE iocccsize.c -o $@

dbg_test: dbg.c Makefile
	${CC} ${CFLAGS} -DDBG_TEST dbg.c -o $@

fnamchk: fnamchk.c dbg.o util.o Makefile
	${CC} ${CFLAGS} fnamchk.c dbg.o util.o -o $@

txzchk: txzchk.c txzchk.h rule_count.o dbg.o util.o Makefile
	${CC} ${CFLAGS} txzchk.c rule_count.o dbg.o util.o -o $@

jauthchk: jauthchk.c json.h rule_count.o dbg.o util.o Makefile
	${CC} ${CFLAGS} jauthchk.c rule_count.o dbg.o util.o -o $@

jinfochk: jinfochk.c rule_count.o dbg.o util.o Makefile
	${CC} ${CFLAGS} jinfochk.c rule_count.o dbg.o util.o -o $@

jstrencode: jstrencode.c jstrencode.h dbg.o json.o util.o Makefile
	${CC} ${CFLAGS} jstrencode.c dbg.o json.o util.o -o $@

jstrdecode: jstrdecode.c jstrdecode.h dbg.o json.o util.o Makefile
	${CC} ${CFLAGS} jstrdecode.c dbg.o json.o util.o -o $@

limit_ioccc.sh: limit_ioccc.h Makefile
	${RM} -f $@
	@echo '#' > $@
	@echo '# Copies of select limit_ioccc.h values for shell script use' >> $@
	@echo '#' >> $@
	${GREP} -E '^#define (RULE_|MAX_|UUID_|MIN_|_VERSION|IOCCC_)' limit_ioccc.h | \
	    ${AWK} '{print $$2 "=\"" $$3 "\"" ;}' | ${TR} -d '[a-z]()' | \
	    ${SED} -e 's/"_/"/' -e 's/""/"/g' -e 's/^/export /' >> $@
	${GREP} -E '^#define (IOCCCSIZE_VERSION|MKIOCCCENTRY_VERSION|TIMESTAMP_EPOCH)' limit_ioccc.h | \
	    ${SED} -e 's/^#define/export/' -e 's/ "/="/' -e 's/"[	 ].*$$/"/' >> $@

# Only run this rule when you wish to invalidate all timestamps
# prior to now, such as when you make a fundamental change to a
# critical JSON format, or make a fundamental change the compressed
# tarball file structure, or make an critical change to limit_ioccc.h
# that is MORE restrictive.
#
# DO NOT run this rule simply for a new IOCCC!
#
# Yes, we make it very hard to run this rule for good reason.
# Only IOCCC judges can perform the ALL the steps needed to complete this action.
#
reset_min_timestamp:
	@HAVE_RPL=`command -v rpl`; if [[ -z "$$HAVE_RPL" ]]; then \
		echo 'If you have not bothered to install the rpl tool, then' 1>&2; \
		echo 'you may not run this rule.'; 1>&2; \
		exit 1; \
	fi
	@echo
	@echo 'Yes, we make it very hard to run this rule for good reasson.'
	@echo 'Only IOCCC judges can perform the ALL the steps needed to complete this action.'
	@echo
	@echo 'WARNING: This rule will invalidate all timestamps prior to now.'
	@echo 'WARNING: Only run this rule when you wish to invalidate all timestamps'
	@echo 'WARNING: because you make a fundamental change to a critical JSON format'
	@echo 'WARNING: when you make a fundamental change to a critical JSON format'
	@echo 'WARNING: or make a fundamental change the compressed tarball file structure,'
	@echo 'WARNING: make an critical change to limit_ioccc.h that is MORE restrictive.'
	@echo
	@echo 'WARNING: DO NOT run this rule simply for a new IOCCC!'
	@echo
	@echo 'WARNING: If you wish to do this, please enter the following phrase:'
	@echo
	@echo '    Please invalidate all IOCCC timestamps prior to now'
	@echo
	@read answer && \
	    if [[ "$$answer" != 'Please invalidate all IOCCC timestamps prior to now' ]]; then \
		echo 'Wise choice, MIN_TIMESTAMP was not changed.' 1>&2; \
		exit 2; \
	    fi
	@echo
	@echo 'WARNING: Are you sure you want to invalidate all existing compressed tarballs'
	@echo 'WARNING: and all prior .info.JSON files that have been made,'
	@echo 'WARNING: and all prior .author.JSON files that have been made,'
	@echo 'WARNING: forcing everyone to rebuild their compressed tarball entries?'
	@echo
	@echo 'WARNING: If you really wish to do this, please enter the following phrase:'
	@echo
	@echo '    I understand this'
	@echo
	@echo 'WARNING: followed by the phrase:'
	@echo
	@echo '    and say sorry to those with existing IOCCC compressed tarballs'
	@echo
	@read answer && \
	    if [[ "$$answer" != 'I understand this and say sorry to those with existing IOCCC compressed tarballs' ]]; then \
		echo 'Wise choice, MIN_TIMESTAMP was not changed.' 1>&2; \
		exit 3; \
	    fi
	@echo
	@echo 'WARNING: Enter the phrase that is required (even if you are from that very nice place):'
	@echo
	@read answer && \
	    if [[ "$$answer" != 'Sorry (tm Canada) :=)' ]]; then \
		echo 'Wise choice, MIN_TIMESTAMP was not changed.' 1>&2; \
		exit 4; \
	    fi
	@echo
	@echo -n 'WARNING: Enter the existing value of MIN_TIMESTAMP: '
	@read OLD_MIN_TIMESTAMP &&\
	    now=`/bin/date '+%s'` &&\
	    if ${GREP} "$$OLD_MIN_TIMESTAMP" limit_ioccc.h; then\
		echo; \
		echo "We guess that you do really really do want to change MIN_TIMESTAMP"; \
		echo; \
	        ${TRUE};\
	    else\
	        echo 'Invalid value of MIN_TIMESTAMP' 1>&2;\
		exit 5;\
	    fi &&\
	    ${RPL} -w -p \
	       "#define MIN_TIMESTAMP ((time_t)$$OLD_MIN_TIMESTAMP)"\
	       "#define MIN_TIMESTAMP ((time_t)$$now)" limit_ioccc.h;\
	    echo;\
	    echo "This line in limit_ioccc.h, as it exists now, is:";\
	    echo;\
	    ${GREP} '^#define MIN_TIMESTAMP' limit_ioccc.h;\
	    echo;\
	    echo 'WARNING: You still need to:';\
	    echo;\
	    echo '    make clobber all test';\
	    echo;\
	    echo 'WARNING: And if all is well, commit and push the change to the GitHib repo!';\
	    echo

configure:
	@echo nothing to configure

clean:
	${RM} -f ${OBJFILES}
	${RM} -rf ${DSYMDIRS}

clobber: clean
	${RM} -f ${TARGETS} ${TEST_TARGETS}
	${RM} -f rule_count.c answers.txt j-test.out j-test2.out
	${RM} -rf test-iocccsize test_src test_work

install: all
	${INSTALL} -m 0555 ${TARGETS} ${DESTDIR}
	${INSTALL} -m 0644 ${MANPAGES} ${MANDIR}

test: all iocccsize-test.sh dbg_test mkiocccentry-test.sh j-test.sh Makefile
	@echo "RUNNING: iocccsize-test.sh"
	./iocccsize-test.sh -v
	@echo "PASSED: iocccsize-test.sh"
	@echo
	@echo "This next test is supposed to fail with the error: FATAL[5]: main: simulated error, ..."
	@echo "RUNNING: dbg_test"
	-./dbg_test -v 1 -e 12 work_dir iocccsize_path
	@echo "PASSED: dbg_test"
	@echo
	@echo "RUNNING: mkiocccentry-test.sh"
	./mkiocccentry-test.sh
	@echo "PASSED: mkiocccentry-test.sh"
	@echo
	@echo "RUNNING j-test.sh"
	./j-test.sh
	@echo "PASSED: j-test.sh"
	@echo
	@echo "All tests PASSED"

depend:
	@LINE="`grep -n '^### DO NOT CHANGE' Makefile|awk -F : '{print $$1}'`"; \
        if [ "$$LINE" = "" ]; then                                              \
                echo "Make depend aborted, tag not found in Makefile.";         \
                exit;                                                           \
        fi;                                                                     \
        mv -f Makefile Makefile.orig;head -n $$LINE Makefile.orig > Makefile;   \
        echo "Generating dependencies.";                               \
        ${CC} ${CFLAGS} -MM ${SRCFILES} >> Makefile
	@echo "Make depend completed.";

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
dbg.o: dbg.c dbg.h
util.o: util.c dbg.h util.h
mkiocccentry.o: mkiocccentry.c mkiocccentry.h util.h json.h dbg.h \
  limit_ioccc.h
iocccsize.o: iocccsize.c
fnamchk.o: fnamchk.c limit_ioccc.h dbg.h util.h
txzchk.o: txzchk.c txzchk.h util.h dbg.h limit_ioccc.h
jauthchk.o: jauthchk.c limit_ioccc.h dbg.h util.h json.h
jinfochk.o: jinfochk.c limit_ioccc.h dbg.h util.h json.h
json.o: json.c dbg.h util.h json.h
jstrencode.o: jstrencode.c jstrencode.h limit_ioccc.h dbg.h util.h json.h
jstrdecode.o: jstrdecode.c jstrdecode.h limit_ioccc.h dbg.h util.h json.h
rule_count.o: rule_count.c
