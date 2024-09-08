#!/usr/bin/env make
#
# dbg - info, debug, warning, error and usage message facilities
#
# Copyright (c) 1989,1997,2018-2024 by Landon Curt Noll.  All Rights Reserved.
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


#############
# utilities #
#############

# suggestion: List utility filenames, not paths.
#	      Do not list shell builtin (echo, cd, ...) tools.
#	      Keep the list in alphabetical order.
#
AR= ar
CAT= cat
CC= cc
CHECKNR= checknr
CMP= cmp
CP= cp
CTAGS= ctags
GREP= grep
INDEPEND= independ
INSTALL= install
LN= ln
PICKY= picky
RANLIB= ranlib
RM= rm
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SORT= sort


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
OUR_NAME= dbg

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

# E= @:					do not echo calling make in another directory (quiet mode)
# E= @					echo calling make in another directory (debug / verbose mode)
#
E=
#E= @

# Q= implies -v 0
# else -v 1
#
ifeq ($(strip ${Q}),@)
Q_V_OPTION="0"
else
Q_V_OPTION="1"
endif

# I= @					do not echo install commands (quiet mode)
# I=					echo install commands (debug / verbose mode
#
I=
#I= @

# installing variables
#

# INSTALL_V=				install w/o -v flag (quiet mode)
# INSTALL_V= -v				install with -v (debug / verbose mode
#
#INSTALL_V=
INSTALL_V= -v

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
C_OPT= -O3 -g3
#C_OPT= -O0 -g

# Compiler warnings
#
WARN_FLAGS= -pedantic -Wall -Wextra -Wno-char-subscripts
#WARN_FLAGS= -pedantic -Wall -Wextra -Werror

# special compiler flags
#
C_SPECIAL=

# special linker flags
#
LD_SPECIAL=

# linker options
#
LDFLAGS= ${LD_SPECIAL}

# how to compile
#
CFLAGS= ${C_STD} ${C_OPT} ${WARN_FLAGS} ${C_SPECIAL} ${LDFLAGS}
#CFLAGS= ${C_STD} -O0 -g ${WARN_FLAGS} ${C_SPECIAL} ${LDFLAGS} -fsanitize=address -fno-omit-frame-pointer


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= dbg.c dbg_example.c dbg_test.c
H_SRC= dbg.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=

# all shell scripts
#
SH_FILES=

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES=
MAN3_PAGES= man/man3/dbg.3 man/man3/err.3 man/man3/msg.3 man/man3/warn.3 man/man3/werr.3 \
	man/man3/printf_usage.3 man/man3/warn_or_err.3 \
	man/man3/errp.3 man/man3/fdbg.3 man/man3/ferr.3 man/man3/ferrp.3 man/man3/fmsg.3 \
	man/man3/fprintf_usage.3 man/man3/fwarn.3 man/man3/fwarn_or_err.3 man/man3/fwarnp.3 \
	man/man3/fwerr.3 man/man3/fwerrp.3 man/man3/sndbg.3 man/man3/snmsg.3 man/man3/snwarn.3 \
	man/man3/snwarnp.3 man/man3/snwerr.3 man/man3/snwerrp.3 man/man3/vdbg.3 man/man3/verr.3 \
	man/man3/verrp.3 man/man3/vfdbg.3 man/man3/vferr.3 man/man3/vferrp.3 man/man3/vfmsg.3 \
	man/man3/vfprintf_usage.3 man/man3/vfwarn.3 man/man3/vfwarn_or_err.3 man/man3/vfwarnp.3 \
	man/man3/vfwerr.3 man/man3/vfwerrp.3 man/man3/vmsg.3 man/man3/vprintf_usage.3 man/man3/vsndbg.3 \
	man/man3/vsnmsg.3 man/man3/vsnwarn.3 man/man3/vsnwarnp.3 man/man3/vsnwerr.3 man/man3/vsnwerrp.3 \
	man/man3/vwarn.3 man/man3/vwarn_or_err.3 man/man3/vwarnp.3 man/man3/vwerr.3 man/man3/vwerrp.3 \
	man/man3/warnp.3 man/man3/werrp.3
MAN8_PAGES=
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# tags for just the files in this directory
#
LOCAL_DIR_TAGS= .local.dir.tags

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= dbg.o

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS= dbg_test.o dbg_example.o

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC= dbg_test.c
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
EXTERN_H= dbg.h
EXTERN_O= dbg.o
EXTERN_MAN= ${ALL_MAN_TARGETS}
EXTERN_LIBA= libdbg.a
EXTERN_PROG= dbg_example dbg_test

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

# libraries to make by all, what to install, and removed by clobber
#
LIBA_TARGETS= libdbg.a

# shell targets to make by all and removed by clobber
#
SH_TARGETS=

# program targets to make by all, installed by install, and removed by clobber
#
PROG_TARGETS= dbg_test dbg_example

# include files but NOT to removed by clobber
#
H_SRC_TARGETS= dbg.h

# what to make by all but NOT to removed by clobber
#
ALL_OTHER_TARGETS= ${SH_TARGETS} ${ALL_MAN_PAGES}

# what to make by all, what to install, and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${LIBA_TARGETS} ${PROG_TARGETS} ${ALL_MAN_BUILT}


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
#       CC:= gcc
#
-include makefile.local


###########################################
# all rule - default rule - must be first #
###########################################

all: ${TARGETS}
	@:


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	extern_include extern_objs extern_liba extern_man extern_prog extern_everything man/man3/dbg.3 \
	man/man3/err.3 man/man3/msg.3 man/man3/warn.3 man/man3/werr.3 man/man3/printf_usage.3 man/man3/warn_or_err.3 \
	test check_man legacy_clean legacy_clobber install_man \
	configure clean clobber install depend tags local_dir_tags all_tags


####################################
# things to make in this directory #
####################################

dbg.o: dbg.c
	${CC} ${CFLAGS} dbg.c -c

libdbg.a: ${LIB_OBJS}
	${Q} ${RM} ${RM_V} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@

dbg_test.c: dbg.c
	${Q} ${RM} ${RM_V} -f $@
	${CP} -v -f dbg.c $@

dbg_test.o: dbg_test.c
	${CC} ${CFLAGS} -DDBG_TEST dbg_test.c -c

dbg_test: dbg_test.o
	${CC} ${CFLAGS} dbg_test.o -o $@

dbg_example.o: dbg_example.c
	${CC} ${CFLAGS} dbg_example.c -c

dbg_example: dbg_example.o dbg.o
	${CC} ${CFLAGS} dbg_example.o dbg.o -o $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################


####################################
# rules for use by other Makefiles #
####################################

extern_include: ${EXTERN_H}
	@:

extern_objs: ${EXTERN_O}
	@:

extern_liba: ${EXTERN_LIBA}
	@:

extern_man: ${EXTERN_MAN}
	@:

extern_prog: ${EXTERN_PROG}
	@:

extern_everything: extern_include extern_objs extern_liba extern_man extern_prog
	@:

man/man3/dbg.3:
	@:

man/man3/warn.3:
	@:

man/man3/msg.3:
	@:

man/man3/warn_or_err.3:
	@:

man/man3/werr.3:
	@:

man/man3/err.3:
	@:

man/man3/printf_usage.3:
	@:


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

test:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${RM} ${RM_V} -f dbg_test.out
	${Q} if [[ ! -x ./dbg_test ]]; then \
	    echo "${OUR_NAME}: ERROR: executable not found: ./dbg_test" 1>&2; \
	    echo "${OUR_NAME}: ERROR: unable to perform complete test" 1>&2; \
	    exit 1; \
	else \
	    echo "./dbg_test -e 2 foo bar baz >dbg_test.out 2>&1"; \
	    ./dbg_test -v 1 -e 2 foo bar baz >dbg_test.out 2>&1; \
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 5 ]]; then \
		echo "${OUR_NAME}: exit status of dbg_test: $$EXIT_CODE != 5"; \
		exit 21; \
	    else \
		${GREP} -q '^ERROR\[5\]: main: simulated error, foo: foo bar: bar: errno\[2\]: No such file or directory$$' dbg_test.out; \
		EXIT_CODE="$$?"; \
		if [[ $$EXIT_CODE -ne 0 ]]; then \
		    echo "${OUR_NAME}: ERROR: did not find the correct dbg_test error message" 1>&2; \
		    echo "${OUR_NAME}: ERROR: beginning dbg_test.out contents" 1>&2; \
		    ${CAT} dbg_test.out 1>&2; \
		    echo "${OUR_NAME}: ERROR: dbg_test.out contents complete" 1>&2; \
		    exit 22; \
		else \
		    echo "${OUR_NAME}: PASSED: dbg_test"; \
		fi; \
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
	${Q} if ! type -P ${SEQCEXIT} >/dev/null 2>&1; then \
	    echo 'The ${SEQCEXIT} tool could not be found.' 1>&2; \
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
	${Q} if ! type -P ${PICKY} >/dev/null 2>&1; then \
	    echo 'The ${PICKY} tool could not be found.' 1>&2; \
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
		echo "picky: ERROR: $$EXIT_CODE" 1>&2; \
		exit 1; \
	    fi; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# inspect and verify shell scripts
#
shellcheck:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${V} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# inspect and verify man pages
#
check_man: ${ALL_MAN_TARGETS}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	-${Q} if ! type -P ${CHECKNR} >/dev/null 2>&1; then \
	    echo 'The ${CHECKNR} command could not be found.' 1>&2; \
	    echo 'The ${CHECKNR} command is required to run the $@ rule.' 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for ${CHECKNR}:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/checknr' 1>&2; \
	    echo ''; 1>&2; \
	    echo 'Or use the package manager in your OS to install it.' 1>&2; \
	else \
	    echo "${CHECKNR} -c.BR.SS.BI.IR.RB.RI ${ALL_MAN_TARGETS}"; \
	    ${CHECKNR} -c.BR.SS.BI.IR.RB.RI ${ALL_MAN_TARGETS}; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"


# install man pages
#
install_man: ${ALL_MAN_TARGETS}
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${MAN3_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${MAN3_TARGETS} ${MAN3_DIR}

# vi/vim tags
#
tags: ${ALL_CSRC} ${ALL_HSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if ! type -P ${CTAGS} >/dev/null 2>&1; then \
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
	${E} ${MAKE} local_dir_tags C_SPECIAL=${C_SPECIAL}
	${Q} echo
	${E} ${MAKE} all_tags C_SPECIAL=${C_SPECIAL}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# use the ${CTAGS} tool to form ${LOCAL_DIR_TAGS} of the source in this directory
#
local_dir_tags: ${ALL_CSRC} ${ALL_HSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if ! type -P ${CTAGS} >/dev/null 2>&1; then \
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
	-${E} ${CTAGS} -w -f ${LOCAL_DIR_TAGS} ${ALL_CSRC} ${ALL_HSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# for a tags file from ${LOCAL_DIR_TAGS}
#
all_tags:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${RM} ${RM_V} -f tags
	${E} ${CP} -f -v ${LOCAL_DIR_TAGS} tags
	${E} ${SORT} tags -o tags
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

legacy_clean:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${V} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

legacy_clobber: legacy_clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${RM} ${RM_V} -f dbg.a
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"


###################################
# standard Makefile utility rules #
###################################

configure:
	${V} echo nothing to $@

clean:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${RM} ${RM_V} -f ${ALL_OBJS} ${ALL_BUILT_SRC}
	${Q} ${RM} ${RM_V} -f dbg_test.out
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

clobber: legacy_clobber clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${RM} ${RM_V} -f ${TARGETS}
	${Q} ${RM} ${RM_V} -f tags ${LOCAL_DIR_TAGS}
	${Q} ${RM} ${RM_V} -f Makefile.orig
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

install: all install_man
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_LIB}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${LIBA_TARGETS} ${DEST_LIB}
	${Q} ${RM} ${RM_V} -f ${DEST_LIB}/`echo ${LIBA_TARGETS} | ${SED} -e 's/^lib//'`
	${I} ${LN} -s ${LIBA_TARGETS} ${DEST_LIB}/`echo ${LIBA_TARGETS} | ${SED} -e 's/^lib//'`
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_INCLUDE}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${H_SRC_TARGETS} ${DEST_INCLUDE}
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

uninstall:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${RM} ${RM_V} -f ${DEST_LIB}/libdbg.a
	${Q} ${RM} ${RM_V} -f ${DEST_LIB}/dbg.a
	${Q} ${RM} ${RM_V} -f ${DEST_DIR}/dbg_example
	${Q} ${RM} ${RM_V} -f ${DEST_DIR}/dbg_test
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/dbg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/err.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/msg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/warn.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/werr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/printf_usage.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/warn_or_err.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/errp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/fdbg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/ferr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/ferrp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/fmsg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/fprintf_usage.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/fwarn.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/fwarn_or_err.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/fwarnp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/fwerr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/fwerrp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/sndbg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/snmsg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/snwarn.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/snwarnp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/snwerr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/snwerrp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vdbg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/verr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/verrp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vfdbg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vferr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vferrp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vfmsg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vfprintf_usage.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vfwarn.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vfwarn_or_err.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vfwarnp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vfwerr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vfwerrp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vmsg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vprintf_usage.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vsndbg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vsnmsg.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vsnwarn.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vsnwarnp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vsnwerr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vsnwerrp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vwarn.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vwarn_or_err.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vwarnp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vwerr.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/vwerrp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/warnp.3
	${Q} ${RM} ${RM_V} -f ${MAN3_DIR}/werrp.3
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

###############
# make depend #
###############

depend: ${ALL_CSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${Q} if ! type -P ${INDEPEND} >/dev/null 2>&1; then \
	    echo '${OUR_NAME}: The ${INDEPEND} command could not be found.' 1>&2; \
	    echo '${OUR_NAME}: The ${INDEPEND} command is required to run the $@ rule'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for ${INDEPEND}:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/independ'; 1>&2; \
	else \
	    if ! ${GREP} -q '^### DO NOT CHANGE MANUALLY BEYOND THIS LINE$$' Makefile; then \
	        echo "${OUR_NAME}: make $@ aborting, Makefile missing: ### DO NOT CHANGE MANUALLY BEYOND THIS LINE" 1>&2; \
		exit 1; \
	    fi; \
	    ${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE$$/p' Makefile; \
	    ${CC} ${CFLAGS} -MM -I. ${ALL_CSRC} | \
		${SED} -E -e 's;\s/usr/local/include/\S+;;g' -e 's;\s/usr/include/\S+;;g' | \
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
dbg.o: dbg.c dbg.h
dbg_example.o: dbg.h dbg_example.c
dbg_test.o: dbg.h dbg_test.c
