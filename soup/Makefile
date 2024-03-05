#!/usr/bin/env make
#
# soup - some delicious IOCCC soup recipes :-)
#
# Copyright (c) 2022-2024 by Landon Curt Noll.  All Rights Reserved.
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
AWK= awk
CC= cc
CHECKNR= checknr
CMP= cmp
CTAGS= ctags
GREP= grep
INDEPEND= independ
INSTALL= install
MAKE= make
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
# TODO - ############################################################################### - TODO #
# TODO - Sometime in 2023 we will will support only c17 so C_STD will become -std=gnu17  - TODO #
# TODO - ############################################################################### - TODO #
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
CFLAGS= ${C_STD} ${C_OPT} ${WARN_FLAGS} ${LDFLAGS}
#CFLAGS= ${C_STD} -O0 -g ${WARN_FLAGS} ${LDFLAGS} -fsanitize=address -fno-omit-frame-pointer


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= chk_sem_info.c chk_sem_auth.c chk_validate.c entry_time.c entry_util.c \
	location_util.c location_main.c sanity.c rule_count.c
H_SRC= chk_validate.h soup.h entry_time.h entry_util.h foo.h limit_ioccc.h location.h \
	sanity.h utf8_posix_map.h version.h iocccsize_err.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC= utf8_posix_map.c foo.c location_tbl.c
LESS_PICKY_HSRC= oebxergfB.h

# all shell scripts
#
SH_FILES= all_sem_ref.sh run_usage.sh vermod.sh \
	reset_tstamp.sh

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES= man/man1/bug_report.1 man/man1/bug_report.sh.1 man/man1/chkentry.1 man/man1/iocccsize.1 \
	man/man1/mkiocccentry.1 man/man1/txzchk.1 man/man1/location.1
MAN3_PAGES=
MAN8_PAGES= man/man8/all_sem_ref.8 man/man8/reset_tstamp.8 man/man8/run_usage.8 man/man8/vermod.8
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# tags for just the files in this directory
#
LOCAL_DIR_TAGS= .local.dir.tags

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= chk_sem_info.o chk_sem_auth.o chk_validate.o entry_time.o entry_util.o \
	location_tbl.o location_util.o sanity.o foo.o utf8_posix_map.o rule_count.o

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
EXTERN_PROG= run_usage.sh vermod.sh reset_tstamp.sh all_sem_ref.sh

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
SH_TARGETS= run_usage.sh vermod.sh reset_tstamp.sh all_sem_ref.sh

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

entry_time.o: entry_time.c
	${CC} ${CFLAGS} entry_time.c -c

entry_util.o: entry_util.c
	${CC} ${CFLAGS} entry_util.c -c

location_tbl.o: location_tbl.c
	${CC} ${CFLAGS} location_tbl.c -c

location_util.o: location_util.c
	${CC} ${CFLAGS} location_util.c -c

location_main.o: location_main.c
	${CC} ${CFLAGS} location_main.c -c

location: location_main.o location_tbl.o location_util.o ../dbg/dbg.a
	${CC} ${CFLAGS} $^ -o $@

foo.o: foo.c
	${CC} ${CFLAGS} foo.c -c

sanity.o: sanity.c
	${CC} ${CFLAGS} sanity.c -c

utf8_posix_map.o: utf8_posix_map.c
	${CC} ${CFLAGS} utf8_posix_map.c -c

soup.a: ${LIB_OBJS}
	${RM} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@

chk_sem_info.c: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/info.json/good/info.reference.json \
		chk.info.head.c chk.info.ptch.c chk.info.tail.c
	${RM} -f $@
	../jparse/jsemcgen.sh -N sem_info -P chk -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/info.json/good/info.reference.json chk.info.head.c \
	    chk.info.ptch.c chk.info.tail.c $@
	${RM} -f .jsemcgen.out.*

chk.info.head.c chk.info.ptch.c chk.info.tail.c:
	@:

chk_sem_info.h: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/info.json/good/info.reference.json \
		chk.info.head.h chk.info.ptch.h chk.info.tail.h
	${RM} -f $@
	../jparse/jsemcgen.sh -N sem_info -P chk -I -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/info.json/good/info.reference.json chk.info.head.h \
	    chk.info.ptch.h chk.info.tail.h $@
	${RM} -f .jsemcgen.out.*

chk_sem_auth.c: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/auth.json/good/auth.reference.json \
		chk.auth.head.c chk.auth.ptch.c chk.auth.tail.c
	${RM} -f $@
	../jparse/jsemcgen.sh -N sem_auth -P chk -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/auth.json/good/auth.reference.json chk.auth.head.c \
	    chk.auth.ptch.c chk.auth.tail.c $@
	${RM} -f .jsemcgen.out.*

chk_sem_auth.h: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/auth.json/good/auth.reference.json \
		chk.auth.head.h chk.auth.ptch.h chk.auth.tail.h
	${RM} -f $@
	../jparse/jsemcgen.sh -N sem_auth -P chk -I -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/auth.json/good/auth.reference.json chk.auth.head.h \
	    chk.auth.ptch.h chk.auth.tail.h $@
	${RM} -f .jsemcgen.out.*

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
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../jparse test_ioccc/test_JSON/info.json/good/info.reference.json

../test_ioccc/test_JSON/auth.json/good/auth.reference.json:
	${Q} ${MAKE} ${MAKE_CD_Q} -C ../jparse test_ioccc/test_JSON/auth.json/good/auth.reference.json


####################################
# rules for use by other Makefiles #
####################################

limit_ioccc.sh: limit_ioccc.h version.h ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h ../jparse/run_bison.sh \
		../jparse/run_flex.sh ../jparse/verge.c
	${Q} ${RM} -f $@
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
all_sem_ref: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/info.json/good ../test_ioccc/test_JSON/auth.json/good all_sem_ref.sh
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
# rule should never be invoked by ../test_ioccc/prep.sh
#
# This rule is run by the repo maintainers only AFTER chk_sem_????.{c,h} files
# are updated by hand.
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
	./reset_tstamp.sh

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
	    echo '    https://github.com/xexyl/picky' 1>&2; \
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
	${Q} if ! type -P ${SHELLCHECK} >/dev/null 2>&1; then \
	    echo 'The ${SHELLCHECK} command could not be found.' 1>&2; \
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
	${Q} ${RM} -f tags
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
	${RM} -f chk_sem_info.c chk_sem_info.h chk_sem_info.o
	${RM} -f chk_sem_auth.c chk_sem_auth.h chk_sem_auth.o

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
	${V} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo "nothing to $@. Try making soup instead."

clean:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${RM} -f ${ALL_OBJS}
	${RM} -f .jsemcgen.out.*
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

clobber: legacy_clobber clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${RM} -f .soup
	${RM} -f tags ${LOCAL_DIR_TAGS}
	${RM} -f .all_ref.*
	${RM} -f ${TARGETS}
	${RM} -rf ref
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
	    ${CC} ${CFLAGS} -MM -I. -DMKIOCCCENTRY_USE ${ALL_CSRC} | \
	      ${SED} -e 's;\.\./jparse/\.\./dyn_array/\.\./dbg/;../dbg/;g' \
	             -e 's;\.\./jparse/\.\./dbg/;../dbg/;g' \
	             -e 's;\.\./jparse/\.\./dyn_array/;../dyn_array/;g' | \
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
chk_sem_auth.o: ../dbg/dbg.h ../dyn_array/dyn_array.h \
    ../jparse/json_parse.h ../jparse/json_sem.h ../jparse/json_util.h \
    ../jparse/util.h chk_sem_auth.c chk_sem_auth.h
chk_sem_info.o: ../dbg/dbg.h ../dyn_array/dyn_array.h \
    ../jparse/json_parse.h ../jparse/json_sem.h ../jparse/json_util.h \
    ../jparse/util.h chk_sem_info.c chk_sem_info.h
chk_validate.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_util.h ../jparse/util.h chk_sem_auth.h chk_sem_info.h \
    chk_validate.c chk_validate.h entry_time.h entry_util.h location.h
entry_time.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_util.h ../jparse/util.h entry_time.c entry_time.h \
    limit_ioccc.h version.h
entry_util.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_util.h ../jparse/util.h entry_time.h entry_util.c \
    entry_util.h limit_ioccc.h location.h version.h
foo.o: ../dbg/dbg.h foo.c foo.h oebxergfB.h
location_main.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_util.h ../jparse/util.h location.h location_main.c
location_tbl.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_util.h ../jparse/util.h location.h location_tbl.c
location_util.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_util.h ../jparse/util.h location.h location_util.c
rule_count.o: ../dbg/dbg.h ../iocccsize.h iocccsize_err.h limit_ioccc.h \
    rule_count.c version.h
sanity.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_util.h ../jparse/util.h chk_sem_auth.h chk_sem_info.h \
    chk_validate.h entry_time.h entry_util.h limit_ioccc.h location.h \
    sanity.c sanity.h soup.h utf8_posix_map.h version.h
utf8_posix_map.o: ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h \
    ../jparse/jparse.tab.h ../jparse/json_parse.h ../jparse/json_sem.h \
    ../jparse/json_util.h ../jparse/util.h limit_ioccc.h utf8_posix_map.c \
    utf8_posix_map.h version.h
