#!/usr/bin/env bash
#
# ioccc_test.sh - perform the complete suite of tests for the mkiocccentry repo
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
export IOCCC_TEST_VERSION="0.462022-04-23"

export USAGE="usage: $0 [-h] [-v level] [-J json_level] [-V]

    -h              print help and exit 1
    -v level        set debug level (def: 0)
    -J json_level   set json debug level (def: 0)
    -V              print version and exit 1

Exit codes:
    0    all tests are OK

    1    -h and help string printed or -V and version string printed
    2    Command line usage error

    3    ./iocccsize_test.sh not found or not executable
    4	 ./dbg not found or not executable
    5    ./mkiocccentry_test.sh not found or not executable
    6    ./jstr_test.sh not found or not executable
    7    ./jnum_chk not found or not executable
    8    ./dyn_test not found or not executable
    9	 ./jparse_test.sh not found or not executable
    10	 ./json_teststr.txt not found or not readable
    11	 ./txzchk not found or not executable

    >=20  some test failed

$0 version: $IOCCC_TEST_VERSION"
export V_FLAG="0"
export J_FLAG="0"
export EXIT_CODE="0"
export FAILURE_SUMMARY=

# parse args
#
while getopts :hv:J:V flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 1
	;;
    v)	V_FLAG="$OPTARG";
	;;
    J)	J_FLAG="$OPTARG";
	;;
    V)	echo "$IOCCC_TEST_VERSION"
	exit 1
	;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
	exit 2
	;;
    :)	echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
	exit 2
	;;
   *)
	;;
    esac
done

# check args
#
shift $(( OPTIND - 1 ));
if [[ $# -gt 0 ]]; then
    echo "$0: ERROR: Expected 0 args, found: $#" 1>&2
    exit 2
fi

eval make all 2>&1 | grep -v 'Nothing to be done for'

# firewall - verify we have the required executables and needed data file(s)
#
# iocccsize_test.sh
if [[ ! -e ./iocccsize_test.sh ]]; then
    echo "$0: ERROR: ./iocccsize_test.sh file not found" 1>&2
    exit 3
fi
if [[ ! -f ./iocccsize_test.sh ]]; then
    echo "$0: ERROR: ./iocccsize_test.sh is not a regular file" 1>&2
    exit 3
fi
if [[ ! -x ./iocccsize_test.sh ]]; then
    echo "$0: ERROR: ./iocccsize_test.sh is not executable" 1>&2
    exit 3
fi
# dbg
if [[ ! -e ./dbg ]]; then
    echo "$0: ERROR: ./dbg file not found" 1>&2
    exit 4
fi
if [[ ! -f ./dbg ]]; then
    echo "$0: ERROR: ./dbg is not a regular file" 1>&2
    exit 4
fi
if [[ ! -x ./dbg ]]; then
    echo "$0: ERROR: ./dbg is not executable" 1>&2
    exit 4
fi
# mkiocccentry_test.sh
if [[ ! -e ./mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: ./mkiocccentry_test.sh file not found" 1>&2
    exit 5
fi
if [[ ! -f ./mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: ./mkiocccentry_test.sh is not a regular file" 1>&2
    exit 5
fi
if [[ ! -x ./mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: ./mkiocccentry_test.sh is not executable" 1>&2
    exit 5
fi
# jstr_test.sh
if [[ ! -e ./jstr_test.sh ]]; then
    echo "$0: ERROR: ./jstr_test.sh file not found" 1>&2
    exit 6
fi
if [[ ! -f ./jstr_test.sh ]]; then
    echo "$0: ERROR: ./jstr_test.sh is not a regular file" 1>&2
    exit 6
fi
if [[ ! -x ./jstr_test.sh ]]; then
    echo "$0: ERROR: ./jstr_test.sh is not executable" 1>&2
    exit 6
fi
# jnum_chk
if [[ ! -e ./jnum_chk ]]; then
    echo "$0: ERROR: ./jnum_chk file not found" 1>&2
    exit 7
fi
if [[ ! -f ./jnum_chk ]]; then
    echo "$0: ERROR: ./jnum_chk is not a regular file" 1>&2
    exit 7
fi
if [[ ! -x ./jnum_chk ]]; then
    echo "$0: ERROR: ./jnum_chk is not executable" 1>&2
    exit 7
fi
# dyn_test
if [[ ! -e ./dyn_test ]]; then
    echo "$0: ERROR: ./dyn_test file not found" 1>&2
    exit 8
fi
if [[ ! -f ./dyn_test ]]; then
    echo "$0: ERROR: ./dyn_test is not a regular file" 1>&2
    exit 8
fi
if [[ ! -x ./dyn_test ]]; then
    echo "$0: ERROR: ./dyn_test is not executable" 1>&2
    exit 8
fi
# jparse_test.sh
if [[ ! -e ./jparse_test.sh ]]; then
    echo "$0: ERROR: ./jparse_test.sh file not found" 1>&2
    exit 9
fi
if [[ ! -f ./jparse_test.sh ]]; then
    echo "$0: ERROR: ./jparse_test.sh is not a regular file" 1>&2
    exit 9
fi
if [[ ! -x ./jparse_test.sh ]]; then
    echo "$0: ERROR: ./jparse_test.sh is not executable" 1>&2
    exit 9
fi

# json_teststr.txt: for jparse_test.sh
if [[ ! -e ./json_teststr.txt ]]; then
    echo "$0: ERROR: json_teststr.txt file not found" 1>&2
    exit 10
fi
if [[ ! -f json_teststr.txt ]]; then
    echo "$0: ERROR: ./jparse_test.sh is not a regular file" 1>&2
    exit 10
fi
if [[ ! -r json_teststr.txt ]]; then
    echo "$0: ERROR: json_teststr.txt is not readable" 1>&2
    exit 10
fi
# txzchk
if [[ ! -e ./txzchk ]]; then
    echo "$0: ERROR: ./txzchk file not found" 1>&2
    exit 11
fi
if [[ ! -f ./txzchk ]]; then
    echo "$0: ERROR: ./txzchk is not a regular file" 1>&2
    exit 11
fi
if [[ ! -x ./txzchk ]]; then
    echo "$0: ERROR: ./txzchk is not executable" 1>&2
    exit 11
fi


# start the test suite
#
echo "Start test suite"
echo

# iocccsize_test.sh
#
echo "RUNNING: iocccsize_test.sh"
echo
echo "./iocccsize_test.sh -v 1"
./iocccsize_test.sh -v 1
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: iocccsize_test.sh non-zero exit code: $status" 1>&2
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    iocccsize_test.sh non-zero exit code: $status"
    EXIT_CODE="20"
else
    echo
    echo "PASSED: iocccsize_test.sh"
fi

# dbg
#
echo
echo "RUNNING: dbg"
rm -f dbg.out
echo
echo "./dbg -e 2 foo bar baz >dbg.out 2>&1"
./dbg -e 2 foo bar baz >dbg.out 2>&1
status="$?"
if [[ $status -ne 5 ]]; then
    echo "exit status of dbg: $status != 5";
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    exit status of dbg: $status != 5";
    EXIT_CODE="21"
else
    grep -q '^ERROR\[5\]: main: simulated error, foo: foo bar: bar: errno\[2\]: No such file or directory$' dbg.out
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: did not find the correct dbg error message" 1>&2
	echo "$0: ERROR: beginning dbg.out contents" 1>&2
	cat dbg.out 1>&2
	echo "$0: ERROR: dbg.out contents complete" 1>&2
	FAILURE_SUMMARY="$FAILURE_SUMMARY
    did not find the correct dbg error message"
	EXIT_CODE="22"
    else
	echo
	echo "PASSED: dbg"
	rm -f dbg.out
    fi
fi

# mkiocccentry_test.sh
#
echo
echo "RUNNING: mkiocccentry_test.sh"
echo
echo "./mkiocccentry_test.sh"
./mkiocccentry_test.sh
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry_test.sh non-zero exit code: $status" 1>&2
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    mkiocccentry_test.sh non-zero exit code: $status"
    EXIT_CODE="23"
else
    echo
    echo "PASSED: mkiocccentry_test.sh"
fi


# jstr_test.sh
#
echo
echo "RUNNING: jstr_test.sh"
echo
echo "./jstr_test.sh"
./jstr_test.sh
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jstr_test.sh non-zero exit code: $status" 1>&2
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jstr_test.sh non-zero exit code: $status"
    EXIT_CODE="24"
else
    echo
    echo "PASSED: jstr_test.sh"
fi

# jnum_chk
#
echo
echo "RUNNING: jnum_chk"
echo
echo "./jnum_chk"
./jnum_chk -J "${J_FLAG}"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jnum_chk non-zero exit code: $status" 1>&2
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jnum_chk non-zero exit code: $status"
    EXIT_CODE="25"
else
    echo
    echo "PASSED: jnum_chk"
fi

# dyn_test
#
echo
echo "RUNNING: dyn_test"
echo
echo "./dyn_test"
./dyn_test
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: dyn_test non-zero exit code: $status" 1>&2
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    dyn_test non-zero exit code: $status"
    EXIT_CODE="26"
else
    echo
    echo "PASSED: dyn_test"
fi

# jparse_test.sh
#
echo
echo "RUNNING: jparse_test.sh"
echo
echo "./jparse_test.sh -J $V_FLAG json_teststr.txt"
./jparse_test.sh -J "$V_FLAG" json_teststr.txt
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jparse_test.sh non-zero exit code: $status" 1>&2
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jparse_test.sh non-zero exit code: $status"
    EXIT_CODE="27"
else
    echo
    echo "PASSED: jparse_test.sh"
fi

# txzchk_test.sh
#
# NOTE: we have to include ./ before the test_txzchk because the error files in
# the test_txzchk/bad subdirectory all have that form. This is a stylistic
# choice that can be changed if so desired but I have the ./ to match the rest
# of the command line.
echo
echo "RUNNING: txzchk_test.sh"
echo
echo "./txzchk_test.sh -t ./txzchk -F ./fnamchk -d ./test_txzchk"
./txzchk_test.sh -t ./txzchk -F ./fnamchk -d ./test_txzchk
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: txzchk_test.sh non-zero exit code: $status" 1>&2
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    txzchk_test.sh non-zero exit code: $status"
    EXIT_CODE="28"
else
    echo
    echo "PASSED: txzchk_test.sh"
fi


# report overall status
#
if [[ $EXIT_CODE -eq 0 ]]; then
    echo
    echo "All tests PASSED"
else
    echo
    echo "These test(s) failed:"
    echo "$FAILURE_SUMMARY"
    echo
    echo "About to exit: $EXIT_CODE"
fi
exit "$EXIT_CODE"
