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
TOUCH= touch


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

# Q= @   				do not echo internal Makefile actions (quiet mode)
# Q=    				echo internal Makefile actions (debug / verbose mode)
#
#Q=
Q= @

# V= @:  				do not echo debug statements (quiet mode)
# V= @   				echo debug statements (debug / verbose mode)
#
V= @:
#V= @

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
# MAKE_CD_Q= 				silence make cd messages (quiet mode)
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
C_SRC= chk_sem_info.c chk_sem_auth.c chk_validate.c entry_time.c entry_util.c \
	location.c sanity.c rule_count.c
H_SRC= chk_validate.h soup.h entry_time.h entry_util.h foo.h limit_ioccc.h location.h \
	sanity.h utf8_posix_map.h version.h iocccsize_err.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC= utf8_posix_map.c foo.c
LESS_PICKY_HSRC= oebxergfB.h

# all shell scripts
#
SH_FILES= all_sem_ref.sh hostchk.sh run_usage.sh vermod.sh \
	reset_tstamp.sh

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES= man/man1/bug_report.1 man/man1/chkentry.1 man/man1/hostchk.1 man/man1/iocccsize.1 \
	man/man1/mkiocccentry.1 man/man1/txzchk.1
MAN3_PAGES=
MAN8_PAGES= man/man8/reset_tstamp.8 man/man8/run_usage.8 man/man8/vermod.8
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= chk_sem_info.o chk_sem_auth.o chk_validate.o entry_time.o entry_util.o \
	location.o sanity.o foo.o utf8_posix_map.o rule_count.o

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
EXTERN_PROG= hostchk.sh run_usage.sh vermod.sh reset_tstamp.sh all_sem_ref.sh

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
SH_TARGETS= hostchk.sh run_usage.sh vermod.sh reset_tstamp.sh all_sem_ref.sh

# program targets to make by all, installed by install, and removed by clobber
#
PROG_TARGETS=

# what to make by all but NOT to removed by clobber
#
ALL_OTHER_TARGETS= ${SH_TARGETS} extern_everything ${ALL_MAN_PAGES}

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


######################################
# all - default rule - must be first #
######################################

all: ${TARGETS}
	@:


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	depend soup eat eating eat_soup eating_soup prep_clean prep_clobber legacy_clean legacy_clobber \
	configure clean clobber install depend \
	prep build release pull reset_min_timestamp clean_mkchk_sem mkchk_sem install_man \
	hostchk.sh limit_ioccc.sh all_sem_ref all_sem_ref_ptch


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

location.o: location.c
	${CC} ${CFLAGS} location.c -c

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

chk.info.head.c chk.info.ptch.c chk.info.tail.c:
	@:

chk_sem_info.h: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/info.json/good/info.reference.json \
		chk.info.head.h chk.info.ptch.h chk.info.tail.h
	${RM} -f $@
	../jparse/jsemcgen.sh -N sem_info -P chk -I -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/info.json/good/info.reference.json chk.info.head.h \
	    chk.info.ptch.h chk.info.tail.h $@

chk_sem_auth.c: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/auth.json/good/auth.reference.json \
		chk.auth.head.c chk.auth.ptch.c chk.auth.tail.c
	${RM} -f $@
	../jparse/jsemcgen.sh -N sem_auth -P chk -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/auth.json/good/auth.reference.json chk.auth.head.c \
	    chk.auth.ptch.c chk.auth.tail.c $@

chk_sem_auth.h: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/auth.json/good/auth.reference.json \
		chk.auth.head.h chk.auth.ptch.h chk.auth.tail.h
	${RM} -f $@
	../jparse/jsemcgen.sh -N sem_auth -P chk -I -j ../jparse/jsemtblgen -- \
	    ../test_ioccc/test_JSON/auth.json/good/auth.reference.json chk.auth.head.h \
	    chk.auth.ptch.h chk.auth.tail.h $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../jparse/jsemtblgen:
	@${MAKE} ${MAKE_CD_Q} -C ../jparse extern_prog

../jparse/jsemcgen.sh:
	@${MAKE} ${MAKE_CD_Q} -C ../jparse extern_prog

../test_ioccc/test_JSON/info.json/good/info.reference.json:
	@${MAKE} ${MAKE_CD_Q} -C ../jparse test_ioccc/test_JSON/info.json/good/info.reference.json

../test_ioccc/test_JSON/auth.json/good/auth.reference.json:
	@${MAKE} ${MAKE_CD_Q} -C ../jparse test_ioccc/test_JSON/auth.json/good/auth.reference.json

####################################
# rules for use by other Makefiles #
####################################

limit_ioccc.sh: limit_ioccc.h version.h ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h
	${RM} -f $@
	@echo '#!/usr/bin/env bash' > $@
	@echo '#' >> $@
	@echo '# Copies of select limit_ioccc.h and version.h values for shell script use' >> $@
	@echo '#' >> $@
	${GREP} -E '^#define (RULE_|MAX_|UUID_|MIN_|IOCCC_)' limit_ioccc.h | \
	    ${AWK} '{print $$2 "=\"" $$3 "\"" ;}' | ${TR} -d '[a-z]()' | \
	    ${SED} -e 's/"_/"/' -e 's/""/"/g' -e 's/^/export /' >> $@
	${GREP} -hE '^#define (.*_VERSION|TIMESTAMP_EPOCH|JSON_PARSING_DIRECTIVE_)' \
		     version.h limit_ioccc.h ../dbg/dbg.h ../dyn_array/dyn_array.h ../jparse/jparse.h | \
	    ${GREP} -v 'UUID_VERSION' | \
	    ${SED} -e 's/^#define/export/' -e 's/ "/="/' -e 's/"[	 ].*$$/"/' >> $@
	-if ${GREP} -q '^#define DIGRAPHS' limit_ioccc.h; then \
	    echo "export DIGRAPHS='yes'"; \
	else \
	    echo "export DIGRAPHS="; \
	fi >> $@
	-if ${GREP} -q '^#define TRIGRAPHS' limit_ioccc.h; then \
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

test: all
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@echo "${OUR_NAME}: nothing to do"
	@echo
	@echo "${OUR_NAME}: make $@ complete"

# Form unpatched semantic tables, without headers and trailers, from the reference info and auth JSON files
#
# rule used by ../test_ioccc/prep.sh
#
all_sem_ref: ../jparse/jsemtblgen ../jparse/jsemcgen.sh ../test_ioccc/test_JSON/info.json/good ../test_ioccc/test_JSON/auth.json/good all_sem_ref.sh
	rm -rf ref
	mkdir -p ref
	./all_sem_ref.sh -v 1 -j ../jparse/jsemtblgen -J ../jparse/jsemcgen.sh -- \
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
# rule used by ../test_ioccc/prep.sh
#
mkchk_sem: chk_sem_info.c chk_sem_info.h chk_sem_auth.c chk_sem_auth.h
	@:

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
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@HAVE_SEQCEXIT="`type -P ${SEQCEXIT}`"; if [[ -z "$$HAVE_SEQCEXIT" ]]; then \
	    echo 'The seqcexit tool could not be found.' 1>&2; \
	    echo 'The seqcexit tool is required for this rule.'; 1>&2; \
	    echo ''; 1>&2; \
	    echo 'See the following GitHub repo for seqcexit:'; 1>&2; \
	    echo ''; 1>&2; \
	    echo '    https://github.com/lcn2/seqcexit'; 1>&2; \
	    echo ''; 1>&2; \
	    exit 1; \
	else \
	    echo "${SEQCEXIT} -D werr_sem_val -D werrp_sem_val -- ${ALL_CSRC}"; \
	    ${SEQCEXIT} -D werr_sem_val -D werrp_sem_val -- ${ALL_CSRC}; \
	fi
	@echo
	@echo "${OUR_NAME}: make $@ complete"

picky: ${ALL_SRC}
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@if ! type -P ${PICKY} >/dev/null 2>&1; then \
	    echo "The picky tool could not be found." 1>&2; \
	    echo "The picky tool is required for this rule." 1>&2; \
	    echo "We recommend you install picky v2.6 or later" 1>&2; \
	    echo "from this URL:" 1>&2; \
	    echo 1>&2; \
	    echo "http://grail.eecs.csuohio.edu/~somos/picky.html" 1>&2; \
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
	@echo
	@echo "${OUR_NAME}: make $@ complete"

# inspect and verify shell scripts
#
shellcheck: ${SH_FILES} .shellcheckrc
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@HAVE_SHELLCHECK="`type -P ${SHELLCHECK}`"; if [[ -z "$$HAVE_SHELLCHECK" ]]; then \
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
	@echo
	@echo "${OUR_NAME}: make $@ complete"

# inspect and verify man pages
#
check_man: ${ALL_MAN_TARGETS}
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
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
	    EXIT_CODE="$$?"; \
	    if [[ $$EXIT_CODE -ne 0 ]]; then \
		echo 'ERROR: checknr: ${CHECKNR} failed, error code: $$EXIT_CODE'; \
		exit 23; \
	    fi; \
	fi
	@echo
	@echo "${OUR_NAME}: make $@ complete"

# install man pages
#
install_man: ${ALL_MAN_TARGETS}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${MAN1_DIR}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0444 ${MAN1_TARGETS} ${MAN1_DIR}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${MAN8_DIR}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0444 ${MAN8_TARGETS} ${MAN8_DIR}

# vi/vim tags
#
tags: ${ALL_CSRC} ${ALL_HSRC}
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	-${CTAGS} ${ALL_CSRC} ${ALL_HSRC} 2>&1 | \
	     ${GREP} -E -v 'Duplicate entry|Second entry ignored'
	@echo
	@echo "${OUR_NAME}: make $@ complete"

clean_mkchk_sem:
	${RM} -f chk_sem_info.c chk_sem_info.h chk_sem_info.o
	${RM} -f chk_sem_auth.c chk_sem_auth.h chk_sem_auth.o

prep_clean: legacy_clean
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@echo "${OUR_NAME}: nothing to do"
	@echo
	@echo "${OUR_NAME}: make $@ complete"

prep_clobber: prep_clean legacy_clobber
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -rf ref
	@echo
	@echo "${OUR_NAME}: make $@ complete"

legacy_clean:
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@echo "${OUR_NAME}: nothing to do"
	@echo
	@echo "${OUR_NAME}: make $@ complete"

legacy_clobber: legacy_clean
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@echo "${OUR_NAME}: nothing to do"
	@echo
	@echo "${OUR_NAME}: make $@ complete"


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo "nothing to $@. Try making soup instead."

clean:
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -f chk_sem_auth.o chk_sem_info.o chk_validate.o
	@echo
	@echo "${OUR_NAME}: make $@ complete"

clobber: clean
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -f .soup
	${RM} -f tags
	${RM} -f .all_ref.*
	${RM} -f ${TARGETS}
	@echo
	@echo "${OUR_NAME}: make $@ complete"

install: all install_man
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_LIB}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0444 ${LIBA_TARGETS} ${DEST_LIB}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	@echo
	@echo "${OUR_NAME}: make $@ complete"


###############
# make depend #
###############

depend: ${ALL_CSRC}
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@HAVE_INDEPEND="`type -P ${INDEPEND}`"; if [[ -z "$$HAVE_INDEPEND" ]]; then \
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
	@echo
	@echo "${OUR_NAME}: make $@ complete"

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
chk_sem_auth.o: ../jparse/../dyn_array/../dbg/dbg.h \
    ../jparse/../dyn_array/dyn_array.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h \
    chk_sem_auth.c chk_sem_auth.h
chk_sem_info.o: ../jparse/../dyn_array/../dbg/dbg.h \
    ../jparse/../dyn_array/dyn_array.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h \
    chk_sem_info.c chk_sem_info.h
chk_validate.o: ../dbg/dbg.h ../jparse/../dbg/dbg.h \
    ../jparse/../dyn_array/../dbg/dbg.h ../jparse/../dyn_array/dyn_array.h \
    ../jparse/jparse.h ../jparse/jparse.tab.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h \
    chk_sem_auth.h chk_sem_info.h chk_validate.c chk_validate.h \
    entry_time.h entry_util.h location.h
entry_time.o: ../dbg/dbg.h ../jparse/../dbg/dbg.h \
    ../jparse/../dyn_array/../dbg/dbg.h ../jparse/../dyn_array/dyn_array.h \
    ../jparse/jparse.h ../jparse/jparse.tab.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h \
    entry_time.c entry_time.h limit_ioccc.h version.h
entry_util.o: ../dbg/dbg.h ../jparse/../dbg/dbg.h \
    ../jparse/../dyn_array/../dbg/dbg.h ../jparse/../dyn_array/dyn_array.h \
    ../jparse/jparse.h ../jparse/jparse.tab.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h \
    entry_time.h entry_util.c entry_util.h limit_ioccc.h location.h \
    version.h
foo.o: ../dbg/dbg.h foo.c foo.h oebxergfB.h
location.o: ../dbg/dbg.h ../jparse/../dbg/dbg.h \
    ../jparse/../dyn_array/../dbg/dbg.h ../jparse/../dyn_array/dyn_array.h \
    ../jparse/jparse.h ../jparse/jparse.tab.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h location.c \
    location.h
rule_count.o: ../dbg/dbg.h ../iocccsize.h iocccsize_err.h limit_ioccc.h \
    rule_count.c version.h
sanity.o: ../dbg/dbg.h ../jparse/../dbg/dbg.h \
    ../jparse/../dyn_array/../dbg/dbg.h ../jparse/../dyn_array/dyn_array.h \
    ../jparse/jparse.h ../jparse/jparse.tab.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h \
    limit_ioccc.h location.h sanity.c sanity.h utf8_posix_map.h version.h
utf8_posix_map.o: ../dbg/dbg.h ../jparse/../dbg/dbg.h \
    ../jparse/../dyn_array/../dbg/dbg.h ../jparse/../dyn_array/dyn_array.h \
    ../jparse/jparse.h ../jparse/jparse.tab.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h \
    limit_ioccc.h utf8_posix_map.c utf8_posix_map.h version.h
