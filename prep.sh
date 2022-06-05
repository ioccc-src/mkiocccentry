#!/usr/bin/env bash
#
# prep.sh - prep for a release - actions for make prep and make release
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

# setup
#
export USAGE="usage: $0 [-h] [-v level] [-V] [-e] [-o] [-m make] [-M Makefile]

    -h              print help and exit 5
    -v level        flag ignored
    -V              print version and exit 5

    -e		    exit in first make action error (def: exit only at end)
    -o		    do NOT use backup files, fail if bison or flex cannot be used (def: use)
    -m make	    make command (def: make)
    -M Makefile	    path to Makefile (def: ./Makefile)

Exit codes:
    0    All is OK for both bison and flex
    1    -h and help string printed or -V and version string printed
    2	 command line error
    3	 Makefile not a readable file that exists
    4	 Internal function error
    >=10 some make action exited non-zero"
export MAKE="make"
export MAKEFILE="./Makefile"
export PREP_VERSION="0.1 2022-04-19"
export V_FLAG="0"
export E_FLAG=
export EXIT_CODE="0"
export O_FLAG=

# parse args
#
while getopts :hv:Veom:M: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 1
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    V) echo "$PREP_VERSION"
       exit 1
       ;;
    m) MAKE="$OPTARG";
       ;;
    M) MAKEFILE="$OPTARG";
       ;;
    e) E_FLAG="-e"
       ;;
    o) O_FLAG="-o"
       ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
       exit 2
       ;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
       exit 2
       ;;
   *)
       ;;
    esac
done

# check args
#
shift $(( OPTIND - 1 ));
if [[ $# -ne 0 ]]; then
    echo "$0: ERROR: expected 0 arguments, found $#" 1>&2
    exit 2
fi

# firewall
#
if [[ ! -e $MAKEFILE ]]; then
    echo "$0: ERROR: Makefile not found: $MAKEFILE" 1>&2
    exit 3
fi
if [[ ! -f $MAKEFILE ]]; then
    echo "$0: ERROR: Makefile not a file: $MAKEFILE" 1>&2
    exit 3
fi
if [[ ! -r $MAKEFILE ]]; then
    echo "$0: ERROR: Makefile not a readable file: $MAKEFILE" 1>&2
    exit 3
fi

# make action
#
# usage:
#	make_action code rule
#
#	code - exit code of rule fails
#	rule - Makefile rule to call
#
make_action() {

    # parse args
    #
    if [[ $# -ne 2 ]]; then
	echo "$0: ERROR: function expects 2 args, found $#" 1>&2
	exit 3
    fi
    local CODE="$1"
    local RULE="$2"

    # announce pre-action
    #
    echo "=-=-= Start: $MAKE $RULE =-=-="
    echo

    # perform action
    #
    echo "$MAKE" -f "$MAKEFILE" "$RULE"
    "$MAKE" -f "$MAKEFILE" "$RULE"
    status="$?"
    if [[ $status -ne 0 ]]; then

	# process a make action failure
	#
	EXIT_CODE="$CODE"
	echo
	echo "$0: Warning: EXIT_CODE is now: $EXIT_CODE" 1>&2
	if [[ -n $E_FLAG ]]; then
	    echo
	    echo "$0: FATAL: $MAKE -f $MAKEFILE $RULE exit status: $status" 1>&2
	    echo
	    echo "=-=-= FAIL: $MAKE $RULE =-=-="
	    echo
	    exit "$EXIT_CODE"
	else
	    echo
	    echo "$0: Warning: $MAKE -f $MAKEFILE $RULE exit status: $status" 1>&2
	    echo
	    echo "=-=-= FAIL: $MAKE $RULE =-=-="
	    echo
	fi

    # announce post-action
    #
    else
	echo
	echo "=-=-= PASS: $MAKE $RULE =-=-="
	echo
    fi
    return 0;
}

# perform make actions
#
echo "=-=-=-=-= Start: $0 =-=-=-=-="
echo
make_action 10 prep_clobber
make_action 11 seqcexit
make_action 12 use_ref
make_action 13 clean_generated_obj
make_action 14 all
if [[ -z $O_FLAG ]]; then
    make_action 15 parser
else
    make_action 15 parser-o
fi
make_action 16 all
make_action 17 depend
make_action 18 shellcheck
make_action 19 picky
make_action 20 test
if [[ $EXIT_CODE -eq 0 ]]; then
    echo "=-=-=-=-= PASS: $0 =-=-=-=-="
    echo
else
    echo "=-=-=-=-= FAIL: $0 =-=-=-=-="
    echo
    echo "=-=-=-=-= Will exit: $EXIT_CODE =-=-=-=-="
    echo
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit "$EXIT_CODE"
