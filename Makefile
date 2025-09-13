#!/usr/bin/env make
#
# mkiocccentry and related tools - how to form an IOCCC submission
#
# For mkiocccentry:
#
# Copyright (c) 2021-2025 by Landon Curt Noll and Cody Boone Ferguson.
# All Rights Reserved.
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
# THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
# ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
# DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# This tool was co-developed in 2021-2025 by Cody Boone Ferguson and Landon
# Curt Noll:
#
#  @xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# Share and enjoy! :-)
#     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
####

####
# See also, the following files for additional Copyright information:
#
# iocccsize.c iocccsize.h soup/rule_count.c jparse/* dbg/* dyn_alloc/*
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
IS_AVAILABLE= soup/is_available.sh
MV= mv
PICKY= picky
RM= rm
RMDIR= rmdir
RSYNC= rsync
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
SLEEP= sleep
SORT= sort
TEE= tee
TR= tr


##########
# locale #
##########

LC_ALL= C
LANG= C
LC_NUMERIC= C



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

# echo-only action commands

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

# action commands that are NOT echo

# Q= @					do not echo internal Makefile actions (quiet mode)
# Q=					echo internal Makefile actions (debug / verbose mode)
#
#Q=
Q= @

# E= @					do not echo calling make in another directory (quiet mode)
# E=					echo calling make in another directory (debug / verbose mode)
#
E=
#E= @

# I= @					do not echo install commands (quiet mode)
# I=					echo install commands (debug / verbose mode
#
I=
#I= @

# other Makefile control related actions

# Q= implies -v 0
# else -v 1
#
ifeq ($(strip ${Q}),@)
Q_V_OPTION="0"
else
Q_V_OPTION="1"
endif

# installing variables
#

# INSTALL_V=				install w/o -v flag (quiet mode)
# INSTALL_V= -v				install with -v (debug / verbose mode
#
#INSTALL_V=
INSTALL_V=

# where to install
#
# Default PREFIX is /usr/local so binaries would be installed in /usr/local/bin,
# libraries in /usr/local/lib etc. If one wishes to override this, say
# installing to /usr, they can do so like:
#
#	make PREFIX=/usr install
#
PREFIX= /usr/local

# uninstalling variables
#

# RM_V=					rm w/o -v flag (quiet mode)
# RM_V= -v				rm with -v (debug / verbose mode)
#
#RM_V= -v
RM_V=

# Additional controls
#

# MAKE_CD_Q= --no-print-directory	silence make cd messages (quiet mode)
# MAKE_CD_Q=				silence make cd messages (quiet mode)
#
MAKE_CD_Q= --no-print-directory
#MAKE_CD_Q=

# Disable parallel Makefile execution
#
# We do not support parallel make.  We have found most
# parallel make systems do not get the rule dependency order
# correct, resulting in a failed attempt to compile.
#
.NOTPARALLEL:


##################
# How to compile #
##################

# C source standards being used
#
# This repo supports c17 and later.
#
# NOTE: at one point we used -std=gnu11 because there were a few older systems
#       in late 2021 that did not have compilers that (yet) supported gnu17.
#       While there may be even more out of date systems that do not support
#       gnu11, we have to draw the line somewhere. Besides, one of those systems
#       reaches its EOL on 30 June 2024 and that's three days away at this
#       point.
#
#	--------------------------------------------------
#
#	^^ the line is above :-)
#
#C_STD= -std=gnu11
C_STD= -std=gnu17

# optimization and debug level
#
C_OPT= -O3
#C_OPT= -O0 -g3

# Compiler warnings
#
WARN_FLAGS= -Wall -Wextra -Wno-char-subscripts
#WARN_FLAGS= -Wall -Wextra -Werror

# special compiler flags
#
C_SPECIAL= -DINTERNAL_INCLUDE

# linker options
#
LDFLAGS=

# where to find libdbg.a and libdyn_array.a
#
# LD_DIR - locations of libdbg.a and libdyn_array.a for the next sub-directory down
# LD_DIR2 - locations of libdbg.a and libdyn_array.a for 2 sub-directories down
#
LD_DIR= -L../dbg -L../dyn_array
LD_DIR2= -L../../dbg -L../../dyn_array

# how to compile
#
# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
CFLAGS= ${C_STD} ${C_OPT} -pedantic ${WARN_FLAGS} ${C_SPECIAL} ${LDFLAGS}


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= mkiocccentry.c iocccsize.c txzchk.c chkentry.c chksubmit.c
H_SRC= mkiocccentry.h iocccsize.h txzchk.h chkentry.h chksubmit.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=

# all shell scripts
#
SH_FILES= bug_report.sh

# all man pages that are NOT built and NOT removed by make clobber
#
MAN1_PAGES=
MAN3_PAGES=
MAN8_PAGES=
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# tags for just the files in this directory
#
LOCAL_DIR_TAGS= .local.dir.tags

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC=
BUILT_H_SRC=
ALL_BUILT_SRC= ${BUILT_C_SRC} ${BUILT_H_SRC}

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS=

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and ARE removed by make clean
#
OTHER_OBJS= mkiocccentry.o iocccsize.o txzchk.o chkentry.o chksubmit.o

# all intermediate files which are also removed by make clean
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
MAN1_DIR= ${PREFIX}/share/man/man1
MAN3_DIR= ${PREFIX}/share/man/man3
MAN8_DIR= ${PREFIX}/share/man/man8
DEST_INCLUDE= ${PREFIX}/include
DEST_LIB= ${PREFIX}/lib
DEST_DIR= ${PREFIX}/bin


#################################
# external Makefile information #
#################################

# may be used outside of this directory
#
EXTERN_H=
EXTERN_O=
EXTERN_MAN=
EXTERN_LIBA=
EXTERN_PROG= bug_report.sh chkentry chksubmit iocccsize mkiocccentry txzchk

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
PROG_TARGETS= mkiocccentry iocccsize txzchk chkentry chksubmit

# directories sometimes built under macOS and removed by clobber
#
DSYMDIRS= mkiocccentry.dSYM iocccsize.dSYM txzchk.dSYM chkentry.dSYM chksubmit.dSYM

# logs for testing
#
TMP_BUILD_LOG= ".build.log.$$$$"
BUILD_LOG= build.log

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

# The directive below retrieves any user specific configurations from Makefile.local.
#
# The - before include means it's not an error if the file does not exist.
#
# We put this directive just before the first all rule so that you may override
# or modify any of the above Makefile variables.  To override a value, use := symbols.
# For example:
#
#	CC:= gcc
#
-include Makefile.local


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
fast_hostchk: test_ioccc/hostchk.sh
	-${Q} ./test_ioccc/hostchk.sh -f -v 0 -Z .; \
	EXIT_CODE="$$?"; \
	if [ "$$EXIT_CODE" -ne 0 ]; then \
	    ${MAKE} hostchk_warning C_SPECIAL="${C_SPECIAL}"; \
	fi

bug_report: bug_report.sh
	-${Q} ./bug_report.sh -v 1

bug_report-tx: bug_report.sh
	${S} echo "${OUR_NAME}: make $@: starting test of bug_report.sh -t -x"
	${S} echo
	${Q} ./bug_report.sh -t -x
	${S} echo
	${S} echo "${OUR_NAME}: ending test of bug_report.sh -t -x"

bug_report-txl: bug_report.sh
	${S} echo "${OUR_NAME}: make $@: starting test of bug_report.sh -t -x -l"
	${Q} ./bug_report.sh -t -x -l
	${S} echo "${OUR_NAME}: ending test of bug_report.sh -t -x -l"

# slower more verbose build environment sanity check
#
hostchk: test_ioccc/hostchk.sh
	-${Q} ./test_ioccc/hostchk.sh -v 1 -Z .; \
	EXIT_CODE="$$?"; \
	if [ "$$EXIT_CODE" -ne 0 ]; then \
	    ${MAKE} hostchk_warning C_SPECIAL="${C_SPECIAL}"; \
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
	@echo '    ./test_ioccc/hostchk.sh -v 1' 1>&2
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
	install uninstall test_ioccc legacy_clobber mkchk_sem parser parser-o picky prep soup \
        pull release seqcexit shellcheck tags local_dir_tags all_tags test test-chkentry use_json_ref \
	eat eating eat eating_soup kitchen soup_kitchen bug_report-txl \
	build release pull reset_min_timestamp load_json_ref build_man bug_report-tx \
	all_dbg all_dyn_array all_jparse all_jparse_test all_man all_soup all_test_ioccc depend



####################################
# things to make in this directory #
####################################

mkiocccentry.o: mkiocccentry.c
	${CC} ${CFLAGS} mkiocccentry.c -c

mkiocccentry: mkiocccentry.o soup/soup.a jparse/libjparse.a dyn_array/libdyn_array.a dbg/libdbg.a
	${CC} ${CFLAGS} $^ -lm -o $@

iocccsize.o: iocccsize.c
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE iocccsize.c -c

iocccsize: iocccsize.o soup/soup.a dbg/libdbg.a
	${CC} ${CFLAGS} $^ -o $@

txzchk.o: txzchk.c
	${CC} ${CFLAGS} txzchk.c -c

txzchk: txzchk.o soup/soup.a jparse/libjparse.a dyn_array/libdyn_array.a dbg/libdbg.a
	${CC} ${CFLAGS} $^ -o $@

chkentry.o: chkentry.c
	${CC} ${CFLAGS} chkentry.c -c

chkentry: chkentry.o soup/soup.a jparse/libjparse.a dyn_array/libdyn_array.a dbg/libdbg.a
	${CC} ${CFLAGS} $^ -lm -o $@

chksubmit.o: chksubmit.c
	${CC} ${CFLAGS} chksubmit.c -c

chksubmit: chksubmit.o soup/soup.a jparse/libjparse.a dyn_array/libdyn_array.a dbg/libdbg.a
	${CC} ${CFLAGS} $^ -lm -o $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

all_dbg: dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg all C_SPECIAL="${C_SPECIAL}"

all_dyn_array: dyn_array/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array all C_SPECIAL="${C_SPECIAL}"

all_jparse: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse all C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

all_jparse_test: jparse/test_jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse/test_jparse all C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR2="${LD_DIR2}"

soup: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup soup C_SPECIAL="${C_SPECIAL}"

all_soup: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup all C_SPECIAL="${C_SPECIAL}"

eat eating eat_soup eating_soup: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup eat C_SPECIAL="${C_SPECIAL}"

kitchen soup_kitchen: soup/kitchen.sh
	-${Q} ${MAKE} ${MAKE_CD_Q} -C soup kitchen C_SPECIAL="${C_SPECIAL}"

all_test_ioccc: test_ioccc/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_ioccc all C_SPECIAL="${C_SPECIAL}"

dbg/dbg.h: dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg extern_include C_SPECIAL="${C_SPECIAL}"

dbg/libdbg.a: dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg extern_liba C_SPECIAL="${C_SPECIAL}"

dbg/dbg_test: dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg dbg_test C_SPECIAL="${C_SPECIAL}"

dyn_array/dyn_array.h: dyn_array/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array extern_include C_SPECIAL="${C_SPECIAL}"

dyn_array/libdyn_array.a: dyn_array/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array extern_liba C_SPECIAL="${C_SPECIAL}"

jparse/jparse.h: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_include C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

jparse/libjparse.a: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_liba C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

jparse/jparse: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_prog C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

jparse/jsemtblgen: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_prog C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

jparse/jsemcgen.sh: jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse extern_prog C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

soup/soup.a: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_liba C_SPECIAL="${C_SPECIAL}"

soup/chk_sem_info.h: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_include C_SPECIAL="${C_SPECIAL}"

soup/chk_sem_auth.h: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_include C_SPECIAL="${C_SPECIAL}"

soup/soup.h: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_include C_SPECIAL="${C_SPECIAL}"

soup/limit_ioccc.sh: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup extern_prog C_SPECIAL="${C_SPECIAL}"

reset_min_timestamp: soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup reset_min_timestamp C_SPECIAL="${C_SPECIAL}"


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
	${S} echo "${OUR_NAME}: make $@ starting"
	${Q} ${RM} -f ${TMP_BUILD_LOG}
	${Q} ./test_ioccc/prep.sh -m${MAKE} -l "${TMP_BUILD_LOG}"; \
	    EXIT_CODE="$$?"; \
	    ${MV} -f ${TMP_BUILD_LOG} ${BUILD_LOG}; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo; \
	        echo "make $@: ERROR: ./test_ioccc/prep.sh exit code: $$EXIT_CODE"; \
	    fi; \
	    echo; \
	    echo "make $@: see ${BUILD_LOG} for details"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		exit "$$EXIT_CODE"; \
	    else \
	        echo "All Done!!! All Done!!! -- Jessica Noll, Age 2"; \
	    fi
	    ${S} echo "${OUR_NAME}: make $@ ending"; \

# a slower version of prep that does not write to a log file so one can see the
# full details.
#
slow_prep: test_ioccc/prep.sh
	${S} echo "${OUR_NAME}: make $@ starting"
	${Q} ${RM} -f ${TMP_BUILD_LOG}
	${Q} ./test_ioccc/prep.sh -m${MAKE}; \
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo; \
	        echo "make $@: ERROR: ./test_ioccc/prep.sh exit code: $$EXIT_CODE"; \
	    fi; \
	    echo; \
	    echo "make $@: see ${BUILD_LOG} for details"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		exit "$$EXIT_CODE"; \
	    else \
	         echo "All Done!!! All Done!!! -- Jessica Noll, Age 2"; \
	    fi
	    ${S} echo "${OUR_NAME}: make $@ ending"; \


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
	${S} echo "${OUR_NAME}: make $@ starting"
	${Q} ${RM} -f ${TMP_BUILD_LOG}
	${Q} ./test_ioccc/prep.sh -m${MAKE} -e -o -l "${TMP_BUILD_LOG}"; \
	    EXIT_CODE="$$?"; \
	    ${MV} -f ${TMP_BUILD_LOG} ${BUILD_LOG}; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo; \
	        echo "make $@: ERROR: ./test_ioccc/prep.sh exit code: $$EXIT_CODE"; \
	    fi; \
	    echo; \
	    echo "make $@: see ${BUILD_LOG} for details"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		exit "$$EXIT_CODE"; \
	    else \
	         echo "All Done!!! All Done!!! -- Jessica Noll, Age 2"; \
	    fi
	    ${S} echo "${OUR_NAME}: make $@ ending"; \

# a slower version of release that does not write to a log file so one can see the
# full details.
#
slow_release: test_ioccc/prep.sh
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo "Starting directory is: $$(/bin/pwd)"
	${Q} ${RM} -f ${TMP_BUILD_LOG}
	${Q} ./test_ioccc/prep.sh -m${MAKE} -e -o; \
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo; \
	        echo "make $@: ERROR: ./test_ioccc/prep.sh exit code: $$EXIT_CODE"; \
	    fi; \
	    echo; \
	    echo "make $@: see ${BUILD_LOG} for details"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		exit "$$EXIT_CODE"; \
	    else \
	         echo "All Done!!! All Done!!! -- Jessica Noll, Age 2"; \
	    fi
	    ${S} echo "${OUR_NAME}: make $@ ending"; \


# force test_ioccc/txzchk_test.sh results into test suite *.err files
#
# If test_ioccc/txzchk_test.sh reports some *.err files under ./test_ioccc/test_txzchk
# does not match the "expected" test result, force the *.err files to become what
# is generated.
#
# IMPORTANT: Only run this rule of you KNOW that the *.err files are simply
#	     out of date and that the difference between what is in the *.err
#	     files and what is being generated is correct.
#
rebuild_txzchk_test_errors force_expectations_for_txzchk_test:
	@./test_ioccc/txzchk_test.sh -B || exit 1

# make parser
#
# Force the rebuild of the JSON parser and then form the reference copies of
# JSON parser C code (if recent enough version of flex and bison are found).
#
parser: jparse/Makefile
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

#
# make parser-o: Force the rebuild of the JSON parser.
#
# NOTE: This does NOT use the reference copies of JSON parser C code.
#
parser-o: jparse/Makefile
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

# load bison/flex reference code from the previous successful make parser
#
load_json_ref: jparse/Makefile
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

# restore bison/flex reference code that was produced by previous successful make parser
#
# This rule forces the use of reference copies of JSON parser C code.
#
use_json_ref: jparse/Makefile
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

# Form unpatched semantic tables, without headers and trailers, from the reference info and auth JSON files
#
# rule used by ../test_ioccc/prep.sh
#
all_sem_ref: soup/Makefile
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"

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
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"


# sequence exit codes
#
seqcexit: ${ALL_CSRC} dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${Q} if ! ${IS_AVAILABLE} ${SEQCEXIT} >/dev/null 2>&1; then \
	    echo 'The ${SEQCEXIT} tool could not be found or is unreliable in your system.' 1>&2; \
	    echo 'The ${SEQCEXIT} tool is required for the $@ rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for ${SEQCEXIT}:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/seqcexit'; 1>&2; \
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
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${Q} if ! ${IS_AVAILABLE} ${PICKY} >/dev/null 2>&1; then \
	    echo 'The ${PICKY} tool could not be found or is unreliable in your system.' 1>&2; \
	    echo 'The ${PICKY} tool is required for the $@ rule.' 1>&2; \
	    echo 1>&2; \
	    echo 'See the following GitHub repo for ${PICKY}:'; 1>&2; \
	    echo 1>&2; \
	    echo '    https://github.com/lcn2/picky' 1>&2; \
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
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${Q} if ! ${IS_AVAILABLE} ${SHELLCHECK} >/dev/null 2>&1; then \
	    echo 'The ${SHELLCHECK} command could not be found or is unreliable in your system.' 1>&2; \
	    echo 'The ${SHELLCHECK} command is required to run the $@ rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for ${SHELLCHECK}:'; 1>&2; \
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
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# build a user convenience man directory
#
build_man: dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg install_man \
		     MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 I=@ \
		     INSTALL_V= C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array install_man \
		MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 I=@ \
		INSTALL_V= C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse install_man \
		     MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 I=@ \
		     INSTALL_V= C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup install_man \
		     MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 I=@ \
		     INSTALL_V= C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_ioccc install_man \
		     MAN1_DIR=../man/man1 MAN3_DIR=../man/man3 MAN8_DIR=../man/man8 I=@ \
		     INSTALL_V= C_SPECIAL="${C_SPECIAL}"
	${V} echo
	${V} echo "${OUR_NAME}: make $@ ending"

# vi/vim tags
#
tags: ${ALL_CSRC} ${ALL_HSRC} dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if ! ${IS_AVAILABLE} ${CTAGS} >/dev/null 2>&1; then \
	    echo 'The ${CTAGS} command could not be found.' 1>&2; \
	    echo 'The ${CTAGS} command is required to run the $@ rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'Use the package manager from your OS to install the ${CTAGS} package.' 1>&2; \
	    echo 'The following GitHub repo may be a useful ${CTAGS} alternative:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/universal-ctags/ctags'; 1>&2; \
	    echo ''; 1>&2; \
	    exit 1; \
	fi
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg local_dir_tags C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array local_dir_tags C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse local_dir_tags C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup local_dir_tags C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc local_dir_tags C_SPECIAL="${C_SPECIAL}"
	${Q} echo
	${E} ${MAKE} local_dir_tags
	${Q} echo
	${E} ${MAKE} all_tags
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# use the ${CTAGS} tool to form ${LOCAL_DIR_TAGS} of the source in this directory
#
local_dir_tags: ${ALL_CSRC} ${ALL_HSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if ! ${IS_AVAILABLE} ${CTAGS} >/dev/null 2>&1; then \
	    echo 'The ${CTAGS} command could not be found.' 1>&2; \
	    echo 'The ${CTAGS} command is required to run the $@ rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'Use the package manager from your OS to install the ${CTAGS} package.' 1>&2; \
	    echo 'The following GitHub repo may be a useful ${CTAGS} alternative:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/universal-ctags/ctags'; 1>&2; \
	    echo ''; 1>&2; \
	    exit 1; \
	fi
	${Q} ${RM} -f ${LOCAL_DIR_TAGS}
	-${E} ${CTAGS} -w -f ${LOCAL_DIR_TAGS} ${ALL_CSRC} ${ALL_HSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# for a tags file from all ${LOCAL_DIR_TAGS} in all of the other directories
#
all_tags:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${Q} echo
	${Q} ${RM} -f tags
	${Q} for dir in . dbg dyn_array jparse jparse/test_jparse soup test_ioccc; do \
	    if [[ -s $$dir/${LOCAL_DIR_TAGS} ]]; then \
		echo "${SED} -e 's;\t;\t'$${dir}'/;' $${dir}/${LOCAL_DIR_TAGS} >> tags"; \
		${SED} -e 's;\t;\t'$${dir}'/;' "$${dir}/${LOCAL_DIR_TAGS}" >> tags; \
	    elif [[ -e $$dir/${LOCAL_DIR_TAGS} ]]; then \
		echo "make $@ Warning: found empty local tags file: $$dir/${LOCAL_DIR_TAGS}" 1>&2 ; \
	    else \
		echo "make $@ Warning: missing local tags file: $$dir/${LOCAL_DIR_TAGS}" 1>&2 ; \
	    fi; \
	done
	${E} ${SORT} tags -o tags
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# perform all of the mkiocccentry repo required tests
#
test:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"
	${S} echo "All done!!! All done!! -- Jessica Noll, Age 2."

# run test-chkentry on test_JSON files
#
test-chkentry: all chkentry test_ioccc/test-chkentry.sh
	${E} ./test_ioccc/test-chkentry.sh -v 1

# rule used by prep.sh and make clean
#
clean_generated_obj: jparse/Makefile
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse clean_generated_obj C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"

# rule used by prep.sh
#
clean_mkchk_sem: soup/Makefile
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"

# rule used by prep.sh
#
mkchk_sem: soup/Makefile
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"

# clean legacy code and files - files that are no longer needed
#
legacy_clean: dbg/Makefile dyn_array/Makefile jparse/Makefile \
	soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${V} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# clobber legacy code and files - files that are no longer needed
#
legacy_clobber: legacy_clean dbg/Makefile dyn_array/Makefile jparse/Makefile \
	jparse/test_jparse/Makefile soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C jparse/test_jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR2="${LD_DIR2}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${V} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"


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
#
# p.s. Yes, we know about "git submodules".  However we believe that "git submodules"
#      are "not ready for prime time" / not ready for production as of 2023 Aug 05
#      (the date when this comment was updated).

.PHONY: dbg.help dbg.setup dbg.clone dbg.clone_status dbg.update_clone dbg.reload_clone \
	dbg.update_from_clone dbg.update_into_clone \
	dbg.diff_dbg_clone dbg.diff_clone_dbg dbg.diff_summary \
	dyn_array.help dyn_array.setup dyn_array.clone dyn_array.clone_status dyn_array.update_clone dyn_array.reload_clone \
	dyn_array.update_from_clone dyn_array.update_into_clone \
	dyn_array.diff_dyn_array_clone dyn_array.diff_clone_dyn_array dyn_array.diff_summary \
	jparse.help jparse.setup jparse.clone jparse.clone_status jparse.update_clone jparse.reload_clone \
	jparse.update_from_clone jparse.update_into_clone \
	jparse.diff_jparse_clone jparse.diff_clone_jparse jparse.diff_summary \
	all.help all.setup all.clone all.clone_status all.update_clone all.reload_clone \
	all.update_from_clone all.update_into_clone \
	all.diff_all_clone all.diff_clone_all all.diff_summary

# rules to help incorporate the external dbg repo
#
dbg.help:
	${S} echo "The following rules are used by the people who maintain this repo."
	${S} echo
	${S} echo "Summary of rules to help incorporate the external dbg repo:"
	${S} echo
	${S} echo "make dbg.help - print this message"
	${S} echo "make dbg.setup - create or update dbg.clone directory from remote repo"
	${S} echo "make dbg.clone - create missing dbg.clone directory from remote repo"
	${S} echo "make dbg.clone_status - git status of dbg.clone directory"
	${S} echo "make dbg.update_clone - update dbg.clone directory from remote repo"
	${S} echo "make dbg.recreate_clone - remove then clone dbg.clone directory from remote repo"
	${S} echo "make dbg.update_from_clone - modify dbg directory from dbg.clone directory"
	${S} echo "make dbg.update_into_clone - modify dbg.clone directory from dbg directory"
	${S} echo "make dbg.diff_dbg_clone - compare dbg directory with dbg.clone directory"
	${S} echo "make dbg.diff_clone_dbg - compare dbg.clone directory with dbg directory"
	${S} echo "make dbg.diff_summary - summarize differnces between dbg and dbg.clone directories"

dbg.setup:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	-@if [[ -d dbg.clone ]]; then \
	    ${MAKE} dbg.update_clone; \
	else \
	    ${MAKE} dbg.clone; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dbg.clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if [[ -d dbg.clone ]]; then \
	    echo "ERROR: dbg.clone exists"; \
	    exit 1; \
	else \
	    echo "If git clone fails because you do not have the ssh key, try:"; \
	    echo; \
	    echo "	${GIT} clone https://github.com/lcn2/dbg.git dbg.clone"; \
	    echo; \
	    ${GIT} clone git@github.com:lcn2/dbg.git dbg.clone; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dbg.clone_status: dbg.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${GIT} status dbg.clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dbg.update_clone: dbg.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} cd dbg.clone && ${GIT} fetch
	${E} cd dbg.clone && ${GIT} fetch --prune --tags
	${E} cd dbg.clone && ${GIT} merge --ff-only || ${GIT} rebase --rebase-merges
	${E} ${MAKE} dbg.clone_status
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dbg.recreate_clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} -rf dbg.clone
	${E} ${MAKE} dbg.clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dbg.update_from_clone: dbg.clone/ dbg/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RSYNC} -a -S -0 --exclude=.git --exclude=.github -C --delete -v dbg.clone/ dbg
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg depend C_SPECIAL=-DINTERNAL_INCLUDE
	${E} ${RM} -f dbg/Makefile.orig
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dbg.update_into_clone: dbg/ dbg.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RSYNC} -a -S -0 --exclude=.git --exclude=.github -C --delete -v dbg/ dbg.clone
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg.clone depend C_SPECIAL=-UINTERNAL_INCLUDE
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dbg.diff_dbg_clone: dbg.clone/ dbg/ .exclude
	-${E} ${DIFF} -u -r --exclude-from=.exclude dbg.clone dbg

dbg.diff_clone_dbg: dbg/ dbg.clone/ .exclude
	-${E} ${DIFF} -u -r --exclude-from=.exclude dbg dbg.clone

dbg.diff_summary: dbg.clone/ dbg/ .exclude
	-${E} ${DIFF} -r --brief --exclude-from=.exclude dbg.clone dbg

# rules to help incorporate the external dyn_array repo
#
dyn_array.help:
	${S} echo "The following rules are used by the people who maintain this repo."
	${S} echo
	${S} echo "Summary of rules to help incorporate the external dyn_array repo:"
	${S} echo
	${S} echo "make dyn_array.help - print this message"
	${S} echo "make dyn_array.setup - create or update dyn_array.clone directory from remote repo"
	${S} echo "make dyn_array.clone - create missing dyn_array.clone directory from remote repo"
	${S} echo "make dyn_array.clone_status - git status of dyn_array.clone directory"
	${S} echo "make dyn_array.update_clone - update dyn_array.clone directory from remote repo"
	${S} echo "make dyn_array.recreate_clone - remove then clone dyn_array.clone directory from remote repo"
	${S} echo "make dyn_array.update_from_clone - modify dyn_array directory from dyn_array.clone directory"
	${S} echo "make dyn_array.update_into_clone - modify dyn_array.clone directory from dyn_array directory"
	${S} echo "make dyn_array.diff_dyn_array_clone - compare dyn_array directory with dyn_array.clone directory"
	${S} echo "make dyn_array.diff_clone_dyn_array - compare dyn_array.clone directory with dyn_array directory"
	${S} echo "make dyn_array.diff_summary - summarize differnces between dyn_array and dyn_array.clone directories"

dyn_array.setup:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	-@if [[ -d dyn_array.clone ]]; then \
	    ${MAKE} dyn_array.update_clone; \
	else \
	    ${MAKE} dyn_array.clone; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dyn_array.clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if [[ -d dyn_array.clone ]]; then \
	    echo "ERROR: dyn_array.clone exists"; \
	    exit 1; \
	else \
	    echo "If git clone fails because you do not have the ssh key, try:"; \
	    echo; \
	    echo "	${GIT} clone https://github.com/lcn2/dyn_array.git dyn_array.clone"; \
	    echo; \
	    ${GIT} clone git@github.com:lcn2/dyn_array.git dyn_array.clone; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dyn_array.clone_status: dyn_array.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${GIT} status dyn_array.clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dyn_array.update_clone: dyn_array.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} cd dyn_array.clone && ${GIT} fetch
	${E} cd dyn_array.clone && ${GIT} fetch --prune --tags
	${E} cd dyn_array.clone && ${GIT} merge --ff-only || ${GIT} rebase --rebase-merges
	${E} ${MAKE} dyn_array.clone_status
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dyn_array.recreate_clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} -rf dyn_array.clone
	${E} ${MAKE} dyn_array.clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dyn_array.update_from_clone: dyn_array.clone/ dyn_array/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RSYNC} -a -S -0 --exclude=.git --exclude=.github -C --delete -v dyn_array.clone/ dyn_array
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array depend C_SPECIAL=-DINTERNAL_INCLUDE
	${E} ${RM} -f dyn_array/Makefile.orig
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dyn_array.update_into_clone: dyn_array/ dyn_array.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RSYNC} -a -S -0 --exclude=.git --exclude=.github -C --delete -v dyn_array/ dyn_array.clone
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array.clone depend C_SPECIAL=-UINTERNAL_INCLUDE
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

dyn_array.diff_dyn_array_clone: dyn_array.clone/ dyn_array/ .exclude
	-${E} ${DIFF} -u -r --exclude-from=.exclude dyn_array.clone dyn_array

dyn_array.diff_clone_dyn_array: dyn_array/ dyn_array.clone/ .exclude
	-${E} ${DIFF} -u -r --exclude-from=.exclude dyn_array dyn_array.clone

dyn_array.diff_summary: dyn_array.clone/ dyn_array/ .exclude
	-${E} ${DIFF} -r --brief --exclude-from=.exclude dyn_array.clone dyn_array

# rules to help incorporate the external jparse repo
#
jparse.help:
	${S} echo "The following rules are used by the people who maintain this repo."
	${S} echo
	${S} echo "Summary of rules to help incorporate the external jparse repo:"
	${S} echo
	${S} echo "make jparse.help - print this message"
	${S} echo "make jparse.setup - create or update jparse.clone directory from remote repo"
	${S} echo "make jparse.clone - create missing jparse.clone directory from remote repo"
	${S} echo "make jparse.clone_status - git status of jparse.clone directory"
	${S} echo "make jparse.update_clone - update jparse.clone directory from remote repo"
	${S} echo "make jparse.recreate_clone - remove then clone jparse.clone directory from remote repo"
	${S} echo "make jparse.update_from_clone - modify jparse directory from jparse.clone directory"
	${S} echo "make jparse.update_into_clone - modify jparse.clone directory from jparse directory"
	${S} echo "make jparse.diff_jparse_clone - compare jparse directory with jparse.clone directory"
	${S} echo "make jparse.diff_clone_jparse - compare jparse.clone directory with jparse directory"
	${S} echo "make jparse.diff_summary - summarize differnces between jparse and jparse.clone directories"

jparse.setup:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	-@if [[ -d jparse.clone ]]; then \
	    ${MAKE} jparse.update_clone; \
	else \
	    ${MAKE} jparse.clone; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

jparse.clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if [[ -d jparse.clone ]]; then \
	    echo "ERROR: jparse.clone exists"; \
	    exit 1; \
	else \
	    echo "If git clone fails because you do not have the ssh key, try:"; \
	    echo; \
	    echo "	${GIT} clone https://github.com/xexyl/jparse.git jparse.clone"; \
	    echo; \
	    ${GIT} clone git@github.com:xexyl/jparse.git jparse.clone; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

jparse.clone_status: jparse.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${GIT} status jparse.clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

jparse.update_clone: jparse.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} cd jparse.clone && ${GIT} fetch
	${E} cd jparse.clone && ${GIT} fetch --prune --tags
	${E} cd jparse.clone && ${GIT} merge --ff-only || ${GIT} rebase --rebase-merges
	${E} ${MAKE} jparse.clone_status
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

jparse.recreate_clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} -rf jparse.clone
	${E} ${MAKE} jparse.clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

jparse.update_from_clone: jparse.clone/ jparse/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RSYNC} -a -S -0 --exclude=.git --exclude=.github -C --delete -v jparse.clone/ jparse
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg libdbg.a
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array libdyn_array.a
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse depend C_SPECIAL=-DINTERNAL_INCLUDE \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

jparse.update_into_clone: jparse/ jparse.clone/
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RSYNC} -a -S -0 --exclude=.git --exclude=.github -C --delete -v jparse/ jparse.clone
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse.clone depend C_SPECIAL=-UINTERNAL_INCLUDE \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

jparse.diff_jparse_clone: jparse.clone/ jparse/ .exclude
	-${E} ${DIFF} -u -r --exclude-from=.exclude jparse.clone jparse

jparse.diff_clone_jparse: jparse/ jparse.clone/ .exclude
	-${E} ${DIFF} -u -r --exclude-from=.exclude jparse jparse.clone

jparse.diff_summary: jparse.clone/ jparse/ .exclude
	-${E} ${DIFF} -r --brief --exclude-from=.exclude jparse.clone jparse

# rules to help incorporate external repos
#
all.help:
	${S} echo "The following rules are used by the people who maintain this repo."
	${S} echo
	${S} echo "Summary of rules to help incorporate external repos:"
	${S} echo
	${S} echo "make all.help - print this message"
	${S} echo "make all.setup - create or update clone directories from all remote repos"
	${S} echo "make all.clone - create missing clone directories from all remote repos"
	${S} echo "make all.clone_status - git status of all clone directories"
	${S} echo "make all.update_clone - update clone directories from all remote repos"
	${S} echo "make all.recreate_clone - remove then clone clone directories from remote repos"
	${S} echo "make all.update_from_clone - modify repo directories from clone directories"
	${S} echo "make all.update_into_clone - modify clone directories from repo directories"
	${S} echo "make all.diff_dir_clone - compare repo directories with clone directories"
	${S} echo "make all.diff_clone_dir - compare clone directories with repo directories"
	${S} echo "make all.diff_summary - summarize differnces between directories and clone directories"

all.setup:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.setup
	${E} ${MAKE} dyn_array.setup
	${E} ${MAKE} jparse.setup
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.clone
	${E} ${MAKE} dyn_array.clone
	${E} ${MAKE} jparse.clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.clone_status:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.clone_status
	${E} ${MAKE} dyn_array.clone_status
	${E} ${MAKE} jparse.clone_status
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.update_clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.update_clone
	${E} ${MAKE} dyn_array.update_clone
	${E} ${MAKE} jparse.update_clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.recreate_clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.recreate_clone
	${E} ${MAKE} dyn_array.recreate_clone
	${E} ${MAKE} jparse.recreate_clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.update_from_clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.update_from_clone
	${E} ${MAKE} dyn_array.update_from_clone
	${E} ${MAKE} jparse.update_from_clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.update_into_clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.update_into_clone
	${E} ${MAKE} dyn_array.update_into_clone
	${E} ${MAKE} jparse.update_into_clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.diff_dir_clone:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.diff_dbg_clone
	${E} echo
	${E} ${MAKE} dyn_array.diff_dyn_array_clone
	${E} echo
	${E} ${MAKE} jparse.diff_jparse_clone
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.diff_clone_dir:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.diff_clone_dbg
	${E} echo
	${E} ${MAKE} dyn_array.diff_clone_dyn_array
	${E} echo
	${E} ${MAKE} jparse.diff_clone_jparse
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

all.diff_summary:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} dbg.diff_summary
	${E} echo
	${E} ${MAKE} dyn_array.diff_summary
	${E} echo
	${E} ${MAKE} jparse.diff_summary
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"


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
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
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
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse/test_jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR2="${LD_DIR2}"
	${V} echo
	${RM} -rf .hostchk.work.*
	${RM} -f .txzchk_test.*
	${RM} -f .sorry.*
	${RM} -f .build.*
	${RM} -f answers.txt random_answers.*
	${RM} -f ${TARGETS}
	${RM} -rf man
	${RM} -f jparse_test.log chksubmit_test.log txzchk_test.log ${BUILD_LOG}
	${RM} -f tags ${LOCAL_DIR_TAGS}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# NOTE: We do NOT install dbg nor do we install dyn_array, nor jparse.
#       This repo do not use installed dbg, nor dyn_array, nor jparse code.
#
install: all dbg/Makefile dyn_array/Makefile jparse/Makefile \
        soup/Makefile test_ioccc/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${V} echo
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# uninstall - uninstall all files installed and directories created via this repo
#
# This is a quick way to uninstall all files installed by this repo.
# No directories are removed by this rule, however.
#
uninstall:
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${RM} -f ${RM_V} ${DEST_DIR}/all_sem_ref.sh
	${RM} -f ${RM_V} ${DEST_DIR}/chkentry
	${RM} -f ${RM_V} ${DEST_DIR}/fnamchk
	${RM} -f ${RM_V} ${DEST_DIR}/hostchk.sh
	${RM} -f ${RM_V} ${DEST_DIR}/iocccsize
	${RM} -f ${RM_V} ${DEST_DIR}/location
	${RM} -f ${RM_V} ${DEST_DIR}/mkiocccentry
	${RM} -f ${RM_V} ${DEST_DIR}/prep.sh
	${RM} -f ${RM_V} ${DEST_DIR}/reset_tstamp.sh
	${RM} -f ${RM_V} ${DEST_DIR}/run_usage.sh
	${RM} -f ${RM_V} ${DEST_DIR}/txzchk
	${RM} -f ${RM_V} ${DEST_DIR}/utf8_test
	${RM} -f ${RM_V} ${DEST_DIR}/vermod.sh
	${RM} -f ${RM_V} ${DEST_LIB}/soup.a
	${RM} -f ${RM_V} ${MAN1_DIR}/bug_report.sh.1
	${RM} -f ${RM_V} ${MAN1_DIR}/chkentry.1
	${RM} -f ${RM_V} ${MAN1_DIR}/fnamchk.1
	${RM} -f ${RM_V} ${MAN1_DIR}/iocccsize.1
	${RM} -f ${RM_V} ${MAN1_DIR}/location.1
	${RM} -f ${RM_V} ${MAN1_DIR}/mkiocccentry.1
	${RM} -f ${RM_V} ${MAN1_DIR}/txzchk.1
	${RM} -f ${RM_V} ${MAN8_DIR}/all_sem_ref.8
	${RM} -f ${RM_V} ${MAN8_DIR}/chksubmit_test.8
	${RM} -f ${RM_V} ${MAN8_DIR}/hostchk.8
	${RM} -f ${RM_V} ${MAN8_DIR}/ioccc_test.8
	${RM} -f ${RM_V} ${MAN8_DIR}/ioccc_test.sh.8
	${RM} -f ${RM_V} ${MAN8_DIR}/iocccsize_test.8
	${RM} -f ${RM_V} ${MAN8_DIR}/mkiocccentry_test.8
	${RM} -f ${RM_V} ${MAN8_DIR}/prep.8
	${RM} -f ${RM_V} ${MAN8_DIR}/prep.sh.8
	${RM} -f ${RM_V} ${MAN8_DIR}/reset_tstamp.8
	${RM} -f ${RM_V} ${MAN8_DIR}/run_usage.8
	${RM} -f ${RM_V} ${MAN8_DIR}/txzchk_test.8
	${RM} -f ${RM_V} ${MAN8_DIR}/utf8_test.8
	${RM} -f ${RM_V} ${MAN8_DIR}/vermod.8
	@if [[ -f /usr/local/lib/jparse.a ]]; then \
	    echo "Legacy uninstall follows:"; \
	    echo ${RM} -f ${RM_V} ${DEST_LIB}/jparse.a; \
	    ${RM} -f ${RM_V} ${DEST_LIB}/jparse.a; \
	fi

###############
# make depend #
###############

depend: ${ALL_CSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${E} ${MAKE} ${MAKE_CD_Q} -C dbg $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C dyn_array $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C jparse $@ C_SPECIAL="${C_SPECIAL}" \
		     LD_DIR="${LD_DIR}" LD_DIR2="${LD_DIR2}"
	${E} ${MAKE} ${MAKE_CD_Q} -C test_ioccc $@ C_SPECIAL="${C_SPECIAL}"
	${E} ${MAKE} ${MAKE_CD_Q} -C soup $@ C_SPECIAL="${C_SPECIAL}"
	${Q} if ! ${IS_AVAILABLE} ${INDEPEND} >/dev/null 2>&1; then \
	    echo '${OUR_NAME}: The ${INDEPEND} command could not be found or is unreliable in your system.' 1>&2; \
	    echo '${OUR_NAME}: The ${INDEPEND} command is required to run the $@ rule'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for ${INDEPEND}:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/independ'; 1>&2; \
	elif ! ${IS_AVAILABLE} ${SED} >/dev/null 2>&1; then \
	    echo '${OUR_NAME}: The ${SED} command could not be found.' 1>&2; \
	    echo '${OUR_NAME}: The ${SED} command is required to run the $@ rule'; 1>&2; \
	    echo ''; 1>&2; \
	elif ! ${IS_AVAILABLE} ${GREP} >/dev/null 2>&1; then \
	    echo '${OUR_NAME}: The ${GREP} command could not be found.' 1>&2; \
	    echo '${OUR_NAME}: The ${GREP} command is required to run the $@ rule'; 1>&2; \
	    echo ''; 1>&2; \
	elif ! ${IS_AVAILABLE} ${CMP} >/dev/null 2>&1; then \
	    echo '${OUR_NAME}: The ${CMP} command could not be found.' 1>&2; \
	    echo '${OUR_NAME}: The ${CMP} command is required to run the $@ rule'; 1>&2; \
	    echo ''; 1>&2; \
	else \
	    if ! ${GREP} -q '^### DO NOT CHANGE MANUALLY BEYOND THIS LINE$$' Makefile; then \
	        echo "${OUR_NAME}: make $@ aborting, Makefile missing: ### DO NOT CHANGE MANUALLY BEYOND THIS LINE" 1>&2; \
		exit 1; \
	    fi; \
	    ${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE$$/p' Makefile; \
	    ${CC} ${CFLAGS} -MM -I. -DMKIOCCCENTRY_USE ${ALL_CSRC} | \
	      ${SED} -e 's;jparse/\.\./dyn_array/;dyn_array/;g' \
	             -e 's;soup/\.\./jparse/;jparse/;g' \
	             -e 's;jparse/\.\./dbg/;dbg/;g' \
	             -e 's;soup/\.\./dbg/;dbg/;g' \
	             -e 's;dyn_array/\.\./dbg/;dbg/;g' | \
	      ${INDEPEND} >> Makefile; \
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
chkentry.o: chkentry.c chkentry.h dbg/c_bool.h dbg/c_compat.h dbg/dbg.h \
    dyn_array/dyn_array.h jparse/jparse.h jparse/jparse.tab.h \
    jparse/json_parse.h jparse/json_sem.h jparse/json_utf8.h \
    jparse/json_util.h jparse/util.h jparse/version.h \
    soup/../dyn_array/dyn_array.h soup/chk_sem_auth.h soup/chk_sem_info.h \
    soup/chk_validate.h soup/default_handle.h soup/entry_util.h \
    soup/file_util.h soup/foo.h soup/limit_ioccc.h soup/location.h \
    soup/sanity.h soup/soup.h soup/version.h
chksubmit.o: chksubmit.c chksubmit.h dbg/c_bool.h dbg/c_compat.h dbg/dbg.h \
    dyn_array/dyn_array.h jparse/jparse.h jparse/jparse.tab.h \
    jparse/json_parse.h jparse/json_sem.h jparse/json_utf8.h \
    jparse/json_util.h jparse/util.h jparse/version.h \
    soup/../dyn_array/dyn_array.h soup/chk_sem_auth.h soup/chk_sem_info.h \
    soup/chk_validate.h soup/default_handle.h soup/entry_util.h \
    soup/file_util.h soup/foo.h soup/limit_ioccc.h soup/location.h \
    soup/sanity.h soup/soup.h soup/version.h
iocccsize.o: dbg/c_bool.h dbg/c_compat.h dbg/dbg.h iocccsize.c iocccsize.h \
    jparse/jparse.h jparse/jparse.tab.h jparse/json_parse.h \
    jparse/json_sem.h jparse/json_utf8.h jparse/json_util.h jparse/util.h \
    jparse/version.h soup/../dbg/dbg.h soup/../dyn_array/dyn_array.h \
    soup/iocccsize_err.h soup/limit_ioccc.h soup/location.h soup/version.h
mkiocccentry.o: dbg/c_bool.h dbg/c_compat.h dbg/dbg.h iocccsize.h \
    jparse/jparse.h jparse/jparse.tab.h jparse/json_parse.h \
    jparse/json_sem.h jparse/json_utf8.h jparse/json_util.h jparse/util.h \
    jparse/version.h mkiocccentry.c mkiocccentry.h soup/../dbg/dbg.h \
    soup/../dyn_array/dyn_array.h soup/chk_sem_auth.h soup/chk_sem_info.h \
    soup/chk_validate.h soup/default_handle.h soup/entry_util.h \
    soup/file_util.h soup/limit_ioccc.h soup/location.h \
    soup/random_answers.h soup/sanity.h soup/soup.h soup/version.h
txzchk.o: dbg/c_bool.h dbg/c_compat.h dbg/dbg.h jparse/jparse.h \
    jparse/jparse.tab.h jparse/json_parse.h jparse/json_sem.h \
    jparse/json_utf8.h jparse/json_util.h jparse/util.h jparse/version.h \
    soup/../dbg/dbg.h soup/../dyn_array/dyn_array.h soup/chk_sem_auth.h \
    soup/chk_sem_info.h soup/chk_validate.h soup/default_handle.h \
    soup/entry_util.h soup/file_util.h soup/limit_ioccc.h soup/location.h \
    soup/sanity.h soup/soup.h soup/version.h txzchk.c txzchk.h
