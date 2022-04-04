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


#############
# utilities #
#############

AWK= awk
BISON = bison
CC= cc
CP= cp
CTAGS= ctags
FLEX = flex
GREP= grep
INSTALL= install
MV= mv
PICKY= picky
RM= rm
RPL= rpl
SED= sed
SEQCEXIT= seqcexit
SHELLCHECK= shellcheck
TR= tr
TRUE= true


#######################
# Makefile parameters #
#######################

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
#	and thus for the -Weverything case, we exclude several directives
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

# where and what to install
#
MANDIR = /usr/local/share/man/man1
DESTDIR= /usr/local/bin
TARGETS= mkiocccentry iocccsize dbg limit_ioccc.sh fnamchk txzchk jauthchk jinfochk \
	jstrencode jstrdecode utf8_test jparse jint jfloat verge
MANPAGES = mkiocccentry.1 txzchk.1 fnamchk.1 iocccsize.1 jinfochk.1 jauthchk.1
TEST_TARGETS= dbg utf8_test
OBJFILES= dbg.o util.o mkiocccentry.o iocccsize.o fnamchk.o txzchk.o jauthchk.o jinfochk.o \
	json.o jstrencode.o jstrdecode.o rule_count.o location.o sanity.o \
	utf8_test.o jint.o jint.test.o jfloat.o jfloat.test.o verge.o
LESS_PICKY_SRC= utf8_posix_map.c
SPECIAL_OBJ= jparse.o jparse.tab.o utf8_posix_map.o
FLEXFILES= jparse.l
BISONFILES= jparse.y
SRCFILES= $(patsubst %.o,%.c,$(OBJFILES))
SRCFILES+= $(patsubst %.o,%.c,$(SPECIAL_OBJ))
H_FILES= dbg.h jauthchk.h jinfochk.h json.h jstrdecode.h jstrencode.h limit_ioccc.h \
	mkiocccentry.h txzchk.h util.h location.h utf8_posix_map.h jparse.h jint.h jfloat.h \
	verge.h
DSYMDIRS= $(patsubst %,%.dSYM,$(TARGETS))
SH_FILES= iocccsize-test.sh jstr-test.sh limit_ioccc.sh mkiocccentry-test.sh json-test.sh \
	  jcodechk.sh vermod.sh bfok.sh

# Where bfok.sh looks for bison and flex with a version
#
# The -B arguments specify where to look for bison with a version,
# that is >= the minimum version (see BISON_VERSION in limit_ioccc.sh),
# before searching for bison on $PATH.
#
# The -F arguments specify where to look for flex with a version,
# that is >= the minimum version (see FLEX_VERSION in limit_ioccc.sh),
# before searching for flex on $PATH.
#
# NOTE: If is OK if these directories do not exist.
#
BFOK_DIRS= \
	-B /opt/homebrew/opt/bison/bin \
	-B /opt/homebrew/bin \
	-B /opt/local/bin \
	-B /usr/local/opt \
	-B /usr/local/bin \
	-B . \
	-F /opt/homebrew/opt/flex/bin \
	-F /opt/homebrew/bin \
	-F /opt/local/bin \
	-F /usr/local/opt \
	-F /usr/local/bin \
	-F .


############################################################
# User specific configurations - override Makefile values  #
############################################################

# The directive below retrieves any user specific configurations from makefile.local.
#
# The - before include means it's not an error if the file does not exist.
#
# We put this directive just before the 1st all rule so that you may override
# or modify any of the above Makefile variables.  To override a value, use := symbols.
# For example:
#
#	CC:= gcc
#
-include makefile.local


######################################
# all - default rule - must be first #
######################################

all: ${TARGETS} ${TEST_TARGETS}

# rules, not file targets
#
.PHONY: all configure clean clobber install test reset_min_timestamp rebuild_jint_test \
	rebuild_jfloat_test picky parser


####################################
# rules to compile and build files #
####################################

rule_count.o: rule_count.c Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE rule_count.c -c

sanity.o: sanity.c sanity.h location.h utf8_posix_map.h
	${CC} ${CFLAGS} sanity.c -c

mkiocccentry: mkiocccentry.c mkiocccentry.h rule_count.o dbg.o util.o json.o location.o \
	utf8_posix_map.o sanity.o
	${CC} ${CFLAGS} mkiocccentry.c rule_count.o dbg.o util.o json.o location.o \
		utf8_posix_map.o sanity.o -o $@

iocccsize: iocccsize.c rule_count.o dbg.o Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE iocccsize.c rule_count.o dbg.o -o $@

dbg: dbg.c Makefile
	${CC} ${CFLAGS} -DDBG_TEST dbg.c -o $@

fnamchk: fnamchk.c fnamchk.h dbg.o util.o
	${CC} ${CFLAGS} fnamchk.c dbg.o util.o -o $@

txzchk: txzchk.c txzchk.h rule_count.o dbg.o util.o location.o json.o utf8_posix_map.o sanity.o
	${CC} ${CFLAGS} txzchk.c rule_count.o dbg.o util.o location.o json.o utf8_posix_map.o sanity.o -o $@

jauthchk: jauthchk.c jauthchk.h json.h rule_count.o json.o dbg.o util.o sanity.o location.o \
	  utf8_posix_map.o
	${CC} ${CFLAGS} jauthchk.c rule_count.o json.o dbg.o util.o sanity.o location.o utf8_posix_map.o -o $@

jinfochk: jinfochk.c jinfochk.h rule_count.o json.o dbg.o util.o sanity.o location.o utf8_posix_map.o
	${CC} ${CFLAGS} jinfochk.c rule_count.o json.o dbg.o util.o sanity.o location.o utf8_posix_map.o -o $@

jstrencode: jstrencode.c jstrencode.h dbg.o json.o util.o
	${CC} ${CFLAGS} jstrencode.c dbg.o json.o util.o -o $@

jstrdecode: jstrdecode.c jstrdecode.h dbg.o json.o util.o
	${CC} ${CFLAGS} jstrdecode.c dbg.o json.o util.o -o $@

jint.test.o: jint.test.c Makefile
	${CC} ${CFLAGS} -DJINT_TEST_ENABLED jint.test.c -c

jint: jint.c dbg.o json.o util.o jint.test.o Makefile
	${CC} ${CFLAGS} -DJINT_TEST_ENABLED jint.c dbg.o json.o util.o jint.test.o -o $@

jfloat.test.o: jfloat.test.c Makefile
	${CC} ${CFLAGS} -DJFLOAT_TEST_ENABLED jfloat.test.c -c

jfloat: jfloat.c dbg.o json.o util.o jfloat.test.o Makefile
	${CC} ${CFLAGS} -DJFLOAT_TEST_ENABLED jfloat.c dbg.o json.o util.o jfloat.test.o -o $@

jparse: jparse.c jparse.h jparse.tab.c jparse.tab.h jparse.y jparse.l util.o dbg.o sanity.o json.o \
	utf8_posix_map.o location.o Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.c jparse.tab.c \
	    util.o dbg.o sanity.o json.o utf8_posix_map.o location.o -o $@

utf8_test: utf8_test.c utf8_posix_map.o dbg.o util.o
	${CC} ${CFLAGS} utf8_test.c utf8_posix_map.o dbg.o util.o -o $@

verge: verge.c dbg.o util.o
	${CC} ${CFLAGS} verge.c dbg.o util.o -o $@

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

jparse.tab.h jparse.tab.c: jparse.y bfok.sh jparse.tab.ref.h jparse.tab.ref.c Makefile
	${RM} -f jparse.tab.h jparse.tab.c
	@if `./bfok.sh ${BFOK_DIRS} 2>/dev/null`; then \
	    BISON_PATH="`./bfok.sh ${BFOK_DIRS} -p bison 2>/dev/null`"; \
	    if [[ -z $$BISON_PATH ]]; then \
	        echo "failed to discover the bison path" 1>&2; \
	        echo "will use backup files instead" 1>&2; \
		echo "${CP} -f -v jparse.tab.ref.h jparse.tab.h"; \
		${CP} -f -v jparse.tab.ref.h jparse.tab.h; \
		echo "${CP} -f -v jparse.tab.ref.c jparse.tab.c"; \
		${CP} -f -v jparse.tab.ref.c jparse.tab.c; \
	    else \
		echo "$$BISON_PATH -d jparse.y"; \
		"$$BISON_PATH" -d jparse.y; \
		if [[ ! -s jparse.tab.h || ! -s jparse.tab.c ]]; then \
		    echo "unable to form jparse.tab.h and/or jparse.tab.c"; \
		    echo "will use the backup file jparse.tab.h" 1>&2; \
		    echo "${CP} -f -v jparse.tab.ref.h jparse.tab.h"; \
		    ${CP} -f -v jparse.tab.ref.h jparse.tab.h; \
		    echo "will use the backup file jparse.tab.c" 1>&2; \
		    echo "${CP} -f -v jparse.tab.ref.c jparse.tab.c"; \
		    ${CP} -f -v jparse.tab.ref.c jparse.tab.c; \
		fi; \
	    fi; \
	else \
	    echo "no bison, with version >= BISON_VERSION in limit_ioccc.sh, found" 1>&2; \
	    echo "will move both backup files in place instead" 1>&2; \
	    echo "${CP} -f -v jparse.tab.ref.h jparse.tab.h"; \
	    ${CP} -f -v jparse.tab.ref.h jparse.tab.h; \
	    echo "${CP} -f -v jparse.tab.ref.c jparse.tab.c"; \
	    ${CP} -f -v jparse.tab.ref.c jparse.tab.c; \
	fi

jparse.c: jparse.l jparse.tab.h bfok.sh jparse.ref.c Makefile
	${RM} -f jparse.c
	@if `./bfok.sh ${BFOK_DIRS} 2>/dev/null`; then \
	    FLEX_PATH="`./bfok.sh ${BFOK_DIRS} -p flex 2>/dev/null`"; \
	    if [[ -z $$FLEX_PATH ]]; then \
	        echo "failed to discover the flex path" 1>&2; \
	        echo "will use backup files" 1>&2; \
		echo "${CP} -f -v jparse.ref.c jparse.c"; \
		${CP} -f -v jparse.ref.c jparse.c; \
	    else \
		echo "$$FLEX_PATH -o jparse.c jparse.l"; \
		"$$FLEX_PATH" -o jparse.c jparse.l; \
		if [[ ! -s jparse.c ]]; then \
		    echo "unable to form jparse.c"; \
		    echo "will use the backup file jparse.ref.c" 1>&2; \
		    echo "${CP} -f -v jparse.ref.c jparse.c"; \
		    ${CP} -f -v jparse.ref.c jparse.c; \
		fi; \
	    fi; \
	else \
	    echo "no flex, with version >= FLEX_VERSION in limit_ioccc.sh, found" 1>&2; \
	    echo "will move both backup file in place instead" 1>&2; \
	    echo "${CP} -f -v jparse.ref.c jparse.c"; \
	    ${CP} -f -v jparse.ref.c jparse.c; \
	fi


##########################################################################
# repo tools - rules mostly intended to be used by the mkiocccentry repo #
##########################################################################

# force the rebuild of the JSON parser and form reference copies of JSON parser C code
#
parser: jparse.y jparse.l Makefile
	${BISON} -d jparse.y
	${RM} -f jparse.tab.ref.h jparse.tab.ref.c
	${CP} -f -v jparse.tab.h jparse.tab.ref.h
	${CP} -f -v jparse.tab.c jparse.tab.ref.c
	${FLEX} -o jparse.c jparse.l
	${RM} -f jparse.ref.c
	${CP} -f -v jparse.c jparse.ref.c

# rebuild jint.test.c
#
rebuild_jint_test: jint.testset jint.c dbg.o json.o util.o Makefile
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
rebuild_jfloat_test: jfloat.testset jfloat.c dbg.o json.o util.o Makefile
	${RM} -f jfloat.set.tmp jfloat_gen
	${CC} ${CFLAGS} jfloat.c dbg.o json.o util.o -o jfloat_gen
	${SED} -n -e '1,/DO NOT REMOVE THIS LINE/p' < jfloat.test.c > jfloat.set.tmp
	echo '#if defined(JFLOAT_TEST_ENABLED)' >> jfloat.set.tmp
	./jfloat_gen -- $$(<jfloat.testset) >> jfloat.set.tmp
	echo '#endif /* JFLOAT_TEST_ENABLED */' >> jfloat.set.tmp
	${MV} -f jfloat.set.tmp jfloat.test.c
	${RM} -f jfloat_gen

# sequence exit codes
#
seqcexit: ${SRCFILES} ${FLEXFILES} ${BISONFILES} Makefile
	@HAVE_SEQCEXIT=`command -v ${SEQCEXIT}`; if [[ -z "$$HAVE_SEQCEXIT" ]]; then \
	    echo 'If you have not installed the seqcexit tool, then' 1>&2; \
	    echo 'you may not run this rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for seqcexit:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/seqcexit'; 1>&2; \
	    echo ''; 1>&2; \
	    exit 1; \
	fi
	${SEQCEXIT} -c -- ${FLEXFILES} ${BISONFILES}
	${SEQCEXIT} -- ${SRCFILES}

picky: ${SRCFILES} ${H_FILES} Makefile
	@if ! command -v ${PICKY} >/dev/null 2>&1; then \
	    echo "The picky command not found." 1>&2; \
	    echo "The picky command is required for this rule." 1>&2; \
	    echo "We recommend you install picky v2.6 or later" 1>&2; \
	    echo "from this URL:" 1>&2; \
	    echo 1>&2; \
	    echo "http://grail.eecs.csuohio.edu/~somos/picky.html" 1>&2; \
	    echo 1>&2; \
	    exit 1; \
	fi
	${PICKY} -w132 -u -s -t8 -v -e -- ${SRCFILES} ${H_FILES}
	${PICKY} -w132 -u -s -t8 -v -e -8 -- ${LESS_PICKY_SRC}

# inspect and verify shell scripts
#
shellcheck: ${SH_FILES} .shellcheckrc Makefile
	@HAVE_SHELLCHECK=`command -v ${SHELLCHECK}`; if [[ -z "$$HAVE_SHELLCHECK" ]]; then \
	    echo 'If you have not installed the shellcheck tool, then' 1>&2; \
	    echo 'you may not run this rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for shellcheck:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/koalaman/shellcheck.net'; 1>&2; \
	    echo ''; 1>&2; \
	    exit 1; \
	fi
	${SHELLCHECK} -f gcc -- ${SH_FILES}

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

# perform all of the mkiocccentry repo required tests
#
test: all iocccsize-test.sh dbg mkiocccentry-test.sh jstr-test.sh jint jfloat Makefile
	@chmod +x iocccsize-test.sh mkiocccentry-test.sh jstr-test.sh
	@echo "RUNNING: iocccsize-test.sh"
	./iocccsize-test.sh -v 1
	@echo "PASSED: iocccsize-test.sh"
	@echo
	@echo "This next test is supposed to fail with the error: FATAL[5]: main: simulated error, ..."
	@echo
	@echo "RUNNING: dbg"
	@${RM} -f dbg.out
	@status=`./dbg -e 2 foo bar baz 2>dbg.out; echo "$$?"`; \
	    if [ "$$status" != 5 ]; then \
		echo "exit status of dbg: $$status != 5"; \
		exit 1; \
	    fi
	@${GREP} -q '^FATAL\[5\]: main: simulated error, foo: foo bar: bar errno\[2\]: No such file or directory$$' dbg.out
	@${RM} -f dbg.out
	@echo "PASSED: dbg"
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

# run json-test.sh on test_JSON files
#
# Currently this is very chatty but the important parts will also be written to
# json-test.log for evaluation.
#
# NOTE: The verbose and debug options are important to see more information and
# this is important even when all cases are resolved as we want to see if there
# are any problems if new files are added.
#
test-jinfochk: all jinfochk Makefile
	./json-test.sh -t jinfo_only -v 1 -D 1 -d test_JSON


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to configure

clean:
	${RM} -f ${OBJFILES}
	${RM} -f ${SPECIAL_OBJ}
	${RM} -rf ${DSYMDIRS}

clobber: clean
	${RM} -f ${TARGETS} ${TEST_TARGETS}
	${RM} -f answers.txt j-test.out j-test2.out json-test.log
	${RM} -rf test-iocccsize test_src test_work tags dbg.out
	${RM} -f jint.set.tmp jint_gen
	${RM} -f jfloat.set.tmp jfloat_gen
	${RM} -rf jint_gen.dSYM jfloat_gen.dSYM

distclean nuke: clobber

install: all
	${INSTALL} -m 0555 ${TARGETS} ${DESTDIR}
	${INSTALL} -m 0644 ${MANPAGES} ${MANDIR}

tags: ${SRCFILES} ${H_FILES}
	-${CTAGS} -- ${SRCFILES} ${H_FILES} 2>&1 | \
	     ${GREP} -E -v 'Duplicate entry|Second entry ignored'

depend:
	@LINE="`grep -n '^### DO NOT CHANGE' Makefile|awk -F : '{print $$1}'`"; \
        if [ "$$LINE" = "" ]; then                                              \
                echo "Make depend aborted, tag not found in Makefile.";         \
                exit;                                                           \
        fi;                                                                     \
        mv -f Makefile Makefile.orig;head -n $$LINE Makefile.orig > Makefile;   \
        echo "Generating dependencies.";					\
        ${CC} ${CFLAGS} -MM ${SRCFILES} >> Makefile
	@echo "Make depend completed.";


###############
# make depend #
###############

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
sanity.o: sanity.c sanity.h util.h dbg.h location.h utf8_posix_map.h \
  json.h
utf8_test.o: utf8_test.c utf8_posix_map.h util.h dbg.h limit_ioccc.h \
  version.h
jint.o: jint.c jint.h dbg.h util.h json.h limit_ioccc.h version.h
jint.test.o: jint.test.c json.h
jfloat.o: jfloat.c jfloat.h dbg.h util.h json.h limit_ioccc.h version.h
jfloat.test.o: jfloat.test.c json.h
verge.o: verge.c verge.h dbg.h util.h limit_ioccc.h version.h
jparse.o: jparse.c jparse.h dbg.h util.h json.h sanity.h location.h \
  utf8_posix_map.h limit_ioccc.h version.h jparse.tab.h
jparse.tab.o: jparse.tab.c jparse.h dbg.h util.h json.h sanity.h \
  location.h utf8_posix_map.h limit_ioccc.h version.h jparse.tab.h
utf8_posix_map.o: utf8_posix_map.c utf8_posix_map.h util.h dbg.h \
  limit_ioccc.h version.h
