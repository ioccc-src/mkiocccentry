#!/usr/bin/env make
#
# cpath - canonicalize paths
#
# "Not all those who wander are lost."
#
#      -- J.R.R. Tolkien
#
# Copyright (c) 1991-2025 by Landon Curt Noll.  All Rights Reserved.
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
# This code was developed between 1991-2025 by Landon Curt Noll:
#
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
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
CC= cc
CHECKNR= checknr
CMP= cmp
CP= cp
CTAGS= ctags
GREP= grep
INDEPEND= independ
INSTALL= install
IS_AVAILABLE= ./is_available.sh
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
OUR_NAME= cpath

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
VERBOSITY="0"
else
VERBOSITY="1"
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
C_OPT= -O3
#C_OPT= -O0 -g3

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

# where to find libdbg.a and libdyn_array.a and libpr.a
#
# NOTE: By default, when ${LD_DIR} is empty, we assume that both
# 	libdbg.a (see https://github.com/lcn2/dbg) and
# 	libdyn_array.a (see https://github.com/lcn2/dyn_array)
# 	installed on the system.
#
# NOTE: For cases such as the [mkiocccentry toolkit](https://github.com/ioccc-src/mkiocccentry)
# 	the top level Makefile can set ${LD_DIR} to something such as:
#
# 	    LD_DIR= -L../dbg -L../dyn_array -L../pr
#
LD_DIR=
#LD_DIR= -L../dbg -L../dyn_array -L../pr

# linker options
#
LDFLAGS= ${LD_SPECIAL}

# how to compile
CFLAGS= ${C_STD} ${C_OPT} ${WARN_FLAGS} ${C_SPECIAL} ${LDFLAGS}
#CFLAGS= ${C_STD} -O0 -g ${WARN_FLAGS} ${C_SPECIAL} ${LDFLAGS} -fsanitize=address -fno-omit-frame-pointer


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= canon_path.c cpath.c
H_SRC= cpath.h
#
PICKY_OPTIONS= -c -e -s -t8 -u -v -w132

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=
#
LESS_PICKY_OPTIONS= -8 -c -e -s -t8 -u -v -w

# all shell scripts
#
SH_FILES= is_available.sh
#
SH_PICKY_OPTIONS= -c -e -s -t8 -u -v -w132

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES= man/man1/cpath.1

CPATH_MAN= man/man3/canon_path.3
CPATH_MAN_DUPS= \
	man/man3/path_sanity_error.3 man/man3/path_sanity_name.3 man/man3/safe_path_str.3 \
	man/man3/safe_str.3
MAN3_PAGES= ${CPATH_MAN} ${CPATH_MAN_DUPS}
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
LIB_OBJS= canon_path.o

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS= cpath.o

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
EXTERN_H= cpath.h
EXTERN_O= canon_path.o
EXTERN_MAN= ${ALL_MAN_TARGETS}
EXTERN_LIBA= libcpath.a
EXTERN_PROG= cpath

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
LIBA_TARGETS= libcpath.a

# shell targets to make by all and removed by clobber
#
SH_TARGETS= test_cpath.sh

# program targets to make by all, installed by install, and removed by clobber
#
PROG_TARGETS= cpath

# include files but NOT to removed by clobber
#
H_SRC_TARGETS= cpath.h

# what to make by all but NOT to removed by clobber
#
ALL_OTHER_TARGETS= ${SH_TARGETS} extern_everything ${ALL_MAN_PAGES}

# what to make by all, what to install, and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${LIBA_TARGETS} ${PROG_TARGETS} ${ALL_MAN_BUILT}


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


###########################################
# all rule - default rule - must be first #
###########################################

all: ${TARGETS} ${ALL_OTHER_TARGETS}
	@:

#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	extern_include extern_objs extern_liba extern_man extern_prog extern_everything \
	test check_man tags local_dir_tags all_tags legacy_clean legacy_clobber install_man \
	configure clean clobber install depend man/man1/cpath.1 man/man3/canon_path.3


####################################
# things to make in this directory #
####################################

canon_path.o: canon_path.c cpath.h
	${CC} ${CFLAGS} canon_path.c -c

libcpath.a: ${LIB_OBJS}
	${Q} ${RM} ${RM_V} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@

cpath.o: cpath.c cpath.h
	${CC} ${CFLAGS} cpath.c -c

cpath: cpath.o canon_path.o
	${CC} ${CFLAGS} cpath.o canon_path.o ${LD_DIR} -ldbg -ldyn_array -lpr -o $@

# form the duplicate copies of the `canon_path(3)` man page
#
${CPATH_MAN_DUPS}: ${CPATH_MAN}
	@for i in ${CPATH_MAN_DUPS}; do \
	    if ! ${CMP} -s ${CPATH_MAN} "$$i" > /dev/null 2>&1; then \
		${CP} -f -p -v ${CPATH_MAN} "$$i"; \
	    fi; \
	done


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

man/man1/cpath.1:
	@:

man/man3/canon_path.3:
	@:


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

test: test_cpath.sh
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} if [[ ! -x ./test_cpath.sh ]]; then \
	    echo "${OUR_NAME}: ERROR: executable not found: ./test_cpath.sh" 1>&2; \
	    echo "${OUR_NAME}: ERROR: unable to perform complete test" 1>&2; \
	    exit 1; \
	else \
	    echo ./test_cpath.sh -v ${VERBOSITY}; \
	    ./test_cpath.sh -v ${VERBOSITY}; \
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo "${OUR_NAME}: ERROR: test_cpath.sh failed, error code: $$EXIT_CODE"; \
		exit "$$EXIT_CODE"; \
	    else \
		echo ${OUR_NAME}: "PASSED: test_cpath.sh"; \
	    fi; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# sequence exit codes
#
# NOTE: do NOT use -v ${VERBOSITY}!
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
	    if [[ -n "${C_SRC}" ]]; then \
		echo "${PICKY} ${PICKY_OPTIONS} -- ${C_SRC}"; \
		${PICKY} ${PICKY_OPTIONS} -- ${C_SRC}; \
		EXIT_CODE="$$?"; \
		if [[ $$EXIT_CODE -ne 0 ]]; then \
		    echo "make $@: ERROR: CODE[1]: $$EXIT_CODE" 1>&2; \
		    exit 1; \
		fi; \
	    fi; \
	    if [[ -n "${H_SRC}" ]]; then \
		echo "${PICKY} ${PICKY_OPTIONS} -- ${H_SRC}"; \
		${PICKY} ${PICKY_OPTIONS} -- ${H_SRC}; \
		EXIT_CODE="$$?"; \
		if [[ $$EXIT_CODE -ne 0 ]]; then \
		    echo "make $@: ERROR: CODE[1]: $$EXIT_CODE" 1>&2; \
		    exit 2; \
		fi; \
	    fi; \
	    if [[ -n "${LESS_PICKY_CSRC}" ]]; then \
		echo "${PICKY} ${LESS_PICKY_OPTIONS} -- ${LESS_PICKY_CSRC}"; \
		${PICKY} ${LESS_PICKY_OPTIONS} -- ${LESS_PICKY_CSRC}; \
		EXIT_CODE="$$?"; \
		if [[ $$EXIT_CODE -ne 0 ]]; then \
		    echo "make $@: ERROR: CODE[1]: $$EXIT_CODE" 1>&2; \
		    exit 3; \
		fi; \
	    fi; \
	    if [[ -n "${LESS_PICKY_HSRC}" ]]; then \
		echo "${PICKY} ${LESS_PICKY_OPTIONS} -- ${LESS_PICKY_HSRC}"; \
		${PICKY} ${LESS_PICKY_OPTIONS} -- ${LESS_PICKY_HSRC}; \
		EXIT_CODE="$$?"; \
		if [[ $$EXIT_CODE -ne 0 ]]; then \
		    echo "make $@: ERROR: CODE[1]: $$EXIT_CODE" 1>&2; \
		    exit 4; \
		fi; \
	    fi; \
	    if [[ -n "${SH_FILES}" ]]; then \
		echo "${PICKY} ${SH_PICKY_OPTIONS} -- ${SH_FILES}"; \
		${PICKY} ${SH_PICKY_OPTIONS} -- ${SH_FILES}; \
		EXIT_CODE="$$?"; \
		if [[ $$EXIT_CODE -ne 0 ]]; then \
		    echo "make $@: ERROR: CODE[2]: $$EXIT_CODE" 1>&2; \
		    exit 5; \
		fi; \
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
	${Q} if ! ${IS_AVAILABLE} ${CHECKNR} >/dev/null 2>&1; then \
	    echo 'The ${CHECKNR} command could not be found or is unreliable in your system.' 1>&2; \
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
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${MAN3_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${MAN3_TARGETS} ${MAN3_DIR}

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
	${Q} echo
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
	-${E} ${CTAGS} -w -f ${LOCAL_DIR_TAGS} ${ALL_CSRC} ${ALL_HSRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# for a tags file from all ${LOCAL_DIR_TAGS} in all of the other directories
#
all_tags:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${RM} ${RM_V} -f tags
	${Q} for dir in .; do \
	    if [[ -s $$dir/${LOCAL_DIR_TAGS} ]]; then \
		echo "${SED} -e 's;\t;\t'$${dir}'/;' $${dir}/${LOCAL_DIR_TAGS} >> tags"; \
		${SED} -e 's;\t;\t'$${dir}'/;' "$${dir}/${LOCAL_DIR_TAGS}" >> tags; \
	    fi; \
	done
	${E} LC_ALL="C" LANG="C" ${SORT} tags -o tags
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
	${S} echo "${OUR_NAME}: nothing to do"
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
	${E} ${RM} ${RM_V} -f ${ALL_OBJS} ${ALL_BUILT_SRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

clobber: legacy_clobber clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} ${RM_V} -f ${TARGETS}
	${E} ${RM} ${RM_V} -f ${EXTERN_CLOBBER}
	${E} ${RM} ${RM_V} -f tags ${LOCAL_DIR_TAGS}
	${E} ${RM} ${RM_V} -f Makefile.orig
	${E} ${RM} ${RM_V} -f ${CPATH_MAN_DUPS}
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
	${E} ${RM} -f ${RM_V} ${DEST_LIB}/libcpath.a
	${E} ${RM} -f ${RM_V} ${DEST_INCLUDE}/cpath.h
	${E} ${RM} -f ${RM_V} ${DEST_DIR}/cpath
	${E} ${RM} -f ${RM_V} ${MAN1_DIR}/cpath.1
	${E} ${RM} -f ${RM_V} ${MAN3_DIR}/canon_path.3
	${E} ${RM} -f ${RM_V} ${MAN3_DIR}/path_sanity_error.3
	${E} ${RM} -f ${RM_V} ${MAN3_DIR}/path_sanity_name.3
	${E} ${RM} -f ${RM_V} ${MAN3_DIR}/safe_path_str.3
	${E} ${RM} -f ${RM_V} ${MAN3_DIR}/safe_str.3
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
canon_path.o: ../dbg/c_bool.h ../dbg/c_compat.h ../dbg/dbg.h \
    ../dyn_array/../dbg/c_bool.h ../dyn_array/../dbg/c_compat.h \
    ../dyn_array/../dbg/dbg.h ../dyn_array/dyn_array.h \
    ../pr/../dbg/c_bool.h ../pr/../dbg/c_compat.h ../pr/../dbg/dbg.h \
    ../pr/../dyn_array/dyn_array.h ../pr/pr.h canon_path.c cpath.h
cpath.o: ../dbg/c_bool.h ../dbg/c_compat.h ../dbg/dbg.h \
    ../dyn_array/../dbg/c_bool.h ../dyn_array/../dbg/c_compat.h \
    ../dyn_array/../dbg/dbg.h ../dyn_array/dyn_array.h \
    ../pr/../dbg/c_bool.h ../pr/../dbg/c_compat.h ../pr/../dbg/dbg.h \
    ../pr/../dyn_array/dyn_array.h ../pr/pr.h cpath.c cpath.h
