#!/usr/bin/make
#
# test_ioccc - mkiocccentry test tools
#
# Copyright (c) 2022-2023 by Landon Curt Noll.  All Rights Reserved.
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
CHECKNR= checknr
CC= cc
CMP= cmp
GREP= grep
INDEPEND= independ
INSTALL= install
MAKE= make
PICKY= picky
RM= rm
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck


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

# Q= @   				do not echo internal Makefile actions (quiet mode)
# Q=    				echo internal Makefile actions (debug / verbose mode)
#
#Q=
Q= @

# Q= implies -v 0
# else -v 1
#
ifeq ($(strip ${Q}),@)
Q_V_OPTION="0"
else
Q_V_OPTION="1"
endif

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
# TODO - ############################################################################### - TODO #
# TODO - Sometime in 2023 we will will support only c17 so C_STD will become -std=gnu17  - TODO #
# TODO - ############################################################################### - TODO #
#
C_STD= -std=gnu11
#C_STD= -std=gnu17

# optimization and debug level
#
#C_OPT= -O3 -g3	# TODO - this will be the production release value - TODO #
C_OPT= -O0 -g

# Compiler warnings
#
#WARN_FLAGS= -Wall -Wextra	# TODO - this will be the production release value - TODO #
WARN_FLAGS= -Wall -Wextra -Werror

# how to compile
#
# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
CFLAGS= ${C_STD} ${C_OPT} -pedantic ${WARN_FLAGS} ${LDFLAGS}

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
C_SRC= utf8_test.c fnamchk.c
H_SRC= fnamchk.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=

# all shell scripts
#
SH_FILES= chkentry_test.sh ioccc_test.sh iocccsize_test.sh mkiocccentry_test.sh txzchk_test.sh \
	prep.sh

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES= man/man1/fnamchk.1
MAN3_PAGES=
MAN8_PAGES= man/man8/all_sem_ref.8 man/man8/chkentry_test.8 man/man8/ioccc_test.8 \
	man/man8/iocccsize_test.8 man/man8/mkiocccentry_test.8 man/man8/txzchk_test.8
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS=

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS= fnamchk.o

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC=
BUILT_H_SRC=
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

TXZCHK_LOG= txzchk_test.log


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
EXTERN_PROG= prep.sh fnamchk

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
SH_TARGETS= prep.sh

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

all: ${TARGETS} ${EXTERN_PROG}
	@:


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	test legacy_clean legacy_clobber install_man \
	configure clean clobber install depend


####################################
# things to make in this directory #
####################################

utf8_test.o: utf8_test.c
	${CC} ${CFLAGS} -I.. utf8_test.c -c

utf8_test: utf8_test.o ../soup/soup.a ../jparse/jparse.a ../dyn_array/dyn_array.a ../dbg/dbg.a
	${CC} ${CFLAGS} -I.. $^ -o $@

ioccc_test.sh: ../dbg/dbg_test ../dyn_array/dyn_test
	@:

fnamchk.o: fnamchk.c fnamchk.h
	${CC} ${CFLAGS} fnamchk.c -c

fnamchk: fnamchk.o ../jparse/jparse.a ../dyn_array/dyn_array.a ../dbg/dbg.a
	${CC} ${CFLAGS} $^ -o $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../dbg/dbg.a: ../dbg/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../dbg extern_liba

../dbg/dbg_test: ../dbg/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../dbg extern_prog

../dyn_array/dyn_array.a: ../dyn_array/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../dyn_array extern_liba

../dyn_array/dyn_test: ../dyn_array/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../dyn_array dyn_test

../soup/soup.a: ../soup/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../soup extern_liba


####################################
# rules for use by other Makefiles #
####################################


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

test: all ioccc_test.sh
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	 ./ioccc_test.sh
	@echo
	@echo "${OUR_NAME}: make $@ complete"

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
	@echo "${OUR_NAME}: nothing to do"
	@echo
	@echo "${OUR_NAME}: make $@ complete"

legacy_clean:
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${Q} ${RM} -f jint.test.o jfloat.test.o
	${V} echo
	${V} echo "${OUR_NAME}: make $@ complete"

legacy_clobber: legacy_clean
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${Q} ${RM} -f jparse_test.log jint.test jfloat.test
	${V} echo
	${V} echo "${OUR_NAME}: make $@ complete"



###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -f utf8_test.o
	@echo
	@echo "${OUR_NAME}: make $@ complete"

clobber: clean
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -f ${TARGETS}
	${RM} -f test_ioccc.log chkentry_test.log txzchk_test.log
	@echo
	@echo "${OUR_NAME}: make $@ complete"

install: all install_man
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	@echo
	@echo "${OUR_NAME}: make $@ complete"


###############
# make depend #
###############

depend: ${ALL_CSRC}
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${Q} HAVE_INDEPEND="`type -P ${INDEPEND}`"; if [[ -z "$$HAVE_INDEPEND" ]]; then \
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
	${V} echo
	${V} echo "${OUR_NAME}: make $@ complete"

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
fnamchk.o: ../dbg/dbg.h ../jparse/../dbg/dbg.h \
    ../jparse/../dyn_array/../dbg/dbg.h ../jparse/../dyn_array/dyn_array.h \
    ../jparse/jparse.h ../jparse/jparse.tab.h ../jparse/json_parse.h \
    ../jparse/json_sem.h ../jparse/json_util.h ../jparse/util.h \
    ../soup/../dbg/dbg.h ../soup/../jparse/jparse.h ../soup/limit_ioccc.h \
    ../soup/utf8_posix_map.h ../soup/version.h fnamchk.c fnamchk.h
utf8_test.o: ../soup/../dbg/dbg.h ../soup/../jparse/../dbg/dbg.h \
    ../soup/../jparse/../dyn_array/../dbg/dbg.h \
    ../soup/../jparse/../dyn_array/dyn_array.h ../soup/../jparse/jparse.h \
    ../soup/../jparse/jparse.tab.h ../soup/../jparse/json_parse.h \
    ../soup/../jparse/json_sem.h ../soup/../jparse/json_util.h \
    ../soup/../jparse/util.h ../soup/limit_ioccc.h ../soup/utf8_posix_map.h \
    ../soup/version.h utf8_test.c
