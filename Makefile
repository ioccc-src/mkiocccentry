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
MAN= man
MAN2HTML= man2html
MKTEMP= mktemp
MV= mv
PICKY= picky
RM= rm
RPL= rpl
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
CHECKNR = checknr
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
# json_parse.o (which is where the -lm is needed) we disable the warning here instead
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
TARGETS= mkiocccentry iocccsize dbg fnamchk txzchk chkentry \
	jstrencode jstrdecode utf8_test jparse verge jnum_chk jnum_gen \
	jsemtblgen
SH_TARGETS=limit_ioccc.sh

# man pages
#
# We explicitly define the man page targets for more than three reasons:
#
# (0) Currently not all targets have man pages.
# (1) Some of the targets that have man pages are not actually in the TARGETS
#     variable. TEST_TARGETS, for example, has utf8_test, which means
#     utf8_test.1 would be left out. SH_TARGETS has limit_ioccc.sh which means
#     limit_ioccc.1 would be left out and so on.
# (2) Even when all targets have man pages if another target is added without
#     adding a man page make install would fail and even if make install is not
#     that likely to be used we still don't want it to fail in the case it
#     actually is used.
# (3) Along the lines of (2) there are some files that will have man pages
#     (run_bison.sh and run_flex.sh for two examples) that are not targets at
#     all but still important parts of the repo so these would be skipped as
#     well if we directly referred to TARGETS.
#
MAN1_TARGETS= mkiocccentry txzchk fnamchk iocccsize chkentry jstrdecode jstrencode \
	      verge jparse limit_ioccc utf8_test
MAN3_TARGETS= dbg
MAN8_TARGETS=
MAN_TARGETS= ${MAN1_TARGETS} ${MAN3_TARGETS} ${MAN8_TARGETS}
HTML_MAN_TARGETS= $(patsubst %,%.html,$(MAN_TARGETS))
# This is a simpler way to do:
#
#   MAN1PAGES = $(patsubst %,%.1,$(MAN1_TARGETS))
#   MAN3PAGES = $(patsubst %,%.3,$(MAN3_TARGETS))
#   MAN8PAGES = $(patsubst %,%.8,$(MAN8_TARGETS))
#
MAN1PAGES= $(MAN1_TARGETS:=.1)
MAN3PAGES= $(MAN3_TARGETS:=.3)
MAN8PAGES= $(MAN8_TARGETS:=.8)
MANPAGES= ${MAN1PAGES} ${MAN3PAGES} ${MAN8PAGES}

TEST_TARGETS= dbg utf8_test dyn_test
OBJFILES= dbg.o util.o mkiocccentry.o iocccsize.o fnamchk.o txzchk.o chkentry.o \
	json_parse.o jstrencode.o jstrdecode.o rule_count.o location.o sanity.o utf8_test.o verge.o \
	dyn_array.o dyn_test.o dbg_test.o jnum_chk.o jnum_gen.o jnum_test.o \
	json_util.o jparse_main.o entry_util.o jsemtblgen.o
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
H_FILES= dbg.h chkentry.h json_parse.h jstrdecode.h jstrencode.h limit_ioccc.h \
	mkiocccentry.h txzchk.h util.h location.h utf8_posix_map.h jparse.h \
	verge.h sorry.tm.ca.h dyn_array.h dyn_test.h json_util.h jnum_chk.h \
	jnum_gen.h jparse_main.h entry_util.h jsemtblgen.h
# This is a simpler way to do:
#
#   DSYMDIRS = $(patsubst %,%.dSYM,$(TARGETS))
#
DSYMDIRS= $(TARGETS:=.dSYM)
SH_FILES= iocccsize_test.sh jstr_test.sh limit_ioccc.sh mkiocccentry_test.sh \
	  vermod.sh prep.sh run_bison.sh run_flex.sh reset_tstamp.sh ioccc_test.sh \
	  jparse_test.sh txzchk_test.sh hostchk.sh
BUILD_LOG= build.log
TXZCHK_LOG=txzchk_test.log

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
# that is >= the minimum version (see MIN_BISON_VERSION in limit_ioccc.sh),
# before searching for bison on $PATH.
#
# NOTE: If is OK if these directories do not exist.
#
BISON_DIRS= \
	-B /opt/homebrew/opt/bison/bin \
	-B /usr/local/opt/bison/bin \
	-B /opt/homebrew/bin \
	-B /opt/local/bin \
	-B /usr/local/opt \
	-B /usr/local/bin \
	-B .

# Additional flags to pass to bison
#
# For --report all it will generate upon execution (if bison successfully
# generates the code) the file jparse.output. With --report all --html it will
# generate a html file which is easier to follow but I'm not sure how portable
# this is; under CentOS (which does not have the right version but actually
# normally generates code fine) the error:
#
#   jparse.y: error: xsltproc failed with status 127
#
# is thrown and since this could happen on other systems even with the
# appropriate version I have not enabled this.
#
# For the -Wcounterexamples it gives counter examples if there are ever
# shift/reduce conflicts in the grammar. The other warnings are of use as well.
#
BISON_FLAGS = -Werror -Wcounterexamples -Wmidrule-values -Wprecedence -Wdeprecated \
	      --report all --header

# the basename of flex (or lex) to look for
#
FLEX_BASENAME= flex
#FLEX_BASENAME= lex

# Where run_flex.sh will search for flex with a recent enough version
#
# The -F arguments specify where to look for flex with a version,
# that is >= the minimum version (see MIN_FLEX_VERSION in limit_ioccc.sh),
# before searching for bison on $PATH.
#
# NOTE: If is OK if these directories do not exist.
#
FLEX_DIRS= \
	-F /opt/homebrew/opt/flex/bin \
	-F /usr/local/opt/flex/bin \
	-F /opt/homebrew/bin \
	-F /opt/local/bin \
	-F /usr/local/opt \
	-F /usr/local/bin \
	-F .

# flags to pass to flex
#
FLEX_FLAGS = -8

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
	picky parser build clean_generated_obj prep_clobber rebuild_jnum_test


#####################################
# rules to compile and build source #
#####################################

rule_count.o: rule_count.c Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE rule_count.c -c

sanity.o: sanity.c Makefile
	${CC} ${CFLAGS} sanity.c -c

entry_util.o: entry_util.c Makefile
	${CC} ${CFLAGS} entry_util.c -c

mkiocccentry.o: mkiocccentry.c Makefile
	${CC} ${CFLAGS} mkiocccentry.c -c

mkiocccentry: mkiocccentry.o rule_count.o dbg.o util.o dyn_array.o json_parse.o entry_util.o \
	json_util.o location.o utf8_posix_map.o sanity.o Makefile
	${CC} ${CFLAGS} mkiocccentry.o rule_count.o dbg.o util.o dyn_array.o json_parse.o \
	    entry_util.o json_util.o location.o utf8_posix_map.o sanity.o -o $@

iocccsize.o: iocccsize.c Makefile
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE iocccsize.c -c

iocccsize: iocccsize.o rule_count.o dbg.o Makefile
	${CC} ${CFLAGS} iocccsize.o rule_count.o dbg.o -o $@

dbg.o: dbg.c dbg.h Makefile
	${CC} ${CFLAGS} dbg.c -c

dbg_test.c: dbg.c Makefile
	${RM} -f dbg_test.c
	${CP} -f -v dbg.c dbg_test.c

dbg_test.o: dbg_test.c Makefile
	${CC} ${CFLAGS} -DDBG_TEST dbg_test.c -c

dbg: dbg_test.o Makefile
	${CC} ${CFLAGS} dbg_test.o -o $@

dbg_example: dbg.c dbg.h dbg_example.c
	${CC} ${CFLAGS} dbg.c dbg_example.c -o $@

fnamchk.o: fnamchk.c fnamchk.h Makefile
	${CC} ${CFLAGS} fnamchk.c -c

fnamchk: fnamchk.o dbg.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} fnamchk.o dbg.o util.o dyn_array.o -o $@

txzchk.o: txzchk.c txzchk.h Makefile
	${CC} ${CFLAGS} txzchk.c -c

txzchk: txzchk.o dbg.o util.o dyn_array.o location.o \
	utf8_posix_map.o sanity.o Makefile
	${CC} ${CFLAGS} txzchk.o dbg.o util.o dyn_array.o location.o \
	     utf8_posix_map.o sanity.o -o $@

chkentry.o: chkentry.c chkentry.h Makefile
	${CC} ${CFLAGS} chkentry.c -c

chkentry: chkentry.o dbg.o util.o dyn_array.o json_parse.o json_util.o Makefile
	${CC} ${CFLAGS} chkentry.o dbg.o util.o dyn_array.o json_parse.o json_util.o -o $@

jstrencode.o: jstrencode.c jstrencode.h json_util.h json_util.c Makefile
	${CC} ${CFLAGS} jstrencode.c -c

jstrencode: jstrencode.o dbg.o json_parse.o json_util.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} jstrencode.o dbg.o json_parse.o json_util.o util.o dyn_array.o -o $@

jstrdecode.o: jstrdecode.c jstrdecode.h json_util.h json_parse.h Makefile
	${CC} ${CFLAGS} jstrdecode.c -c

jstrdecode: jstrdecode.o dbg.o json_parse.o json_util.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} jstrdecode.o dbg.o json_parse.o json_util.o util.o dyn_array.o -o $@

jnum_test.o: jnum_test.c Makefile
	${CC} ${CFLAGS} jnum_test.c -c

jnum_chk.o: jnum_chk.c jnum_chk.h Makefile
	${CC} ${CFLAGS} jnum_chk.c -c

jnum_chk: jnum_chk.o dbg.o json_parse.o json_util.o util.o dyn_array.o jnum_test.o Makefile
	${CC} ${CFLAGS} jnum_chk.o dbg.o json_parse.o json_util.o util.o dyn_array.o jnum_test.o -o $@

jnum_gen.o: jnum_gen.c jnum_gen.h Makefile
	${CC} ${CFLAGS} jnum_gen.c -c

jnum_gen: jnum_gen.o dbg.o json_parse.o json_util.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} jnum_gen.o dbg.o json_parse.o json_util.o util.o dyn_array.o -o $@

jparse.o: jparse.c jparse.h Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.c -c

json_util.o: json_util.c json_util.h Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration json_util.c -c

jparse.tab.o: jparse.tab.c Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.tab.c -c

jparse_main.o: jparse_main.c Makefile
	${CC} ${CFLAGS} jparse_main.c -c

jparse: jparse.o jparse.tab.o util.o dyn_array.o dbg.o json_parse.o \
	json_util.o jparse_main.o Makefile
	${CC} ${CFLAGS} jparse.o jparse.tab.o util.o dyn_array.o dbg.o json_parse.o \
			json_util.o jparse_main.o -o $@

jsemtblgen.o: jsemtblgen.c Makefile
	${CC} ${CFLAGS} jsemtblgen.c -c

jsemtblgen: jsemtblgen.o jparse.o jparse.tab.o util.o dyn_array.o dbg.o json_parse.o \
	    json_util.o rule_count.o Makefile
	${CC} ${CFLAGS} jsemtblgen.o jparse.o jparse.tab.o util.o dyn_array.o dbg.o json_parse.o \
			json_util.o rule_count.o -o $@

utf8_test.o: utf8_test.c utf8_posix_map.h Makefile
	${CC} ${CFLAGS} utf8_test.c -c

utf8_test: utf8_test.o utf8_posix_map.o dbg.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} utf8_test.o utf8_posix_map.o dbg.o util.o dyn_array.o -o $@

verge.o: verge.c verge.h Makefile
	${CC} ${CFLAGS} verge.c -c

verge: verge.o dbg.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} verge.o dbg.o util.o dyn_array.o -o $@

dyn_array.o: dyn_array.c Makefile
	${CC} ${CFLAGS} dyn_array.c -c

dyn_test.o: dyn_test.c Makefile
	${CC} ${CFLAGS} dyn_test.c -c

dyn_test: dyn_test.o dbg.o util.o dyn_array.o Makefile
	${CC} ${CFLAGS} dyn_test.o dbg.o util.o dyn_array.o -o $@

limit_ioccc.sh: limit_ioccc.h version.h dbg.h dyn_array.h dyn_test.h jparse.h jparse_main.h \
		Makefile
	${RM} -f $@
	@echo '#!/usr/bin/env bash' > $@
	@echo '#' >> $@
	@echo '# Copies of select limit_ioccc.h and version.h values for shell script use' >> $@
	@echo '#' >> $@
	${GREP} -E '^#define (RULE_|MAX_|UUID_|MIN_|IOCCC_)' limit_ioccc.h | \
	    ${AWK} '{print $$2 "=\"" $$3 "\"" ;}' | ${TR} -d '[a-z]()' | \
	    ${SED} -e 's/"_/"/' -e 's/""/"/g' -e 's/^/export /' >> $@
	${GREP} -hE '^#define (.*_VERSION|TIMESTAMP_EPOCH|JSON_PARSING_DIRECTIVE_)' \
		     version.h limit_ioccc.h dbg.h dyn_array.h dyn_test.h jparse.h jparse_main.h | \
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
# Convert jparse.y into jparse.tab.c and jparse.tab.c via bison, if bison is
# found and has a recent enough version. Otherwise, if RUN_O_FLAG is NOT
# specified use a pre-built reference copies stored in jparse.tab.ref.h and
# jparse.tab.ref.c. If it IS specified it is an error.
#
# NOTE: The value of RUN_O_FLAG depends on what rule called this rule.
jparse.tab.c jparse.tab.h bison: jparse.y jparse.h sorry.tm.ca.h run_bison.sh limit_ioccc.sh \
	verge jparse.tab.ref.c jparse.tab.ref.h Makefile
	./run_bison.sh -b ${BISON_BASENAME} ${BISON_DIRS} -p jparse -v 1 ${RUN_O_FLAG} -- \
		       ${BISON_FLAGS}

# How to create jparse.c
#
# Convert jparse.l into jparse.c via flex, if flex found and has a recent enough
# version. Otherwise, if RUN_O_FLAG is NOT set use the pre-built reference copy
# stored in jparse.ref.c. If it IS specified it is an error.
#
# NOTE: The value of RUN_O_FLAG depends on what rule called this rule.
jparse.c flex: jparse.l jparse.h sorry.tm.ca.h jparse.tab.h run_flex.sh limit_ioccc.sh \
	       verge jparse.ref.c Makefile
	./run_flex.sh -f ${FLEX_BASENAME} ${FLEX_DIRS} -p jparse -v 1 ${RUN_O_FLAG} -- \
		      ${FLEX_FLAGS} -o jparse.c


###################################################################
# repo tools - rules for those who maintain the mkiocccentry repo #
###################################################################

#
# make build release pull
#
# Things to do before a release, forming a pull request and/or updating the
# GitHub repo.
#
# This runs through all of the prep steps, exiting on the first failure.
#
# NOTE: The reference copies of the JSON parser C code will NOT be used
# so if bison and/or flex is not found or too old THIS RULE WILL FAIL!
#
# NOTE: Please try this rule BEFORE make prep.
#
build release pull: prep.sh
	${RM} -f ${BUILD_LOG}
	@echo "./prep.sh -e -o 2>&1 | ${TEE} ${BUILD_LOG}"
	@./prep.sh -e -o 2>&1 | ${TEE} "${BUILD_LOG}"; \
	    EXIT_CODE="$${PIPESTATUS[0]}"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo "=-=-=-=-= Warning: prep.sh error code: $$EXIT_CODE =-=-=-=-=" 1>&2; \
		echo 1>&2; \
		echo "NOTE: The above details were saved in the file: ${BUILD_LOG}"; \
		echo 1>&2; \
		exit 1; \
	    else \
		echo "NOTE: The above details were saved in the file: ${BUILD_LOG}"; \
	    fi

#
# make prep
#
# Things to do before a release, forming a pull request and/or updating the
# GitHub repo.
#
# This runs through all of the prep steps.  If some step failed along the way,
# exit non-zero at the end.
#
# NOTE: This rule is useful if for example you're not working on the parser and
# you're on a system without the proper versions of flex and/or bison but you
# still want to work on the repo. Another example use is if you don't have
# shellcheck and/or picky and you want to work on the repo.
#
# The point is: if you're working on this repo and make build fails, try this
# rule instead.
#
prep: prep.sh
	${RM} -f ${BUILD_LOG}
	@echo "./prep.sh 2>&1 | ${TEE} ${BUILD_LOG}"
	@-./prep.sh 2>&1 | ${TEE} "${BUILD_LOG}"; \
	    EXIT_CODE="$${PIPESTATUS[0]}"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo "=-=-=-=-= Warning: prep.sh error code: $$EXIT_CODE =-=-=-=-=" 1>&2; \
		echo 1>&2; \
	    fi
	@echo NOTE: The above details were saved in the file: ${BUILD_LOG}

#
# make parser
#
# Force the rebuild of the JSON parser and then form the reference copies of
# JSON parser C code (if recent enough version of flex and bison are found).
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
	${MAKE} jparse jsemtblgen

#
# make parser-o: Force the rebuild of the JSON parser.
#
# NOTE: This does NOT use the reference copies of JSON parser C code.
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

# use jnum_gen to regenerate test jnum_chk test suite
#
rebuild_jnum_test: jnum_gen jnum.testset jnum_header.c Makefile
	${RM} -f jnum_test.c
	${CP} -f -v jnum_header.c jnum_test.c
	./jnum_gen jnum.testset >> jnum_test.c

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

# inspect and verify man pages
#
checknr: ${MANPAGES}
	@HAVE_CHECKNR=`command -v ${CHECKNR}`; if [[ -z "$$HAVE_CHECKNR" ]]; then \
	    echo 'The checknr command could not found.' 1>&2; \
	    echo 'The checknr command is required to run this rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    exit 1; \
	else \
	    echo "${CHECKNR} -c.BR.SS.BI ${MANPAGES}"; \
	    ${CHECKNR} -c.BR.SS.BI ${MANPAGES}; \
	fi

man2html: ${MANPAGES}
	@HAVE_MAN2HTML=`command -v ${MAN2HTML}`; \
	 HAVE_MAN=`command -v ${MAN}`; \
	if [[ -z "$$HAVE_MAN2HTML" ]]; then \
	    echo 'The man2html command could not found.' 1>&2; \
	    echo 'The man2html command is required to run this rule.'; 1>&2; \
	fi; \
	if [[ -z "$$HAVE_MAN" ]]; then \
	    echo 'The man command could not found.' 1>&2; \
	    echo 'The man command is required to run this rule.'; 1>&2; \
	fi; \
	if [[ -z "$$HAVE_MAN2HTML" || -z "$$HAVE_MAN" ]]; then \
	    echo ''; 1>&2; \
	    exit 1; \
	fi
	@for i in ${MAN1_TARGETS}; do \
	    echo ${RM} -f "$$i.html"; \
	    ${RM} -f "$$i.html"; \
	    echo "${MAN} ./$$i.1 | ${MAN2HTML} -compress -title $$i > $$i.html"; \
	    ${MAN} "./$$i.1" | ${MAN2HTML} -compress -title "$$i" > "$$i.html"; \
	done
	@for i in ${MAN3_TARGETS}; do \
	    echo ${RM} -f "$$i.html"; \
	    ${RM} -f "$$i.html"; \
	    echo "${MAN} ./$$i.3 | ${MAN2HTML} -compress -title $$i > $$i.html"; \
	    ${MAN} "./$$i.3" | ${MAN2HTML} -compress -title "$$i" > "$$i.html"; \
	done
	@for i in ${MAN8_TARGETS}; do \
	    echo ${RM} -f "$$i.html"; \
	    ${RM} -f "$$i.html"; \
	    echo "${MAN} ./$$i.8 | ${MAN2HTML} -compress -title $$i > $$i.html"; \
	    ${MAN} "./$$i.8" | ${MAN2HTML} -compress -title "$$i" > "$$i.html"; \
	done

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
test ioccc_test: ioccc_test.sh iocccsize_test.sh dbg mkiocccentry_test.sh jstr_test.sh \
		 jnum_chk dyn_test txzchk_test.sh txzchk jparse Makefile
	./ioccc_test.sh

hostchk bug-report: hostchk.sh
	./hostchk.sh

# run test-chkentry on test_JSON files
#
test-chkentry: all chkentry Makefile
	@echo TBD

# rule used by prep.sh and make clean
#
clean_generated_obj:
	${RM} -f ${GENERATED_OBJ}

# rule used by prep.sh and make clobber
#
prep_clobber:
	${RM} -f ${TARGETS} ${TEST_TARGETS}
	${RM} -f ${GENERATED_CSRC} ${GENERATED_HSRC}
	${RM} -f answers.txt
	${RM} -f jstr_test.out jstr_test2.out
	${RM} -rf test_iocccsize test_src test_work tags dbg.out
	${RM} -f jparse.output jparse.html
	${RM} -f ${TXZCHK_LOG}
	${RM} -f dbg_test.c
	${RM} -rf dyn_test.dSYM
	${RM} -f jnum_chk
	${RM} -rf jnum_chk.dSYM
	${RM} -f jnum_gen
	${RM} -rf jnum_gen.dSYM
	# XXX - remove legacy code - XXX
	${RM} -f jnumber
	${RM} -rf jnumber.dSYM
	${RM} -f jint jfloat
	${RM} -f jint.set.tmp jint_gen
	${RM} -f jfloat.set.tmp jfloat_gen
	${RM} -rf jint_gen.dSYM jfloat_gen.dSYM
	${RM} -f .sorry.*
	${RM} -f .exit_code.*


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
	${RM} -f jparse_test.log chkentry_test.log txzchk_test.log
	${RM} -f ${HTML_MAN_TARGETS}

distclean nuke: clobber

install: all
	${INSTALL} -v -m 0555 ${TARGETS} ${SH_TARGETS} ${DESTDIR}
	${INSTALL} -v -m 0644 ${MANPAGES} ${MANDIR} 2>/dev/null

tags: ${ALL_CSRC} ${H_FILES}
	-${CTAGS} ${ALL_CSRC} ${H_FILES} 2>&1 | \
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
jparse.o: jparse.c jparse.h dbg.h util.h dyn_array.h json_parse.h \
  json_util.h jparse.tab.h
jparse.tab.o: jparse.tab.c jparse.h dbg.h util.h dyn_array.h json_parse.h \
  json_util.h jparse.tab.h
dbg.o: dbg.c dbg.h
util.o: util.c dbg.h util.h dyn_array.h limit_ioccc.h version.h
mkiocccentry.o: mkiocccentry.c mkiocccentry.h util.h dyn_array.h dbg.h \
  location.h utf8_posix_map.h limit_ioccc.h version.h sanity.h \
  iocccsize.h json_util.h json_parse.h entry_util.h
iocccsize.o: iocccsize.c iocccsize_err.h iocccsize.h
fnamchk.o: fnamchk.c fnamchk.h dbg.h util.h dyn_array.h limit_ioccc.h \
  version.h utf8_posix_map.h
txzchk.o: txzchk.c txzchk.h util.h dyn_array.h dbg.h sanity.h location.h \
  utf8_posix_map.h limit_ioccc.h version.h
chkentry.o: chkentry.c chkentry.h dbg.h json_util.h dyn_array.h \
  json_parse.h util.h version.h
json_parse.o: json_parse.c dbg.h util.h dyn_array.h json_parse.h \
  json_util.h
jstrencode.o: jstrencode.c jstrencode.h dbg.h util.h dyn_array.h \
  json_parse.h limit_ioccc.h version.h
jstrdecode.o: jstrdecode.c jstrdecode.h dbg.h util.h dyn_array.h \
  json_parse.h limit_ioccc.h version.h
rule_count.o: rule_count.c iocccsize_err.h iocccsize.h
location.o: location.c location.h util.h dyn_array.h dbg.h
sanity.o: sanity.c sanity.h util.h dyn_array.h dbg.h location.h \
  utf8_posix_map.h limit_ioccc.h version.h
utf8_test.o: utf8_test.c utf8_posix_map.h util.h dyn_array.h dbg.h \
  limit_ioccc.h version.h
verge.o: verge.c verge.h dbg.h util.h dyn_array.h limit_ioccc.h version.h
dyn_array.o: dyn_array.c dyn_array.h dbg.h
dyn_test.o: dyn_test.c dyn_test.h util.h dyn_array.h dbg.h
dbg_test.o: dbg_test.c dbg.h
jnum_chk.o: jnum_chk.c jnum_chk.h dbg.h util.h dyn_array.h json_parse.h \
  json_util.h version.h
jnum_gen.o: jnum_gen.c jnum_gen.h dbg.h util.h dyn_array.h json_parse.h \
  json_util.h limit_ioccc.h version.h
jnum_test.o: jnum_test.c json_parse.h util.h dyn_array.h dbg.h
json_util.o: json_util.c dbg.h json_parse.h util.h dyn_array.h \
  json_util.h
jparse_main.o: jparse_main.c jparse_main.h dbg.h util.h dyn_array.h \
  jparse.h json_parse.h json_util.h jparse.tab.h
entry_util.o: entry_util.c dbg.h util.h dyn_array.h entry_util.h \
  json_parse.h
jsemtblgen.o: jsemtblgen.c jsemtblgen.h dbg.h util.h dyn_array.h \
  json_util.h json_parse.h jparse.h jparse.tab.h json_sem.h iocccsize.h
