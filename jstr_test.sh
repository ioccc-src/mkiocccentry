#!/usr/bin/env bash
#
# jstr_test.sh - JSON string encoding and decoding test

export JSTRENCODE="./jstrencode"
export JSTRDECODE="./jstrdecode"
export TEST_FILE="jstr_test.out"
export TEST_FILE2="jstr_test2.out"

export USAGE="usage: $0 [-h] [-v level] [-e jstrencode] [-d jstrdecode]

    -h		print help and exit 1
    -v		set verbosity level for this script (def level: 0)
    -e		path to jstrencode tool (def: $JSTRENCODE)
    -d		path to jstrdecode tool (def: $JSTRDECODE)

Exit codes:
     0	 all is well
     1	 test failed
     2	 invalid command line, invalid option or option missing an argument
     3	 help message printed
 >= 10	 internal error
"
# parse args
#
export V_FLAG="0"
while getopts :hv:e:d: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 3
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    e) JSTRENCODE="$OPTARG"
       ;;
    d) JSTRDECODE="$OPTARG"
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

eval make all 2>&1 | grep -v 'Nothing to be done for'

# verify we have the tools and conditions to test
#
if [[ ! -x "$JSTRENCODE" ]]; then
    echo "$0: ERROR: missing jstrencode tool: $JSTRENCODE" 1>&2
    exit 10
fi
if [[ ! -x "$JSTRDECODE" ]]; then
    echo "$0: ERROR: missing jstrdecode tool: $JSTRDECODE" 1>&2
    exit 11
fi
# try removing the test files first
echo rm -f "$TEST_FILE $TEST_FILE2"
rm -f "$TEST_FILE $TEST_FILE2"

# if the files still exist let the user know and don't proceed
if [[ -e "$TEST_FILE" ]]; then
    echo "$0: ERROR: found $TEST_FILE; move or remove before running this test" 1>&2
    exit 12
fi
if [[ -e "$TEST_FILE2" ]]; then
    echo "$0: ERROR: found $TEST_FILE2; move or remove before running this test" 1>&2
    exit 13
fi
export EXIT_CODE=0
trap "rm -f \$TEST_FILE \$TEST_FILE2; exit"  0 1 2 3 15

# run the basic encoding test
#
echo "$0: about to run test #0"
"$JSTRENCODE" -v "$V_FLAG" -t
status="$?"
if [[ $status -eq 0 ]]; then
    echo "$0: test #0 passed"
else
    echo "$0: test #0 failed" 1>&2
    EXIT_CODE=42
fi

# test JSON encoding and decoding pipe
#
echo "$0: about to run test #1"
echo "$JSTRENCODE -v $V_FLAG -n < $JSTRENCODE -v $V_FLAG |  $JSTRDECODE -v $V_FLAG -n > $TEST_FILE"
# shellcheck disable=SC2094
"$JSTRENCODE" -v "$V_FLAG" -n < "$JSTRENCODE" -v "$V_FLAG" | $JSTRDECODE -v "$V_FLAG" -n > "$TEST_FILE"
if cmp -s "$JSTRENCODE" "$TEST_FILE"; then
    echo "$0: test #1 passed"
else
    echo "$0: test #1 failed" 1>&2
    EXIT_CODE=43
fi
echo "$0: about to run test #2"
echo "$JSTRENCODE -v $V_FLAG -n < $JSTRDECODE -v $V_FLAG | $JSTRDECODE -v $V_FLAG -n > $TEST_FILE"
# shellcheck disable=SC2094
"$JSTRENCODE" -v "$V_FLAG" -n < "$JSTRDECODE" -v "$V_FLAG" | "$JSTRDECODE" -v "$V_FLAG" -n > "$TEST_FILE"
if cmp -s "$JSTRDECODE" "$TEST_FILE"; then
    echo "$0: test #2 passed"
else
    echo "$0: test #2 failed" 1>&2
    EXIT_CODE=44
fi

# test some text holes in the encoding and decoding pipe
#
echo "$0: about to run test #3"
export SRC_SET="jstr_test.sh dbg.c dbg.h fnamchk.c iocccsize.c"
SRC_SET="$SRC_SET chkentry.c json_parse.c json_parse.h jstrdecode.c jstrencode.c"
SRC_SET="$SRC_SET limit_ioccc.h mkiocccentry.c txzchk.c util.c util.h"
echo "cat \$SRC_SET | $JSTRENCODE -v $V_FLAG -n | $JSTRDECODE -v $V_FLAG -n > $TEST_FILE"
# shellcheck disable=SC2086
# shellcheck disable=SC2002
cat $SRC_SET | $JSTRENCODE -v "$V_FLAG" -n | $JSTRDECODE -v "$V_FLAG" -n > "$TEST_FILE"
# shellcheck disable=SC2086
cat $SRC_SET > "$TEST_FILE2"
if cmp -s "$TEST_FILE2" "$TEST_FILE"; then
    echo "$0: test #3 passed"
else
    echo "$0: test #3 failed" 1>&2
    EXIT_CODE=45
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
