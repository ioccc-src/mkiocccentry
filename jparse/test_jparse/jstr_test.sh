#!/usr/bin/env bash
#
# jstr_test.sh - JSON string encoding and decoding test
#
# "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
#
# This JSON parser was co-developed in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# "Share and Enjoy!"
#     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)

# setup
#
export JSTRENCODE="./jparse/jstrencode"
export JSTRDECODE="./jparse/jstrdecode"
export TEST_FILE="./jparse/test_jparse/jstr_test.out"
export TEST_FILE2="./jparse/test_jparse/jstr_test2.out"
export JSTR_TEST_VERSION="1.0.1 2024-03-02"
export TOPDIR=

export USAGE="usage: $0 [-h] [-V] [-v level] [-e jstrencode] [-d jstrdecode] [-Z topdir]

    -h		print help and exit
    -V		print version and exit
    -v		set verbosity level for this script (def level: 0)
    -e		path to jstrencode tool (def: $JSTRENCODE)
    -d		path to jstrdecode tool (def: $JSTRDECODE)
    -Z topdir	top level build directory (def: try . or ..)

Exit codes:
     0	 all tests OK
     2	 help or version printed
     3	 invalid command line, invalid option or option missing an argument
     4	 test #0 failed
     5	 test #1 failed
     6	 test #2 failed
     7	 test #3 failed
     8	 missing or non-executable jstrencode
     9	 missing or non-executable jstrdecode
  >= 10	 internal error

jstr_test.sh version: $JSTR_TEST_VERSION"

# parse args
#
export V_FLAG="0"
while getopts :hVv:e:d:Z: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$JSTR_TEST_VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    e)	JSTRENCODE="$OPTARG"
	;;
    d)	JSTRDECODE="$OPTARG"
	;;
    Z)	TOPDIR="$OPTARG";
	;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
    :)	echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
       ;;
   *)
       ;;
    esac
done

# change to the top level directory as needed
#
if [[ -n $TOPDIR ]]; then
    if [[ ! -d $TOPDIR ]]; then
	echo "$0: ERROR: -Z $TOPDIR given: not a directory: $TOPDIR" 1>&2
	exit 3
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: -Z $TOPDIR given, about to cd $TOPDIR" 1>&2
    fi
    # SC2164 (warning): Use 'cd ... || exit' or 'cd ... || return' in case cd fails.
    # https://www.shellcheck.net/wiki/SC2164
    # shellcheck disable=SC2164
    cd "$TOPDIR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: -Z $TOPDIR given: cd $TOPDIR exit code: $status" 1>&2
	exit 3
    fi
elif [[ -f mkiocccentry.c ]]; then
    TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume TOPDIR is .: $TOPDIR" 1>&2
    fi
elif [[ -f ../mkiocccentry.c ]]; then
    cd ..
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cd .. exit code: $status" 1>&2
	exit 3
    fi
    TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume TOPDIR is ..: $TOPDIR" 1>&2
    fi
else
    echo "$0: ERROR: cannot determine TOPDIR, use -Z topdir" 1>&2
    exit 3
fi
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: TOPDIR is the current directory: $TOPDIR" 1>&2
fi

# verify we have the tools and conditions to test
#
if [[ ! -x "$JSTRENCODE" ]]; then
    echo "$0: ERROR: missing jstrencode tool: $JSTRENCODE" 1>&2
    exit 8
fi
if [[ ! -x "$JSTRDECODE" ]]; then
    echo "$0: ERROR: missing jstrdecode tool: $JSTRDECODE" 1>&2
    exit 9
fi

# try removing the test files first
#
echo rm -f "$TEST_FILE $TEST_FILE2"
rm -f "$TEST_FILE $TEST_FILE2"

# if the files still exist let the user know and don't proceed
#
if [[ -e "$TEST_FILE" ]]; then
    echo "$0: ERROR: found $TEST_FILE; move or remove before running this test" 1>&2
    exit 10
fi
if [[ -e "$TEST_FILE2" ]]; then
    echo "$0: ERROR: found $TEST_FILE2; move or remove before running this test" 1>&2
    exit 11
fi
export EXIT_CODE=0
trap "rm -f \$TEST_FILE \$TEST_FILE2; exit" 0 1 2 3 15

# run the basic encoding test
#
echo "$0: about to run test #0"
"$JSTRENCODE" -v "$V_FLAG" -t
status="$?"
if [[ $status -eq 0 ]]; then
    echo "$0: test #0 passed"
else
    echo "$0: test #0 failed" 1>&2
    EXIT_CODE=4
fi

# test JSON encoding and decoding pipe
#
echo "$0: about to run test #1"
echo "$JSTRENCODE -v $V_FLAG -n < $JSTRENCODE | $JSTRDECODE -v $V_FLAG -n > $TEST_FILE"
# This warning is not correct in our case:
#
# SC2094 (info): Make sure not to read and write the same file in the same pipeline.
# https://www.shellcheck.net/wiki/SC2094
# shellcheck disable=SC2094
"$JSTRENCODE" -v "$V_FLAG" -n < "$JSTRENCODE" | $JSTRDECODE -v "$V_FLAG" -n > "$TEST_FILE"
if cmp -s "$JSTRENCODE" "$TEST_FILE"; then
    echo "$0: test #1 passed"
else
    echo "$0: test #1 failed" 1>&2
    EXIT_CODE=5
fi
echo "$0: about to run test #2"
echo "$JSTRENCODE -v $V_FLAG -n < $JSTRDECODE | $JSTRDECODE -v $V_FLAG -n > $TEST_FILE"
# This warning is incorrect in our case:
#
# SC2094 (info): Make sure not to read and write the same file in the same pipeline.
# https://www.shellcheck.net/wiki/SC2094
# shellcheck disable=SC2094
"$JSTRENCODE" -v "$V_FLAG" -n < "$JSTRDECODE" | "$JSTRDECODE" -v "$V_FLAG" -n > "$TEST_FILE"
if cmp -s "$JSTRDECODE" "$TEST_FILE"; then
    echo "$0: test #2 passed"
else
    echo "$0: test #2 failed" 1>&2
    EXIT_CODE=6
fi

# test some text holes in the encoding and decoding pipe
#
echo "$0: about to run test #3"
export SRC_SET="$0 dbg/dbg.c dbg/dbg.h test_ioccc/fnamchk.c iocccsize.c"
SRC_SET="$SRC_SET chkentry.c ./jparse/json_parse.c ./jparse/json_parse.h ./jparse/jstrdecode.c ./jparse/jstrencode.c"
SRC_SET="$SRC_SET soup/limit_ioccc.h mkiocccentry.c txzchk.c jparse/util.c jparse/util.h"
echo "cat \$SRC_SET | $JSTRENCODE -v $V_FLAG -n | $JSTRDECODE -v $V_FLAG -n > $TEST_FILE"
# We cannot double-quote $SRC_SET because doing so would make the shell try
# catting the list of files as a single file name which obviously would not work
# so we disable the following check:
#
# And although this issue won't be detected until the previous test is resolved
# (or in our case ignored) the below is not a useless use of cat. If we were to
# reorder the command line to say:
#
#   "$JSTRENCODE" -v "$V_FLAG" -n | "$JSTRDECODE" -v "$V_FLAG" -n < $SRC_SET > "$TEST_FILE"
#
# we would see something like:
#
#	./jstr_test.sh: line 139: $SRC_SET: ambiguous redirect
#
# and if we were to reorder the command line to say:
#
#   < $SRC_SET "$JSTRENCODE" -v "$V_FLAG" -n | "$JSTRDECODE" -v "$V_FLAG" -n > "$TEST_FILE"
#
# we would see similar. There might be a way to do it but the typical ways won't
# work so we disable the useless use of cat check SC2002 as well.
#
# SC2086 (info): Double quote to prevent globbing and word splitting.
# https://www.shellcheck.net/wiki/SC2086
# shellcheck disable=SC2086
cat $SRC_SET | "$JSTRENCODE" -v "$V_FLAG" -n | "$JSTRDECODE" -v "$V_FLAG" -n > "$TEST_FILE"
# copy the PIPESTATUS array as the following command will destroy its contents
STATUS=("${PIPESTATUS[@]}")
# test each part of the pipeline
ERROR=
for status in "${STATUS[@]}"; do
    if [[ "$status" -ne 0 ]]; then
	echo "$0: test #3 failed" 1>&2
	EXIT_CODE=7
	ERROR=1
	break
    fi
done
if [[ -z "$ERROR" ]]; then
    # We cannot double quote "$SRC_SET" because it would make the shell think it's a
    # single file which of course does not exist by that name as it's actually a
    # list of files. Thus we disable shellcheck check SC2086.
    #
    # SC2086 (info): Double quote to prevent globbing and word splitting.
    # https://www.shellcheck.net/wiki/SC2086
    # shellcheck disable=SC2086
    if ! cat $SRC_SET > "$TEST_FILE2"; then
	echo "$0: test #3 failed" 1>&2
	EXIT_CODE=7
    elif cmp -s "$TEST_FILE2" "$TEST_FILE"; then
	echo "$0: test #3 passed"
    else
	echo "$0: test #3 failed" 1>&2
	EXIT_CODE=7
    fi
fi

# All Done!!! -- Jessica Noll, Age 2
#
if [[ $EXIT_CODE == 0 ]]; then
    echo "$0: all tests passed"
else
    echo "$0: ERROR: test failure detected, about to exit: $EXIT_CODE" 1>&2
fi
rm -f "$TEST_FILE" "$TEST_FILE2"
exit "$EXIT_CODE"
