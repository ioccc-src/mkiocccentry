#!/usr/bin/env make
#
# mkiocccentry and related tools - how to form an IOCCC entry
#
# For the mkiocccentry utility:
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
# For the iocccsize utility:
#
# This IOCCC size tool source file.  See below for the VERSION string.
#
# "You are not expected to understand this" :-)
#
# Public Domain 1992, 2015, 2018, 2019, 2021 by Anthony Howe.  All rights released.
# With IOCCC mods in 2019-2022 by chongo (Landon Curt Noll) ^oo^
####

####
# For the dbg facility:
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
CAT= cat
CC= cc
CP= cp
CTAGS= ctags
GREP= grep
INSTALL= install
MAKE= make
MKTEMP= mktemp
MV= mv
PICKY= picky
RM= rm
RPL= rpl
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
# We need the following linker options for some systems:
#
#   -lm for floorl() under CentOS 7.
#
# If any more linker options are needed we will try documenting the reason why
# here.
#
LDFLAGS = -lm

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
STD_SRC= -D_DEFAULT_SOURCE -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=600 -std=gnu11

# optimization and debug level
#
#COPT= -O3 -g3
COPT= -O0 -g

# We disable specific warnings for some systems that differ from the others. Not
# all warnings are disabled here because there's at least one warning that only
# is needed in a few rules but others are needed in more. The following is the
# list of the warnings we disable _after_ enabling -Wall -Wextra -Werror:
#
#   -Wno-unused-command-line-argument
#
# This is needed because under CentOS we need -lm for floorl() (as described
# above); however under macOS you will get:
#
#	clang: error: -lm: 'linker' input unused
#	[-Werror,-Wunused-command-line-argument]
#	make: *** [sanity.o] Error 1
#
# for any object that does not need -lm and since many objects need to link in
# json.o (which is where the -lm is needed) we disable the warning here instead
# as it otherwise becomes very tedious.
#
# Additionally, the following are triggered with -Weverything but are entirely
# irrelevant to us. Although we don't enable -Weverything we have it here to
# make it not a problem in case we ever do enable it:
#
#   -Wno-poison-system-directories -Wno-unreachable-code-break -Wno-padded
#
WARN_FLAGS= -Wall -Wextra -Werror -Wno-unused-command-line-argument \
	    -Wno-poison-system-directories -Wno-unreachable-code-break -Wno-padded


# how to compile
#
# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
CFLAGS= ${STD_SRC} ${COPT} -pedantic ${WARN_FLAGS} ${LDFLAGS}


# NOTE: If you use ASAN, set this environment var:
#	ASAN_OPTIONS="detect_stack_use_after_return=1"
#
#CFLAGS= ${STD_SRC} -O0 -g -pedantic ${WARN_FLAGS} -fsanitize=address -fno-omit-frame-pointer

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
	jstrencode jstrdecode utf8_test jparse verge jnumber

# man pages
#
# Currently we explicitly specify a variable for man page targets as not all
# targets have a man page but probably all targets should have a man page. When
# this is done we can change the below two uncommented lines to be just:
#
#   MANPAGES = $(TARGETS:=.1)
#
# But until then it must be the next two lines (alternatively we could
# explicitly specify the man pages but this makes it simpler). When a new man
# page is written the MAN_TARGETS should have the tool name (without any
# extension) added to it.  Eventually MAN_TARGETS can be removed entirely and
# MANPAGES will act on TARGETS.
MAN_TARGETS = mkiocccentry txzchk fnamchk iocccsize jinfochk jauthchk jstrdecode jstrencode
MANPAGES= $(MAN_TARGETS:=.1)

TEST_TARGETS= dbg utf8_test dyn_test
OBJFILES= dbg.o util.o mkiocccentry.o iocccsize.o fnamchk.o txzchk.o jauthchk.o jinfochk.o \
	json.o jstrencode.o jstrdecode.o rule_count.o location.o sanity.o \
	utf8_test.o verge.o \
	dyn_array.o dyn_test.o json_chk.o json_entry.o dbg_test.o
LESS_PICKY_CSRC= utf8_posix_map.c
LESS_PICKY_OBJ= utf8_posix_map.o
GENERATED_CSRC= jparse.c jparse.tab.c
GENERATED_HSRC= jparse.tab.h
GENERATED_OBJ= jparse.o jparse.tab.o
FLEXFILES= jparse.l
BISONFILES= jparse.y
# This is a simpler way to do:
#
#   SRCFILES =  $(patsubst %.o,%.c,$(OBJFILES))
#
SRCFILES= $(OBJFILES:.o=.c)
ALL_CSRC= ${LESS_PICKY_CSRC} ${GENERATED_CSRC} ${SRCFILES}
H_FILES= dbg.h jauthchk.h jinfochk.h json.h jstrdecode.h jstrencode.h limit_ioccc.h \
	mkiocccentry.h txzchk.h util.h location.h utf8_posix_map.h jparse.h \
	verge.h sorry.tm.ca.h dyn_array.h dyn_test.h json_entry.h json_util.h
# This is a simpler way to do:
#
#   DSYMDIRS= $(patsubst %,%.dSYM,$(TARGETS))
#
DSYMDIRS= $(TARGETS:=.dSYM)
SH_FILES= iocccsize-test.sh jstr-test.sh limit_ioccc.sh mkiocccentry-test.sh json-test.sh \
	  jcodechk.sh vermod.sh prep.sh run_bison.sh run_flex.sh reset_tstamp.sh test.sh
BUILD_LOG= build.log

# RUN_O_FLAG - determine if the bison and flex backup files should be used
#
# RUN_O_FLAG=		use bison and flex backup files,
#			    if bison and/or flex not found or too old
# RUN_O_FLAG= -o	do not use bison and flex backup files,
#			    instead fail if bison and/or flex not found or too old
#
RUN_O_FLAG=
#RUN_O_FLAG= -o

# the basename of bison (or yacc) to look for
#
BISON_BASENAME = bison
#BISON_BASENAME = yacc

# Where run_bison.sh will search for bison with a recent enough version
#
# The -B arguments specify where to look for bison with a version,
# that is >= the minimum version (see BISON_VERSION in limit_ioccc.sh),
# before searching for bison on $PATH.
#
# NOTE: If is OK if these directories do not exist.
#
BISON_DIRS= \
	-B /opt/homebrew/opt/bison/bin \
	-B /opt/homebrew/bin \
	-B /opt/local/bin \
	-B /usr/local/opt \
	-B /usr/local/bin \
	-B .

# the basename of flex (or lex) to look for
#
FLEX_BASENAME= flex
#FLEX_BASENAME= lex

# Where run_flex.sh will search for flex with a recent enough version
#
# The -F arguments specify where to look for flex with a version,
# that is >= the minimum version (see FLEX_VERSION in limit_ioccc.sh),
# before searching for bison on $PATH.
#
# NOTE: If is OK if these directories do not exist.
#
FLEX_DIRS= \
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
# We put this directive just before the first all rule so that you may override
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
.PHONY: all configure clean clobber install test reset_min_timestamp \
	picky parser build clean_generated_obj prep_clobber


#####################################
# rules to compile and build source #
#####################################

rule_count.o: rule_count.c Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE rule_count.c -c

sanity.o: sanity.c json_chk.o Makefile
	${CC} ${CFLAGS} sanity.c json_chk.o -c

mkiocccentry.o: mkiocccentry.c Makefile
	${CC} ${CFLAGS} mkiocccentry.c -c

mkiocccentry: mkiocccentry.o rule_count.o dbg.o util.o dyn_array.o json.o json_entry.o json_chk.o \
	json_util.o location.o utf8_posix_map.o sanity.o Makefile
	${CC} ${CFLAGS} mkiocccentry.o rule_count.o dbg.o util.o dyn_array.o json.o json_entry.o \
	    json_chk.o json_util.o location.o utf8_posix_map.o sanity.o -o $@

iocccsize.o: iocccsize.c Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE iocccsize.c -c

iocccsize: iocccsize.o rule_count.o dbg.o Makefile
	${CC} ${CFLAGS} iocccsize.o rule_count.o dbg.o -o $@

dbg.o: dbg.c Makefile
	${CC} ${CFLAGS} dbg.c -c

dbg_test.c: dbg.c Makefile
	${RM} -f dbg_test.c
	${CP} -f -v dbg.c dbg_test.c

dbg_test.o: dbg_test.c Makefile
	${CC} ${CFLAGS} -DDBG_TEST dbg_test.c -c

dbg: dbg_test.o Makefile
	${CC} ${CFLAGS} dbg_test.o -o $@

fnamchk.o: fnamchk.c Makefile
	${CC} ${CFLAGS} fnamchk.c -c

fnamchk: fnamchk.o dbg.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} fnamchk.o dbg.o util.o dyn_array.o -o $@

txzchk.o: txzchk.c Makefile
	${CC} ${CFLAGS} txzchk.c -c

txzchk: txzchk.o rule_count.o dbg.o util.o dyn_array.o location.o json.o json_chk.o json_util.o \
	utf8_posix_map.o sanity.o Makefile
	${CC} ${CFLAGS} txzchk.o rule_count.o dbg.o util.o dyn_array.o location.o json.o \
	    json_chk.o json_util.o utf8_posix_map.o sanity.o -o $@

jauthchk.o: jauthchk.c Makefile
	${CC} ${CFLAGS} jauthchk.c -c

jauthchk: jauthchk.o rule_count.o json.o json_entry.o dbg.o util.o json_util.o \
	dyn_array.o sanity.o location.o utf8_posix_map.o Makefile
	${CC} ${CFLAGS} jauthchk.o rule_count.o json.o json_entry.o dbg.o util.o json_util.o \
	    dyn_array.o sanity.o json_chk.o location.o utf8_posix_map.o -o $@

jinfochk.o: jinfochk.c Makefile
	${CC} ${CFLAGS} jinfochk.c -c

jinfochk: jinfochk.o rule_count.o json.o json_entry.o dbg.o util.o json_util.o \
	dyn_array.o sanity.o location.o utf8_posix_map.o Makefile
	${CC} ${CFLAGS} jinfochk.o rule_count.o json.o json_entry.o dbg.o util.o json_util.o \
	    dyn_array.o sanity.o json_chk.o location.o utf8_posix_map.o -o $@

jstrencode.o: jstrencode.c Makefile
	${CC} ${CFLAGS} jstrencode.c -c

jstrencode: jstrencode.o dbg.o json.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} jstrencode.o dbg.o json.o util.o dyn_array.o -o $@

jstrdecode.o: jstrdecode.c Makefile
	${CC} ${CFLAGS} jstrdecode.c -c

jstrdecode: jstrdecode.o dbg.o json.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} jstrdecode.o dbg.o json.o util.o dyn_array.o -o $@

# XXX - jint is going away
#
jint.test.o: jint.test.c Makefile
	${CC} ${CFLAGS} -DJINT_TEST_ENABLED jint.test.c -c

jint.o: jint.c Makefile
	${CC} ${CFLAGS} -DJINT_TEST_ENABLED jint.c -c

jint: jint.o dbg.o json.o util.o dyn_array.o jint.test.o Makefile
	${CC} ${CFLAGS} jint.o dbg.o json.o util.o dyn_array.o jint.test.o -o $@

# XXX - jfloat is going away
#
jfloat.test.o: jfloat.test.c Makefile
	${CC} ${CFLAGS} -DJFLOAT_TEST_ENABLED jfloat.test.c -c

jfloat.o: jfloat.c Makefile
	${CC} ${CFLAGS} -DJFLOAT_TEST_ENABLED jfloat.c -c

jfloat: jfloat.o dbg.o json.o util.o dyn_array.o jfloat.test.o Makefile
	${CC} ${CFLAGS} jfloat.o dbg.o json.o util.o dyn_array.o jfloat.test.o -o $@

# XXX - stub until we make jnumber.c
#
jnumber: Makefile
	${CP} -f -v /usr/bin/true $@

jparse.o: jparse.c Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.c -c

json_chk.o: json_chk.c Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration json_chk.c -c

json_util.o: json_util.c Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration json_util.c -c

json_entry.o: json_entry.c Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration json_entry.c -c

jparse.tab.o: jparse.tab.c Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.tab.c -c

jparse: jparse.o jparse.tab.o util.o dyn_array.o dbg.o sanity.o json.o json_entry.o json_chk.o \
	json_util.o utf8_posix_map.o location.o Makefile
	${CC} ${CFLAGS} jparse.o jparse.tab.o util.o dyn_array.o dbg.o sanity.o \
	    json.o json_entry.o json_chk.o json_util.o utf8_posix_map.o location.o -o $@

utf8_test.o: utf8_test.c Makefile
	${CC} ${CFLAGS} utf8_test.c -c

utf8_test: utf8_test.o utf8_posix_map.o dbg.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} utf8_test.o utf8_posix_map.o dbg.o util.o dyn_array.o -o $@

verge.o: verge.c Makefile
	${CC} ${CFLAGS} verge.c -c

verge: verge.o dbg.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} verge.o dbg.o util.o dyn_array.o -o $@

dyn_array.o: dyn_array.c Makefile
	${CC} ${CFLAGS} dyn_array.c -c

dyn_test: dbg.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} dyn_test.c dbg.o util.o dyn_array.o -o $@

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

# How to create jparse.tab.c and jparse.tab.h
#
# Convert jparse.y into jparse.tab.as well as jparse.tab.c via bison,
# if bison is found and has a recent enough version, otherwise
# use a pre-built reference copies stored in jparse.tab.ref.h and jparse.tab.ref.c.
#
jparse.tab.c jparse.tab.h: jparse.y jparse.h sorry.tm.ca.h run_bison.sh limit_ioccc.sh verge \
	jparse.tab.ref.c jparse.tab.ref.h Makefile
	./run_bison.sh -b ${BISON_BASENAME} ${BISON_DIRS} -p jparse -v 1 ${RUN_O_FLAG} -- -d

# How to create jparse.c
#
# Convert jparse.l into jparse.c via flex,
# if flex found and has a recent enough version, otherwise
# use a pre-built reference copy stored in jparse.ref.c
#
jparse.c: jparse.l jparse.h sorry.tm.ca.h jparse.tab.h run_flex.sh limit_ioccc.sh verge jparse.ref.c Makefile
	./run_flex.sh -f ${FLEX_BASENAME} ${FLEX_DIRS} -p jparse -v 1 ${RUN_O_FLAG} -- -d -8 -o jparse.c


###################################################################
# repo tools - rules for those who maintain the mkiocccentry repo #
###################################################################

# Things to do before a release, forming a pull request and/or updating the
# GitHub repo
#
# Run through all of the prep steps.  If some step failed along the way, exit
# non-zero at the end.
#
prep: prep.sh
	${RM} -f ${BUILD_LOG}
	./prep.sh 2>&1 | ${TEE} ${BUILD_LOG}
	@echo NOTE: The above details were saved in the file: ${BUILD_LOG}

# Things to do before a release, forming a pull request and/or updating the
# GitHub repo.
#
# Run through all of the prep steps.  If a step fails, exit immediately.
# Moreover, the reference copies of JSON parser C code will not be used,
# so if bison and/or flex is not found or is too old, this rule will fail.
#
build release pull: prep.sh
	${RM} -f ${BUILD_LOG}
	./prep.sh -e -o 2>&1 | ${TEE} ${BUILD_LOG}
	@echo NOTE: The above details were saved in the file: ${BUILD_LOG}

# Force the rebuild of the JSON parser and form reference copies of JSON parser C code.
#
parser: jparse.y jparse.l Makefile
	${RM} -f jparse.tab.c jparse.tab.h
	${MAKE} jparse.tab.c jparse.tab.h
	${MAKE} jparse.tab.o
	${RM} -f jparse.c
	${MAKE} jparse.c
	${MAKE} jparse.o
	${RM} -f jparse.tab.ref.c
	${CP} -f -v jparse.tab.c jparse.tab.ref.c
	${RM} -f jparse.tab.ref.h
	${CP} -f -v jparse.tab.h jparse.tab.ref.h
	${RM} -f jparse.ref.c
	${CP} -f -v jparse.c jparse.ref.c

# Force the rebuild of the JSON parser, do NOT use reference copies of JSON parser C code.
#
parser-o: jparse.y jparse.l Makefile
	${MAKE} parser RUN_O_FLAG='-o'

# restore reference code that was produced by previous successful make parser
#
# This rule forces the use of reference copies of JSON parser C code.
use_ref: jparse.tab.ref.c jparse.tab.ref.h jparse.ref.c
	${RM} -f jparse.tab.c
	${CP} -f -v jparse.tab.ref.c jparse.tab.c
	${RM} -f jparse.tab.h
	${CP} -f -v jparse.tab.ref.h jparse.tab.h
	${RM} -f jparse.c
	${CP} -f -v jparse.ref.c jparse.c

# XXX - jint is going away
#
# rebuild jint.test.c
#
rebuild_jint_test: jint.testset jint.c dbg.o json.o util.o dyn_array.o Makefile
	${RM} -f jint.set.tmp jint_gen
	${CC} ${CFLAGS} jint.c dbg.o json.o util.o dyn_array.o -o jint_gen
	${SED} -n -e '1,/DO NOT REMOVE THIS LINE/p' < jint.test.c > jint.set.tmp
	echo '#if defined(JINT_TEST_ENABLED)' >> jint.set.tmp
	./jint_gen -- $$(<jint.testset) >> jint.set.tmp
	echo '#endif /* JINT_TEST_ENABLED */' >> jint.set.tmp
	${MV} -f jint.set.tmp jint.test.c
	${RM} -f jint_gen

# XXX - jfloat is going away
#
# rebuild jfloat.test.c
#
rebuild_jfloat_test: jfloat.testset jfloat.c dbg.o json.o util.o dyn_array.o Makefile
	${RM} -f jfloat.set.tmp jfloat_gen
	${CC} ${CFLAGS} jfloat.c dbg.o json.o util.o dyn_array.o -o jfloat_gen
	${SED} -n -e '1,/DO NOT REMOVE THIS LINE/p' < jfloat.test.c > jfloat.set.tmp
	echo '#if defined(JFLOAT_TEST_ENABLED)' >> jfloat.set.tmp
	./jfloat_gen -- $$(<jfloat.testset) >> jfloat.set.tmp
	echo '#endif /* JFLOAT_TEST_ENABLED */' >> jfloat.set.tmp
	${MV} -f jfloat.set.tmp jfloat.test.c
	${RM} -f jfloat_gen

# XXX - stub until we make jnumber.c
#
rebuild_jnumber_test: Makefile

# sequence exit codes
#
seqcexit: Makefile
	@HAVE_SEQCEXIT=`command -v ${SEQCEXIT}`; if [[ -z "$$HAVE_SEQCEXIT" ]]; then \
	    echo 'The seqcexit tool could not be found.' 1>&2; \
	    echo 'The seqcexit tool is required for this rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for seqcexit:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/seqcexit'; 1>&2; \
	    echo ''; 1>&2; \
	    exit 1; \
	else \
	    echo "${SEQCEXIT} -c -- ${FLEXFILES} ${BISONFILES}"; \
	    ${SEQCEXIT} -c -- ${FLEXFILES} ${BISONFILES}; \
	    echo "${SEQCEXIT} -- ${ALL_CSRC}"; \
	    ${SEQCEXIT} -- ${ALL_CSRC}; \
	fi

picky: ${ALL_CSRC} ${H_FILES} Makefile
	@if ! command -v ${PICKY} >/dev/null 2>&1; then \
	    echo "The picky tool could not found." 1>&2; \
	    echo "The picky tool is required for this rule." 1>&2; \
	    echo "We recommend you install picky v2.6 or later" 1>&2; \
	    echo "from this URL:" 1>&2; \
	    echo 1>&2; \
	    echo "http://grail.eecs.csuohio.edu/~somos/picky.html" 1>&2; \
	    echo 1>&2; \
	    exit 1; \
	else \
	    echo "${PICKY} -w132 -u -s -t8 -v -e -- ${SRCFILES} ${H_FILES}"; \
	    ${PICKY} -w132 -u -s -t8 -v -e -- ${SRCFILES} ${H_FILES}; \
	    echo "${PICKY} -w132 -u -s -t8 -v -e -8 -- ${LESS_PICKY_CSRC}"; \
	    ${PICKY} -w132 -u -s -t8 -v -e -8 -- ${LESS_PICKY_CSRC}; \
	fi

# inspect and verify shell scripts
#
shellcheck: ${SH_FILES} .shellcheckrc Makefile
	@HAVE_SHELLCHECK=`command -v ${SHELLCHECK}`; if [[ -z "$$HAVE_SHELLCHECK" ]]; then \
	    echo 'The shellcheck command could not found.' 1>&2; \
	    echo 'The shellcheck command is required to run this rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for shellcheck:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/koalaman/shellcheck.net'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'Or use the package manager in your OS to install it.' 1>&2; \
	    exit 1; \
	else \
	    echo "${SHELLCHECK} -f gcc -- ${SH_FILES}"; \
	    ${SHELLCHECK} -f gcc -- ${SH_FILES}; \
	fi

# Only run this rule when you wish to invalidate all timestamps
# prior to now, such as when you make a fundamental change to a
# critical JSON format, or make a fundamental change the compressed
# tarball file structure, or make a critical change to limit_ioccc.h
# that is MORE restrictive.
#
# DO NOT run this rule simply for a new IOCCC!
#
# Yes, we make it very hard to run this rule for good reason.
# Only IOCCC judges can perform ALL the steps needed to complete this action.
#
reset_min_timestamp: reset_tstamp.sh
	./reset_tstamp.sh

# perform all of the mkiocccentry repo required tests
#
test: test.sh iocccsize-test.sh dbg mkiocccentry-test.sh jstr-test.sh jnumber dyn_test Makefile
	./test.sh

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

# rule used by prep.sh and make clean
#
clean_generated_obj:
	${RM} -f ${GENERATED_OBJ}

# rule used by prep.sh and make clobber
#
prep_clobber:
	${RM} -f ${TARGETS} ${TEST_TARGETS}
	${RM} -f ${GENERATED_CSRC} ${GENERATED_HSRC}
	${RM} -f answers.txt j-test.out j-test2.out json-test.log
	${RM} -rf test-iocccsize test_src test_work tags dbg.out
	${RM} -f dbg_test.c
	${RM} -rf dyn_test.dSYM
	${RM} -f jnumber jnumber_gen
	${RM} -f jnumber.set.tmp jnumber_gen
	${RM} -rf jnumber.dSYM jnumber_gen.dSYM
	# XXX - remove legacy code - XXX
	${RM} -f jint jfloat
	${RM} -f jint.set.tmp jint_gen
	${RM} -f jfloat.set.tmp jfloat_gen
	${RM} -rf jint_gen.dSYM jfloat_gen.dSYM


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to configure

clean: clean_generated_obj
	${RM} -f ${OBJFILES}
	${RM} -f ${GENERATED_OBJ}
	${RM} -f ${LESS_PICKY_OBJ}
	${RM} -rf ${DSYMDIRS}
	# XXX - remove legacy code - XXX
	${RM} -f jint.o jint.test.o jint_gen.o
	${RM} -f jfloat.o jfloat.test.o jfloat_gen.o

clobber: clean prep_clobber
	${RM} -f ${BUILD_LOG}

distclean nuke: clobber

install: all
	${INSTALL} -v -m 0555 ${TARGETS} ${DESTDIR}
	${INSTALL} -v -m 0644 ${MANPAGES} ${MANDIR} 2>/dev/null

tags: ${ALL_CSRC} ${H_FILES}
	-${CTAGS} -- ${ALL_CSRC} ${H_FILES} 2>&1 | \
	     ${GREP} -E -v 'Duplicate entry|Second entry ignored'

depend: all
	@LINE="`grep -n '^### DO NOT CHANGE' Makefile|awk -F : '{print $$1}'`"; \
        if [ "$$LINE" = "" ]; then                                              \
                echo "Make depend aborted, tag not found in Makefile.";         \
                exit;                                                           \
        fi;                                                                     \
        mv -f Makefile Makefile.orig;head -n $$LINE Makefile.orig > Makefile;   \
        echo "Generating dependencies.";					\
        ${CC} ${CFLAGS} -MM ${ALL_CSRC} >> Makefile
	@echo "Make depend completed.";


###############
# make depend #
###############

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
utf8_posix_map.o: utf8_posix_map.c utf8_posix_map.h util.h dyn_array.h \
  dbg.h limit_ioccc.h version.h
jparse.o: jparse.c jparse.h dbg.h util.h dyn_array.h json.h sanity.h \
  location.h utf8_posix_map.h json_chk.h json_util.h limit_ioccc.h \
  version.h jparse.tab.h
jparse.tab.o: jparse.tab.c jparse.h dbg.h util.h dyn_array.h json.h \
  sanity.h location.h utf8_posix_map.h json_chk.h json_util.h \
  limit_ioccc.h version.h jparse.tab.h
dbg.o: dbg.c dbg.h
util.o: util.c dbg.h util.h dyn_array.h limit_ioccc.h version.h
mkiocccentry.o: mkiocccentry.c mkiocccentry.h util.h dyn_array.h dbg.h \
  json_entry.h location.h utf8_posix_map.h sanity.h json.h json_chk.h \
  json_util.h limit_ioccc.h version.h iocccsize.h
iocccsize.o: iocccsize.c iocccsize_err.h iocccsize.h
fnamchk.o: fnamchk.c fnamchk.h dbg.h util.h dyn_array.h limit_ioccc.h \
  version.h utf8_posix_map.h
txzchk.o: txzchk.c txzchk.h util.h dyn_array.h dbg.h sanity.h location.h \
  utf8_posix_map.h json.h json_chk.h json_util.h limit_ioccc.h version.h
jauthchk.o: jauthchk.c jauthchk.h dbg.h util.h dyn_array.h json.h \
  json_entry.h json_util.h sanity.h location.h utf8_posix_map.h \
  json_chk.h limit_ioccc.h version.h
jinfochk.o: jinfochk.c jinfochk.h dbg.h util.h dyn_array.h json.h \
  json_entry.h json_util.h sanity.h location.h utf8_posix_map.h \
  json_chk.h limit_ioccc.h version.h
json.o: json.c dbg.h util.h dyn_array.h limit_ioccc.h version.h json.h
jstrencode.o: jstrencode.c jstrencode.h dbg.h util.h dyn_array.h json.h \
  limit_ioccc.h version.h
jstrdecode.o: jstrdecode.c jstrdecode.h dbg.h util.h dyn_array.h json.h \
  limit_ioccc.h version.h
rule_count.o: rule_count.c iocccsize_err.h iocccsize.h
location.o: location.c location.h util.h dyn_array.h dbg.h
sanity.o: sanity.c sanity.h util.h dyn_array.h dbg.h location.h \
  utf8_posix_map.h json.h json_chk.h json_util.h limit_ioccc.h version.h
utf8_test.o: utf8_test.c utf8_posix_map.h util.h dyn_array.h dbg.h \
  limit_ioccc.h version.h
verge.o: verge.c verge.h dbg.h util.h dyn_array.h limit_ioccc.h version.h
dyn_array.o: dyn_array.c dyn_array.h util.h dbg.h
dyn_test.o: dyn_test.c dyn_test.h util.h dyn_array.h dbg.h version.h
json_chk.o: json_chk.c json_chk.h util.h dyn_array.h dbg.h json.h \
  json_util.h limit_ioccc.h version.h
json_entry.o: json_entry.c dbg.h util.h dyn_array.h json.h json_entry.h
dbg_test.o: dbg_test.c dbg.h
