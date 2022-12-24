#!/usr/bin/make
#
# dyn_array - dynamic array facility
#
# Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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
INSTALL= install
RANLIB= ranlib
RM= rm
SED= sed
SHELL= bash


##################
# How to compile #
##################

# C source standards being used
#
# This repo supports c11 and later.
#
# NOTE: The use of -std=gnu11 is because there are a few older systems
#       in late 2021 that do not have compilers that (yet) support gnu17.
#       While there may be even more out of date systems that do not
#       support gnu11, we have to draw the line somewhere.
#
#       --------------------------------------------------
#
#       ^^ the line is above :-)
#
# TODO - ###################################################################### - TODO #
# TODO - In 2023 we will will support only c17 so C_STD will become -std=gnu17  - TODO #
# TODO - ###################################################################### - TODO #
#
C_STD= -std=gnu11
#C_STD= -std=gnu17

# optimization and debug level
#
C_OPT= -O3 -g3
#C_OPT= -O0 -g

# Compiler warnings
#
WARN_FLAGS= -pedantic -Wall -Wextra
#WARN_FLAGS= -pedantic -Wall -Wextra -Werror

# linker options
#
LDFLAGS=

# how to compile
#
# NOTE: If you use ASAN, set this environment var:
#       ASAN_OPTIONS="detect_stack_use_after_return=1"
#
CFLAGS= ${C_STD} ${C_OPT} ${WARN_FLAGS} ${LDFLAGS}
#CFLAGS= ${C_STD} -O0 -g ${WARN_FLAGS} ${LDFLAGS} -fsanitize=address -fno-omit-frame-pointer


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= dyn_array.c dyn_test.c
H_SRC= dyn_array.h dyn_array.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=


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
LIB_OBJS= dyn_array.o

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS= dyn_test.o

# all intermediate files and removed by make clean
#
ALL_OBJS= ${LIB_OBJS} ${OTHER_OBJS}

# all source files
#
ALL_CSRC= ${C_SRC} ${LESS_PICKY_CSRC} ${BUILT_C_SRC}
ALL_HSRC= ${H_SRC} ${LESS_PICKY_HSRC} ${BUILT_H_SRC}
ALL_SRC= ${ALL_CSRC} ${ALL_HSRC}


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
EXTERN_H= dyn_array.h
EXTERN_O= dyn_array.o
EXTERN_MAN= ${ALL_MAN_TARGETS}
EXTERN_LIBA= dyn_array.a
EXTERN_PROG= dyn_test

# NOTE: ${EXTERN_CLOBBER} used outside of this directory and removed by make clobber
#
EXTERN_CLOBBER= ${EXTERN_O} ${EXTERN_LIBA} ${EXTERN_PROG}


######################
# target information #
######################

# man pages
#
MAN1_TARGETS=
MAN3_TARGETS= dyn_array.3
MAN8_TARGETS=
ALL_MAN_TARGETS= ${MAN1_TARGETS} ${MAN3_TARGETS} ${MAN8_TARGETS}

# libraries
#
LIBA_TARGETS= dyn_array.a

# include files
#
H_SRC_TARGETS= dyn_array.h

# what to make by all but NOT to removed by clobber (because they are not files)
#
ALL_OTHER_TARGETS= extern_everything

# what to make by all and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${LIBA_TARGETS} dyn_test


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

all: ${TARGETS} ${ALL_OTHER_TARGETS} Makefile
	@:


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	extern_include extern_objs extern_liba extern_man extern_prog extern_everything dyn_array.3 \
	test check_man tags prep_clean prep_clobber legacy_clean legacy_clobber \
	configure clean clobber install


####################################
# things to make in this directory #
####################################

dyn_array.o: dyn_array.c dyn_array.h
	${CC} ${CFLAGS} dyn_array.c -c

dyn_array.a: ${LIB_OBJS}
	${RM} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@

dyn_test.o: dyn_test.c dyn_array.h
	${CC} ${CFLAGS} dyn_test.c -c

dyn_test: dyn_test.o dyn_array.a ../dbg/dbg.a Makefile
	${CC} ${CFLAGS} dyn_test.o dyn_array.a ../dbg/dbg.a -o dyn_test


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../dbg/dbg.a: ../dbg/Makefile
	@${MAKE} -C ../dbg extern_liba


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

dyn_array.3:
	@:


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

test: check_man dyn_test Makefile
	@-./dyn_test; \
	status="$$?"; \
	if [[ $$status -ne 0 ]]; then \
	    echo 'ERROR: dyn_test failed, error code: $$status'; \
	else \
	    echo "PASSED: dyn_test"; \
	fi

# inspect and verify man pages
#
check_man: ${ALL_MAN_TARGETS} Makefile
	@-HAVE_CHECKNR="`type -P ${CHECKNR}`"; if [[ -z "$$HAVE_CHECKNR" ]]; then \
	    echo 'The checknr command could not be found.' 1>&2; \
	    echo 'The checknr command is required to run the $@ rule.' 1>&2; \
	    echo ''; 1>&2; \
	    echo 'The source code and install instructions for checknr are available from this GitHub repo:' 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/checknr' 1>&2; \
	    echo ''; 1>&2; \
	else \
	    echo "${CHECKNR} -c.BR.SS.BI ${ALL_MAN_TARGETS}"; \
	    ${CHECKNR} -c.BR.SS.BI ${ALL_MAN_TARGETS}; \
	    status="$$?"; \
	    if [[ $$status -ne 0 ]]; then \
		echo 'ERROR: checknr: ${CHECKNR} failed, error code: $$status'; \
		exit 23; \
	    fi; \
	fi

tags: ${ALL_SRC}
	-${CTAGS} ${ALL_SRC} 2>&1 | \
	     ${GREP} -E -v 'Duplicate entry|Second entry ignored'

prep_clean: legacy_clean
	@:

prep_clobber: legacy_clobber
	@:

legacy_clean:
	@:

legacy_clobber:
	@:


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	${RM} -f ${ALL_OBJS}
	${RM} -f ${ALL_BUILT_SRC}

clobber: clean
	${RM} -f ${TARGETS}
	${RM} -f ${EXTERN_CLOBBER}
	${RM} -f tags

install: all
	${INSTALL} -v -d -m 0775 ${DEST_LIB}
#none#	${INSTALL} -v -d -m 0775 ${DEST_DIR}
#none#	${INSTALL} -v -d -m 0775 ${MAN1_DIR}
	${INSTALL} -v -d -m 0775 ${MAN3_DIR}
#none#	${INSTALL} -v -d -m 0775 ${MAN8_DIR}
	${INSTALL} -v -d -m 0775 ${DEST_INCLUDE}
#none#	${INSTALL} -v -m 0444 ${MAN1_TARGETS} ${MAN1_DIR}
	${INSTALL} -v -m 0444 ${MAN3_TARGETS} ${MAN3_DIR}
#none#	${INSTALL} -v -m 0444 ${MAN8_TARGETS} ${MAN8_DIR}
	${INSTALL} -v -m 0444 ${LIBA_TARGETS} ${DEST_LIB}
	${INSTALL} -v -m 0444 ${H_SRC_TARGETS} ${DEST_INCLUDE}


###############
# make depend #
###############

depend: ${ALL_CSRC} ../soup/fmt_depend.sh
	@echo
	@echo "make depend starting"
	@echo
	@${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE/p' Makefile
	${CC} ${CFLAGS} -MM ${ALL_CSRC} | ../soup/fmt_depend.sh >> Makefile
	@-if ${CMP} -s Makefile.orig Makefile; then \
	    ${RM} -f Makefile.orig; \
	else \
	    echo; echo "Makefile dependencies updated"; echo; echo "Previous version may be found in: Makefile.orig"; \
	fi
	@echo
	@echo "make depend completed"

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
dyn_array.o: dyn_array.c dyn_array.h ../dbg/dbg.h
dyn_test.o: dyn_test.c dyn_test.h ../dbg/dbg.h dyn_array.h
