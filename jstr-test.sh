#!/usr/bin/env bash
#
# jstr-test.sh - JSON string encoding and decoding test

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
export TEST_FILE="jstr-test.out"
export TEST_FILE2="jstr-test2.out"
if [[ -e "$TEST_FILE" ]]; then
    echo "$0: found $TEST_FILE move or remove before running this test" 1>&2
    exit 102
fi
if [[ -e "$TEST_FILE2" ]]; then
    echo "$0: found $TEST_FILE2 move or remove before running this test" 1>&2
    exit 103
fi
export EXIT_CODE=0
trap "rm -f \$TEST_FILE \$TEST_FILE2; exit" 0 1 2 3 15

# run the basic encoding test
#
echo "$0: about to run test #1"
./jstrencode -t
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
echo "./jstrencode -n < ./jstrencode | ./jstrdecode -n > $TEST_FILE"
# shellcheck disable=SC2094
./jstrencode -n < ./jstrencode | ./jstrdecode -n > "$TEST_FILE"
if cmp -s ./jstrencode "$TEST_FILE"; then
    echo "$0: test #2 passed"
else
    echo "$0: test #2 failed" 1>&2
    EXIT_CODE=2
fi
echo "$0: about to run test #3"
echo "./jstrencode -n < ./jstrdecode | ./jstrdecode -n > $TEST_FILE"
# shellcheck disable=SC2094
./jstrencode -n < ./jstrdecode | ./jstrdecode -n > "$TEST_FILE"
if cmp -s ./jstrdecode "$TEST_FILE"; then
    echo "$0: test #3 passed"
else
    echo "$0: test #3 failed" 1>&2
    EXIT_CODE=3
fi

# test JSON encoding and decoding pipe in strict mode
#
echo "$0: about to run test #4"
echo "./jstrencode -n < ./jstrencode | ./jstrdecode -n -s > $TEST_FILE"
# shellcheck disable=SC2094
./jstrencode -n < ./jstrencode | ./jstrdecode -n -s > "$TEST_FILE"
if cmp -s ./jstrencode "$TEST_FILE"; then
    echo "$0: test #4 passed"
else
    echo "$0: test #4 failed" 1>&2
    EXIT_CODE=4
fi
echo "$0: about to run test #5"
echo "./jstrencode -n < ./jstrdecode | ./jstrdecode -n -s > $TEST_FILE"
# shellcheck disable=SC2094
./jstrencode -n < ./jstrdecode | ./jstrdecode -n -s > "$TEST_FILE"
if cmp -s ./jstrdecode "$TEST_FILE"; then
    echo "$0: test #5 passed"
else
    echo "$0: test #5 failed" 1>&2
    EXIT_CODE=5
fi

# test some text foles in the encoding and decoding pipe
#
echo "$0: about to run test #6"
export SRC_SET="jstr-test.sh dbg.c dbg.h fnamchk.c iocccsize.c jauthchk.c"
SRC_SET="$SRC_SET jinfochk.c json.c json.h jstrdecode.c jstrencode.c"
SRC_SET="$SRC_SET limit_ioccc.h mkiocccentry.c txzchk.c util.c util.h"
echo "cat \$SRC_SET | ./jstrencode -n | ./jstrdecode -n > $TEST_FILE"
# shellcheck disable=SC2086
cat $SRC_SET | ./jstrencode -n | ./jstrdecode -n > "$TEST_FILE"
# shellcheck disable=SC2086
cat $SRC_SET > "$TEST_FILE2"
if cmp -s "$TEST_FILE2" "$TEST_FILE"; then
    echo "$0: test #6 passed"
else
    echo "$0: test #6 failed" 1>&2
    EXIT_CODE=6
fi

# test some text foles in the encoding and decoding pipe in strict mode
#
echo "$0: about to run test #7"
export SRC_SET="jstr-test.sh dbg.c dbg.h fnamchk.c iocccsize.c jauthchk.c"
SRC_SET="$SRC_SET jinfochk.c json.c json.h jstrdecode.c jstrencode.c"
SRC_SET="$SRC_SET limit_ioccc.h mkiocccentry.c txzchk.c util.c util.h"
echo "cat \$SRC_SET | ./jstrencode -n | ./jstrdecode -n -s > $TEST_FILE"
# shellcheck disable=SC2086
cat $SRC_SET | ./jstrencode -n | ./jstrdecode -n -s > "$TEST_FILE"
# shellcheck disable=SC2086
cat $SRC_SET > "$TEST_FILE2"
if cmp -s "$TEST_FILE2" "$TEST_FILE"; then
    echo "$0: test #7 passed"
else
    echo "$0: test #7 failed" 1>&2
    EXIT_CODE=7
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
