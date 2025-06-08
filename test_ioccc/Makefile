#!/usr/bin/env make
#
# test_ioccc - mkiocccentry test tools
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
# These tools were co-developed in 2021-2025 by Cody Boone Ferguson and Landon
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



#############
# utilities #
#############

# suggestion: List utility filenames, not paths.
#	      Do not list shell builtin (echo, cd, ...) tools.
#	      Keep the list in alphabetical order.
#
CHECKNR= checknr
CC= cc
CTAGS= ctags
CMP= cmp
GREP= grep
INDEPEND= independ
INSTALL= install
IS_AVAILABLE= ../soup/is_available.sh
PICKY= picky
RM= rm
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
SORT= sort


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
OUR_NAME= test_ioccc

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
C_SRC= utf8_test.c fnamchk.c
H_SRC= fnamchk.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=

# all shell scripts
#
SH_FILES= chkentry_test.sh ioccc_test.sh iocccsize_test.sh mkiocccentry_test.sh txzchk_test.sh \
	prep.sh hostchk.sh gen_test_JSON.sh

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES= man/man1/fnamchk.1
MAN3_PAGES=
MAN8_PAGES= man/man8/chkentry_test.8 man/man8/ioccc_test.8 man/man8/iocccsize_test.8 \
	    man/man8/mkiocccentry_test.8 man/man8/txzchk_test.8 man/man8/hostchk.8 \
	    man/man8/prep.8 man/man8/utf8_test.8
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# tags for just the files in this directory
#
LOCAL_DIR_TAGS= .local.dir.tags

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS=

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS= utf8_test.o fnamchk.o

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC=
BUILT_H_SRC=
ALL_BUILT_SRC= ${BUILT_C_SRC} ${BUILT_H_SRC}

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

TXZCHK_LOG= txzchk_test.log


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
EXTERN_MAN= ${ALL_MAN_TARGETS}
EXTERN_LIBA=
EXTERN_PROG= prep.sh fnamchk hostchk.sh

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
PROG_TARGETS= utf8_test fnamchk

# what to make by all but NOT to removed by clobber
#
ALL_OTHER_TARGETS= ${SH_TARGETS} extern_everything ${ALL_MAN_PAGES}

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
#       CC:= gcc
#
-include Makefile.local


######################################
# all - default rule - must be first #
######################################

all: ${TARGETS} ${EXTERN_PROG}
	@:


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	test legacy_clean legacy_clobber install_man hostchk.sh \
	tags local_dir_tags all_tags \
	configure clean clobber install depend uninstall


####################################
# things to make in this directory #
####################################

utf8_test.o: utf8_test.c
	${CC} ${CFLAGS} utf8_test.c -c

utf8_test: utf8_test.o ../soup/soup.a ../jparse/libjparse.a ../dyn_array/libdyn_array.a ../dbg/libdbg.a
	${CC} ${CFLAGS} $^ -o $@

ioccc_test.sh: ../dbg/dbg_test ../dyn_array/dyn_test
	@:

fnamchk.o: fnamchk.c fnamchk.h
	${CC} ${CFLAGS} fnamchk.c -c

fnamchk: fnamchk.o ../soup/soup.a ../jparse/libjparse.a ../dyn_array/libdyn_array.a ../dbg/libdbg.a
	${CC} ${CFLAGS} $^ -o $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../dbg/libdbg.a: ../dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../dbg extern_liba

../dbg/dbg_test: ../dbg/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../dbg extern_prog

../dyn_array/libdyn_array.a: ../dyn_array/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../dyn_array extern_liba

../dyn_array/dyn_test: ../dyn_array/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../dyn_array dyn_test

../soup/soup.a: ../soup/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../soup extern_liba


####################################
# rules for use by other Makefiles #
####################################


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

test_JSON: ../soup/limit_ioccc.h ../soup/version.h
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ./gen_test_JSON.sh template.test_JSON test_JSON
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"
	${S} echo

test: test_JSON
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if [[ ! -x ./ioccc_test.sh ]]; then \
	    echo "${OUR_NAME}: ERROR: executable not found: ./ioccc_test.sh" 1>&2; \
	    echo "${OUR_NAME}: ERROR: unable to perform complete test" 1>&2; \
	    exit 1; \
	else \
	    echo ./ioccc_test.sh; \
	    ./ioccc_test.sh; \
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo "${OUR_NAME}: ERROR: ioccc_test.sh failed, error code: $$EXIT_CODE"; \
		exit "$$EXIT_CODE"; \
	    else \
		echo ${OUR_NAME}: "PASSED: ioccc_test.sh"; \
	    fi; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# sequence exit codes
#
seqcexit: ${ALL_CSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
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

picky: ${ALL_SRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
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
shellcheck: ${SH_FILES} .shellcheckrc
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
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
check_man: ${ALL_MAN_TARGETS}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if ! ${IS_AVAILABLE} ${CHECKNR} >/dev/null 2>&1; then \
	    echo 'The ${CHECKNR} command could not be found or is unreliable in your system.' 1>&2; \
	    echo 'The ${CHECKNR} command is required to run the $@ rule.' 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for ${CHECKNR}:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/checknr' 1>&2; \
	else \
	    echo "${CHECKNR} -c.BR.SS.BI.IR.RB.RI ${ALL_MAN_TARGETS}"; \
	    ${CHECKNR} -c.BR.SS.BI.IR.RB.RI ${ALL_MAN_TARGETS}; \
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo "make $@: ERROR: CODE[1]: $$EXIT_CODE" 1>&2; \
		exit 1; \
	    fi; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"



# install man pages
#
install_man: ${ALL_MAN_TARGETS}
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${MAN1_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${MAN1_TARGETS} ${MAN1_DIR}
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${MAN8_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${MAN8_TARGETS} ${MAN8_DIR}

# vi/vim tags
#
tags:
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
	${Q} for dir in ../dbg ../dyn_alloc ../jparse ../jpaese/test_jparse ../soup; do \
	    if [[ -f $$dir/Makefile && ! -f $$dir/${LOCAL_DIR_TAGS} ]]; then \
		echo ${MAKE} ${MAKE_CD_Q} -C $$dir local_dir_tags; \
		${MAKE} ${MAKE_CD_Q} -C $$dir local_dir_tags; \
	    fi; \
	done
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
	${Q} ${RM} ${RM_V} -f ${LOCAL_DIR_TAGS}
	-${S} ${CTAGS} -w -f ${LOCAL_DIR_TAGS} ${ALL_CSRC} ${ALL_HSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# for a tags file from all ${LOCAL_DIR_TAGS} in all of the other directories
#
all_tags:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${RM} ${RM_V} -f tags
	${Q} for dir in . ../dbg ../dyn_alloc ../jparse ../jparse/test_jparse ../soup; do \
	    if [[ -s $$dir/${LOCAL_DIR_TAGS} ]]; then \
		echo "${SED} -e 's;\t;\t'$${dir}'/;' $${dir}/${LOCAL_DIR_TAGS} >> tags"; \
		${SED} -e 's;\t;\t'$${dir}'/;' "$${dir}/${LOCAL_DIR_TAGS}" >> tags; \
	    fi; \
	done
	${E} ${SORT} tags -o tags
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

legacy_clean:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${S} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

legacy_clobber: legacy_clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${V} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"



###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean: legacy_clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} ${RM_V} -f ${OTHER_OBJS}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

clobber: legacy_clobber clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} ${RM_V} -f ${TARGETS}
	${E} ${RM} ${RM_V} -f test_ioccc.log chkentry_test.log txzchk_test.log
	${E} ${RM} ${RM_V} -f Makefile.orig
	${E} ${RM} ${RM_V} -f tags ${LOCAL_DIR_TAGS}
	${E} ${RM} ${RM_V} -rf test_iocccsize topdir workdir test_JSON
	${E} ${RM} ${RM_V} -rf slot/good/workdir slot/bad/workdir
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

install: all install_man
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

uninstall:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/utf8_test
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/fnamchk
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/prep.sh
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/hostchk.sh
	${E} ${RM} ${RM_V} -f ${MAN1_DIR}/fnamchk.1
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/chkentry_test.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/ioccc_test.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/ioccc_test.sh.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/iocccsize_test.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/mkiocccentry_test.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/txzchk_test.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/hostchk.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/ioccc_test.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/prep.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/prep.sh.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/utf8_test.8
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

###############
# make depend #
###############

depend: ${ALL_CSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
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
	      ${SED} -e 's;\.\./soup/\.\./dbg/;../dbg/;g' \
	             -e 's;\.\./soup/\.\./jparse/\.\./dyn_array/;../dyn_array/;g' \
	             -e 's;\.\./soup/\.\./jparse/\.\./dbg/;../dbg/;g' \
	             -e 's;\.\./soup/\.\./jparse/;../jparse/;g' \
	             -e 's;\.\./dyn_array/\.\./dbg/;../dbg/;g' \
	             -e 's;\.\./jparse/\.\./dbg/;../dbg/;g' \
	             -e 's;\.\./jparse/\.\./dyn_array/;../dyn_array/;g' | \
	      ${INDEPEND} >> Makefile; \
	    if ${CMP} -s Makefile.orig Makefile; then \
		${RM} ${RM_V} -f Makefile.orig; \
	    else \
		echo "${OUR_NAME}: Makefile dependencies updated"; \
		echo; \
		echo "${OUR_NAME}: Previous version may be found in: Makefile.orig"; \
	    fi; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
fnamchk.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/version.h ../soup/default_handle.h ../soup/limit_ioccc.h \
    ../soup/location.h ../soup/version.h fnamchk.c fnamchk.h
utf8_test.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/version.h ../soup/default_handle.h ../soup/limit_ioccc.h \
    ../soup/location.h ../soup/version.h utf8_test.c
