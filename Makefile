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
AWK= awk
CC= cc
CP= cp
CTAGS= ctags
GREP= grep
INSTALL= install
MV= mv
RM= rm
RPL= rpl
SED= sed
SEQCEXIT= seqcexit
SHELLCHECK= shellcheck
TR= tr
TRUE= true
FLEX = flex
BISON = bison

# User specific configurations
#
# This retrieves any user specific configurations in the file makefile.local
# (added to .gitignore).
#
# This was added by @xexyl on 19 March 2022 for when he starts using flex(1) and
# bison(1) under macOS (not a problem with his linux boxes) as the macOS
# versions are out of date; however he uses MacPorts which has an unusual prefix
# (/opt/local) so instead of assuming the path of flex and bison is the same
# (since it's not always) I can have my makefile.local override the path to the
# tools I need.
#
# The - before include means it's not an error if the file does not exist (which
# it won't in most if not all systems but my MacBook Pro). Lower case name
# because I don't need it at the beginning of the directory listing.
#
# NOTE: This is what my makefile.local looks like so you can see how it works:
#
#	FLEX = /opt/local/bin/flex
#	BISON = /opt/local/bin/bison
#
# After this whenever ${FLEX} and ${BISON} are referenced it will translate to
# the tools under /opt/local/bin.
#
-include makefile.local

# C source standards being used
#
# NOTE: The use of -std=gnu11 is because there are a few older systems
#	in late 2021 that do not have compilers that (yet) support gnu17.
#	While there may be even more out of date systems that do not
#	support gnu11, we have to draw the line somewhere.
#
# NOTE: The -D_* lines in STD_SRC are due to a few older systems in
#	late 2021 that need those defines to compile this code. CentOS
#	7 is such a system.
#
# NOTE: The code in the mkiocccentry repo is to help you form and
#	submit a compressed tarball that meets the IOCCC requirements.
#	Your IOCCC entry is free to require older C standards, or
#	even not specify a C standard at all.  Moreover, your entry's
#	Makefile, can do what it needs to do, perhaps by using the
#	Makefile.example as a basis.
#
# XXX - XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX - XXX
# XXX - In 2024 we will change the STD_SRC line to be just - XXX
# XXX - STD_SRC= -std=gnu17				   - XXX
# XXX - XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX - XXX
#
STD_SRC= -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -std=gnu11

# optimization and debug level
#
#COPT= -O3 -g3
COPT= -O0 -g

# how to compile
#
#CFLAGS= ${STD_SRC} ${COPT} -pedantic -Wall -Wextra
CFLAGS= ${STD_SRC} ${COPT} -pedantic -Wall -Wextra -Werror

# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
#CFLAGS= ${STD_SRC} ${COPT} -pedantic -Wall -Wextra -Werror

# NOTE: There are some things clang -Weverything warns about that are not relevant
# 	and thus for the -Weverything case, we exclude several directives
#
#CFLAGS= ${STD_SRC} ${COPT} -pedantic -Wall -Wextra -Werror -Weverything \
#     -Wno-poison-system-directories -Wno-unreachable-code-break -Wno-padded

# NOTE: If you use ASAN, set this environment var:
#	ASAN_OPTIONS="detect_stack_use_after_return=1"
#
#CFLAGS= ${STD_SRC} -O0 -g -pedantic -Wall -Wextra -Werror -fsanitize=address -fno-omit-frame-pointer

# NOTE: For valgrind, run with:
#
#	valgrind --leak-check=yes --track-origins=yes --leak-resolution=high --read-var-info=yes \
#           --leak-check=full --show-leak-kinds=all ./mkiocccentry ...
#
# NOTE: Replace mkiocccentry with whichever tool you want to test and the ...
# with the arguments and options you want.
#

# where and what to install
#
MANDIR = /usr/local/share/man/man1
DESTDIR= /usr/local/bin
TARGETS= mkiocccentry iocccsize dbg_test limit_ioccc.sh fnamchk txzchk jauthchk jinfochk \
	jstrencode jstrdecode utf8_test jparse jint jfloat
MANPAGES = mkiocccentry.1 txzchk.1 fnamchk.1 iocccsize.1 jinfochk.1 jauthchk.1
TEST_TARGETS= dbg_test utf8_test
OBJFILES= dbg.o util.o mkiocccentry.o iocccsize.o fnamchk.o txzchk.o jauthchk.o jinfochk.o \
	json.o jstrencode.o jstrdecode.o rule_count.o location.o utf8_posix_map.o sanity.o \
	utf8_test.o jint.o jint.test.o jfloat.o jfloat.test.o
SPECIAL_OBJ= jparse.o jparse.tab.o
FLEXFILES= jparse.l
BISONFILES= jparse.y
SRCFILES= $(patsubst %.o,%.c,$(OBJFILES))
H_FILES= dbg.h jauthchk.h jinfochk.h json.h jstrdecode.h jstrencode.h limit_ioccc.h \
	mkiocccentry.h txzchk.h util.h location.h utf8_posix_map.h jparse.h jint.h jfloat.h
DSYMDIRS= $(patsubst %,%.dSYM,$(TARGETS))
SH_FILES= iocccsize-test.sh jstr-test.sh limit_ioccc.sh mkiocccentry-test.sh json-test.sh \
	  jcodechk.sh vermod.sh

all: ${TARGETS} ${TEST_TARGETS}

# rules, not file targets
#
.PHONY: all configure clean clobber install test reset_min_timestamp rebuild_jint_test \
	rebuild_jfloat_test

# Remove built-in rules that cause overwriting jparse.c from bison/yacc. The
# reason I (@xexyl) have flex write to jparse.c instead of bison is that bison
# would write to jparse.h as well and we use that as the header file for the
# tool so that would be overwritten.
#
# NOTES: When ever I (@xexyl) update jparse.y or jparse.l I have to run make
# parser and this will regenerate the jparse source files. If I don't do this
# then the changes won't take effect but having it this allows for those without
# flex and bison (or different versions or some other problems I'm unaware of)
# to compile the code. In time the Makefile will try and generate these files
# but fallback to the provided files if this fails.
#
# To help remind me to run make parser I have made jparse depend on jparse.l and
# jparse.y (it does not need to use them but whenever they're modified jparse
# will be recompiled so it shouldn't be a problem as the lexer/parser files are
# in the repo and so are the source files and header files).
%.c: %.y
%.c: %.l


rule_count.o: rule_count.c Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE rule_count.c -c

sanity.o: sanity.c sanity.h location.h utf8_posix_map.h Makefile
	${CC} ${CFLAGS} sanity.c -c

mkiocccentry: mkiocccentry.c mkiocccentry.h rule_count.o dbg.o util.o json.o location.o \
	utf8_posix_map.o sanity.o Makefile
	${CC} ${CFLAGS} mkiocccentry.c rule_count.o dbg.o util.o json.o location.o \
		utf8_posix_map.o sanity.o -o $@

iocccsize: iocccsize.c rule_count.o dbg.o Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE iocccsize.c rule_count.o dbg.o -o $@

dbg_test: dbg.c Makefile
	${CC} ${CFLAGS} -DDBG_TEST dbg.c -o $@

fnamchk: fnamchk.c fnamchk.h dbg.o util.o Makefile
	${CC} ${CFLAGS} fnamchk.c dbg.o util.o -o $@

txzchk: txzchk.c txzchk.h rule_count.o dbg.o util.o location.o json.o utf8_posix_map.o sanity.o Makefile
	${CC} ${CFLAGS} txzchk.c rule_count.o dbg.o util.o location.o json.o utf8_posix_map.o sanity.o -o $@

jauthchk: jauthchk.c jauthchk.h json.h rule_count.o json.o dbg.o util.o sanity.o location.o utf8_posix_map.o Makefile
	${CC} ${CFLAGS} jauthchk.c rule_count.o json.o dbg.o util.o sanity.o location.o utf8_posix_map.o -o $@

jinfochk: jinfochk.c jinfochk.h rule_count.o json.o dbg.o util.o sanity.o location.o utf8_posix_map.o Makefile
	${CC} ${CFLAGS} jinfochk.c rule_count.o json.o dbg.o util.o sanity.o location.o utf8_posix_map.o -o $@

jstrencode: jstrencode.c jstrencode.h dbg.o json.o util.o Makefile
	${CC} ${CFLAGS} jstrencode.c dbg.o json.o util.o -o $@

jstrdecode: jstrdecode.c jstrdecode.h dbg.o json.o util.o Makefile
	${CC} ${CFLAGS} jstrdecode.c dbg.o json.o util.o -o $@

jint.test.o: jint.test.c
	${CC} ${CFLAGS} -DJINT_TEST_ENABLED jint.test.c -c

jint: jint.c dbg.o json.o util.o jint.test.o Makefile
	${CC} ${CFLAGS} -DJINT_TEST_ENABLED jint.c dbg.o json.o util.o jint.test.o -o $@

jfloat.test.o: jfloat.test.c
	${CC} ${CFLAGS} -DJFLOAT_TEST_ENABLED jfloat.test.c -c

jfloat: jfloat.c dbg.o json.o util.o jfloat.test.o Makefile
	${CC} ${CFLAGS} -DJFLOAT_TEST_ENABLED jfloat.c dbg.o json.o util.o jfloat.test.o -o $@

jparse: jparse.c jparse.h jparse.tab.c jparse.tab.h jparse.y jparse.l util.o dbg.o sanity.o json.o utf8_posix_map.o location.o Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.c jparse.tab.c \
	    util.o dbg.o sanity.o json.o utf8_posix_map.o location.o -o $@

utf8_test: utf8_test.c utf8_posix_map.o dbg.o util.o
	${CC} ${CFLAGS} utf8_test.c utf8_posix_map.o dbg.o util.o -o $@

# NOTE: This temporary rule produces the following C code files:
#
# jparse.tab.h
# jparse.tab.c
# jparse.c
#
parser: jparse.y jparse.l
	${BISON} -d jparse.y
	${FLEX} -o jparse.c jparse.l

# rebuild jint.test.c
#
rebuild_jint_test: jint.testset jint.c dbg.o json.o util.o
	${RM} -f jint.set.tmp jint_gen
	${CC} ${CFLAGS} jint.c dbg.o json.o util.o -o jint_gen
	${SED} -n -e '1,/DO NOT REMOVE THIS LINE/p' < jint.test.c > jint.set.tmp
	echo '#if defined(JINT_TEST_ENABLED)' >> jint.set.tmp
	./jint_gen -- $$(<jint.testset) >> jint.set.tmp
	echo '#endif /* JINT_TEST_ENABLED */' >> jint.set.tmp
	${MV} -f jint.set.tmp jint.test.c
	${RM} -f jint_gen

# rebuild jfloat.test.c
#
rebuild_jfloat_test: jfloat.testset jfloat.c dbg.o json.o util.o
	${RM} -f jfloat.set.tmp jfloat_gen
	${CC} ${CFLAGS} jfloat.c dbg.o json.o util.o -o jfloat_gen
	${SED} -n -e '1,/DO NOT REMOVE THIS LINE/p' < jfloat.test.c > jfloat.set.tmp
	echo '#if defined(JFLOAT_TEST_ENABLED)' >> jfloat.set.tmp
	./jfloat_gen -- $$(<jfloat.testset) >> jfloat.set.tmp
	echo '#endif /* JFLOAT_TEST_ENABLED */' >> jfloat.set.tmp
	${MV} -f jfloat.set.tmp jfloat.test.c
	${RM} -f jfloat_gen

limit_ioccc.sh: limit_ioccc.h version.h Makefile
	${RM} -f $@
	@echo '#!/usr/bin/env bash' > $@
	@echo '#' >> $@
	@echo '# Copies of select limit_ioccc.h and version.h values for shell script use' >> $@
	@echo '#' >> $@
	${GREP} -E '^#define (RULE_|MAX_|UUID_|MIN_|IOCCC_)' limit_ioccc.h | \
	    ${AWK} '{print $$2 "=\"" $$3 "\"" ;}' | ${TR} -d '[a-z]()' | \
	    ${SED} -e 's/"_/"/' -e 's/""/"/g' -e 's/^/export /' >> $@
	${GREP} -hE '^#define (.*_VERSION|TIMESTAMP_EPOCH)' version.h limit_ioccc.h | \
	    ${GREP} -v 'UUID_VERSION' | \
	    ${SED} -e 's/^#define/export/' -e 's/ "/="/' -e 's/"[	 ].*$$/"/' >> $@
	-if ${GREP} -q '^#define DIGRAPHS' limit_ioccc.h; then \
	    echo "export DIGRAPHS='yes'"; \
	else \
	    echo "export DIGRAPHS="; \
	fi >> $@
	-if ${GREP} -q '^#define TRIGRAPHS' limit_ioccc.h; then \
	    echo "export TRIGRAPHS='yes'"; \
	else \
	    echo "export TRIGRAPHS="; \
	fi >> $@

seqcexit: ${SRCFILES} ${FLEXFILES} ${BISONFILES}
	@HAVE_SEQCEXIT=`command -v seqcexit`; if [[ -z "$$HAVE_SEQCEXIT" ]]; then \
		echo 'If you have not bothered to install the seqcexit tool, then' 1>&2; \
		echo 'you may not run this rule.'; 1>&2; \
		echo ''; 1>&2; \
		echo 'See the following GitHub repo for seqcexit:'; 1>&2; \
		echo ''; 1>&2; \
		echo 'https://github.com/lcn2/seqcexit'; 1>&2; \
		exit 1; \
	fi
	${SEQCEXIT} -c ${FLEXFILES} ${BISONFILES}
	${SEQCEXIT} ${SRCFILES}

shellcheck: ${SH_FILES}
	${SHELLCHECK} -f gcc ${SH_FILES}

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
	${RM} -f ${SPECIAL_OBJ}
	${RM} -rf ${DSYMDIRS}

clobber distclean: clean
	${RM} -f ${TARGETS} ${TEST_TARGETS}
	${RM} -f answers.txt j-test.out j-test2.out json-test.log
	${RM} -rf test-iocccsize test_src test_work tags dbg_test.out
	${RM} -f jint.set.tmp jint_gen
	${RM} -f jfloat.set.tmp jfloat_gen
	${RM} -rf jint_gen.dSYM jfloat_gen.dSYM

install: all
	${INSTALL} -m 0555 ${TARGETS} ${DESTDIR}
	${INSTALL} -m 0644 ${MANPAGES} ${MANDIR}

test: all iocccsize-test.sh dbg_test mkiocccentry-test.sh jstr-test.sh jint jfloat Makefile
	@chmod +x iocccsize-test.sh mkiocccentry-test.sh jstr-test.sh
	@echo "RUNNING: iocccsize-test.sh"
	./iocccsize-test.sh -v 1
	@echo "PASSED: iocccsize-test.sh"
	@echo
	@echo "This next test is supposed to fail with the error: FATAL[5]: main: simulated error, ..."
	@echo "RUNNING: dbg_test"
	@${RM} -f dbg_test.out
	@status=`./dbg_test -e 2 foo bar baz 2>dbg_test.out; echo "$$?"`; \
	    if [ "$$status" != 5 ]; then \
		echo "exit status of dbg_test: $$status != 5"; \
		exit 1; \
	    fi
	@${GREP} -q '^FATAL\[5\]: main: simulated error, foo: foo bar: bar errno\[2\]: No such file or directory$$' dbg_test.out
	@${RM} -f dbg_test.out
	@echo "PASSED: dbg_test"
	@echo
	@echo "RUNNING: mkiocccentry-test.sh"
	./mkiocccentry-test.sh
	@echo "PASSED: mkiocccentry-test.sh"
	@echo
	@echo "RUNNING: jstr-test.sh"
	./jstr-test.sh
	@echo "PASSED: jstr-test.sh"
	@echo
	@echo "RUNNING: jint -t"
	./jint -t
	@echo "PASSED: jint -t"
	@echo
	@echo "RUNNING: jfloat -t"
	./jfloat -t
	@echo "PASSED: jfloat -t"
	@echo
	@echo "All tests PASSED"

test-mkiocccentry: all mkiocccentry-test.sh Makefile
	@echo "RUNNING: mkiocccentry-test.sh"
	./mkiocccentry-test.sh
	@echo "PASSED: mkiocccentry-test.sh"

# run json-test.sh on test_JSON files
#
# Currently this is very chatty but the important parts will also be written to
# json-test.log for evaluation.
#
# NOTE: The verbose and debug options are important to see more information and
# this is important even when all cases are resolved as we want to see if there
# are any problems if new files are added.
test-jinfochk: all jinfochk Makefile
	./json-test.sh -t jinfo_only -v 1 -D 1 -d test_JSON

tags: ${SRCFILES} ${H_FILES} Makefile
	-${CTAGS} -- ${SRCFILES} ${H_FILES} 2>&1 | \
	     ${GREP} -E -v 'Duplicate entry|Second entry ignored'

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
util.o: util.c dbg.h util.h limit_ioccc.h version.h
mkiocccentry.o: mkiocccentry.c mkiocccentry.h util.h json.h dbg.h \
  location.h utf8_posix_map.h sanity.h limit_ioccc.h version.h \
  iocccsize.h
iocccsize.o: iocccsize.c iocccsize_err.h iocccsize.h
fnamchk.o: fnamchk.c fnamchk.h dbg.h util.h limit_ioccc.h version.h \
  utf8_posix_map.h
txzchk.o: txzchk.c txzchk.h util.h dbg.h sanity.h location.h \
  utf8_posix_map.h json.h limit_ioccc.h version.h
jauthchk.o: jauthchk.c jauthchk.h dbg.h util.h json.h sanity.h location.h \
  utf8_posix_map.h limit_ioccc.h version.h
jinfochk.o: jinfochk.c jinfochk.h dbg.h util.h json.h sanity.h location.h \
  utf8_posix_map.h limit_ioccc.h version.h
json.o: json.c dbg.h util.h limit_ioccc.h version.h json.h
jstrencode.o: jstrencode.c jstrencode.h dbg.h util.h json.h limit_ioccc.h \
  version.h
jstrdecode.o: jstrdecode.c jstrdecode.h dbg.h util.h json.h limit_ioccc.h \
  version.h
rule_count.o: rule_count.c iocccsize_err.h iocccsize.h
location.o: location.c location.h util.h dbg.h
utf8_posix_map.o: utf8_posix_map.c utf8_posix_map.h util.h dbg.h \
  limit_ioccc.h version.h
sanity.o: sanity.c sanity.h util.h dbg.h location.h utf8_posix_map.h \
  json.h
jparse.o: jparse.c util.h dbg.h jparse.tab.h
jparse.tab.o: jparse.tab.c jparse.h dbg.h util.h json.h sanity.h \
  location.h utf8_posix_map.h limit_ioccc.h version.h jparse.tab.h
