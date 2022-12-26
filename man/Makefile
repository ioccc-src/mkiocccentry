#!/usr/bin/make
#
# man - man pages for the mkiocccentry repo
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


#############
# utilities #
#############

# suggestion: List utility filenames, not paths.
#	      Do not list shell builtin (echo, cd, ...) tools.
#	      Keep the list in alphabetical order.
#
CHECKNR= checknr
CP= cp
INSTALL= install
MAKE= make
RM= rm
SHELL= bash


##################
# How to compile #
##################


###############
# source code #
###############

# source files that are permanent (not made, nor removed)
#
C_SRC=
H_SRC=

# source files that do not conform to strict picky standards
#
LESS_PICKY_CSRC=
LESS_PICKY_HSRC=

# all shell scripts
#
SH_FILES=

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

# man pages
#
MAN1_TARGETS= man1/bug_report.1 man1/chkentry.1 man1/fnamchk.1 man1/hostchk.1 man1/iocccsize.1 \
	      man1/jparse.1 man1/jstrdecode.1 man1/jstrencode.1 man1/mkiocccentry.1 man1/run_bison.1 \
	      man1/run_flex.1 man1/txzchk.1
MAN3_TARGETS= man3/dbg.3 man3/dyn_array.3 man3/err.3 man3/msg.3 man3/warn.3 man3/werr.3 man3/printf_usage.3 \
	      man3/warn_or_err.3
MAN8_TARGETS= man8/all_ref.8 man8/chkentry_test.8 man8/ioccc_test.8 man8/iocccsize_test.8 man8/jnum_chk.8 \
	      man8/jnum_gen.8 man8/jparse_test.8 man8/jsemcgen.8 man8/jsemtblgen.8 man8/jstr_test.8 \
	      man8/mkiocccentry_test.8 man8/reset_tstamp.8 man8/run_usage.8 man8/txzchk_test.8 man8/utf8_test.8 \
	      man8/verge.8 man8/vermod.8
ALL_MAN_TARGETS= ${MAN1_TARGETS} ${MAN3_TARGETS} ${MAN8_TARGETS}

# These man pages are built and removed by make clobber
#
BUILT_MAN_TARGETS= man3/dbg.3 man3/err.3 man3/msg.3 man3/warn.3 man3/werr.3 man3/printf_usage.3 man3/warn_or_err.3 \
		   man3/dyn_array.3 man1/jparse.1 man8/jparse_test.8

# libraries
#
LIBA_TARGETS=

# include files
#
H_SRC_TARGETS=

# what to make by all but NOT to removed by clobber (because they are not files)
#
ALL_OTHER_TARGETS= extern_everything ${BUILT_MAN_TARGETS}

# what to make by all and removed by clobber (and thus not ${ALL_OTHER_TARGETS})
#
TARGETS= ${LIBA_TARGETS} ${BUILT_MAN_TARGETS}


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
	test check_man prep_clean prep_clobber legacy_clean legacy_clobber \
	extern_include extern_objs extern_liba extern_man extern_prog extern_everything \
	configure clean clobber install


####################################
# things to make in this directory #
####################################

man3/dbg.3: ../dbg/man/man3/dbg.3
	@${RM} -f $@
	@${CP} -f $^ $@

man3/err.3: ../dbg/man/man3/err.3
	@${RM} -f $@
	@${CP} -f $^ $@

man3/printf_usage.3: ../dbg/man/man3/printf_usage.3
	@${RM} -f $@
	@${CP} -f $^ $@

man3/warn.3: ../dbg/man/man3/warn.3
	@${RM} -f $@
	@${CP} -f $^ $@

man3/warn_or_err.3: ../dbg/man/man3/warn_or_err.3
	@${RM} -f $@
	@${CP} -f $^ $@

man3/msg.3: ../dbg/man/man3/msg.3
	@${RM} -f $@
	@${CP} -f $^ $@

man3/werr.3: ../dbg/man/man3/werr.3
	@${RM} -f $@
	@${CP} -f $^ $@

man3/dyn_array.3: ../dyn_array/man/man3/dyn_array.3
	@${RM} -f $@
	@${CP} -f $^ $@

man1/jparse.1: ../jparse/man/man1/jparse.1
	@${RM} -f $@
	@${CP} -f $^ $@

man8/jparse_test.8: ../jparse/man/man8/jparse_test.8
	@${RM} -f $@
	@${CP} -f $^ $@


#########################################################
# rules that invoke Makefile rules in other directories #
#########################################################

../dbg/dbg.3:
	@${MAKE} -C ../dbg man3/dbg.3

../dyn_array/dyn_array.3:
	@${MAKE} -C ../dyn_array man3/dyn_array.3

../jparse/man/jparse.1:
	@${MAKE} -C ../jparse man1/jparse.1

../jparse/man/jparse_test.8:
	@${MAKE} -C ../jparse man8/jparse_test.8


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


###########################################################
# repo tools - rules for those who maintain the this repo #
###########################################################

test: check_man Makefile
	@:

# sequence exit codes
#
seqcexit:
	@:

picky: ${ALL_SRC}
	@:

# inspect and verify shell scripts
#
shellcheck:
	@:

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

# vi/vim tags
#
tags: ${ALL_CSRC} ${ALL_HSRC}
	@:

prep_clean: legacy_clean
	@:

prep_clobber: prep_clean legacy_clobber
	@:

legacy_clean:
	@:

legacy_clobber: legacy_clean
	${RM} -f *.html


###################################
# standard Makefile utility rules #
###################################

configure:
	@echo nothing to $@

clean:
	@:

clobber: clean
	${RM} -f ${TARGETS}

install: all
#none#	${INSTALL} -v -d -m 0775 ${DEST_LIB}
#none#	${INSTALL} -v -d -m 0775 ${DEST_DIR}
	${INSTALL} -v -d -m 0775 ${MAN1_DIR}
	${INSTALL} -v -d -m 0775 ${MAN3_DIR}
	${INSTALL} -v -d -m 0775 ${MAN8_DIR}
	${INSTALL} -v -d -m 0775 ${DEST_INCLUDE}
	${INSTALL} -v -m 0444 ${MAN1_TARGETS} ${MAN1_DIR}
	${INSTALL} -v -m 0444 ${MAN3_TARGETS} ${MAN3_DIR}
	${INSTALL} -v -m 0444 ${MAN8_TARGETS} ${MAN8_DIR}
#none#	${INSTALL} -v -m 0444 ${LIBA_TARGETS} ${DEST_LIB}
#none#	${INSTALL} -v -m 0444 ${H_SRC_TARGETS} ${DEST_INCLUDE}


###############
# make depend #
###############

depend:
	@:

### DO NOT CHANGE MANUALLY BEYOND THIS LINE
