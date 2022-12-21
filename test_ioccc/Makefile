#!/usr/bin/make
#
# test - mkiocccentry test tools
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
####


#############
# utilities #
#############

# suggestion: List utility filenames, not paths.
#	      Do not list shell builtin (echo, cd, ...) tools.
#	      Keep the list in alphabetical order.
#
AWK= awk
BASENAME= basename
CAT= cat
CC= cc
CHECKNR= checknr
CMP= cmp
CP= cp
CTAGS= ctags
CUT= cut
DIFF= diff
FMT= fmt
GIT= git
GREP= grep
HEAD= head
INSTALL= install
MAKE= make
MAN2HTML= man2html
MAN= man
MKTEMP= mktemp
MV= mv
PICKY= picky
RM= rm
RPL= rpl
RSYNC= rsync
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
TEE= tee
TR= tr
TRUE= true


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
# TODO - ###################################################################### - TODO #
# TODO - In 2023 we will will support only c17 so C_STD will become c-std=gnu17 - TODO #
# TODO - ###################################################################### - TODO #
#
C_STD= -std=gnu11
#C_STD= -std=gnu17

# optimization and debug level
#
#COPT= -O3 -g3	# TODO - this will be the production release value - TODO #
COPT= -O0 -g

# Compiler warnings
#
#WARN_FLAGS= -Wall -Wextra	# TODO - this will be the production release value - TODO #
WARN_FLAGS= -Wall -Wextra -Werror


# how to compile
#
# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
CFLAGS= ${C_STD} ${COPT} -pedantic ${WARN_FLAGS} ${LDFLAGS}


# NOTE: If you use ASAN, set this environment var:
#	ASAN_OPTIONS="detect_stack_use_after_return=1"
#
#CFLAGS= ${C_STD} -O0 -g -pedantic ${WARN_FLAGS} ${LDFLAGS} -fsanitize=address -fno-omit-frame-pointer

# NOTE: For valgrind, run with:
#
#	valgrind --leak-check=yes --track-origins=yes --leak-resolution=high --read-var-info=yes \
#           --leak-check=full --show-leak-kinds=all ./mkiocccentry ...
#
# NOTE: Replace mkiocccentry with whichever tool you want to test and the ...
# with the arguments and options you want.


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= utf8_test.c
H_SRC=
ALL_CSRC= ${C_SRC} ${H_SRC}


######################
# intermediate files #
######################

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS=

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS=

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC=
BUILT_H_SRC=
ALL_BUILT_SRC= ${BUILT_C_SRC} ${BUILT_H_SRC}

# all intermediate files and removed by make clean
#
ALL_OBJS= ${LIB_OBJS} ${OTHER_OBJS}


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
EXTERN_MAN= ${ALL_MAN_TARGETS}
EXTERN_LIBA=
EXTERN_PROG=

# NOTE: ${EXTERN_CLOBBER} used outside of this directory and removed by make clobber
#
EXTERN_CLOBBER= ${EXTERN_O} ${EXTERN_LIBA} ${EXTERN_PROG}


######################
# target information #
######################

# where and what to install
#
DESTDIR= /usr/local/bin
TARGETS= utf8_test
SH_TARGETS=


######################################
# all - default rule - must be first #
######################################

all: ${TARGETS}


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	test prep_clean prep_clobber legacy_clean legacy_clobber \
	configure clean clobber install


####################################
# things to make in this directory #
####################################

utf8_test.o: utf8_test.c ../utf8_posix_map.h Makefile
	${CC} ${CFLAGS} -I.. utf8_test.c -c

utf8_test: utf8_test.o ../utf8_posix_map.o ../dbg/dbg.a ../jparse/util.o ../dyn_array/dyn_array.a Makefile
	${CC} ${CFLAGS} -I.. utf8_test.o ../utf8_posix_map.o ../dbg/dbg.a ../jparse/util.o ../dyn_array/dyn_array.a -o utf8_test

ioccc_test.sh: ../dbg/dbg_test ../dyn_array/dyn_test
	@:


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../jparse/util.o: ../Makefile
	${MAKE} -C ../jparse util.o

../utf8_posix_map.h: ../Makefile
	${MAKE} -C .. utf8_posix_map.h

../utf8_posix_map.o: ../Makefile
	${MAKE} -C .. utf8_posix_map.o

../dbg/dbg.a: ../dbg/Makefile
	${MAKE} -C ../dbg dbg.a

../dbg/dbg_test: ../dbg/Makefile
	${MAKE} -C ../dbg dbg_test

../dyn_array/dyn_array.a: ../dyn_array/Makefile
	${MAKE} -C ../dyn_array dyn_array.a

../dyn_array/dyn_test: ../dyn_array/Makefile
	${MAKE} -C ../dyn_array dyn_test


####################################
# rules for use by other Makefiles #
####################################


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

test: all ioccc_test.sh
	 ./ioccc_test.sh

prep_clean: legacy_clean
	@:

prep_clobber: legacy_clobber
	${RM} -rf test_iocccsize test_src test_work

legacy_clean:
	${RM} -f jint.test.o jfloat.test.o

legacy_clobber:
	${RM} -f jparse_test.log jint.test jfloat.test



###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	${RM} -f utf8_test.o

clobber: clean
	${RM} -f ${TARGETS}
	${RM} -f test_ioccc.log chkentry_test.log txzchk_test.log

install: all
	@echo nothing to $@


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
utf8_test.o: utf8_test.c ../utf8_posix_map.h ../jparse/jparse.h ../jparse/../dbg/dbg.h \
	../jparse/util.h ../jparse/../dyn_array/dyn_array.h ../jparse/../dyn_array/../dbg/dbg.h \
	../jparse/json_parse.h ../jparse/json_util.h ../jparse/json_sem.h ../jparse/jparse.tab.h \
	../dbg/dbg.h ../limit_ioccc.h ../version.h
