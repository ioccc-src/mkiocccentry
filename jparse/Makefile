#!/usr/bin/make
#
# jparse - a JSON parser written in C
#
#  This JSON parser was co-developed by:
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
MAKE= make
PICKY= picky
RANLIB= ranlib
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
OUR_NAME= jparse

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
WARN_FLAGS= -pedantic -Wall -Wextra -Wno-unneeded-internal-declaration
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
#CFLAGS= -O3 -g3 -pedantic -Wall -Werror


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC= jparse_main.c json_parse.c json_sem.c json_util.c jnum_gen.c jnum_header.c \
       jsemtblgen.c jstrdecode.c jstrencode.c util.c verge.c
H_SRC= jparse.h jnum_gen.h jparse_main.h jsemtblgen.h json_parse.h json_sem.h json_util.h \
       jstrdecode.h jstrencode.h sorry.tm.ca.h util.h verge.h \
       jparse.tab.ref.h

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
MAN1_PAGES= man/man1/jparse.1 man/man1/jstrdecode.1 man/man1/jstrencode.1 man/man1/run_bison.1 \
	man/man1/run_flex.1
MAN3_PAGES=
MAN8_PAGES= man/man8/jnum_chk.8 man/man8/jnum_gen.8 man/man8/jparse_test.8 man/man8/jsemcgen.8 \
	man/man8/jsemtblgen.8 man/man8/jstr_test.8 man/man8/utf8_test.8 man/man8/verge.8
ALL_MAN_PAGES= ${MAN1_PAGES} ${MAN3_PAGES} ${MAN8_PAGES}


######################
# intermediate files #
######################

# NOTE: intermediate files to make and removed by make clean
#
BUILT_C_SRC= jparse.tab.c
BUILT_H_SRC= jparse.tab.h jparse.lex.h
ALL_BUILT_SRC= ${BUILT_C_SRC} ${BUILT_H_SRC}

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= jparse.o jparse.tab.o jparse_main.o json_parse.o json_sem.o json_util.o util.o

# NOTE: ${OTHER_OBJS} are objects NOT put into a library and removed by make clean
#
OTHER_OBJS= verge.o

# all intermediate files and removed by make clean
#
ALL_OBJS= ${LIB_OBJS} ${OTHER_OBJS}

# rule used by prep.sh and make clean via make clean_generated_obj
#
BUILT_OBJS= jparse.o jparse.tab.o jparse.tab.o

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
# that is >= the minimum version (see MIN_BISON_VERSION in ../soup/limit_ioccc.sh),
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
# For --report all it will generate upon execution (if bison successfully
# generates the code) the file jparse.output. With --report all --html it will
# generate a html file which is easier to follow but I'm not sure how portable
# this is; under CentOS (which does not have the right version but actually
# normally generates code fine) the error:
#
#   jparse.y: error: xsltproc failed with status 127
#
# is thrown and since this could happen on other systems even with the
# appropriate version I have not enabled this.
#
# For the -Wcounterexamples it gives counter examples if there are ever
# shift/reduce conflicts in the grammar. The other warnings are of use as well.
#
BISON_FLAGS= -Werror -Wcounterexamples -Wmidrule-values -Wprecedence -Wdeprecated \
	      --report all --header

# the basename of flex (or lex) to look for
#
FLEX_BASENAME= flex
#FLEX_BASENAME= lex

# Where run_flex.sh will search for flex with a recent enough version
#
# The -F arguments specify where to look for flex with a version,
# that is >= the minimum version (see MIN_FLEX_VERSION in ../soup/limit_ioccc.sh),
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
DEST_INCLUDE= /usr/local/include
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
EXTERN_LIBA= jparse.a
EXTERN_PROG= jparse jsemtblgen jsemcgen.sh jnum_gen jstrdecode jstrencode

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
LIBA_TARGETS= jparse.a

# shell targets to make by all and removed by clobber
#
SH_TARGETS=

# program targets to make by all, installed by install, and removed by clobber
#
PROG_TARGETS= jparse verge jsemtblgen jnum_gen jstrdecode jstrencode

# include files NOT to removed by clobber
#
H_SRC_TARGETS= jparse.h

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
	@:


#################################################
# .PHONY list of rules that do not create files #
#################################################

.PHONY: all \
	extern_include extern_objs extern_liba extern_man extern_prog extern_everything man/man1/jparse.1 man/man8/jparse_test.8 \
	parser parser-o use_json_ref bison flex test tags check_man prep_clean prep_clobber legacy_clean legacy_clobber \
	load_json_ref install_man \
	configure clean clobber install depend


####################################
# things to make in this directory #
####################################

json_util.o: json_util.c json_util.h
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration json_util.c -c

jparse.tab.o: jparse.tab.c
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.tab.c -c

jparse_main.o: jparse_main.c
	${CC} ${CFLAGS} jparse_main.c -c

jparse.o: jparse.c jparse.h
	${CC} ${CFLAGS} -Wno-unused-but-set-variable -Wno-unused-function -Wno-unneeded-internal-declaration jparse.c -c

jparse: jparse_main.o jparse.a ../dyn_array/dyn_array.a ../dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@

jstrencode.o: jstrencode.c jstrencode.h json_util.h json_util.c
	${CC} ${CFLAGS} jstrencode.c -c

jstrencode: jstrencode.o jparse.a ../dyn_array/dyn_array.a ../dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@

jstrdecode.o: jstrdecode.c jstrdecode.h json_util.h json_parse.h
	${CC} ${CFLAGS} jstrdecode.c -c

jstrdecode: jstrdecode.o jparse.a ../dyn_array/dyn_array.a ../dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@

json_parse.o: json_parse.c
	${CC} ${CFLAGS} json_parse.c -c

jnum_gen.o: jnum_gen.c jnum_gen.h
	${CC} ${CFLAGS} jnum_gen.c -c

jnum_gen: jnum_gen.o jparse.a ../dyn_array/dyn_array.a ../dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@

jsemtblgen.o: jsemtblgen.c jparse.tab.h
	${CC} ${CFLAGS} jsemtblgen.c -c

jsemtblgen: jsemtblgen.o jparse.a ../dyn_array/dyn_array.a ../dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@

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
# XXX - we should not use ../soup/limit_ioccc.sh to determine the minimum bison version
jparse.tab.c jparse.tab.h: jparse.y jparse.h sorry.tm.ca.h run_bison.sh ../soup/limit_ioccc.sh \
	verge jparse.tab.ref.c jparse.tab.ref.h
	./run_bison.sh -g ./verge -s sorry.tm.ca.h -l ../soup/limit_ioccc.sh -b ${BISON_BASENAME} ${BISON_DIRS} \
	    -p jparse -v 1 ${RUN_O_FLAG} -- ${BISON_FLAGS}

# How to create jparse.c and jparse.lex.h
#
# Convert jparse.l into jparse.c via flex, if flex found and has a recent enough
# version. Otherwise, if RUN_O_FLAG is NOT set use the pre-built reference copy
# stored in jparse.ref.c. If it IS specified it is an error.
#
# NOTE: The value of RUN_O_FLAG depends on what rule called this rule.
#
# XXX - we should not use ../soup/limit_ioccc.sh to determine the minimum flex version
jparse.c jparse.lex.h: jparse.l jparse.h ./sorry.tm.ca.h jparse.tab.h ./run_flex.sh ../soup/limit_ioccc.sh \
	verge jparse.ref.c jparse.lex.ref.h
	./run_flex.sh -g ./verge -s sorry.tm.ca.h -l ../soup/limit_ioccc.sh -f ${FLEX_BASENAME} ${FLEX_DIRS} \
	    -p jparse -v 1 ${RUN_O_FLAG} -- ${FLEX_FLAGS} -o jparse.c

util.o: util.c util.h
	${CC} ${CFLAGS} util.c -c

verge.o: verge.c verge.h
	${CC} ${CFLAGS} verge.c -c

verge: verge.o util.o ../dbg/dbg.a ../dyn_array/dyn_array.a
	${CC} ${CFLAGS} $^ -o $@

jparse.a: ${LIB_OBJS}
	${RM} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../dbg/dbg.a: ../dbg/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../dbg extern_liba

../dyn_array/dyn_array.a: ../dyn_array/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../dyn_array extern_liba

test_jparse/test_JSON/info.json/good/info.reference.json: test_jparse/Makefile
	@${MAKE} ${MAKE_CD_Q} -C test_jparse test_JSON/info.json/good/info.reference.json

test_jparse/test_JSON/auth.json/good/auth.reference.json: test_jparse/Makefile
	@${MAKE} ${MAKE_CD_Q} -C test_jparse test_JSON/auth.json/good/auth.reference.json

../soup/limit_ioccc.sh: ../soup/Makefile
	@${MAKE} ${MAKE_CD_Q} -C ../soup limit_ioccc.sh


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
	${MAKE} jparse.tab.c jparse.tab.h
	${MAKE} jparse.tab.o
	${RM} -f jparse.c jparse.lex.h
	${MAKE} jparse.c jparse.lex.h
	${MAKE} jparse.o
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
	${MAKE} parser RUN_O_FLAG='-o'

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
rebuild_jnum_test: jnum_gen test_jparse/jnum.testset jnum_header.c
	${RM} -f test_jparse/jnum_test.c
	${CP} -f -v jnum_header.c test_jparse/jnum_test.c
	./jnum_gen test_jparse/jnum.testset >> test_jparse/jnum_test.c

bison: jparse.tab.c jparse.tab.h
	@:

flex: jparse.c jparse.lex.h
	@:

test: check_man test_jparse/Makefile
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@${MAKE} ${MAKE_CD_Q} -C test_jparse all test
	@echo
	@echo "${OUR_NAME}: make $@ complete"

# rule used by prep.sh and make clean
#
clean_generated_obj:
	${RM} -f ${BUILT_OBJS}

# sequence exit codes
#
seqcexit: ${FLEXFILES} ${BISONFILES} ${ALL_CSRC} test_jparse/Makefile
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@${MAKE} ${MAKE_CD_Q} -C test_jparse all $@
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
	    echo "${SEQCEXIT} -c -D werr_sem_val -D werrp_sem_val -- ${FLEXFILES} ${BISONFILES}"; \
	    ${SEQCEXIT} -c -D werr_sem_val -D werrp_sem_val -- ${FLEXFILES} ${BISONFILES}; \
	    echo "${SEQCEXIT} -D werr_sem_val -D werrp_sem_val -- ${ALL_CSRC}"; \
	    ${SEQCEXIT} -D werr_sem_val -D werrp_sem_val -- ${ALL_CSRC}; \
	fi
	@echo
	@echo "${OUR_NAME}: make $@ complete"

picky: ${ALL_SRC} test_jparse/Makefile
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@${MAKE} ${MAKE_CD_Q} -C test_jparse all $@
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
	    echo "${PICKY} -w -u -s -t8 -v -e -8 -- ${FLEXFILES} ${BISONFILES}"; \
	    ${PICKY} -w -u -s -t8 -v -e -8 -- ${FLEXFILES} ${BISONFILES}; \
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
shellcheck: ${SH_FILES} .shellcheckrc test_jparse/Makefile
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@${MAKE} ${MAKE_CD_Q} -C test_jparse all $@
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
tags: ${ALL_CSRC} ${ALL_HSRC} test_jparse/Makefile
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	@${MAKE} ${MAKE_CD_Q} -C test_jparse all $@
	-${CTAGS} ${ALL_CSRC} ${ALL_HSRC} 2>&1 | \
	     ${GREP} -E -v 'Duplicate entry|Second entry ignored'
	@echo
	@echo "${OUR_NAME}: make $@ complete"

prep_clean: legacy_clean test_jparse/Makefile
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${MAKE} ${MAKE_CD_Q} -C test_jparse $@
	@echo
	@echo "${OUR_NAME}: make $@ complete"

prep_clobber: prep_clean legacy_clobber
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -f jnum_gen
	${RM} -rf jnum_gen.dSYM
	${MAKE} ${MAKE_CD_Q} -C test_jparse $@
	@echo
	@echo "${OUR_NAME}: make $@ complete"

legacy_clean: test_jparse/Makefile
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${MAKE} ${MAKE_CD_Q} -C test_jparse $@
	@echo
	@echo "${OUR_NAME}: make $@ complete"

legacy_clobber: legacy_clean test_jparse/Makefile
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -f jstr_test.out jstr_test2.out
	${MAKE} ${MAKE_CD_Q} -C test_jparse $@
	@echo
	@echo "${OUR_NAME}: make $@ complete"


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

clobber: clean
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${RM} -f ${TARGETS}
	${RM} -f jparse.output
	${RM} -f jsemcgen.out.*
	${RM} -f tags
	@echo
	@echo "${OUR_NAME}: make $@ complete"

install: all test_jparse/Makefile install_man
	@echo
	@echo "${OUR_NAME}: make $@ starting"
	@echo
	${MAKE} ${MAKE_CD_Q} -C test_jparse all $@
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_LIB}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0444 ${LIBA_TARGETS} ${DEST_LIB}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_INCLUDE}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0444 ${H_SRC_TARGETS} ${DEST_INCLUDE}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -d -m 0775 ${DEST_DIR}
	${INSTALL_Q} ${INSTALL} ${INSTALL_V} -m 0555 ${SH_TARGETS} ${PROG_TARGETS} ${DEST_DIR}
	@echo
	@echo "${OUR_NAME}: make $@ complete"


###############
# make depend #
###############

depend: ${ALL_CSRC}
	${MAKE} ${MAKE_CD_Q} -C test_jparse $@
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
jnum_gen.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h ../dyn_array/dyn_array.h \
    jnum_gen.c jnum_gen.h json_parse.h json_util.h util.h
jnum_header.o: ../dyn_array/../dbg/dbg.h ../dyn_array/dyn_array.h \
    jnum_header.c json_parse.h util.h
jparse.ref.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h jparse.h jparse.ref.c jparse.tab.h \
    json_parse.h json_sem.h json_util.h util.h
jparse.tab.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h jparse.h jparse.lex.h jparse.tab.c \
    jparse.tab.h json_parse.h json_sem.h json_util.h util.h
jparse.tab.ref.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h jparse.h jparse.lex.h jparse.tab.h \
    jparse.tab.ref.c json_parse.h json_sem.h json_util.h util.h
jparse_main.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h jparse.h jparse.tab.h jparse_main.c \
    jparse_main.h json_parse.h json_sem.h json_util.h util.h
jsemtblgen.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h ../iocccsize.h jparse.h jparse.tab.h \
    jsemtblgen.c jsemtblgen.h json_parse.h json_sem.h json_util.h util.h
json_parse.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h json_parse.c json_parse.h json_util.h util.h
json_sem.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h ../dyn_array/dyn_array.h \
    json_parse.h json_sem.c json_sem.h json_util.h util.h
json_util.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h json_parse.h json_util.c json_util.h util.h
jstrdecode.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h json_parse.h jstrdecode.c jstrdecode.h util.h
jstrencode.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h \
    ../dyn_array/dyn_array.h json_parse.h jstrencode.c jstrencode.h util.h
util.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h ../dyn_array/dyn_array.h \
    util.c util.h
verge.o: ../dbg/dbg.h ../dyn_array/../dbg/dbg.h ../dyn_array/dyn_array.h \
    ../soup/limit_ioccc.h ../soup/version.h util.h verge.c verge.h
