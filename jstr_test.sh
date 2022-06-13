#!/usr/bin/env bash
#
# jstr_test.sh - JSON string encoding and decoding test

# parse args
#
export V_FLAG="0"
while getopts :hv: flag; do
    case "$flag" in
    h) echo "usage: $0 [-h] [-v level]" 1>&2
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    \?) echo "invalid option: -$OPTARG" 1>&2
       exit 3
       ;;
    :) echo "option -$OPTARG requires an argument" 1>&2
       exit 4
       ;;
   *)
       ;;
    esac
done

# verify we have the tools and conditions to test
#
if [[ ! -x jstrencode ]]; then
    echo "$0: missing jstrencode tool" 1>&2
    exit 100
fi
if [[ ! -x jstrdecode ]]; then
    echo "$0: missing jstrdecode tool" 1>&2
    exit 101
fi
export TEST_FILE="jstr_test.out"
export TEST_FILE2="jstr_test2.out"
if [[ -e "$TEST_FILE" ]]; then
    echo "$0: found $TEST_FILE; move or remove before running this test" 1>&2
    exit 102
fi
if [[ -e "$TEST_FILE2" ]]; then
    echo "$0: found $TEST_FILE2; move or remove before running this test" 1>&2
    exit 103
fi
export EXIT_CODE=0
trap "rm -f \$TEST_FILE \$TEST_FILE2; exit" 0 1 2 3 15

# run the basic encoding test
#
echo "$0: about to run test #1"
./jstrencode -v "$V_FLAG" -t
status="$?"
if [[ $status -eq 0 ]]; then
    echo "$0: test #1 passed"
else
    echo "$0: test #1 failed" 1>&2
    EXIT_CODE=1
fi

# test JSON encoding and decoding pipe
#
echo "$0: about to run test #2"
echo "./jstrencode -v $V_FLAG -n < ./jstrencode -v $V_FLAG | ./jstrdecode -v $V_FLAG -n > $TEST_FILE"
# shellcheck disable=SC2094
./jstrencode -v "$V_FLAG" -n < ./jstrencode -v "$V_FLAG" | ./jstrdecode -v "$V_FLAG" -n > "$TEST_FILE"
if cmp -s ./jstrencode "$TEST_FILE"; then
    echo "$0: test #2 passed"
else
    echo "$0: test #2 failed" 1>&2
    EXIT_CODE=2
fi
echo "$0: about to run test #3"
echo "./jstrencode -v $V_FLAG -n < ./jstrdecode -v $V_FLAG | ./jstrdecode -v $V_FLAG -n > $TEST_FILE"
# shellcheck disable=SC2094
./jstrencode -v "$V_FLAG" -n < ./jstrdecode -v "$V_FLAG" | ./jstrdecode -v "$V_FLAG" -n > "$TEST_FILE"
if cmp -s ./jstrdecode "$TEST_FILE"; then
    echo "$0: test #3 passed"
else
    echo "$0: test #3 failed" 1>&2
    EXIT_CODE=3
fi

# test some text holes in the encoding and decoding pipe
#
echo "$0: about to run test #6"
export SRC_SET="jstr_test.sh dbg.c dbg.h fnamchk.c iocccsize.c chkauth.c"
SRC_SET="$SRC_SET chkinfo.c json_parse.c json_parse.h jstrdecode.c jstrencode.c"
SRC_SET="$SRC_SET limit_ioccc.h mkiocccentry.c txzchk.c util.c util.h"
echo "cat \$SRC_SET | ./jstrencode -v $V_FLAG -n | ./jstrdecode -v $V_FLAG -n > $TEST_FILE"
# shellcheck disable=SC2086
# shellcheck disable=SC2002
cat $SRC_SET | ./jstrencode -v "$V_FLAG" -n | ./jstrdecode -v "$V_FLAG" -n > "$TEST_FILE"
# shellcheck disable=SC2086
cat $SRC_SET > "$TEST_FILE2"
if cmp -s "$TEST_FILE2" "$TEST_FILE"; then
    echo "$0: test #6 passed"
else
    echo "$0: test #6 failed" 1>&2
    EXIT_CODE=6
fi

# All Done!!! -- Jessica Noll, Age 2
#
if [[ $EXIT_CODE == 0 ]]; then
    echo "$0: all tests passed"
else
    echo "$0: test failure detected, about to exit: $EXIT_CODE" 1>&2
fi
rm -f "$TEST_FILE" "$TEST_FILE2"
exit "$EXIT_CODE"
