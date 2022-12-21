#!/usr/bin/make
#
# test_jparse - mkiocccentry jparse test tools
#
# "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
# 
# This JSON parser was co-developed by:
# 
#	@xexyl
# 	https://xexyl.net		Cody Boone Ferguson
# 	https://ioccc.xexyl.net
# and:
# 	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
# 
# "Because sometimes even the IOCCC Judges need some help." :-)
# 
# "Share and Enjoy!"
#	--  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
####


#############
# utilities #
#############

AWK= awk
BASENAME= basename
CAT= cat
CMP= cmp
CUT= cut
CC= cc
CP= cp
CTAGS= ctags
DIFF= diff
FMT= fmt
GIT= git
GREP= grep
HEAD= head
INSTALL= install
MAKE= make
MAN= man
MAN2HTML= man2html
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
CHECKNR= checknr
TEE= tee
TR= tr
TRUE= true


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
# TODO - ###################################################################### - TODO #
# TODO - In 2023 we will will support only c17 so C_STD will become c-std=gnu17 - TODO #
# TODO - ###################################################################### - TODO #
#
C_STD= -std=gnu11
#C_STD= -std=gnu17

# optimization and debug level
#
#COPT= -O3 -g3	# TODO - this will be the production release value - TODO #
COPT= -O0 -g

# Compiler warnings
#
#WARN_FLAGS= -Wall -Wextra	# TODO - this will be the production release value - TODO #
WARN_FLAGS= -Wall -Wextra -Werror


# how to compile
#
# We test by forcing warnings to be errors so you don't have to (allegedly :-) )
#
CFLAGS= ${C_STD} ${COPT} -pedantic ${WARN_FLAGS} ${LDFLAGS}


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

# where and what to install
#
DESTDIR= /usr/local/bin
TARGETS= jnum_chk
SH_TARGETS=


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


######################
# intermediate files #
######################

# NOTE: ${LIB_OBJS} are objects to put into a library and removed by make clean
#
LIB_OBJS=

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
ALL_SRC= ${ALL_CSRC} ${ALL_HSRC}


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
EXTERN_PROG=

# NOTE: ${EXTERN_CLOBBER} used outside of this directory and removed by make clobber
#
EXTERN_CLOBBER= ${EXTERN_O} ${EXTERN_LIBA} ${EXTERN_PROG}


######################
# target information #
######################


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
	test prep_clean prep_clobber legacy_clean legacy_clobber \
	configure clean clobber install \
	test_JSON/info.json/good/info.reference.json \
	test_JSON/auth.json/good/auth.reference.json


####################################
# things to make in this directory #
####################################

jnum_test.o: jnum_test.c Makefile
	${CC} ${CFLAGS} -I../.. jnum_test.c -c

jnum_chk.o: jnum_chk.c jnum_chk.h Makefile
	${CC} ${CFLAGS} -I../.. jnum_chk.c -c

jnum_chk: jnum_chk.o jnum_test.o ../jparse.a ../../dyn_array/dyn_array.o ../../dbg/dbg.a
	${CC} ${CFLAGS} $^ -lm -o $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../../dbg/dbg.a: ../../dbg/Makefile
	@${MAKE} -C ../../dbg extern_liba

../../dyn_array/dyn_array.a: ../../dyn_array/Makefile
	@${MAKE} -C ../../dyn_array extern_liba

../jparse.a: ../Makefile
	@${MAKE} -C .. extern_liba


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

test: all jparse_test.sh
	 ./jparse_test.sh -j ../jparse -d ./test_JSON ./json_teststr.txt

prep_clean: legacy_clean
	${RM} -f jnum_chk.o jnum_gen.o

prep_clobber: legacy_clobber
	${RM} -f jstr_test.out jstr_test2.out
	${RM} -f jnum_chk jnum_gen
	${RM} -rf jnum_chk.dSYM
	${RM} -rf jnum_gen.dSYM

legacy_clean:
	@:

legacy_clobber:
	@:


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	${RM} -f jnum_chk.o jnum_test.o

clobber: clean
	${RM} -f ${TARGETS}
	${RM} -f jparse_test.log chkentry_test.log txzchk_test.log

install: all
	@echo nothing to $@


###############
# make depend #
###############

depend: ${ALL_CSRC} ../../soup/fmt_depend.sh
	@echo
	@echo "make depend starting"
	@echo
	@${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE/p' Makefile
	${CC} ${CFLAGS} -MM ${ALL_CSRC} | ../../soup/fmt_depend.sh >> Makefile
	@-if ${CMP} -s Makefile.orig Makefile; then \
	    ${RM} -f Makefile.orig; \
	else \
	    echo; echo "Makefile dependencies updated"; echo; echo "Previous version may be found in: Makefile.orig"; \
	fi
	@echo
	@echo "make depend completed"

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
jnum_chk.o: jnum_chk.c jnum_chk.h ../../dbg/dbg.h ../util.h ../../dyn_array/dyn_array.h \
	../../dyn_array/../dbg/dbg.h ../../jparse/json_parse.h ../../jparse/json_util.h ../json_parse.h \
	../../version.h
jnum_test.o: jnum_test.c ../../jparse/json_parse.h ../../jparse/util.h \
	../../jparse/../dyn_array/dyn_array.h ../../jparse/../dyn_array/../dbg/dbg.h
jnum_chk.o: jnum_chk.h ../../dbg/dbg.h ../util.h ../../dyn_array/dyn_array.h \
	../../dyn_array/../dbg/dbg.h ../../jparse/json_parse.h ../../jparse/json_util.h ../json_parse.h
