#!/usr/bin/env make
#
# mkiocccentry and related tools - how to form an IOCCC entry
#
# For the mkiocccentry utility:
#
# Copyright (c) 2021-2023 by Landon Curt Noll.  All Rights Reserved.
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
# See also, the following files for additional Copyright information:
#
# iocccsize.c iocccsize.h soup/rule_count.c dbg/* dyn_alloc/*
####


#############
# utilities #
#############

# suggestion: List utility filenames, not paths.
#	      Do not list shell builtin (echo, cd, ...) tools.
#	      Keep the list in alphabetical order.
#
AWK= awk
CC= cc
CMP= cmp
CTAGS= ctags
DIFF= diff
GIT= git
GREP= grep
INDEPEND= independ
INSTALL= install
MAKE= make
MV= mv
PICKY= picky
RM= rm
RSYNC= rsync
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
SLEEP= sleep
TEE= tee
TR= tr

######################
# URLs for utilities #
######################
CHECKNR_URL = https://github.com/lcn2/checknr
PICKY_URL= https://github.com/xexyl/picky
SEQCEXIT_URL= https://github.com/lcn2/seqcexit

####################
# Makefile control #
####################

# The name of this directory
#
# This value is used to print the generic name of this directory
# so that various echo statements below can use this name
# to help distinguish themselves from echo statements used
# by Makefiles in other directories.
#
OUR_NAME= mkiocccentry

# Q= @					do not echo internal Makefile actions (quiet mode)
# Q=					echo internal Makefile actions (debug / verbose mode)
#
#Q=
Q= @

# V= @:					do not echo debug statements (quiet mode)
# V= @					echo debug statements (debug / verbose mode)
#
V= @:
#V= @

# S= @:					do not echo start or end of a make rule (quiet mode)
# S= @					echo start or end of a make rule (debug / verbose mode)
#
#S= @:
S= @

# M= @:					do not echo calling make in another directory (quiet mode)
# M= @					echo calling make in another directory (debug / verbose mode)
#
M=
#M= @

# Q= implies -v 0
# else -v 1
#
ifeq ($(strip ${Q}),@)
Q_V_OPTION="0"
else
Q_V_OPTION="1"
endif

# INSTALL_Q= @				do not echo install commands (quiet mode)
# INSTALL_Q=				echo install commands (debug / verbose mode
#
INSTALL_Q=
#INSTALL_Q= @

# INSTALL_V=				install w/o -v flag (quiet mode)
# INSTALL_V= -v				install with -v (debug / verbose mode
#
#INSTALL_V=
INSTALL_V= -v

# MAKE_CD_Q= --no-print-directory	silence make cd messages (quiet mode)
# MAKE_CD_Q=				silence make cd messages (quiet mode)
#
MAKE_CD_Q= --no-print-directory
#MAKE_CD_Q=


##################
# How to compile #
##################

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
# TODO - ############################################################################### - TODO #
# TODO - Sometime in 2023 we will will support only c17 so C_STD will become -std=gnu17  - TODO #
# TODO - ############################################################################### - TODO #
#
C_STD= -std=gnu11
#C_STD= -std=gnu17

# optimization and debug level
#
#C_OPT= -O3 -g3	# TODO - this will be the production release value - TODO #
C_OPT= -O0 -g

# Compiler warnings
#
#WARN_FLAGS= -Wall -Wextra	# TODO - this will be the production release value - TODO #
WARN_FLAGS= -Wall -Wextra -Werror


# how to compile
#
# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
CFLAGS= ${C_STD} ${C_OPT} -pedantic ${WARN_FLAGS} ${LDFLAGS}


# NOTE: If you use ASAN, set this environment var:
#	ASAN_OPTIONS="detect_stack_use_after_return=1"
#
#CFLAGS= ${C_STD} -O0 -g -pedantic ${WARN_FLAGS} ${LDFLAGS} -fsanitize=address -fno-omit-frame-pointer


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= mkiocccentry.c iocccsize.c txzchk.c chkentry.c
H_SRC= chkentry.h mkiocccentry.h txzchk.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=

# all shell scripts
#
SH_FILES= bug_report.sh

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES=
MAN3_PAGES=
MAN8_PAGES=
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC=
BUILT_H_SRC=
ALL_BUILT_SRC= ${BUILT_C_SRC} ${BUILT_H_SRC}

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS=

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS= mkiocccentry.o iocccsize.o txzchk.o chkentry.o

# all intermediate files and removed by make clean
#
ALL_OBJS= ${LIB_OBJS} ${OTHER_OBJS}

# all source files
#
ALL_CSRC= ${C_SRC} ${LESS_PICKY_CSRC} ${BUILT_C_SRC}
ALL_HSRC= ${H_SRC} ${LESS_PICKY_HSRC} ${BUILT_H_SRC}
ALL_SRC= ${ALL_CSRC} ${ALL_HSRC} ${SH_FILES}

# all man pages that built and removed by make clobber
#
MAN1_BUILT=
MAN3_BUILT=
MAN8_BUILT=
ALL_MAN_BUILT= ${MAN1_BUILT} ${MAN3_BUILT} ${MAN8_BUILT}


#######################
# install information #
#######################

# where to install
#
MAN1_DIR= /usr/local/share/man/man1
MAN3_DIR= /usr/local/share/man/man3
MAN8_DIR= /usr/local/share/man/man8
DEST_INCLUDE= /usr/local/include
DEST_LIB= /usr/local/lib
DEST_DIR= /usr/local/bin


#################################
# external Makefile information #
#################################

# may be used outside of this directory
#
EXTERN_H=
EXTERN_O=
EXTERN_MAN=
EXTERN_LIBA=
EXTERN_PROG= bug_report.sh chkentry iocccsize mkiocccentry txzchk

# NOTE: ${EXTERN_CLOBBER} used outside of this directory and removed by make clobber
#
EXTERN_CLOBBER= ${EXTERN_O} ${EXTERN_LIBA} ${EXTERN_PROG}


######################
# target information #
######################

# man pages
#
MAN1_TARGETS= ${MAN1_PAGES} ${MAN1_BUILT}
MAN3_TARGETS= ${MAN3_PAGES} ${MAN3_BUILT}
MAN8_TARGETS= ${MAN8_PAGES} ${MAN8_BUILT}
ALL_MAN_TARGETS= ${MAN1_TARGETS} ${MAN3_TARGETS} ${MAN8_TARGETS}

# shell targets to make by all and removed by clobber
#
SH_TARGETS=

# program targets to make by all, installed by install, and removed by clobber
#
PROG_TARGETS= mkiocccentry iocccsize txzchk chkentry

# directories sometimes build under macOS and removed by clobber
#
DSYMDIRS= mkiocccentry.dSYM iocccsize.dSYM txzchk.dSYM chkentry.dSYM

TMP_BUILD_LOG= .build.log
BUILD_LOG= build.log

# NOTE: For valgrind, run with:
#
#	valgrind --leak-check=yes --track-origins=yes --leak-resolution=high --read-var-info=yes \
#           --leak-check=full --show-leak-kinds=all ./mkiocccentry ...
#
# NOTE: Replace mkiocccentry with whichever tool you want to test and the ...
# with the arguments and options you want.

ALL_SUBDIRS= all_dbg all_dyn_array all_jparse all_jparse_test all_man all_soup all_test_ioccc

# what to make by all but NOT to removed by clobber
#
ALL_OTHER_TARGETS= ${SH_TARGETS} ${ALL_SUBDIRS} ${ALL_MAN_PAGES} build_man

# what to make by all, what to install, and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${PROG_TARGETS} ${ALL_MAN_BUILT}


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

all: fast_hostchk just_all
	@:

# The original make all that bypasses running hostchk.sh
#
just_all: ${ALL_SUBDIRS} ${TARGETS} build_man

# fast build environment sanity check
#
fast_hostchk: soup/hostchk.sh
	@-./soup/hostchk.sh -f -v 0; \
	EXIT_CODE="$$?"; \
	if [ "$$EXIT_CODE" -ne 0 ]; then \
	    ${MAKE} hostchk_warning; \
	fi

bug_report: bug_report.sh
	@-./bug_report.sh -v 1

bug_report-tx: bug_report.sh
	@echo "${OUR_NAME}: make $@: starting test of bug_report.sh -t -x"
	@echo
	@-./bug_report.sh -t -x
	@echo
	@echo "${OUR_NAME}: finished test of bug_report.sh -t -x"

bug_report-txl: bug_report.sh
	@echo "${OUR_NAME}: make $@: starting test of bug_report.sh -t -x -l"
	@-./bug_report.sh -t -x -l
	@echo "${OUR_NAME}: finished test of bug_report.sh -t -x -l"

# slower more verbose build environment sanity check
#
hostchk: soup/hostchk.sh
	@-./soup/hostchk.sh -v 1; \
	EXIT_CODE="$$?"; \
	if [ "$$EXIT_CODE" -ne 0 ]; then \
	    ${MAKE} hostchk_warning; \
	fi

# get the users attention when hostchk.sh finds a problem
#
hostchk_warning:
	@echo 1>&2
	@echo '=-= WARNING WARNING WARNING =-=' 1>&2
	@echo '=-= hostchk exited non-zero =-=' 1>&2
	@echo 1>&2
	@echo '=-= WARNING WARNING WARNING =-=' 1>&2
	@echo '=-= your build environment may not be able to compile =-=' 1>&2
	@echo '=-= mkiocccentry and friends =-=' 1>&2
	@echo 1>&2
	@echo '=-= WARNING WARNING WARNING =-=' 1>&2
	@echo '=-= For hints as to what might be wrong try running:' 1>&2
	@echo 1>&2
	@echo '    ./soup/hostchk.sh -v 1' 1>&2
	@echo 1>&2
	@echo '=-= WARNING WARNING WARNING =-=' 1>&2
	@echo '=-= If you think this is a bug, consider filing a bug report via:' 1>&2
	@echo 1>&2
	@echo '    ./bug_report.sh' 1>&2
	@echo 1>&2
	@echo '=-= about to sleep 10 seconds =-=' 1>&2
	@echo 1>&2
	${SLEEP} 10
	@echo '=-= Letting the compile continue in hopes it might be OK, =-=' 1>&2
	@echo '=-= even though we doubt it will be OK. =-=' 1>&2
	@echo 1>&2


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all just_all fast_hostchk hostchk hostchk_warning all_sem_ref all_sem_ref_ptch bug_report build \
	check_man clean clean_generated_obj clean_mkchk_sem clobber configure depend hostchk \
	install test_ioccc legacy_clobber mkchk_sem parser parser-o picky prep \
        pull rebuild_jnum_test release seqcexit shellcheck tags test test-chkentry use_json_ref \
	build release pull reset_min_timestamp load_json_ref build_man bug_report-tx \
	all_dbg all_dyn_array all_jparse all_jparse_test all_man all_soup all_test_ioccc depend


####################################
# things to make in this directory #
####################################

mkiocccentry.o: mkiocccentry.c
	${CC} ${CFLAGS} mkiocccentry.c -c

mkiocccentry: mkiocccentry.o soup/soup.a jparse/jparse.a dyn_array/dyn_array.a dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@

iocccsize.o: iocccsize.c
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE iocccsize.c -c

iocccsize: iocccsize.o soup/soup.a dbg/dbg.a
	${CC} ${CFLAGS} $^ -o $@

txzchk.o: txzchk.c
	${CC} ${CFLAGS} txzchk.c -c

txzchk: txzchk.o soup/soup.a jparse/jparse.a dyn_array/dyn_array.a dbg/dbg.a
	${CC} ${CFLAGS} $^ -o $@

chkentry.o: chkentry.c
	${CC} ${CFLAGS} chkentry.c -c

chkentry: chkentry.o soup/soup.a jparse/jparse.a dyn_array/dyn_array.a dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

all_dbg: dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg all

all_dyn_array: dyn_array/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array all

all_jparse: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse all

all_jparse_test: jparse/test_jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse/test_jparse all

all_soup: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup all

all_test_ioccc: test_ioccc/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_ioccc all

dbg/dbg.h: dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg extern_include

dbg/dbg.a: dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg extern_liba

dbg/dbg_test: dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg dbg_test

dyn_array/dyn_array.h: dyn_array/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array extern_include

dyn_array/dyn_array.a: dyn_array/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array extern_liba

jparse/jparse.h: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_include

jparse/jparse.a: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_liba

jparse/jparse: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_prog

jparse/jsemtblgen: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_prog

jparse/jsemcgen.sh: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_prog

soup/soup.a: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_liba

soup/chk_sem_info.h: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_include

soup/chk_sem_auth.h: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_include

soup/soup.h: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_include

soup/limit_ioccc.sh: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_prog

reset_min_timestamp: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup reset_min_timestamp


####################################
# rules for use by other Makefiles #
####################################


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

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
prep: test_ioccc/prep.sh
	${Q} ${RM} -f ${TMP_BUILD_LOG}
	${Q} ./test_ioccc/prep.sh -l "${TMP_BUILD_LOG}"; \
	    EXIT_CODE="$$?"; \
	    ${MV} -f ${TMP_BUILD_LOG} ${BUILD_LOG}; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo; \
	        echo "make $@: ERROR: prep.sh exit code: $$EXIT_CODE"; \
		echo; \
		echo "make $@: see ${BUILD_LOG} for build details"; \
		echo; \
		exit $$EXIT_CODE; \
	    fi

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
build: release
pull: release
release: test_ioccc/prep.sh
	${Q} ${RM} -f ${TMP_BUILD_LOG}
	${Q} ./test_ioccc/prep.sh -e -o -l "${TMP_BUILD_LOG}"; \
	    EXIT_CODE="$$?"; \
	    ${MV} -f ${TMP_BUILD_LOG} ${BUILD_LOG}; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo; \
	        echo "make $@: ERROR: prep.sh exit code: $$EXIT_CODE"; \
		echo; \
		echo "make $@: see ${BUILD_LOG} for build details"; \
		echo; \
		exit $$EXIT_CODE; \
	    fi

# make parser
#
# Force the rebuild of the JSON parser and then form the reference copies of
# JSON parser C code (if recent enough version of flex and bison are found).
#
parser: jparse/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@

#
# make parser-o: Force the rebuild of the JSON parser.
#
# NOTE: This does NOT use the reference copies of JSON parser C code.
#
parser-o: jparse/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@

# load bison/flex reference code from the previous successful make parser
#
load_json_ref: jparse/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@

# restore bison/flex reference code that was produced by previous successful make parser
#
# This rule forces the use of reference copies of JSON parser C code.
#
use_json_ref: jparse/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@

# Form unpatched semantic tables, without headers and trailers, from the reference info and auth JSON files
#
# rule used by ../test_ioccc/prep.sh
#
all_sem_ref: soup/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@

# form chk.????.ptch.{c,h} files
#
# Given a correct set of chk_sem_????.{c,h} files, we form chk.????.ptch.{c,h}
# diff patch relative to the ref/*.reference.json.{c,h} files.
#
# rule should never be invoked by ../test_ioccc/prep.sh
#
# This rule is run by the repo maintainers only AFTER chk_sem_????.{c,h} files
# are updated by hand.
#
all_sem_ref_ptch: soup/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@

# sequence exit codes
#
seqcexit: ${ALL_CSRC} dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${M} ${MAKE} ${MAKE_CD_Q} SEQCEXIT_URL="${SEQCEXIT_URL}" -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} SEQCEXIT_URL="${SEQCEXIT_URL}" -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} SEQCEXIT_URL="${SEQCEXIT_URL}" -C jparse $@
	${M} ${MAKE} ${MAKE_CD_Q} SEQCEXIT_URL="${SEQCEXIT_URL}" -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} SEQCEXIT_URL="${SEQCEXIT_URL}" -C test_ioccc $@
	${Q} HAVE_SEQCEXIT="`type -P ${SEQCEXIT}`"; if [[ -z "$$HAVE_SEQCEXIT" ]]; then \
	    echo 'The seqcexit tool could not be found.' 1>&2; \
	    echo 'The seqcexit tool is required for this rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for seqcexit:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    ${SEQCEXIT_URL}'; 1>&2; \
	    echo ''; 1>&2; \
	    exit 1; \
	else \
	    echo "${SEQCEXIT} -D werr_sem_val -D werrp_sem_val -- ${ALL_CSRC}"; \
	    ${SEQCEXIT} -D werr_sem_val -D werrp_sem_val -- ${ALL_CSRC}; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

picky: ${ALL_SRC} dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${M} ${MAKE} ${MAKE_CD_Q} PICKY_URL="${PICKY_URL}" -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} PICKY_URL="${PICKY_URL}" -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} PICKY_URL="${PICKY_URL}" -C jparse $@
	${M} ${MAKE} ${MAKE_CD_Q} PICKY_URL="${PICKY_URL}" -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} PICKY_URL="${PICKY_URL}" -C test_ioccc $@
	${Q} if ! type -P ${PICKY} >/dev/null 2>&1; then \
	    echo 'The picky tool could not be found.' 1>&2; \
	    echo 'The picky tool is required for this rule.' 1>&2; \
	    echo 'See the following GitHub repo for picky:'; 1>&2; \
	    echo 1>&2; \
	    echo '    ${PICKY_URL}' 1>&2; \
	    echo 1>&2; \
	    exit 1; \
	else \
	    echo "${PICKY} -w132 -u -s -t8 -v -e -- ${C_SRC} ${H_SRC}"; \
	    ${PICKY} -w132 -u -s -t8 -v -e -- ${C_SRC} ${H_SRC}; \
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
	        echo "make $@: ERROR: CODE[1]: $$EXIT_CODE" 1>&2; \
		exit 1; \
	    fi; \
	    echo "${PICKY} -w -u -s -t8 -v -e -8 -- ${SH_FILES}"; \
	    ${PICKY} -w -u -s -t8 -v -e -8 -- ${SH_FILES}; \
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
	        echo "make $@: ERROR: CODE[2]: $$EXIT_CODE" 1>&2; \
		exit 2; \
	    fi; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# inspect and verify shell scripts
#
shellcheck: ${SH_FILES} .shellcheckrc dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${M} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	${Q} HAVE_SHELLCHECK="`type -P ${SHELLCHECK}`"; if [[ -z "$$HAVE_SHELLCHECK" ]]; then \
	    echo 'The shellcheck command could not be found.' 1>&2; \
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
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo "make $@: ERROR: CODE[1]: $$EXIT_CODE" 1>&2; \
		exit 1; \
	    fi; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# inspect and verify man pages
#
check_man: dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${M} ${MAKE} ${MAKE_CD_Q} CHECKNR_URL="${CHECKNR_URL}" -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} CHECKNR_URL="${CHECKNR_URL}" -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} CHECKNR_URL="${CHECKNR_URL}" -C jparse $@
	${M} ${MAKE} ${MAKE_CD_Q} CHECKNR_URL="${CHECKNR_URL}" -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} CHECKNR_URL="${CHECKNR_URL}" -C test_ioccc $@
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# build a user convenience man directory
#
build_man: dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${V} ${MAKE} ${MAKE_CD_Q} -C dbg install_man \
		MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 INSTALL_Q=@ INSTALL_V=
	${V} ${MAKE} ${MAKE_CD_Q} -C dyn_array install_man \
		MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 INSTALL_Q=@ INSTALL_V=
	${V} ${MAKE} ${MAKE_CD_Q} -C jparse install_man \
		MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 INSTALL_Q=@ INSTALL_V=
	${V} ${MAKE} ${MAKE_CD_Q} -C soup install_man \
		MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 INSTALL_Q=@ INSTALL_V=
	${V} ${MAKE} ${MAKE_CD_Q} -C test_ioccc install_man \
		MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 INSTALL_Q=@ INSTALL_V=
	${V} echo
	${V} echo "${OUR_NAME}: make $@ ending"

# vi/vim tags
#
tags: ${ALL_CSRC} ${ALL_HSRC} dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${M} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	-${Q} ${CTAGS} ${ALL_CSRC} ${ALL_HSRC} 2>&1 | \
	     ${GREP} -E -v 'Duplicate entry|Second entry ignored'
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# perform all of the mkiocccentry repo required tests
#
test:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	-${M} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	-${M} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	-${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	-${M} ${MAKE} ${MAKE_CD_Q} -C soup $@
	-${M} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# run test-chkentry on test_JSON files
#
test-chkentry: all chkentry test_ioccc/test-chkentry.sh
	./test_ioccc/test-chkentry.sh -v 1

# rule used by prep.sh and make clean
#
clean_generated_obj: jparse/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse clean_generated_obj

# rule used by prep.sh
#
clean_mkchk_sem: soup/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@

# rule used by prep.sh
#
mkchk_sem: soup/Makefile
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@

# clean legacy code and files - files that are no longer needed
#
legacy_clean: dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	${Q} ${RM} -f jint.o jint_gen.o
	${Q} ${RM} -f jfloat.o jfloat_gen.o
	${Q} ${RM} -f jauthchk.o jinfochk.o jnum_gen.o jsemtblgen.o jstrdecode.o jstrencode.o verge.o
	${Q} ${RM} -f chkentry.o entry_time.o entry_util.o fnamchk.o foo.o jparse.o json_sem.o
	${Q} ${RM} -f json_util.o location.o rule_count.o sanity.o utf8_posix_map.o util.o
	${Q} ${RM} -f sanity.o utf8_posix_map.o util.o
	${Q} ${RM} -f dbg.o dbg_test.o dyn_array.o dyn_test.o jnum_chk.o jnum_test.o json_chk.o
	${Q} ${RM} -f json_entry.o json.o utf8_test.o
	${V} echo
	${V} echo "${OUR_NAME}: make $@ ending"

# clobber legacy code and files - files that are no longer needed
#
legacy_clobber: legacy_clean dbg/Makefile dyn_array/Makefile jparse/Makefile \
	jparse/test_jparse/Makefile soup/Makefile test_ioccc/Makefile
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse/test_jparse $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup $@
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	${Q} ${RM} -f jint jfloat
	${Q} ${RM} -f jint.set.tmp jint_gen
	${Q} ${RM} -f jfloat.set.tmp jfloat_gen
	${Q} ${RM} -rf jint_gen.dSYM jfloat_gen.dSYM
	${Q} ${RM} -f jnumber
	${Q} ${RM} -rf jnumber.dSYM
	${Q} ${RM} -f jauthchk
	${Q} ${RM} -rf jauthchk.dSYM
	${Q} ${RM} -f jinfochk
	${Q} ${RM} -rf jinfochk.dSYM
	${Q} ${RM} -rf ref
	${Q} ${RM} -f chk.{auth,info}.{head,tail,ptch}.{c,h}
	${Q} ${RM} -f chk_sem_auth.o chk_sem_info.o chk_validate.o
	${Q} ${RM} -rf test_iocccsize test_src test_work
	${Q} ${RM} -f dbg_test.c dbg_test.out
	${Q} ${RM} -rf dyn_test.dSYM
	${Q} ${RM} -f jnum_chk
	${Q} ${RM} -rf jnum_chk.dSYM
	${Q} ${RM} -rf test_iocccsize test_src test_work
	${Q} ${RM} -f .exit_code.*
	${Q} ${RM} -f .all_ref.*
	${Q} ${RM} -f dbg.out
	${Q} ${RM} -rf ioccc_test test
	${Q} ${RM} -f jparse.lex.h jparse.tab.c jparse.tab.h jparse.c
	${Q} ${RM} -f fnamchk jnum_gen jsemtblgen jstrdecode jstrencode verge
	${Q} ${RM} -f hostchk.sh prep.sh reset_tstamp.sh run_usage.sh vermod.sh
	${Q} ${RM} -f utf8_test
	${V} echo
	${V} echo "${OUR_NAME}: make $@ ending"


########################################################################
# external repositories - rules to help incorporate other repositories #
########################################################################

# This repo is designed to be a standalone repo.  Even though we use other
# repositories, we prefer to NOT clone them.  We want this repo to depend
# on a specific version of such code so that a change in the code of those
# external repositories will NOT impact this repo.
#
# For that reason, and others, we maintain a private copy of an external
# repository as clone.repo.  The clone.repo directory is excluded from
# out repo via the .gitignore file.  We copy clone.repo/ into repo/ and
# check in those file directly into this repo.
#
# The following rules are used by the people who maintain this repo.
# If you are not someone who maintains this repo, the rules in this section
# are probably not for you.

.PHONY: dbg.clone dbg.diff dbg.fetch dbg.reclone dbg.reload dbg.rsync dbg.status \
        dyn_array/dyn_array.clone dyn_array.diff dyn_array.fetch dyn_array.reclone dyn_array.reload \
	dyn_array.rsync dyn_array.status jparse.clone jparse.diff jparse.fetch jparse.reclone \
	jparse.reload jparse.rsync jparse.status all.clone all.diff all.fetch all.reclone \
	all.reload all.rsync all.status

# dbg external repo
#
dbg.clone:
	${GIT} clone https://github.com/lcn2/dbg.git dbg.clone

dbg.diff: dbg.clone/ dbg/ .exclude
	${DIFF} -u -r --exclude-from=.exclude dbg.clone dbg

dbg.fetch: dbg.clone/
	cd dbg.clone && ${GIT} fetch
	cd dbg.clone && ${GIT} fetch --prune --tags
	cd dbg.clone && ${GIT} merge --ff-only || ${GIT} rebase --rebase-merges
	${GIT} status dbg.clone

dbg.reclone:
	${RM} -rf dbg.clone
	${M} ${MAKE} dbg.clone

dbg.reload: dbg.clone/
	${RM} -rf dbg
	${M} ${MAKE} dbg.rsync

dbg.rsync: dbg.clone/ .exclude
	${RSYNC} -a -S -0 --delete -C --exclude-from=.exclude -v dbg.clone/ dbg

dbg.status: dbg.clone/
	${GIT} status dbg.clone

# dyn_array external repo
#
dyn_array/dyn_array.clone:
	@echo 'rule disabled, enable once dyn_array repo exists'
	@#${GIT} clone https://github.com/lcn2/dyn_array.git dyn_array/dyn_array.clone

dyn_array.diff: dyn_array/dyn_array.clone/ dyn_array/ .exclude
	@#${DIFF} -u -r --exclude-from=.exclude dyn_array/dyn_array.clone dyn_array

dyn_array.fetch: dyn_array/dyn_array.clone/
	@#cd dyn_array/dyn_array.clone && ${GIT} fetch
	@#cd dyn_array/dyn_array.clone && ${GIT} fetch --prune --tags
	@#cd dyn_array/dyn_array.clone && ${GIT} merge --ff-only || ${GIT} rebase --rebase-merges
	@#${GIT} status dyn_array/dyn_array.clone

dyn_array.reclone:
	@echo 'rule disabled, enable once dyn_array repo exists'
	@#${RM} -rf dyn_array/dyn_array.clone
	@#${M} ${MAKE} dyn_array/dyn_array.clone

dyn_array.reload: dyn_array/dyn_array.clone/
	@#${RM} -rf dyn_array
	@#${M} ${MAKE} dyn_array.rsync

dyn_array.rsync: dyn_array/dyn_array.clone/ .exclude
	@#${RSYNC} -a -S -0 --delete -C --exclude-from=.exclude -v dyn_array/dyn_array.clone/ dyn_array

dyn_array.status: dyn_array/dyn_array.clone/
	@#${GIT} status dyn_array/dyn_array.clone

# jparse external repo
#
jparse.clone:
	@echo 'rule disabled, enable once jparse repo exists'
	@#${GIT} clone https://github.com/xexyl/jparse.git jparse.clone

jparse.diff: jparse.clone/ jparse/ .exclude
	@#${DIFF} -u -r --exclude-from=.exclude jparse.clone jparse

jparse.fetch: jparse.clone/
	@#cd jparse.clone && ${GIT} fetch
	@#cd jparse.clone && ${GIT} fetch --prune --tags
	@#cd jparse.clone && ${GIT} merge --ff-only || ${GIT} rebase --rebase-merges
	@#${GIT} status jparse.clone

jparse.reclone:
	@echo 'rule disabled, enable once jparse repo exists'
	@#${RM} -rf jparse.clone
	@#${M} ${MAKE} jparse.clone

jparse.reload: jparse.clone/
	@#${RM} -rf jparse
	@#${M} ${MAKE} jparse.rsync

jparse.rsync: jparse.clone/ .exclude
	@#${RSYNC} -a -S -0 --delete -C --exclude-from=.exclude -v jparse.clone/ jparse

jparse.status: jparse.clone/
	@#${GIT} status jparse.clone


# rules to operate on all external repositories
#
all.clone: dbg.clone dyn_array/dyn_array.clone jparse.clone

all.diff: dbg.diff dyn_array.diff jparse.diff

all.fetch: dbg.fetch dyn_array.fetch jparse.fetch

all.reclone: dbg.reclone dyn_array.reclone jparse.reclone

all.reload: dbg.reload dyn_array.reload jparse.reload

all.rsync: dbg.rsync dyn_array.rsync jparse.rsync

all.status: dbg.status dyn_array.status jparse.status


###################################
# standard Makefile utility rules #
###################################

configure:
	${V} echo nothing to configure

clean: clean_generated_obj legacy_clean dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${M} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	${V} echo
	${RM} -f ${OTHER_OBJS} ${LESS_PICKY_OBJS}
	${RM} -rf ${DSYMDIRS}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

clobber: legacy_clobber clean dbg/Makefile dyn_array/Makefile jparse/Makefile \
	jparse/test_jparse/Makefile soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${M} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse/test_jparse $@
	${V} echo
	${RM} -rf .hostchk.work.*
	${RM} -f .txzchk_test.*
	${RM} -f .sorry.*
	${RM} -f ${TARGETS}
	${RM} -rf man
	${RM} -f jparse_test.log chkentry_test.log txzchk_test.log ${BUILD_LOG}
	${RM} -f tags
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

install: all dbg/Makefile dyn_array/Makefile jparse/Makefile \
        soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${M} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	${V} echo
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"


###############
# make depend #
###############

depend: ${ALL_CSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${M} ${MAKE} ${MAKE_CD_Q} -C dbg $@
	${M} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@
	${M} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@
	${M} ${MAKE} ${MAKE_CD_Q} -C soup $@
	${M} ${MAKE} ${MAKE_CD_Q} -C jparse $@
	${Q} HAVE_INDEPEND="`type -P ${INDEPEND}`"; if [[ -z "$$HAVE_INDEPEND" ]]; then \
	    echo '${OUR_NAME}: The independ command could not be found.' 1>&2; \
	    echo '${OUR_NAME}: The independ command is required to perform: make $@'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for where to obtain independ:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/independ'; 1>&2; \
	else \
	    if ! ${GREP} -q '^### DO NOT CHANGE MANUALLY BEYOND THIS LINE$$' Makefile; then \
	        echo "${OUR_NAME}: make $@ aborting, Makefile missing: ### DO NOT CHANGE MANUALLY BEYOND THIS LINE" 1>&2; \
		exit 1; \
	    fi; \
	    ${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE$$/p' Makefile; \
	    ${CC} ${CFLAGS} -MM -I. -DMKIOCCCENTRY_USE ${ALL_CSRC} | ${INDEPEND} >> Makefile; \
	    if ${CMP} -s Makefile.orig Makefile; then \
		${RM} -f Makefile.orig; \
	    else \
		echo "${OUR_NAME}: Makefile dependencies updated"; \
		echo; \
		echo "${OUR_NAME}: Previous version may be found in: Makefile.orig"; \
	    fi; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
chkentry.o: chkentry.c chkentry.h dbg/dbg.h jparse/../dbg/dbg.h \
    jparse/../dyn_array/../dbg/dbg.h jparse/../dyn_array/dyn_array.h \
    jparse/jparse.h jparse/jparse.tab.h jparse/json_parse.h \
    jparse/json_sem.h jparse/json_util.h jparse/util.h soup/../dbg/dbg.h \
    soup/../jparse/jparse.h soup/../jparse/json_sem.h \
    soup/../jparse/json_util.h soup/chk_sem_auth.h soup/chk_sem_info.h \
    soup/chk_validate.h soup/entry_time.h soup/entry_util.h soup/foo.h \
    soup/limit_ioccc.h soup/location.h soup/sanity.h soup/soup.h \
    soup/utf8_posix_map.h soup/version.h
iocccsize.o: iocccsize.c iocccsize.h soup/../dbg/dbg.h soup/iocccsize_err.h \
    soup/limit_ioccc.h soup/version.h
mkiocccentry.o: dbg/dbg.h iocccsize.h jparse/../dbg/dbg.h \
    jparse/../dyn_array/../dbg/dbg.h jparse/../dyn_array/dyn_array.h \
    jparse/jparse.h jparse/jparse.tab.h jparse/json_parse.h \
    jparse/json_sem.h jparse/json_util.h jparse/util.h mkiocccentry.c \
    mkiocccentry.h soup/../dbg/dbg.h soup/../jparse/jparse.h \
    soup/entry_util.h soup/limit_ioccc.h soup/location.h soup/sanity.h \
    soup/utf8_posix_map.h soup/version.h
txzchk.o: dbg/dbg.h jparse/../dbg/dbg.h jparse/../dyn_array/../dbg/dbg.h \
    jparse/../dyn_array/dyn_array.h jparse/jparse.h jparse/jparse.tab.h \
    jparse/json_parse.h jparse/json_sem.h jparse/json_util.h jparse/util.h \
    soup/../dbg/dbg.h soup/../jparse/jparse.h soup/entry_util.h \
    soup/limit_ioccc.h soup/location.h soup/sanity.h soup/utf8_posix_map.h \
    soup/version.h txzchk.c txzchk.h
