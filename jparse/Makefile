#!/usr/bin/make
#
# jparse - a JSON parser written in C
# 
#  This JSON parser was co-developed by:
# 
# 	@xexyl
# 	https://xexyl.net		Cody Boone Ferguson
# 	https://ioccc.xexyl.net
#  and:
# 	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
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
CHECKNR = checknr
CMP = cmp
CP= cp
CTAGS= ctags
CUT = cut
DIFF= diff
ECHO = echo
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
SLEEP = sleep
TEE= tee
TOUCH = touch
TR= tr
TRUE= true

GENERATED_CSRC= jparse.c jparse.tab.c
GENERATED_HSRC= jparse.tab.h jparse.lex.h
GENERATED_OBJ= jparse.o jparse.tab.o
OBJFILES= jnum_gen.o jparse.tab.o jsemtblgen.o json_sem.o jstrdecode.o verge.o jparse.o \
	  jparse_main.o json_parse.o json_util.o jstrencode.o
# This is a simpler way to do:
#
#   SRCFILES =  $(patsubst %.o,%.c,$(OBJFILES))
#
SRCFILES= $(OBJFILES:.o=.c)
ALL_CSRC= ${GENERATED_CSRC} ${SRCFILES}

TARGETS= jparse verge jsemtblgen jnum_gen jstrdecode jstrencode

all: ${TARGETS} ${OBJFILES}
	@:

CFLAGS= -O3 -g3 -pedantic -Wall -Wno-unneeded-internal-declaration
#CFLAGS= -O3 -g3 -pedantic -Wall -Werror

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
BISON_BASENAME = bison
#BISON_BASENAME = yacc

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
BISON_FLAGS = -Werror -Wcounterexamples -Wmidrule-values -Wprecedence -Wdeprecated \
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
FLEX_FLAGS = -8

######################################################
# List rules that do not create themselves as .PHONY #
######################################################
.PHONY: all clean clobber

#############################################################
# rules that invoke rules in Makefiles in other directories #
#############################################################

../dbg/dbg.a: ../dbg/Makefile
	${MAKE} -C ../dbg extern_liba

../util.o: ../Makefile
	${MAKE} -C .. util.o

../chkentry: ../chkentry.h ../chkentry.c ../Makefile
	${MAKE} -C .. chkentry

#
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


json_util.o: json_util.c json_util.h Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration json_util.c -c

jparse.tab.o: jparse.tab.c Makefile
	${CC} ${CFLAGS} -Wno-unused-function -Wno-unneeded-internal-declaration jparse.tab.c -c

jparse_main.o: jparse_main.c Makefile
	${CC} ${CFLAGS} jparse_main.c -c

jparse.o: jparse.c jparse.h Makefile
	${CC} ${CFLAGS} -Wno-unused-but-set-variable -Wno-unused-function -Wno-unneeded-internal-declaration jparse.c -c

jparse: jparse.o jparse.tab.o ../util.o ../dyn_array/dyn_array.o ../dbg/dbg.o json_parse.o \
	json_util.o jparse_main.o Makefile
	${CC} ${CFLAGS} -Wno-unneeded-internal-declaration jparse.o jparse.tab.o ../util.o ../dyn_array/dyn_array.o ../dbg/dbg.o json_parse.o \
			json_util.o jparse_main.o -lm -o $@

jstrencode.o: jstrencode.c jstrencode.h json_util.h json_util.c Makefile
	${CC} ${CFLAGS} jstrencode.c -c

jstrencode: jstrencode.o ../dbg/dbg.a json_parse.o json_util.o ../util.o ../dyn_array/dyn_array.a Makefile
	${CC} ${CFLAGS} jstrencode.o ../dbg/dbg.a json_parse.o json_util.o ../util.o ../dyn_array/dyn_array.a -lm -o $@

jstrdecode.o: jstrdecode.c jstrdecode.h json_util.h json_parse.h Makefile
	${CC} ${CFLAGS} jstrdecode.c -c

jstrdecode: jstrdecode.o ../dbg/dbg.a json_parse.o json_util.o ../util.o ../dyn_array/dyn_array.a Makefile
	${CC} ${CFLAGS} jstrdecode.o ../dbg/dbg.a json_parse.o json_util.o ../util.o ../dyn_array/dyn_array.a -lm -o $@

json_parse.o: json_parse.c Makefile
	${CC} ${CFLAGS} json_parse.c -c

jnum_gen.o: jnum_gen.c jnum_gen.h Makefile
	${CC} ${CFLAGS} jnum_gen.c -c

jnum_gen: jnum_gen.o ../dbg/dbg.a json_parse.o json_util.o ../util.o ../dyn_array/dyn_array.a Makefile
	${CC} ${CFLAGS} jnum_gen.o ../dbg/dbg.a json_parse.o json_util.o ../util.o ../dyn_array/dyn_array.a -lm -o $@

jsemtblgen.o: jsemtblgen.c Makefile
	${CC} ${CFLAGS} jsemtblgen.c -c

jsemtblgen: jsemtblgen.o jparse.o jparse.tab.o ../util.o ../dyn_array/dyn_array.a ../dbg/dbg.a json_parse.o \
	    json_util.o ../rule_count.o Makefile
	${CC} ${CFLAGS} jsemtblgen.o jparse.o jparse.tab.o ../util.o ../dyn_array/dyn_array.a ../dbg/dbg.a json_parse.o \
			json_util.o ../rule_count.o -lm -o $@

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
jparse.tab.c jparse.tab.h bison: jparse.y jparse.h sorry.tm.ca.h run_bison.sh ../limit_ioccc.sh \
	verge jparse.tab.ref.c jparse.tab.ref.h Makefile
	./run_bison.sh -g ./verge -s ./sorry.tm.ca.h -l ../limit_ioccc.sh -b ${BISON_BASENAME} ${BISON_DIRS} -p jparse -v 1 ${RUN_O_FLAG} -- \
		       ${BISON_FLAGS}

# How to create jparse.c and jparse.lex.h
#
# Convert jparse.l into jparse.c via flex, if flex found and has a recent enough
# version. Otherwise, if RUN_O_FLAG is NOT set use the pre-built reference copy
# stored in jparse.ref.c. If it IS specified it is an error.
#
# NOTE: The value of RUN_O_FLAG depends on what rule called this rule.
jparse.c jparse.lex.h flex: jparse.l jparse.h ./sorry.tm.ca.h jparse.tab.h ./run_flex.sh ../limit_ioccc.sh \
	       verge jparse.ref.c jparse.lex.ref.h Makefile
	./run_flex.sh -g verge -s ./sorry.tm.ca.h -l ../limit_ioccc.sh -f ${FLEX_BASENAME} ${FLEX_DIRS} -p jparse -v 1 ${RUN_O_FLAG} -- \
		      ${FLEX_FLAGS} -o jparse.c

verge.o: verge.c verge.h Makefile
	${CC} ${CFLAGS} verge.c -c

verge: verge.o ../dbg/dbg.a ../util.o ../dyn_array/dyn_array.o Makefile
	${CC} ${CFLAGS} verge.o ../dbg/dbg.o ../util.o ../dyn_array/dyn_array.o -o $@



configure:
	@echo nothing to $@

clean:
	${RM} -f ${OBJFILES}

clobber: clean
	${RM} -f ${TARGETS}

install: all
	@echo nothing to $@

###################################################################
# repo tools - rules for those who maintain the mkiocccentry repo #
###################################################################

depend: ../soup/fmt_depend.sh
	@echo
	@echo "make depend starting"
	@echo
	@${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE/p' Makefile
	${CC} ${CFLAGS} -MM -I../dbg -I../dyn_array -I.. ${ALL_CSRC} | ../soup/fmt_depend.sh >> Makefile
	@-if ${CMP} -s Makefile.orig Makefile; then \
	    ${RM} -f Makefile.orig; \
	else \
	    echo; echo "Makefile dependencies updated"; echo; echo "Previous version may be found in: Makefile.orig"; \
	fi
	@echo
	@echo "make depend completed"

###############
# make depend #
###############

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
jparse.o: jparse.c jparse.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h jparse.tab.h
jparse.tab.o: jparse.tab.c jparse.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h jparse.tab.h jparse.lex.h
jnum_gen.o: jnum_gen.c jnum_gen.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h ../limit_ioccc.h ../version.h
jparse.tab.o: jparse.tab.c jparse.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h jparse.tab.h jparse.lex.h
jsemtblgen.o: jsemtblgen.c jsemtblgen.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_util.h json_parse.h jparse.h jparse.tab.h json_sem.h ../iocccsize.h
json_sem.o: json_sem.c ../dbg/dbg.h json_sem.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h
jstrdecode.o: jstrdecode.c jstrdecode.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h ../limit_ioccc.h ../version.h
verge.o: verge.c verge.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h ../dyn_array/../dbg/dbg.h \
	../limit_ioccc.h ../version.h
jparse.o: jparse.c jparse.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h json_util.h jparse.tab.h
jparse_main.o: jparse_main.c jparse_main.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h jparse.h json_parse.h json_util.h jparse.tab.h
json_parse.o: json_parse.c ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h ../dyn_array/../dbg/dbg.h \
	json_parse.h json_util.h
json_util.o: json_util.c ../dbg/dbg.h json_parse.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_util.h
jstrencode.o: jstrencode.c jstrencode.h ../dbg/dbg.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h json_parse.h ../limit_ioccc.h ../version.h
