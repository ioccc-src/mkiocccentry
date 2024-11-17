#!/usr/bin/env bash
#
# jstr_test.sh - JSON string decoding and encoding tests
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
export JSTRDECODE="./jstrdecode"
export JSTRENCODE="./jstrencode"
export TEST_FILE="./test_jparse/jstr_test.out"
export TEST_FILE2="./test_jparse/jstr_test2.out"
export JSTR_TEST_TXT="./test_jparse/jstr_test.txt"
export JSTR_TEST_VERSION="1.2.3 2024-11-17" # version format: major.minor YYYY-MM-DD
export TOPDIR=

export USAGE="usage: $0 [-h] [-V] [-v level] [-e jstrencode] [-d jstrdecode] [-Z topdir]

    -h		print help and exit
    -V		print version and exit
    -v		set verbosity level for this script (def level: 0)
    -e		path to jstrdecode tool (def: $JSTRDECODE)
    -d		path to jstrencode tool (def: $JSTRENCODE)
    -Z topdir	top level build directory (def: try . or ..)

Exit codes:
     0	 all tests OK
     2	 help or version printed
     3	 invalid command line, invalid option or option missing an argument
     4	 one or more tests failed
     5	 missing or non-executable jstrencode
     6	 missing or non-executable jstrdecode
     7   missing or unreadable jstr_test.txt file
  >= 10	 internal error

jstr_test.sh version: $JSTR_TEST_VERSION"

# parse args
#
export V_FLAG=0
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
    e)	JSTRDECODE="$OPTARG"
	;;
    d)	JSTRENCODE="$OPTARG"
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
elif [[ -f jparse.c ]]; then
    TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume TOPDIR is .: $TOPDIR" 1>&2
    fi
elif [[ -f ../jparse.c ]]; then
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
elif [[ -f ../../jparse.c ]]; then
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
if [[ ! -x "$JSTRDECODE" ]]; then
    echo "$0: ERROR: missing jstrdecode tool: $JSTRDECODE" 1>&2
    exit 5
fi
if [[ ! -x "$JSTRENCODE" ]]; then
    echo "$0: ERROR: missing jstrencode tool: $JSTRENCODE" 1>&2
    exit 6
fi

if [[ ! -r "$JSTR_TEST_TXT" ]]; then
    echo "$0: ERROR: file jstr_test.txt missing or not regular readable file" 1>&2
    exit 7
fi

# try removing the test files first
#
# NOTE: do NOT remove the jstr_test.txt file!
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

# run the basic decoding test
#
echo "$0: about to run test #0" 1>&2
"$JSTRENCODE" -v "$V_FLAG" -t
status="$?"
if [[ $status -eq 0 ]]; then
    echo "$0: test #0 passed" 1>&2
else
    echo "$0: test #0 failed" 1>&2
    EXIT_CODE=4
fi

# test JSON decoding and encoding pipe
echo "$0: about to run test #1"
echo "$JSTRENCODE -v $V_FLAG -n < $JSTRENCODE | $JSTRDECODE -v $V_FLAG -n > $TEST_FILE"
# This warning is not correct in our case:
#
# SC2094 (info): Make sure not to read and write the same file in the same pipeline.
# https://www.shellcheck.net/wiki/SC2094
# shellcheck disable=SC2094
"$JSTRENCODE" -v "$V_FLAG" -n < "$JSTRENCODE" | $JSTRDECODE -v "$V_FLAG" -n > "$TEST_FILE"
if cmp -s "$JSTRENCODE" "$TEST_FILE"; then
    echo "$0: test #1 passed" 1>&2
else
    echo "$0: test #1 failed" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #2"
echo "$JSTRENCODE -v $V_FLAG -n < $JSTRDECODE | $JSTRDECODE -v $V_FLAG -n > $TEST_FILE"
# This warning is incorrect in our case:
#
# SC2094 (info): Make sure not to read and write the same file in the same pipeline.
# https://www.shellcheck.net/wiki/SC2094
# shellcheck disable=SC2094
#
"$JSTRENCODE" -v "$V_FLAG" -n < "$JSTRDECODE" | "$JSTRDECODE" -v "$V_FLAG" -n > "$TEST_FILE"
if cmp -s "$JSTRDECODE" "$TEST_FILE"; then
    echo "$0: test #2 passed"
else
    echo "$0: test #2 failed" 1>&2
    EXIT_CODE=4
fi

# test some text holes in the decoding and encoding pipe
#
echo "$0: about to run test #3" 1>&2
export SRC_SET="jparse.c json_parse.c json_parse.h jstrdecode.c jstrencode.c util.h util.c"
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
#

ERROR=
for status in "${STATUS[@]}"; do
    if [[ "$status" -ne 0 ]]; then
	echo "$0: test #3 failed" 1>&2
	EXIT_CODE=4
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
	EXIT_CODE=4
    elif cmp -s "$TEST_FILE2" "$TEST_FILE"; then
	echo "$0: test #3 passed" 1>&2
    else
	echo "$0: test #3 failed" 1>&2
	EXIT_CODE=4
    fi
fi

echo "$0: about to run test #4" 1>&2
echo "$JSTRDECODE -d -Q -n foo bar"
EXPECTED='"foobar"'
RESULT="$($JSTRDECODE -d -Q -n foo bar)"
if [[ "$RESULT" = "$EXPECTED" ]]; then
    echo "$0: test #4 passed" 1>&2
else
    echo "$0: test #4 failed" 1>&2
    echo "$0: test #4 EXPECTED: $EXPECTED"  1>&2
    echo "$0: test #4 RESULT: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #5" 1>&2
echo "$JSTRDECODE -d -Q -e -n foo bar"
EXPECTED='"\"foo\"\"bar\""'
RESULT="$($JSTRDECODE -d -Q -e -n foo bar)"
if [[ "$RESULT" = "$EXPECTED" ]]; then
    echo "$0: test #5 passed" 1>&2
else
    echo "$0: test #5 failed" 1>&2
    echo "$0: test #5 EXPECTED: $EXPECTED"  1>&2
    echo "$0: test #5 RESULT: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #6" 1>&2
echo "$JSTRDECODE -d -e -n foo bar"
EXPECTED='\"foo\"\"bar\"'
RESULT="$($JSTRDECODE -d -e -n foo bar)"
if [[ "$RESULT" = "$EXPECTED" ]]; then
    echo "$0: test #6 passed" 1>&2
else
    echo "$0: test #6 failed" 1>&2
    echo "$0: test #6 EXPECTED: $EXPECTED"  1>&2
    echo "$0: test #6 RESULT: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #7" 1>&2
echo "$JSTRDECODE -d '\\u0153\\u00df\\u00e5\\u00e9'" 1>&2
"$JSTRDECODE" -d "\\u0153\\u00df\\u00e5\\u00e9" > "$TEST_FILE"
if cmp "$JSTR_TEST_TXT" "$TEST_FILE"; then
    echo "$0: test #7 passed" 1>&2
else
    echo "$0: test #7 failed: result: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #8" 1>&2
echo "$JSTRDECODE" "$("$JSTRENCODE" '\\u0153\\u00df\\u00e5\\u00e9')" 1>&2
EXPECTED="\\u0153\\u00df\\u00e5\\u00e9"
RESULT=$("$JSTRDECODE" "$("$JSTRENCODE" '\u0153\u00df\u00e5\u00e9')")
if [[ "$RESULT" = "$EXPECTED" ]]; then
    echo "$0: test #8 passed" 1>&2
else
    echo "$0: test #8 failed: result: $RESULT" 1>&2
    echo "$0: test #8 EXPECTED: $EXPECTED"  1>&2
    echo "$0: test #8 RESULT: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #9" 1>&2
echo "$JSTRDECODE" "$("$JSTRENCODE" "$("$JSTRDECODE" -d '\\u0153\\u00df\\u00e5\\u00e9'))")"
"$JSTRDECODE" "$("$JSTRENCODE" "$("$JSTRDECODE" -d '\u0153\u00df\u00e5\u00e9')")" > "$TEST_FILE"
if cmp "$JSTR_TEST_TXT" "$TEST_FILE"; then
    echo "$0: test #9 passed" 1>&2
else
    echo "$0: test #9 failed: result: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #10" 1>&2
echo "$JSTRDECODE -d -n foo bar"
EXPECTED='foobar'
RESULT="$($JSTRDECODE -d -n foo bar)"
if [[ "$RESULT" = "$EXPECTED" ]]; then
    echo "$0: test #10 passed" 1>&2
else
    echo "$0: test #10 failed" 1>&2
    echo "$0: test #10 EXPECTED: $EXPECTED"  1>&2
    echo "$0: test #10 RESULT: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #11" 1>&2
echo "$JSTRDECODE -d -Q -n foo"
EXPECTED='"foo"'
RESULT="$($JSTRDECODE -d -Q -n foo)"
if [[ "$RESULT" = "$EXPECTED" ]]; then
    echo "$0: test #11 passed" 1>&2
else
    echo "$0: test #11 failed" 1>&2
    echo "$0: test #11 EXPECTED: $EXPECTED"  1>&2
    echo "$0: test #11 RESULT: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #12" 1>&2
echo "$JSTRDECODE -d -Q -e -n foo"
EXPECTED='"foo"'
RESULT="$($JSTRDECODE -d -Q -e -n foo)"
if [[ "$RESULT" = "$EXPECTED" ]]; then
    echo "$0: test #12 passed" 1>&2
else
    echo "$0: test #12 failed" 1>&2
    echo "$0: test #12 EXPECTED: $EXPECTED"  1>&2
    echo "$0: test #12 RESULT: $RESULT" 1>&2
    EXIT_CODE=4
fi

echo "$0: about to run test #13" 1>&2
echo "$JSTRDECODE -d -e -n foo"
EXPECTED='foo'
RESULT="$($JSTRDECODE -d -e -n foo)"
if [[ "$RESULT" = "$EXPECTED" ]]; then
    echo "$0: test #13 passed" 1>&2
else
    echo "$0: test #13 failed" 1>&2
    echo "$0: test #13 EXPECTED: $EXPECTED"  1>&2
    echo "$0: test #13 RESULT: $RESULT" 1>&2
    EXIT_CODE=4
fi

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
if [[ $EXIT_CODE == 0 ]]; then
    echo "$0: all tests passed"
else
    echo "$0: ERROR: test failure detected, about to exit: $EXIT_CODE" 1>&2
fi
rm -f "$TEST_FILE" "$TEST_FILE2"
exit "$EXIT_CODE"
