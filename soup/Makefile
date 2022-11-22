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

AWK= awk
BASENAME= basename
CAT= cat
CMP = cmp
CUT = cut
CC= cc
CP= cp
CTAGS= ctags
DIFF= diff
ECHO = echo
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
SLEEP = sleep
CHECKNR = checknr
TEE= tee
TOUCH = touch
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
# TODO - In 2024 we will will support only c17 so C_STD will become c-std=gnu17 - TODO #
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
TARGETS= ${OBJFILES}
SH_TARGETS=

OBJFILES= chk_sem_auth.o chk_sem_info.o chk_validate.o

# This is a simpler way to do:
#
#   SRCFILES =  $(patsubst %.o,%.c,$(OBJFILES))
#
SRCFILES= $(OBJFILES:.o=.c)
ALL_CSRC= ${SRCFILES}


######################################
# all - default rule - must be first #
######################################

all: ${OBJFILES}

.PHONY: all soup eat eating eat_soup eating_soup configure clean clobber install

all:
	@:

chk_sem_auth.o: chk_sem_auth.c chk_sem_auth.h Makefile
	${CC} ${CFLAGS} -I.. chk_sem_auth.c -c

chk_sem_info.o: chk_sem_info.c chk_sem_info.h Makefile
	${CC} ${CFLAGS} -I.. chk_sem_info.c -c

chk_validate.o: chk_validate.c Makefile
	${CC} ${CFLAGS} -I.. chk_validate.c -c


###################################################################
# repo tools - rules for those who maintain the mkiocccentry repo #
###################################################################

depend: all ./fmt_depend.sh
	@echo
	@echo "make depend starting"
	@echo
	@${SED} -i.orig -n -e '1,/^### DO NOT CHANGE MANUALLY BEYOND THIS LINE/p' Makefile
	${CC} ${CFLAGS} -MM -I../dbg -I.. ${ALL_CSRC} | ./fmt_depend.sh >> Makefile
	@-if ${CMP} -s Makefile.orig Makefile; then \
	    ${RM} -f Makefile.orig; \
	else \
	    echo; echo "Makefile dependencies updated"; echo; echo "Previous version may be found in: Makefile.orig"; \
	fi
	@echo
	@echo "make depend completed"


soup:
	@-if [[ -r .soup ]]; then \
	    ${ECHO} "Soup already cooked. Don't forget to eat it before it goes cold!"; \
        else \
	    ${ECHO} "Mixing the ingredients..."; \
	    ${ECHO} "Cooking the soup..."; \
	    ${TOUCH} .soup; \
	    ${SLEEP} 2; \
	    ${ECHO} "Soup made. Don't forget to eat it before it goes cold!"; \
	fi

eat eating eat_soup eating_soup: all
	@-if [[ -r .soup ]]; then \
	    ${ECHO} "That was mighty delicous soup but now you feel bloated! :-("; \
	    ${RM} -f .soup; \
	else \
	    ${ECHO} "How can you eat soup that you haven't made yet?"; \
	fi


###################################
# standard Makefile utility rules #
###################################

configure:
	@${ECHO} "nothing to $@. Try making some soup instead."

clean:
	${RM} -f *.o

clobber: clean
	${RM} -f .soup

install: all
	@${ECHO} "nothing to $@. Try eating the soup instead."

###############
# make depend #
###############

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
chk_sem_auth.o: chk_sem_auth.c chk_sem_auth.h ../json_sem.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h ../json_parse.h ../json_util.h
chk_sem_info.o: chk_sem_info.c chk_sem_info.h ../json_sem.h ../util.h ../dyn_array/dyn_array.h \
	../dyn_array/../dbg/dbg.h ../json_parse.h ../json_util.h
chk_validate.o: chk_validate.c chk_validate.h ../entry_util.h ../json_parse.h ../util.h \
	../dyn_array/dyn_array.h ../dyn_array/../dbg/dbg.h ../json_sem.h ../json_util.h chk_sem_auth.h \
	chk_sem_info.h ../entry_time.h ../location.h ../dbg/dbg.h
