#!/usr/bin/env make
#
# prog - an example Makefile
#
# IMPORTANT: You need to change this Makefile, including
#	     removing this comment!
#
# We suggest you rename this file to Makefile before using.
#
# Modify this Makefile as needed before submitting with your entry.

# utilities
#
SHELL= bash
CC= cc
RM= rm

# The standard to compile against
#
# Your IOCCC entry is free to require older C standards, or
# even not specify a C standard at all.  We suggest trying
# for -std=gnu17, but that is not a requirement and you won't
# be penalized if you name CSTD empty or use another
# well known and reasonably widely implemented C standard.
#
CSTD= -std=gnu17

# How to optimize and set debug levels
#
COPT= -O3 -g3

# How to handle compiler warnings
#
CWARN= -Wall -Wextra -pedantic

# Add various -Wno-foo flags to CSILENCE to silence,
# if needed, common compiler warnings.
#
CSILENCE=

# Add -Dfoo line defines here
#
CDEFINE=

# other compiler options
#
COTHER=

# compile and link options
#
CFLAGS= ${CSTD} ${COPT} ${CWARN} ${CSILENCE} ${CDEFINE} ${COTHER}
LDFLAGS=

# list any data files supplied with the entry
#
DATA=

# default rule - compile the program
#
# The all rule should be the first rule.
#
all: prog ${DATA}

# list of rules that are not file targets
#
.PHONY: all clean clobber install try

# compile the program
#
prog: prog.c Makefile
	${CC} ${CFLAGS} prog.c ${LDFLAGS} -o $@

# remove any intermediate files created while compiling
#
clean:
	${RM} -f prog.o
	${RM} -rf prog.dSYM

# restore directory to original submission state
#
clobber: clean
	${RM} -f prog

# do not install
#
install: all
	@echo you have got to be kidding

# one suggested way to run the program
#
try: prog ${DATA}
	@# remove these comments
	@# notice how we do not assume . is in the path
	@# change this next line as needed
	./prog some arguments ${DATA}
