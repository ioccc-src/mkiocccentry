#!/usr/bin/env make
#
# soup - some delicious IOCCC soup recipes :-)
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
# The recipes in this Makefile :-) and the tools in this directory were
# co-developed in 2021-2025 by Cody Boone Ferguson and Landon Curt Noll:
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
AR= ar
AWK= awk
CC= cc
CHECKNR= checknr
CMP= cmp
CTAGS= ctags
GREP= grep
INDEPEND= independ
INSTALL= install
IS_AVAILABLE= ./is_available.sh
PICKY= picky
RANLIB= ranlib
RM= rm
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
SLEEP= sleep
SORT= sort
TOUCH= touch
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
OUR_NAME= soup

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
WARN_FLAGS= -pedantic -Wall -Wextra -Wno-char-subscripts -Wno-sign-conversion
#WARN_FLAGS= -pedantic -Wall -Wextra -Werror

# special compiler flags
#
C_SPECIAL= -DINTERNAL_INCLUDE

# linker options
#
LDFLAGS=

# how to compile
CFLAGS= ${C_STD} ${C_OPT} ${WARN_FLAGS} ${C_SPECIAL} ${LDFLAGS}
#CFLAGS= ${C_STD} -O0 -g ${WARN_FLAGS} ${C_SPECIAL} ${LDFLAGS} -fsanitize=address -fno-omit-frame-pointer


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= chk_validate.c entry_util.c \
	location_util.c location_main.c sanity.c rule_count.c random_answers.c
H_SRC= chk_validate.h soup.h entry_util.h foo.h limit_ioccc.h location.h \
	sanity.h default_handle.h version.h iocccsize_err.h random_answers.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC= default_handle.c foo.c location_tbl.c chk_sem_info.c chk_sem_info.c chk_sem_auth.c 
LESS_PICKY_HSRC= oebxergfB.h

# all shell scripts
#
SH_FILES= all_sem_ref.sh reset_tstamp.sh not_a_comment.sh

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES= man/man1/bug_report.1 man/man1/chkentry.1 man/man1/iocccsize.1 \
	man/man1/mkiocccentry.1 man/man1/txzchk.1 man/man1/location.1
MAN3_PAGES=
MAN8_PAGES= man/man8/all_sem_ref.8 man/man8/reset_tstamp.8
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# tags for just the files in this directory
#
LOCAL_DIR_TAGS= .local.dir.tags

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= chk_sem_info.o chk_sem_auth.o chk_validate.o entry_util.o \
	location_tbl.o location_util.o sanity.o foo.o default_handle.o rule_count.o \
	random_answers.o

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS= location_main.o location.o

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC= chk_sem_info.c chk_sem_auth.c
BUILT_H_SRC= chk_sem_info.h chk_sem_auth.h
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
EXTERN_H= chk_sem_info.h chk_sem_auth.h
EXTERN_O=
EXTERN_MAN= ${ALL_MAN_TARGETS}
EXTERN_LIBA= soup.a
EXTERN_PROG= reset_tstamp.sh all_sem_ref.sh

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

# libraries
#
LIBA_TARGETS= soup.a

# include files
#
H_SRC_TARGETS=

# shell targets to make by all and removed by clobber
#
SH_TARGETS= reset_tstamp.sh all_sem_ref.sh

# program targets to make by all, installed by install, and removed by clobber
#
PROG_TARGETS= location

# what to make by all but NOT to removed by clobber
#
ALL_OTHER_TARGETS= ${SH_TARGETS} extern_everything ${ALL_MAN_PAGES}

# what to make by all, what to install, and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${LIBA_TARGETS} ${PROG_TARGETS} ${ALL_MAN_BUILT} limit_ioccc.sh


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

all: ${TARGETS}
	@:


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	depend kitchen soup_kitchen soup eat eating eat_soup eating_soup legacy_clean legacy_clobber \
	configure clean clobber install depend tags local_dir_tags all_tags \
	prep build release pull reset_min_timestamp clean_mkchk_sem mkchk_sem install_man \
	limit_ioccc.sh all_sem_ref all_sem_ref_ptch


####################################
# things to make in this directory #
####################################

rule_count.o: rule_count.c
	${CC} ${CFLAGS} -DMKIOCCCENTRY_USE rule_count.c -c

chk_sem_info.o: chk_sem_info.c chk_sem_info.h
	${CC} ${CFLAGS} chk_sem_info.c -c

chk_sem_auth.o: chk_sem_auth.c chk_sem_auth.h
	${CC} ${CFLAGS} chk_sem_auth.c -c

chk_validate.o: chk_validate.c
	${CC} ${CFLAGS} chk_validate.c -c

entry_util.o: entry_util.c
	${CC} ${CFLAGS} entry_util.c -c

location_tbl.o: location_tbl.c
	${CC} ${CFLAGS} location_tbl.c -c

location_util.o: location_util.c
	${CC} ${CFLAGS} location_util.c -c

location_main.o: location_main.c
	${CC} ${CFLAGS} location_main.c -c

location: location_main.o location_tbl.o location_util.o ../jparse/libjparse.a ../dyn_array/libdyn_array.a ../dbg/libdbg.a
	${CC} ${CFLAGS} $^ -o $@

foo.o: foo.c
	${CC} ${CFLAGS} foo.c -c

sanity.o: sanity.c
	${CC} ${CFLAGS} sanity.c -c

default_handle.o: default_handle.c
	${CC} ${CFLAGS} default_handle.c -c

soup.a: ${LIB_OBJS}
	${Q} ${RM} ${RM_V} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@
	${MAKE} limit_ioccc.sh

chk_sem_info.c: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/info.json/good/info.reference.json \
		chk.info.head.c chk.info.ptch.c chk.info.tail.c
	${Q} ${RM} ${RM_V} -f $@
	../jparse/jsemcgen.sh -N sem_info -P chk -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/info.json/good/info.reference.json chk.info.head.c \
	    chk.info.ptch.c chk.info.tail.c $@
	${E} ${RM} ${RM_V} -f .jsemcgen.out.*

chk.info.head.c chk.info.ptch.c chk.info.tail.c:
	@:

chk_sem_info.h: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/info.json/good/info.reference.json \
		chk.info.head.h chk.info.ptch.h chk.info.tail.h
	${Q} ${RM} ${RM_V} -f $@
	../jparse/jsemcgen.sh -N sem_info -P chk -I -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/info.json/good/info.reference.json chk.info.head.h \
	    chk.info.ptch.h chk.info.tail.h $@
	${E} ${RM} ${RM_V} -f .jsemcgen.out.*

chk_sem_auth.c: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/auth.json/good/auth.reference.json \
		chk.auth.head.c chk.auth.ptch.c chk.auth.tail.c
	${Q} ${RM} ${RM_V} -f $@
	../jparse/jsemcgen.sh -N sem_auth -P chk -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/auth.json/good/auth.reference.json chk.auth.head.c \
	    chk.auth.ptch.c chk.auth.tail.c $@
	${Q} ${RM} ${RM_V} -f .jsemcgen.out.*

chk_sem_auth.h: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/auth.json/good/auth.reference.json \
		chk.auth.head.h chk.auth.ptch.h chk.auth.tail.h
	${Q} ${RM} ${RM_V} -f $@
	../jparse/jsemcgen.sh -N sem_auth -P chk -I -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/auth.json/good/auth.reference.json chk.auth.head.h \
	    chk.auth.ptch.h chk.auth.tail.h $@
	${Q} ${RM} ${RM_V} -f .jsemcgen.out.*

kitchen soup_kitchen: kitchen.sh
	-@./kitchen.sh

#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../jparse/jsemtblgen:
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../jparse extern_prog

../jparse/jsemcgen.sh:
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../jparse extern_prog

../test_ioccc/test_JSON/info.json/good/info.reference.json:
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../test_ioccc test_JSON

../test_ioccc/test_JSON/auth.json/good/auth.reference.json:
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../test_ioccc test_JSON


####################################
# rules for use by other Makefiles #
####################################

limit_ioccc.sh:
	${Q} ${RM} ${RM_V} -f $@
	${Q} echo '#!/usr/bin/env bash' > $@
	${Q} echo '#' >> $@
	${Q} echo '# Copies of select limit_ioccc.h and version.h values for shell script use' >> $@
	${Q} echo '#' >> $@
	${Q} ${GREP} -Eh '^export .*_VERSION=' ../jparse/run_bison.sh ../jparse/run_flex.sh >> $@
	${Q} ${GREP} -E '^#define (RULE_|MAX_|UUID_|MIN_|IOCCC_)' limit_ioccc.h | \
	    ${AWK} '{print $$2 "=\"" $$3 "\"" ;}' | ${TR} -d '[a-z]()' | \
	    ${SED} -e 's/"_/"/' -e 's/""/"/g' -e 's/^/export /' >> $@
	${Q} ${GREP} -hE '^#define (.*_VERSION|TIMESTAMP_EPOCH|JSON_PARSING_DIRECTIVE_)' \
		     version.h limit_ioccc.h ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
		     ../jparse/verge.c | \
	    ${GREP} -v 'UUID_VERSION' | \
	    ${SED} -e 's/^#define/export/' -e 's/ "/="/' -e 's/"[	 ].*$$/"/' >> $@
	-${Q} if ${GREP} -q '^#define DIGRAPHS' limit_ioccc.h; then \
	    echo "export DIGRAPHS='yes'"; \
	else \
	    echo "export DIGRAPHS="; \
	fi >> $@
	-${Q} if ${GREP} -q '^#define TRIGRAPHS' limit_ioccc.h; then \
	    echo "export TRIGRAPHS='yes'"; \
	else \
	    echo "export TRIGRAPHS="; \
	fi >> $@

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

test:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${V} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# Form unpatched semantic tables, without headers and trailers, from the reference info and auth JSON files
#
# rule used by ../test_ioccc/prep.sh
#
all_sem_ref: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/info.json/good \
    ../test_ioccc/test_JSON/auth.json/good all_sem_ref.sh
	${E} rm -rf ref
	${E} mkdir -p ref
	${E} ./all_sem_ref.sh -v 1 -j ../jparse/jsemtblgen -J ../jparse/jsemcgen.sh -- \
	    chk.info.head.c chk.info.tail.c chk.info.head.h chk.info.tail.h \
	    chk.auth.head.c chk.auth.tail.c chk.auth.head.h chk.auth.tail.h \
	    ../test_ioccc/test_JSON/info.json/good \
	    ../test_ioccc/test_JSON/auth.json/good ref

# form chk.????.ptch.{c,h} files
#
# Given a correct set of chk_sem_????.{c,h} files, we form chk.????.ptch.{c,h}
# diff patch relative to the ref/*.reference.json.{c,h} files.
#
# This rule should never be invoked by ../test_ioccc/prep.sh.
#
# This rule is run by the repo maintainers only AFTER chk_sem_????.{c,h} files
# are updated by hand.
# 
# Last, run "make all_sem_ref" before running "make all_sem_ref_ptch".
#
all_sem_ref_ptch: ref/info.reference.json.c ref/info.reference.json.h \
	      ref/auth.reference.json.c ref/auth.reference.json.h
	${E} rm -f chk.info.ptch.c
	-${E} diff -u ref/info.reference.json.c chk_sem_info.c > chk.info.ptch.c
	${E} rm -f chk.info.ptch.h
	-${E} diff -u ref/info.reference.json.h chk_sem_info.h > chk.info.ptch.h
	${E} rm -f chk.auth.ptch.c
	-${E} diff -u ref/auth.reference.json.c chk_sem_auth.c > chk.auth.ptch.c
	${E} rm -f chk.auth.ptch.h
	-${E} diff -u ref/auth.reference.json.h chk_sem_auth.h > chk.auth.ptch.h

# Form the chk_sem_????.{c,h} files
#
# rule used by ../test_ioccc/prep.sh
#
mkchk_sem: chk_sem_info.c chk_sem_info.h chk_sem_auth.c chk_sem_auth.h
	@:

soup:
	-${Q} if [[ -r .soup ]]; then \
	    echo "As you add more ingredients to the pot, the soup overflows, burns" ; \
	    echo "you and falls to the floor! Next time try eating the soup BEFORE you"; \
	    echo "cook more in the SAME pot!" ; \
	    ${RM} -f .soup; \
        else \
	    echo "Mixing the ingredients..."; \
	    echo "Cooking the soup..."; \
	    ${TOUCH} .soup; \
	    ${SLEEP} 2; \
	    echo "Soup made. Don't forget to eat it before it goes cold!"; \
	fi

eat eating eat_soup eating_soup: all
	-${Q} if [[ -r .soup ]]; then \
	    echo "That was mighty delicous soup but now you feel bloated! :-("; \
	    ${SLEEP} 2; \
	    ${RM} -f .soup; \
	else \
	    echo "How can you eat soup that you haven't made yet?"; \
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
	cd ..; ./soup/reset_tstamp.sh

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
	    echo ''; 1>&2; \
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
	${Q} for dir in ../dbg ../dyn_alloc ../jparse ../jpaese/test_jparse; do \
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
	${Q} for dir in . ../dbg ../dyn_alloc ../jparse ../jparse/test_jparse; do \
	    if [[ -s $$dir/${LOCAL_DIR_TAGS} ]]; then \
		echo "${SED} -e 's;\t;\t'$${dir}'/;' $${dir}/${LOCAL_DIR_TAGS} >> tags"; \
		${SED} -e 's;\t;\t'$${dir}'/;' "$${dir}/${LOCAL_DIR_TAGS}" >> tags; \
	    fi; \
	done
	${E} ${SORT} tags -o tags
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

clean_mkchk_sem:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} ${RM_V} -f chk_sem_info.c chk_sem_info.h chk_sem_info.o
	${E} ${RM} ${RM_V} -f chk_sem_auth.c chk_sem_auth.h chk_sem_auth.o
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
	@echo "nothing to $@. Try making soup instead."

clean: legacy_clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} ${RM_V} -f ${ALL_OBJS}
	${E} ${RM} ${RM_V} -f .jsemcgen.out.*
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

clobber: legacy_clobber clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} ${RM_V} -f .soup
	${E} ${RM} ${RM_V} -f tags ${LOCAL_DIR_TAGS}
	${E} ${RM} ${RM_V} -f .all_ref.*
	${E} ${RM} ${RM_V} -f .jsemcgen.out.*
	${E} ${RM} ${RM_V} -f Makefile.orig
	${E} ${RM} ${RM_V} -f ${TARGETS}
	${E} ${RM} ${RM_V} -rf ref
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

install: all install_man
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_LIB}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${LIBA_TARGETS} ${DEST_LIB}
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

uninstall:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${RM} ${RM_V} -f ${DEST_LIB}/soup.a
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/vermod.sh
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/reset_tstamp.sh
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/all_sem_ref.sh
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/location
	${E} ${RM} ${RM_V} -f ${DEST_DIR}/run_usage.sh
	${E} ${RM} ${RM_V} -f ${MAN1_DIR}/bug_report.1
	${E} ${RM} ${RM_V} -f ${MAN1_DIR}/bug_report.sh.1
	${E} ${RM} ${RM_V} -f ${MAN1_DIR}/chkentry.1
	${E} ${RM} ${RM_V} -f ${MAN1_DIR}/iocccsize.1
	${E} ${RM} ${RM_V} -f ${MAN1_DIR}/mkiocccentry.1
	${E} ${RM} ${RM_V} -f ${MAN1_DIR}/txzchk.1
	${E} ${RM} ${RM_V} -f ${MAN1_DIR}/location.1
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/all_sem_ref.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/reset_tstamp.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/run_usage.8
	${E} ${RM} ${RM_V} -f ${MAN8_DIR}/vermod.8
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
	      ${SED} -e 's;\.\./jparse/\.\./dyn_array/\.\./dbg/;../dbg/;g' \
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
chk_sem_auth.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    chk_sem_auth.c chk_sem_auth.h
chk_sem_info.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    chk_sem_info.c chk_sem_info.h
chk_validate.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/version.h chk_sem_auth.h chk_sem_info.h chk_validate.c \
    chk_validate.h entry_util.h location.h
default_handle.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    default_handle.c default_handle.h limit_ioccc.h version.h
entry_util.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/verge.h ../jparse/version.h entry_util.c entry_util.h \
    limit_ioccc.h location.h version.h
foo.o: ../dbg/dbg.h foo.c foo.h oebxergfB.h
location_main.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/version.h location.h location_main.c
location_tbl.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/version.h location.h location_tbl.c
location_util.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/version.h location.h location_util.c
random_answers.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/version.h default_handle.h limit_ioccc.h location.h \
    random_answers.c random_answers.h version.h
rule_count.o: ../dbg/dbg.h ../iocccsize.h iocccsize_err.h limit_ioccc.h \
    rule_count.c version.h
sanity.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_utf8.h ../jparse/json_util.h ../jparse/util.h \
    ../jparse/version.h chk_sem_auth.h chk_sem_info.h chk_validate.h \
    default_handle.h entry_util.h limit_ioccc.h location.h sanity.c \
    sanity.h soup.h version.h
