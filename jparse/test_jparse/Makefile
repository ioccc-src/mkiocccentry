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
CMP = cmp
CUT = cut
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
CHECKNR = checknr
TEE= tee
TR= tr
TRUE= true


#######################
# Makefile parameters #
#######################

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
	configure clean clobber install


####################################
# things to make in this directory #
####################################

jnum_test.o: jnum_test.c Makefile
	${CC} ${CFLAGS} -I../.. jnum_test.c -c

jnum_chk.o: jnum_chk.c jnum_chk.h Makefile
	${CC} ${CFLAGS} -I../.. jnum_chk.c -c

jnum_chk: jnum_chk.o ../../dbg/dbg.a ../json_parse.o ../json_util.o ../../util.o ../../dyn_array/dyn_array.o \
	jnum_test.o Makefile
	${CC} ${CFLAGS} -I../.. jnum_chk.o ../../dbg/dbg.a ../json_parse.o ../json_util.o ../../util.o \
		../../dyn_array/dyn_array.o jnum_test.o -lm -o jnum_chk


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../../dbg/dbg.a: ../../dbg/Makefile
	${MAKE} -C ../../dbg extern_liba

../../util.o: ../../Makefile
	${MAKE} -C ../.. util.o


####################################
# rules for use by other Makefiles #
####################################


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

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
