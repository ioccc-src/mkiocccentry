#!/usr/bin/make
#
# test_jparse - mkiocccentry jparse test tools
#
# "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
#
# This JSON parser was co-developed by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# "Share and Enjoy!"
#	--  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
####


#############
# utilities #
#############

# suggestion: List utility filenames, not paths.
#	      Do not list shell builtin (echo, cd, ...) tools.
#	      Keep the list in alphabetical order.
#
CC= cc
CMP= cmp
CTAGS= ctags
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
OUR_NAME= test_jparse

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
C_SRC= jnum_chk.c jnum_test.c
H_SRC= jnum_chk.h

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=

# all shell scripts
#
SH_FILES= jparse_test.sh jstr_test.sh


######################
# intermediate files #
######################

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= jnum_chk.o jnum_test.o

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

# all source files
#
ALL_CSRC= ${C_SRC} ${LESS_PICKY_CSRC} ${BUILT_C_SRC}
ALL_HSRC= ${H_SRC} ${LESS_PICKY_HSRC} ${BUILT_H_SRC}
ALL_SRC= ${ALL_CSRC} ${ALL_HSRC} ${SH_FILES}


#######################
# install information #
#######################

# where to install
#
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
EXTERN_LIBA=
EXTERN_PROG=

# NOTE: ${EXTERN_CLOBBER} used outside of this directory and removed by make clobber
#
EXTERN_CLOBBER= ${EXTERN_O} ${EXTERN_LIBA} ${EXTERN_PROG}


######################
# target information #
######################

# shell targets to make by all and removed by clobber
#
SH_TARGETS=

# program targets to make by all, installed by install, and removed by clobber
#
PROG_TARGETS= jnum_chk

# what to make by all but NOT to removed by clobber
#
ALL_OTHER_TARGETS= ${SH_TARGETS} extern_everything

# what to make by all, what to install, and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${PROG_TARGETS}


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
	test legacy_clean legacy_clobber \
	configure clean clobber install depend tags \
	test_JSON/info.json/good/info.reference.json \
	test_JSON/auth.json/good/auth.reference.json


####################################
# things to make in this directory #
####################################

jnum_test.o: jnum_test.c
	${CC} ${CFLAGS} -I../.. jnum_test.c -c

jnum_chk.o: jnum_chk.c jnum_chk.h
	${CC} ${CFLAGS} -I../.. jnum_chk.c -c

jnum_chk: jnum_chk.o jnum_test.o ../jparse.a ../../dyn_array/dyn_array.a ../../dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../../dbg/dbg.a: ../../dbg/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../../dbg extern_liba

../../dyn_array/dyn_array.a: ../../dyn_array/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../../dyn_array extern_liba

../jparse.a: ../Makefile
	@${MAKE} ${MAKE_CD_Q} -C .. extern_liba


####################################
# rules for use by other Makefiles #
####################################

test_JSON/info.json/good/info.reference.json:
	@:

test_JSON/auth.json/good/auth.reference.json:
	@:


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

test: jparse_test.sh
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${Q} ./jparse_test.sh -j ../jparse -d test_JSON -s . ./json_teststr.txt
	${V} echo
	${V} echo "${OUR_NAME}: make $@ complete"

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
check_man:
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@echo "${OUR_NAME}: nothing to do"
	@echo
	@echo "${OUR_NAME}: make $@ complete"

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

legacy_clean:
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${V} echo "${OUR_NAME}: nothing to do"
	${V} echo
	${V} echo "${OUR_NAME}: make $@ complete"

legacy_clobber: legacy_clean
	${V} echo
	${V} echo "${OUR_NAME}: make $@ starting"
	${V} echo
	${V} echo "${OUR_NAME}: nothing to do"
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
	${RM} -f ${ALL_OBJS} ${ALL_BUILT_SRC}
	@echo
	@echo "${OUR_NAME}: make $@ complete"

clobber: legacy_clobber clean
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -f ${TARGETS}
	${RM} -f jparse_test.log chkentry_test.log txzchk_test.log
	@echo
	@echo "${OUR_NAME}: make $@ complete"
	${RM} -f tags

install: all
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
jnum_chk.o: ../../dbg/dbg.h ../../dyn_array/../dbg/dbg.h \
    ../../dyn_array/dyn_array.h ../../jparse/../dyn_array/dyn_array.h \
    ../../jparse/json_parse.h ../../jparse/json_util.h ../../jparse/util.h \
    ../util.h jnum_chk.c jnum_chk.h
jnum_test.o: ../../jparse/../dyn_array/../dbg/dbg.h \
    ../../jparse/../dyn_array/dyn_array.h ../../jparse/json_parse.h \
    ../../jparse/util.h jnum_test.c
