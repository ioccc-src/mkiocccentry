#!/usr/bin/env make
#
# jparse - a JSON parser written in C
#
#  This JSON parser was co-developed in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#  and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
#  "Because sometimes even the IOCCC Judges need some help." :-)
#
#  "Share and Enjoy!"
#      --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)

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
PICKY= picky
RANLIB= ranlib
RM= rm
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
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
OUR_NAME= jparse

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
# INSTALL_V= -v				install with -v (debug / verbose mode)
#
#INSTALL_V=
INSTALL_V= -v

# RM_V=					rm w/o -v flag (quiet mode)
# RM_V= -v				rm with -v (debug / verbose mode)
#
#RM_V=
RM_V= -v



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
C_OPT= -O3 -g3
#C_OPT= -O0 -g

# Compiler warnings
#
WARN_FLAGS= -pedantic -Wall -Wextra -Wno-unused-but-set-variable -Wno-char-subscripts
#WARN_FLAGS= -pedantic -Wall -Wextra -Werror

# special compiler flags
#
C_SPECIAL=

# linker options
#
LDFLAGS=

# how to compile
CFLAGS= ${C_STD} ${C_OPT} ${WARN_FLAGS} ${C_SPECIAL} ${LDFLAGS}
#CFLAGS= -O3 -g3 -pedantic -Wall -Werror ${C_SPECIAL}


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= jparse_main.c json_parse.c json_sem.c json_util.c \
       jsemtblgen.c jstrdecode.c jstrencode.c util.c verge.c
H_SRC= jparse.h jparse_main.h jsemtblgen.h json_parse.h json_sem.h json_util.h \
       jstrdecode.h jstrencode.h sorry.tm.ca.h util.h verge.h jparse.tab.ref.h
# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC= jparse.ref.c jparse.tab.ref.c
LESS_PICKY_HSRC= jparse.lex.ref.h

# bison and flex
#
FLEXFILES= jparse.l
BISONFILES= jparse.y

# all shell scripts
#
SH_FILES= jsemcgen.sh run_bison.sh run_flex.sh

# all man pages that NOT built and NOT removed by make clobber
#
MAN1_PAGES= man/man1/jparse.1 man/man1/jstrdecode.1 man/man1/jstrencode.1
MAN3_PAGES= man/man3/jparse.3 man/man3/json_dbg.3 man/man3/json_dbg_allowed.3 \
	    man/man3/json_err_allowed.3 man/man3/json_warn_allowed.3 man/man3/parse_json.3 \
	    man/man3/parse_json_file.3 man/man3/parse_json_stream.3
MAN8_PAGES= man/man8/jnum_chk.8 man/man8/jnum_gen.8 man/man8/jparse_test.8 man/man8/jsemcgen.8 \
	man/man8/jsemtblgen.8 man/man8/jstr_test.8 man/man8/verge.8 \
	man/man8/run_bison.8 man/man8/run_bison.sh.8 man/man8/run_flex.8 man/man8/run_flex.sh.8 \
	man/man8/jsemcgen.sh.8
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# tags for just the files in this directory
#
LOCAL_DIR_TAGS= .local.dir.tags

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC= jparse.tab.c jparse.c
BUILT_H_SRC= jparse.tab.h jparse.lex.h
ALL_BUILT_SRC= ${BUILT_C_SRC} ${BUILT_H_SRC}

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= jparse.o jparse.tab.o json_parse.o json_sem.o json_util.o util.o

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and ARE removed by make clean
#
OTHER_OBJS= verge.o jsemtblgen.o jstrdecode.o jstrencode.o jparse_main.o

# all intermediate files which are also removed by make clean
#
ALL_OBJS= ${LIB_OBJS} ${OTHER_OBJS}

# rule used by prep.sh and make clean via make clean_generated_obj
#
BUILT_OBJS= jparse.o jparse.tab.o

# RUN_O_FLAG - determine if the bison and flex backup files should be used
#
# RUN_O_FLAG=		use bison and flex backup files,
#			    if bison and/or flex not found or too old
# RUN_O_FLAG= -o	do not use bison and flex backup files,
#			    instead fail if bison and/or flex not found or too old
#
RUN_O_FLAG=
#RUN_O_FLAG= -o

# the basename of bison (or yacc) to look for
#
BISON_BASENAME= bison
#BISON_BASENAME= yacc

# Where run_bison.sh will search for bison with a recent enough version
#
# The -B arguments specify where to look for bison with a version,
# that is >= the minimum version (see MIN_BISON_VERSION in run_bison.sh),
# before searching for bison on $PATH.
#
# NOTE: If is OK if these directories do not exist.
#
BISON_DIRS= \
	-B /opt/homebrew/opt/bison/bin \
	-B /usr/local/opt/bison/bin \
	-B /opt/homebrew/bin \
	-B /opt/local/bin \
	-B /usr/local/opt \
	-B /usr/local/bin \
	-B .

# Additional flags to pass to bison
#
# For the -Wcounterexamples it gives counter examples if there are ever
# shift/reduce conflicts in the grammar. The other warnings are of use as well.
#
BISON_FLAGS= -Werror -Wcounterexamples -Wmidrule-values -Wprecedence -Wdeprecated \
	      --header

# the basename of flex (or lex) to look for
#
FLEX_BASENAME= flex
#FLEX_BASENAME= lex

# Where run_flex.sh will search for flex with a recent enough version
#
# The -F arguments specify where to look for flex with a version,
# that is >= the minimum version (see MIN_FLEX_VERSION in run_flex.sh),
# before searching for bison on $PATH.
#
# NOTE: If is OK if these directories do not exist.
#
FLEX_DIRS= \
	-F /opt/homebrew/opt/flex/bin \
	-F /usr/local/opt/flex/bin \
	-F /opt/homebrew/bin \
	-F /opt/local/bin \
	-F /usr/local/opt \
	-F /usr/local/bin \
	-F .

# flags to pass to flex
#
FLEX_FLAGS= -8

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
MAN1_DIR= /usr/local/share/man/man1
MAN3_DIR= /usr/local/share/man/man3
MAN8_DIR= /usr/local/share/man/man8
DEST_INCLUDE= /usr/local/include/jparse
DEST_LIB= /usr/local/lib
DEST_DIR= /usr/local/bin


#################################
# external Makefile information #
#################################

# may be used outside of this directory
#
EXTERN_H= jparse.h
EXTERN_O=
EXTERN_MAN= ${ALL_MAN_TARGETS}
EXTERN_LIBA= libjparse.a
EXTERN_PROG= jparse jsemtblgen jsemcgen.sh jstrdecode jstrencode

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
LIBA_TARGETS= libjparse.a

# shell targets to make by all and removed by clobber
#
SH_TARGETS=

# program targets to make by all, installed by install, and removed by clobber
#
PROG_TARGETS= jparse verge jsemtblgen jstrdecode jstrencode

# include files NOT to removed by clobber
#
H_SRC_TARGETS= jparse.h jparse.lex.h jparse.lex.ref.h jparse.tab.h jparse.tab.ref.h \
	       jparse_main.h json_parse.h json_sem.h json_util.h sorry.tm.ca.h util.h

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


###########################################
# all rule - default rule - must be first #
###########################################

all: ${TARGETS} ${ALL_OTHER_TARGETS} Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_jparse all C_SPECIAL=${C_SPECIAL}


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all extern_include extern_objs extern_liba extern_man extern_prog \
	extern_everything man/man1/jparse.1 man/man8/jparse_test.8 \
	parser parser-o use_json_ref rebuild_jnum_test bison flex test \
	tags local_dir_tags all_tags check_man legacy_clean legacy_clobber \
	load_json_ref install_man configure clean clobber install depend


####################################
# things to make in this directory #
####################################

json_util.o: json_util.c json_util.h
	${CC} ${CFLAGS} json_util.c -c

jparse.tab.o: jparse.tab.c
	${CC} ${CFLAGS} jparse.tab.c -c

jparse_main.o: jparse_main.c
	${CC} ${CFLAGS} jparse_main.c -c

jparse.o: jparse.c jparse.h
	${CC} ${CFLAGS} jparse.c -c

jparse: jparse_main.o libjparse.a
	${CC} ${CFLAGS} $^ -lm -o $@ -ldbg -ldyn_array

jstrencode.o: jstrencode.c jstrencode.h json_util.h json_util.c
	${CC} ${CFLAGS} jstrencode.c -c

jstrencode: jstrencode.o libjparse.a
	${CC} ${CFLAGS} $^ -lm -o $@ -ldbg -ldyn_array

jstrdecode.o: jstrdecode.c jstrdecode.h json_util.h json_parse.h
	${CC} ${CFLAGS} jstrdecode.c -c

jstrdecode: jstrdecode.o libjparse.a
	${CC} ${CFLAGS} $^ -lm -o $@ -ldbg -ldyn_array

json_parse.o: json_parse.c
	${CC} ${CFLAGS} json_parse.c -c

jsemtblgen.o: jsemtblgen.c jparse.tab.h
	${CC} ${CFLAGS} jsemtblgen.c -c

jsemtblgen: jsemtblgen.o libjparse.a
	${CC} ${CFLAGS} $^ -lm -o $@ -ldbg -ldyn_array

json_sem.o: json_sem.c
	${CC} ${CFLAGS} json_sem.c -c


# How to create jparse.tab.c and jparse.tab.h
#
# Convert jparse.y into jparse.tab.c and jparse.tab.c via bison, if bison is
# found and has a recent enough version. Otherwise, if RUN_O_FLAG is NOT
# specified use a pre-built reference copies stored in jparse.tab.ref.h and
# jparse.tab.ref.c. If it IS specified it is an error.
#
# NOTE: The value of RUN_O_FLAG depends on what rule called this rule.
#
jparse.tab.c jparse.tab.h: jparse.y jparse.h sorry.tm.ca.h run_bison.sh verge jparse.tab.ref.c jparse.tab.ref.h
	${Q} ./run_bison.sh -g ./verge -s sorry.tm.ca.h -b ${BISON_BASENAME} ${BISON_DIRS} \
	    -p jparse -v ${Q_V_OPTION} ${RUN_O_FLAG} -- ${BISON_FLAGS}

# How to create jparse.c and jparse.lex.h
#
# Convert jparse.l into jparse.c via flex, if flex found and has a recent enough
# version. Otherwise, if RUN_O_FLAG is NOT set use the pre-built reference copy
# stored in jparse.ref.c. If it IS specified it is an error.
#
# NOTE: The value of RUN_O_FLAG depends on what rule called this rule.
#
jparse.c jparse.lex.h: jparse.l jparse.h ./sorry.tm.ca.h jparse.tab.h ./run_flex.sh \
	verge jparse.ref.c jparse.lex.ref.h
	${Q} ./run_flex.sh -g ./verge -s sorry.tm.ca.h -f ${FLEX_BASENAME} ${FLEX_DIRS} \
	    -p jparse -v ${Q_V_OPTION} ${RUN_O_FLAG} -- ${FLEX_FLAGS} -o jparse.c

util.o: util.c util.h
	${CC} ${CFLAGS} util.c -c

verge.o: verge.c verge.h
	${CC} ${CFLAGS} verge.c -c

verge: verge.o util.o
	${CC} ${CFLAGS} $^ -o $@ -ldbg -ldyn_array

libjparse.a: ${LIB_OBJS}
	${RM} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@

run_bison-v7: verge sorry.tm.ca.h
	./run_bison.sh -v 7 -s ./sorry.tm.ca.h -g ./verge -D .


run_flex-v7: verge sorry.tm.ca.h
	./run_flex.sh -v 7 -s ./sorry.tm.ca.h -g ./verge -D .

#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

test_jparse/test_JSON/info.json/good/info.reference.json: test_jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_jparse test_JSON/info.json/good/info.reference.json C_SPECIAL=${C_SPECIAL}

test_jparse/test_JSON/auth.json/good/auth.reference.json: test_jparse/Makefile
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_jparse test_JSON/auth.json/good/auth.reference.json C_SPECIAL=${C_SPECIAL}


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

man/man1/jparse.1:
	@:

man/man8/jparse_test.8:
	@:


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

# make parser
#
# Force the rebuild of the JSON parser and then form the reference copies of
# JSON parser C code (if recent enough version of flex and bison are found).
#
parser: jparse.y jparse.l
	${RM} -f jparse.tab.c jparse.tab.h
	@# make jparse.tab.c implies make jparse.tab.h too
	${E} ${MAKE} jparse.tab.c C_SPECIAL=${C_SPECIAL}
	${E} ${MAKE} jparse.tab.o C_SPECIAL=${C_SPECIAL}
	${RM} -f jparse.c jparse.lex.h
	@# make jparse.c implies make jparse.lex.h too
	${E} ${MAKE} jparse.c C_SPECIAL=${C_SPECIAL}
	${E} ${MAKE} jparse.o C_SPECIAL=${C_SPECIAL}
	${RM} -f jparse.tab.ref.c
	${CP} -f -v jparse.tab.c jparse.tab.ref.c
	${RM} -f jparse.tab.ref.h
	${CP} -f -v jparse.tab.h jparse.tab.ref.h
	${RM} -f jparse.ref.c
	${CP} -f -v jparse.c jparse.ref.c
	${RM} -f -v jparse.lex.ref.h
	${CP} -f -v jparse.lex.h jparse.lex.ref.h

# make parser-o: Force the rebuild of the JSON parser.
#
# NOTE: This does NOT use the reference copies of JSON parser C code.
#
parser-o: jparse.y jparse.l
	${E} ${MAKE} parser RUN_O_FLAG='-o' C_SPECIAL=${C_SPECIAL}

# load reference code from the previous successful make parser
#
load_json_ref: jparse.tab.c jparse.tab.h jparse.c jparse.lex.h
	${RM} -f jparse.tab.ref.c
	${CP} -f -v jparse.tab.c jparse.tab.ref.c
	${RM} -f jparse.tab.ref.h
	${CP} -f -v jparse.tab.h jparse.tab.ref.h
	${RM} -f jparse.ref.c
	${CP} -f -v jparse.c jparse.ref.c
	${RM} -f jparse.lex.ref.h
	${CP} -f -v jparse.lex.h jparse.lex.ref.h

# restore reference code that was produced by previous successful make parser
#
# This rule forces the use of reference copies of JSON parser C code.
#
use_json_ref: jparse.tab.ref.c jparse.tab.ref.h jparse.ref.c jparse.lex.ref.h
	${RM} -f jparse.tab.c
	${CP} -f -v jparse.tab.ref.c jparse.tab.c
	${RM} -f jparse.tab.h
	${CP} -f -v jparse.tab.ref.h jparse.tab.h
	${RM} -f jparse.c
	${CP} -f -v jparse.ref.c jparse.c
	${RM} -f jparse.lex.h
	${CP} -f -v jparse.lex.ref.h jparse.lex.h

# use jnum_gen to regenerate test jnum_chk test suite
#
# IMPORTANT: DO NOT run this tool unless you KNOW that
#	     the tables produced by jnum_gen are CORRECT!
#
# WARNING: If you use this rule and generate invalid tables, then you will cause the
#	   jnum_chk(8) tool to check against bogus test results!
#
rebuild_jnum_test:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse $@ C_SPECIAL=${C_SPECIAL}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

bison: jparse.tab.c jparse.tab.h
	@:

flex: jparse.c jparse.lex.h
	@:

test:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse $@ C_SPECIAL=${C_SPECIAL}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# rule used by prep.sh and make clean
#
clean_generated_obj:
	${RM} -f ${BUILT_OBJS}

# sequence exit codes
#
seqcexit: ${FLEXFILES} ${BISONFILES} ${ALL_CSRC} test_jparse/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse all $@ C_SPECIAL=${C_SPECIAL}
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
	    echo "${SEQCEXIT} -c -D werr_sem_val -D werrp_sem_val -- ${FLEXFILES} ${BISONFILES}"; \
	    ${SEQCEXIT} -c -D werr_sem_val -D werrp_sem_val -- ${FLEXFILES} ${BISONFILES}; \
	    echo "${SEQCEXIT} -D werr_sem_val -D werrp_sem_val -- ${ALL_CSRC}"; \
	    ${SEQCEXIT} -D werr_sem_val -D werrp_sem_val -- ${ALL_CSRC}; \
	fi
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

picky: ${ALL_SRC} test_jparse/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse all $@ C_SPECIAL=${C_SPECIAL}
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
		echo "make $@: ERROR: CODE[1]: $$EXIT_CODE" 1>&2; \
		exit 1; \
	    fi; \
	    echo "${PICKY} -w -u -s -t8 -v -e -8 -- ${FLEXFILES} ${BISONFILES}"; \
	    ${PICKY} -w -u -s -t8 -v -e -8 -- ${FLEXFILES} ${BISONFILES}; \
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
shellcheck: ${SH_FILES} .shellcheckrc test_jparse/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse all $@ C_SPECIAL=${C_SPECIAL}
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
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${MAN3_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${MAN3_TARGETS} ${MAN3_DIR}

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
	${Q} for dir in ../dbg ../dyn_alloc; do \
	    if [[ -f $$dir/Makefile && ! -f $$dir/${LOCAL_DIR_TAGS} ]]; then \
		echo ${MAKE} ${MAKE_CD_Q} -C $$dir local_dir_tags C_SPECIAL=${C_SPECIAL}; \
		${MAKE} ${MAKE_CD_Q} -C $$dir local_dir_tags C_SPECIAL=${C_SPECIAL}; \
	    fi; \
	done
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
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse $@ C_SPECIAL=${C_SPECIAL}
	${Q} echo
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
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse $@ C_SPECIAL=${C_SPECIAL}
	${Q} echo
	${Q} ${RM} -f tags
	${Q} for dir in . ../dbg ../dyn_alloc test_jparse; do \
	    if [[ -s $$dir/${LOCAL_DIR_TAGS} ]]; then \
		echo "${SED} -e 's;\t;\t'$${dir}'/;' $${dir}/${LOCAL_DIR_TAGS} >> tags"; \
		${SED} -e 's;\t;\t'$${dir}'/;' "$${dir}/${LOCAL_DIR_TAGS}" >> tags; \
	    fi; \
	done
	${E} ${SORT} tags -o tags
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

legacy_clean: test_jparse/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_jparse $@ C_SPECIAL=${C_SPECIAL}
	${V} echo
	${S} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

legacy_clobber: legacy_clean test_jparse/Makefile
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${Q} ${MAKE} ${MAKE_CD_Q} -C test_jparse $@ C_SPECIAL=${C_SPECIAL}
	${V} echo
	${S} echo "${OUR_NAME}: nothing to do"
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${RM} -f ${ALL_OBJS} ${ALL_BUILT_SRC}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

clobber: legacy_clobber clean
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse $@ C_SPECIAL=${C_SPECIAL}
	${RM} -f ${TARGETS}
	${RM} -f jparse.output lex.yy.c jparse.c lex.jparse_.c
	${RM} -f jsemcgen.out.*
	${RM} -f tags ${LOCAL_DIR_TAGS}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

install: all test_jparse/Makefile install_man
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse all $@ C_SPECIAL=${C_SPECIAL}
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_LIB}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${LIBA_TARGETS} ${DEST_LIB}
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_INCLUDE}
	${I} ${INSTALL} ${INSTALL_V} -m 0444 ${H_SRC_TARGETS} ${DEST_INCLUDE}
	${I} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${I} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"

# uninstall: we provide this in case someone wants to deobfuscate their system. :-)
uninstall:
	${S} echo
	${S} echo "${OUR_NAME}: make $@ starting"
	${S} echo
	${I} ${RM} -r -f ${RM_V} ${DEST_LIB}/libjparse.a
	${I} ${RM} -r -f ${RM_V} ${DEST_INCLUDE}
	${RM} -r -f ${RM_V} ${DEST_DIR}/jparse
	${RM} -r -f ${RM_V} ${DEST_DIR}/verge
	${RM} -r -f ${RM_V} ${DEST_DIR}/jsemtblgen
	${RM} -r -f ${RM_V} ${DEST_DIR}/jstrdecode
	${RM} -r -f ${RM_V} ${DEST_DIR}/jstrencode
	${RM} -r -f ${RM_V} ${DEST_DIR}/jsemcgen.sh
	${RM} -r -f ${RM_V} ${DEST_DIR}/run_bison.sh
	${RM} -r -f ${RM_V} ${DEST_DIR}/run_flex.sh
	${RM} -r -f ${RM_V} ${MAN1_DIR}/jparse.1
	${RM} -r -f ${RM_V} ${MAN1_DIR}/jstrdecode.1
	${RM} -r -f ${RM_V} ${MAN1_DIR}/jstrencode.1
	${RM} -r -f ${RM_V} ${MAN3_DIR}/jparse.3
	${RM} -r -f ${RM_V} ${MAN3_DIR}/json_dbg.3
	${RM} -r -f ${RM_V} ${MAN3_DIR}/json_dbg_allowed.3
	${RM} -r -f ${RM_V} ${MAN3_DIR}/json_err_allowed.3
	${RM} -r -f ${RM_V} ${MAN3_DIR}/json_warn_allowed.3
	${RM} -r -f ${RM_V} ${MAN3_DIR}/parse_json.3
	${RM} -r -f ${RM_V} ${MAN3_DIR}/parse_json_file.3
	${RM} -r -f ${RM_V} ${MAN3_DIR}/parse_json_stream.3
	${RM} -r -f ${RM_V} ${MAN8_DIR}/jnum_chk.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/jnum_gen.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/jparse_test.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/jsemcgen.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/jsemtblgen.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/jstr_test.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/verge.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/run_bison.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/run_bison.sh.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/run_flex.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/run_flex.sh.8
	${RM} -r -f ${RM_V} ${MAN8_DIR}/jsemcgen.sh.8
	${S} echo
	${S} echo "${OUR_NAME}: make $@ ending"


###############
# make depend #
###############

depend: ${ALL_CSRC}
	${E} ${MAKE} ${MAKE_CD_Q} -C test_jparse $@ C_SPECIAL=${C_SPECIAL}
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
	      ${SED} -e 's;\.\./dyn_array/\.\./dbg/;../dbg/;g' | \
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
jparse.o: ../dbg/dbg.h ../dyn_array/dyn_array.h jparse.c jparse.h \
    jparse.tab.h json_parse.h json_sem.h json_util.h util.h
jparse.ref.o: ../dbg/dbg.h ../dyn_array/dyn_array.h jparse.h jparse.ref.c \
    jparse.tab.h json_parse.h json_sem.h json_util.h util.h
jparse.tab.o: ../dbg/dbg.h ../dyn_array/dyn_array.h jparse.h jparse.lex.h \
    jparse.tab.c jparse.tab.h json_parse.h json_sem.h json_util.h util.h
jparse.tab.ref.o: ../dbg/dbg.h ../dyn_array/dyn_array.h jparse.h \
    jparse.lex.h jparse.tab.h jparse.tab.ref.c json_parse.h json_sem.h \
    json_util.h util.h
jparse_main.o: ../dbg/dbg.h ../dyn_array/dyn_array.h jparse.h jparse.tab.h \
    jparse_main.c jparse_main.h json_parse.h json_sem.h json_util.h util.h
jsemtblgen.o: ../dbg/dbg.h ../dyn_array/dyn_array.h jparse.h jparse.tab.h \
    jsemtblgen.c jsemtblgen.h json_parse.h json_sem.h json_util.h util.h
json_parse.o: ../dbg/dbg.h ../dyn_array/dyn_array.h json_parse.c \
    json_parse.h json_util.h util.h
json_sem.o: ../dbg/dbg.h ../dyn_array/dyn_array.h json_parse.h json_sem.c \
    json_sem.h json_util.h util.h
json_util.o: ../dbg/dbg.h ../dyn_array/dyn_array.h json_parse.h json_util.c \
    json_util.h util.h
jstrdecode.o: ../dbg/dbg.h ../dyn_array/dyn_array.h json_parse.h \
    jstrdecode.c jstrdecode.h util.h
jstrencode.o: ../dbg/dbg.h ../dyn_array/dyn_array.h json_parse.h \
    jstrencode.c jstrencode.h util.h
util.o: ../dbg/dbg.h ../dyn_array/dyn_array.h util.c util.h
verge.o: ../dbg/dbg.h ../dyn_array/dyn_array.h util.h verge.c verge.h
