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
RANLIB= ranlib
RM= rm
RPL= rpl
RSYNC= rsync
SED= sed
SEQCEXIT= seqcexit
SHELL= bash
SHELLCHECK= shellcheck
SLEEP= sleep
TEE= tee
TOUCH= touch
TR= tr
TRUE= true

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
       jsemtblgen.c jstrdecode.c jstrencode.c util.c
H_SRC= jparse.h jnum_gen.h jparse_main.h jsemtblgen.h json_parse.h json_sem.h json_util.h \
       jstrdecode.h jstrencode.h sorry.tm.ca.h util.h verge.h
ALL_SRC= ${C_SRC} ${H_SRC}


######################
# intermediate files #
######################

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS= jparse.o jparse.tab.o jparse_main.o json_parse.o json_sem.o json_util.o util.o

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
# that is >= the minimum version (see MIN_BISON_VERSION in limit_ioccc.sh),
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
# that is >= the minimum version (see MIN_FLEX_VERSION in limit_ioccc.sh),
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
EXTERN_PROG= jparse jsemtblgen jsemcgen.sh jstrdecode jstrencode

# NOTE: ${EXTERN_CLOBBER} used outside of this directory and removed by make clobber
#
EXTERN_CLOBBER= ${EXTERN_O} ${EXTERN_LIBA} ${EXTERN_PROG}

GENERATED_HSRC= jparse.tab.h jparse.lex.h
GENERATED_OBJ= jparse.o jparse.tab.o jparse.tab.o
SRCFILES= jnum_gen.c jnum_header.c jparse.ref.c jparse.tab.ref.c jparse_main.c jsemtblgen.c \
	json_parse.c json_sem.c json_util.c jstrdecode.c jstrencode.c util.c verge.c
GENERATED_CSRC= jparse.tab.c
ALL_CSRC= ${GENERATED_CSRC} ${SRCFILES}


######################
# target information #
######################

# man pages
#
MAN1_TARGETS= man/jparse.1
MAN3_TARGETS=
MAN8_TARGETS= man/jparse_test.8
ALL_MAN_TARGETS= ${MAN1_TARGETS} ${MAN8_TARGETS}
EXTERN_MAN= ${ALL_MAN_TARGETS}


# libraries
#
LIBA_TARGETS= jparse.a

# include files
#
H_SRC_TARGETS= jparse.h

# what to make by all but NOT to removed by clobber (because they are not files)
#
ALL_OTHER_TARGETS= extern_everything

# what to make by all and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${LIBA_TARGETS} jparse verge jsemtblgen jnum_gen jstrdecode jstrencode


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
	extern_include extern_objs extern_liba extern_man extern_prog extern_everything man/jparse.1 man/jparse_test.8 \
	parser parser-o use_ref bison flex test tags check_man prep_clean prep_clobber legacy_clean legacy_clobber \
	configure clean clobber install \
	depend


####################################
# things to make in this directory #
####################################

json_util.o: json_util.c json_util.h Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration json_util.c -c

jparse.tab.o: jparse.tab.c Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.tab.c -c

jparse_main.o: jparse_main.c Makefile
	${CC} ${CFLAGS} jparse_main.c -c

jparse.o: jparse.c jparse.h Makefile
	${CC} ${CFLAGS} -Wno-unused-but-set-variable -Wno-unused-function -Wno-unneeded-internal-declaration jparse.c -c

jparse: jparse_main.o ../dbg/dbg.a ../dyn_array/dyn_array.a jparse.a
	${CC} ${CFLAGS} $^ -lm -o $@

jstrencode.o: jstrencode.c jstrencode.h json_util.h json_util.c Makefile
	${CC} ${CFLAGS} jstrencode.c -c

jstrencode: jstrencode.o ../dbg/dbg.a ../dyn_array/dyn_array.a jparse.a
	${CC} ${CFLAGS} $^ -lm -o $@

jstrdecode.o: jstrdecode.c jstrdecode.h json_util.h json_parse.h Makefile
	${CC} ${CFLAGS} jstrdecode.c -c

jstrdecode: jstrdecode.o ../dbg/dbg.a ../dyn_array/dyn_array.a jparse.a
	${CC} ${CFLAGS} $^ -lm -o $@

json_parse.o: json_parse.c Makefile
	${CC} ${CFLAGS} json_parse.c -c

jnum_gen.o: jnum_gen.c jnum_gen.h Makefile
	${CC} ${CFLAGS} jnum_gen.c -c

jnum_gen: jnum_gen.o ../dbg/dbg.a ../dyn_array/dyn_array.a jparse.a
	${CC} ${CFLAGS} $^ -lm -o $@

jsemtblgen.o: jsemtblgen.c jparse.tab.h Makefile
	${CC} ${CFLAGS} jsemtblgen.c -c

jsemtblgen: jsemtblgen.o ../dbg/dbg.a ../dyn_array/dyn_array.a jparse.a
	${CC} ${CFLAGS} $^ -lm -o $@

json_sem.o: json_sem.c Makefile
	${CC} ${CFLAGS} json_sem.c -c

# How to create jparse.tab.c and jparse.tab.h
#
# Convert jparse.y into jparse.tab.c and jparse.tab.c via bison, if bison is
# found and has a recent enough version. Otherwise, if RUN_O_FLAG is NOT
# specified use a pre-built reference copies stored in jparse.tab.ref.h and
# jparse.tab.ref.c. If it IS specified it is an error.
#
# NOTE: The value of RUN_O_FLAG depends on what rule called this rule.
jparse.tab.c jparse.tab.h: jparse.y jparse.h sorry.tm.ca.h run_bison.sh ../limit_ioccc.sh \
	verge jparse.tab.ref.c jparse.tab.ref.h Makefile
	./run_bison.sh -g ./verge -s ./sorry.tm.ca.h -l ../limit_ioccc.sh -b ${BISON_BASENAME} ${BISON_DIRS} \
	    -p jparse -v 1 ${RUN_O_FLAG} -- ${BISON_FLAGS}

# How to create jparse.c and jparse.lex.h
#
# Convert jparse.l into jparse.c via flex, if flex found and has a recent enough
# version. Otherwise, if RUN_O_FLAG is NOT set use the pre-built reference copy
# stored in jparse.ref.c. If it IS specified it is an error.
#
# NOTE: The value of RUN_O_FLAG depends on what rule called this rule.
jparse.c jparse.lex.h: jparse.l jparse.h ./sorry.tm.ca.h jparse.tab.h ./run_flex.sh ../limit_ioccc.sh \
	verge jparse.ref.c jparse.lex.ref.h Makefile
	./run_flex.sh -g ./verge -s ./sorry.tm.ca.h -l ../limit_ioccc.sh -f ${FLEX_BASENAME} ${FLEX_DIRS} \
	    -p jparse -v 1 ${RUN_O_FLAG} -- ${FLEX_FLAGS} -o jparse.c

util.o: util.c util.h Makefile
	${CC} ${CFLAGS} util.c -c

verge.o: verge.c verge.h Makefile
	${CC} ${CFLAGS} verge.c -c

verge: verge.o ../dbg/dbg.a util.o ../dyn_array/dyn_array.a Makefile
	${CC} ${CFLAGS} verge.o ../dbg/dbg.a util.o ../dyn_array/dyn_array.a -o $@

jparse.a: ${LIB_OBJS}
	${RM} -f $@
	${AR} -r -u -v $@ $^
	${RANLIB} $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../dbg/dbg.a: ../dbg/Makefile
	${MAKE} -C ../dbg extern_liba

../dyn_array/dyn_array.a: ../dyn_array/Makefile
	${MAKE} -C ../dyn_array extern_liba

test_jparse/test_JSON/auth.json/good/auth.reference.json: test_jparse/Makefile
	${MAKE} -C test_jparse test_JSON/auth.json/good/auth.reference.json


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

man/jparse.1:
	@:

man/jparse_test.8:
	@:


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

# make parser
#
# Force the rebuild of the JSON parser and then form the reference copies of
# JSON parser C code (if recent enough version of flex and bison are found).
#
parser: jparse.y jparse.l Makefile
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
	${MAKE} -C .. all

#
# make parser-o: Force the rebuild of the JSON parser.
#
# NOTE: This does NOT use the reference copies of JSON parser C code.
#
parser-o: jparse.y jparse.l Makefile
	${MAKE} parser RUN_O_FLAG='-o'

# restore reference code that was produced by previous successful make parser
#
# This rule forces the use of reference copies of JSON parser C code.
use_ref: jparse.tab.ref.c jparse.tab.ref.h jparse.ref.c jparse.lex.ref.h
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
rebuild_jnum_test: jnum_gen test_jparse/jnum.testset jnum_header.c Makefile
	${RM} -f test_jparse/jnum_test.c
	${CP} -f -v jnum_header.c test_jparse/jnum_test.c
	./jnum_gen test_jparse/jnum.testset >> test_jparse/jnum_test.c

bison: jparse.tab.c jparse.tab.h
	@:

flex: jparse.c jparse.lex.h
	@:

test: check_man Makefile
	@${MAKE} -C test_jparse all test

tags: ${ALL_SRC} ${ALL_BUILT_SRC}
	-${CTAGS} ${ALL_SRC} ${ALL_BUILT_SRC} 2>&1 | \
	     ${GREP} -E -v 'Duplicate entry|Second entry ignored'

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

prep_clean: legacy_clean
	${MAKE} -C test_jparse $@

prep_clobber: legacy_clobber
	${RM} -f jnum_gen
	${RM} -rf jnum_gen.dSYM
	${MAKE} -C test_jparse $@

legacy_clean:
	${MAKE} -C test_jparse $@

legacy_clobber:
	${RM} -f jstr_test.out jstr_test2.out
	${MAKE} -C test_jparse $@


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	${RM} -f ${LIB_OBJS}

clobber: clean
	${RM} -f ${TARGETS}
	${RM} -f jparse.output
	${RM} -f jsemcgen.out.*

install: all
	${INSTALL} -v -d -m 0775 ${DEST_LIB}
#none#	${INSTALL} -v -d -m 0775 ${DEST_DIR}
	${INSTALL} -v -d -m 0775 ${MAN1_DIR}
#none#	${INSTALL} -v -d -m 0775 ${MAN3_DIR}
	${INSTALL} -v -d -m 0775 ${MAN8_DIR}
	${INSTALL} -v -d -m 0775 ${DEST_INCLUDE}
	${INSTALL} -v -m 0444 ${MAN1_TARGETS} ${MAN1_DIR}
#none#	${INSTALL} -v -m 0444 ${MAN3_TARGETS} ${MAN3_DIR}
	${INSTALL} -v -m 0444 ${MAN8_TARGETS} ${MAN8_DIR}
	${INSTALL} -v -m 0444 ${LIBA_TARGETS} ${DEST_LIB}
	${INSTALL} -v -m 0444 ${H_SRC_TARGETS} ${DEST_INCLUDE}


###############
# make depend #
###############

depend: ${ALL_CSRC} ../soup/fmt_depend.sh
	@echo
	@echo "jparse make depend starting"
	@echo
	@${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE/p' Makefile
	${CC} ${CFLAGS} -MM -I../dbg -I../dyn_array -I.. ${ALL_CSRC} | ../soup/fmt_depend.sh >> Makefile
	@-if ${CMP} -s Makefile.orig Makefile; then \
	    ${RM} -f Makefile.orig; \
	else \
	    echo; echo "Makefile dependencies updated"; echo; echo "Previous version may be found in: Makefile.orig"; \
	fi
	@echo
	@echo "jparse make depend completed"

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
jparse.tab.o: jparse.tab.c jparse.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h json_sem.h jparse.tab.h jparse.lex.h
jnum_gen.o: jnum_gen.c jnum_gen.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h ../limit_ioccc.h ../version.h
jnum_header.o: jnum_header.c json_parse.h util.h ../dyn_array/dyn_array.h ../dyn_array/../dbg/dbg.h
jparse.ref.o: jparse.ref.c jparse.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h json_sem.h jparse.tab.h
jparse.tab.ref.o: jparse.tab.ref.c jparse.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h json_sem.h jparse.tab.h jparse.lex.h
jparse_main.o: jparse_main.c jparse_main.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h jparse.h json_parse.h json_util.h json_sem.h jparse.tab.h
jsemtblgen.o: jsemtblgen.c jsemtblgen.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_util.h json_parse.h jparse.h json_sem.h jparse.tab.h ../iocccsize.h
json_parse.o: json_parse.c ../dbg/dbg.h util.h ../dyn_array/dyn_array.h ../dyn_array/../dbg/dbg.h \
	json_parse.h json_util.h
json_sem.o: json_sem.c ../dbg/dbg.h json_sem.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h
json_util.o: json_util.c ../dbg/dbg.h json_parse.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_util.h
jstrdecode.o: jstrdecode.c jstrdecode.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h ../limit_ioccc.h ../version.h
jstrencode.o: jstrencode.c jstrencode.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h ../limit_ioccc.h ../version.h
util.o: util.c ../dbg/dbg.h util.h ../dyn_array/dyn_array.h ../dyn_array/../dbg/dbg.h
verge.o: verge.c verge.h ../dbg/dbg.h util.h ../dyn_array/dyn_array.h ../dyn_array/../dbg/dbg.h \
	../limit_ioccc.h ../version.h
