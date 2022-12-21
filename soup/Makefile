#!/usr/bin/make
#
# soup - some delicious IOCCC soup recipes :-)
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
AR= ar
CC= cc
CMP= cmp
MAKE= make
RANLIB= ranlib
RM= rm
SED= sed
SHELL= bash
SLEEP= sleep
TOUCH= touch


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
C_SRC= chk_sem_info.c chk_sem_auth.c chk_validate.c
H_SRC= chk_validate.h soup.h
ALL_SRC= ${C_SRC} ${H_SRC}


######################
# intermediate files #
######################

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= chk_sem_info.o chk_sem_auth.o chk_validate.o

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS=

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC= chk_sem_info.c chk_sem_auth.c
BUILT_H_SRC= chk_sem_info.h chk_sem_auth.h
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
EXTERN_H= chk_sem_info.h chk_sem_auth.h
EXTERN_O=
EXTERN_MAN= ${ALL_MAN_TARGETS}
EXTERN_LIBA= soup.a
EXTERN_PROG=

# NOTE: ${EXTERN_CLOBBER} used outside of this directory and removed by make clobber
#
EXTERN_CLOBBER= ${EXTERN_O} ${EXTERN_LIBA} ${EXTERN_PROG}


######################
# target information #
######################

# man pages
#
MAN1_TARGETS=
MAN3_TARGETS=
MAN8_TARGETS=
ALL_MAN_TARGETS= ${MAN1_TARGETS} ${MAN3_TARGETS} ${MAN8_TARGETS}

# libraries
#
LIBA_TARGETS= soup.a

# include files
#
H_SRC_TARGETS=

# what to make by all but NOT to removed by clobber (because they are not files)
#
ALL_OTHER_TARGETS= extern_everything

# what to make by all and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${LIBA_TARGETS}


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


######################################
# all - default rule - must be first #
######################################

all: ${TARGETS}


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	depend soup eat eating eat_soup eating_soup prep_clean prep_clobber legacy_clean legacy_clobber \
	configure clean clobber install


####################################
# things to make in this directory #
####################################

chk_sem_info.o: chk_sem_info.c chk_sem_info.h Makefile
	${CC} ${CFLAGS} -I.. chk_sem_info.c -c

chk_sem_auth.o: chk_sem_auth.c chk_sem_auth.h Makefile
	${CC} ${CFLAGS} -I.. chk_sem_auth.c -c

chk_validate.o: chk_validate.c Makefile
	${CC} ${CFLAGS} -I.. chk_validate.c -c

soup.a: ${LIB_OBJS}
	${RM} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@

chk_sem_info.c: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../jparse/test_jparse/test_JSON/info.json/good/info.reference.json \
		chk.info.head.c chk.info.ptch.c chk.info.tail.c Makefile
	@${RM} -f $@
	../jparse/jsemcgen.sh -N sem_info -P chk -j ../jparse/jsemtblgen -- \
	    ../jparse/test_jparse/test_JSON/info.json/good/info.reference.json chk.info.head.c \
	    chk.info.ptch.c chk.info.tail.c $@

chk_sem_info.h: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../jparse/test_jparse/test_JSON/info.json/good/info.reference.json \
		chk.info.head.h chk.info.ptch.h chk.info.tail.h Makefile
	@${RM} -f $@
	../jparse/jsemcgen.sh -N sem_info -P chk -I -j ../jparse/jsemtblgen -- \
	    ../jparse/test_jparse/test_JSON/info.json/good/info.reference.json chk.info.head.h \
	    chk.info.ptch.h chk.info.tail.h $@

chk_sem_auth.c: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../jparse/test_jparse/test_JSON/auth.json/good/auth.reference.json \
		chk.auth.head.c chk.auth.ptch.c chk.auth.tail.c Makefile
	@${RM} -f $@
	../jparse/jsemcgen.sh -N sem_auth -P chk -j ../jparse/jsemtblgen -- \
	    ../jparse/test_jparse/test_JSON/auth.json/good/auth.reference.json chk.auth.head.c \
	    chk.auth.ptch.c chk.auth.tail.c $@

chk_sem_auth.h: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../jparse/test_jparse/test_JSON/auth.json/good/auth.reference.json \
		chk.auth.head.h chk.auth.ptch.h chk.auth.tail.h Makefile
	@${RM} -f $@
	../jparse/jsemcgen.sh -N sem_auth -P chk -I -j ../jparse/jsemtblgen -- \
	    ../jparse/test_jparse/test_JSON/auth.json/good/auth.reference.json chk.auth.head.h \
	    chk.auth.ptch.h chk.auth.tail.h $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../jparse/jsemtblgen: ../jparse/Makefile
	${MAKE} -C ../jparse extern_prog

../jparse/jsemcgen.sh: ../jparse/Makefile
	${MAKE} -C ../jparse extern_prog

../jparse/test_jparse/test_JSON/info.json/good/info.reference.json: ../jparse/Makefile
	${MAKE} -C ../jparse test_jparse/test_JSON/info.json/good/info.reference.json

../jparse/test_jparse/test_JSON/auth.json/good/auth.reference.json: ../jparse/Makefile
	${MAKE} -C ../jparse test_jparse/test_JSON/auth.json/good/auth.reference.json


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


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

# Form unpatched semantic tables, without headers and trailers, from the reference info and auth JSON files
#
# rule used by prep.sh
#
all_ref: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../jparse/test_jparse/test_JSON/info.json/good ../jparse/test_jparse/test_JSON/auth.json/good all_ref.sh
	rm -rf ref
	mkdir -p ref
	./all_ref.sh -v 1 -j ../jparse/jsemcgen.sh \
	    chk.info.head.c chk.info.tail.c chk.info.head.h chk.info.tail.h \
	    chk.auth.head.c chk.auth.tail.c chk.auth.head.h chk.auth.tail.h \
	    ../jparse/test_jparse/test_JSON/info.json/good ../jparse/test_jparse/test_JSON/auth.json/good ref

# form chk.????.ptch.{c,h} files
#
# Given a correct set of chk_sem_????.{c,h} files, we form chk.????.ptch.{c,h}
# diff patch relative to the ref/*.reference.json.{c,h} files.
#
# rule should never be invoked by prep.sh
#
# This rule is run by the repo maintainers only AFTER chk_sem_????.{c,h} files
# are updated by hand.
#
all_ref_ptch: ref/info.reference.json.c ref/info.reference.json.h \
	      ref/auth.reference.json.c ref/auth.reference.json.h
	rm -f chk.info.ptch.c
	-diff -u ref/info.reference.json.c chk_sem_info.c > chk.info.ptch.c
	rm -f chk.info.ptch.h
	-diff -u ref/info.reference.json.h chk_sem_info.h > chk.info.ptch.h
	rm -f chk.auth.ptch.c
	-diff -u ref/auth.reference.json.c chk_sem_auth.c > chk.auth.ptch.c
	rm -f chk.auth.ptch.h
	-diff -u ref/auth.reference.json.h chk_sem_auth.h > chk.auth.ptch.h

# Form the chk_sem_????.{c,h} files
#
# rule used by prep.sh
#
mkchk_sem: chk_sem_auth.c chk_sem_auth.h chk_sem_info.c chk_sem_info.h

soup:
	@-if [[ -r .soup ]]; then \
	    echo "Soup already cooked. Don't forget to eat it before it goes cold!"; \
        else \
	    echo "Mixing the ingredients..."; \
	    echo "Cooking the soup..."; \
	    ${TOUCH} .soup; \
	    ${SLEEP} 2; \
	    echo "Soup made. Don't forget to eat it before it goes cold!"; \
	fi

eat eating eat_soup eating_soup: all
	@-if [[ -r .soup ]]; then \
	    echo "That was mighty delicous soup but now you feel bloated! :-("; \
	    ${RM} -f .soup; \
	else \
	    echo "How can you eat soup that you haven't made yet?"; \
	fi

prep_clean: legacy_clean
	@:

prep_clobber: legacy_clobber
	${RM} -rf ref

legacy_clean:
	@:

legacy_clobber:
	@:


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo "nothing to $@. Try making soup instead."

clean:
	${RM} -f chk_sem_auth.o chk_sem_info.o chk_validate.o

clobber: clean
	${RM} -f .soup

install: all
	@echo "nothing to $@. Try eating soup instead."


###############
# make depend #
###############

depend: ./fmt_depend.sh
	@echo
	@echo "soup make depend starting"
	@echo
	@${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE/p' Makefile
	${CC} ${CFLAGS} -MM -I../dbg -I.. ${ALL_SRC} | ./fmt_depend.sh >> Makefile
	@-if ${CMP} -s Makefile.orig Makefile; then \
	    ${RM} -f Makefile.orig; \
	else \
	    echo; echo "Makefile dependencies updated"; echo; echo "Previous version may be found in: Makefile.orig"; \
	fi
	@echo
	@echo "soup make depend completed"

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
chk_sem_info.o: chk_sem_info.c chk_sem_info.h ../jparse/json_sem.h ../jparse/util.h \
	../jparse/../dyn_array/dyn_array.h ../jparse/../dyn_array/../dbg/dbg.h ../jparse/json_parse.h \
	../jparse/json_util.h
chk_sem_auth.o: chk_sem_auth.c chk_sem_auth.h ../jparse/json_sem.h ../jparse/util.h \
	../jparse/../dyn_array/dyn_array.h ../jparse/../dyn_array/../dbg/dbg.h ../jparse/json_parse.h \
	../jparse/json_util.h
chk_validate.o: chk_validate.c chk_validate.h ../entry_util.h ../jparse/jparse.h \
	../jparse/../dbg/dbg.h ../jparse/util.h ../jparse/../dyn_array/dyn_array.h \
	../jparse/../dyn_array/../dbg/dbg.h ../jparse/json_parse.h ../jparse/json_util.h \
	../jparse/json_sem.h ../jparse/jparse.tab.h chk_sem_auth.h chk_sem_info.h ../entry_time.h \
	../location.h ../dbg/dbg.h
chk_validate.o: chk_validate.h ../entry_util.h ../jparse/jparse.h ../jparse/../dbg/dbg.h \
	../jparse/util.h ../jparse/../dyn_array/dyn_array.h ../jparse/../dyn_array/../dbg/dbg.h \
	../jparse/json_parse.h ../jparse/json_util.h ../jparse/json_sem.h ../jparse/jparse.tab.h \
	chk_sem_auth.h chk_sem_info.h
soup.o: soup.h ../entry_util.h ../jparse/jparse.h ../jparse/../dbg/dbg.h ../jparse/util.h \
	../jparse/../dyn_array/dyn_array.h ../jparse/../dyn_array/../dbg/dbg.h ../jparse/json_parse.h \
	../jparse/json_util.h ../jparse/json_sem.h ../jparse/jparse.tab.h chk_sem_auth.h chk_sem_info.h \
	chk_validate.h
