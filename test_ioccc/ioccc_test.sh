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
export IOCCC_TEST_VERSION="0.5 2022-11=04"

export USAGE="usage: $0 [-h] [-v level] [-J json_level] [-V] [-Z topdir]

    -h              print help and exit
    -v level        set debug level (def: 0)
    -J json_level   set json debug level (def: 0)
    -V              print version and exit
    -Z topdir	    top level build directory (def: try . or ..)

Exit codes:
     0   all tests are OK
     2   -h and help string printed or -V and version string printed
     3   command line usage error
  4-19   something not found, not a file, or not executable
 >= 20   some test failed

$0 version: $IOCCC_TEST_VERSION"
export V_FLAG="0"
export J_FLAG="0"
export EXIT_CODE="0"
export FAILURE_SUMMARY=
export LOGFILE="test_ioccc/test_ioccc.log"
export TOPDIR=

# parse args
#
while getopts :hv:J:VZ: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    J)	J_FLAG="$OPTARG";
	;;
    V)	echo "$IOCCC_TEST_VERSION"
	exit 2
	;;
    Z)  TOPDIR="$OPTARG";
        ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
	exit 3
	;;
    :)	echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
	exit 3
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
    exit 3
fi

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
    # warning: Use 'cd ... || exit' or 'cd ... || return' in case cd fails. [SC2164]
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

# firewall - verify we have the required executables and needed data file(s)
#
# iocccsize_test.sh
if [[ ! -e test_ioccc/iocccsize_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/iocccsize_test.sh file not found" 1>&2
    exit 4
fi
if [[ ! -f test_ioccc/iocccsize_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/iocccsize_test.sh is not a regular file" 1>&2
    exit 4
fi
if [[ ! -x test_ioccc/iocccsize_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/iocccsize_test.sh is not executable" 1>&2
    exit 4
fi
# dbg_test
if [[ ! -e dbg/dbg_test ]]; then
    echo "$0: ERROR: dbg_test file not found" 1>&2
    exit 5
fi
if [[ ! -f dbg/dbg_test ]]; then
    echo "$0: ERROR: dbg_test is not a regular file" 1>&2
    exit 5
fi
if [[ ! -x dbg/dbg_test ]]; then
    echo "$0: ERROR: dbg_test is not executable" 1>&2
    exit 5
fi
# mkiocccentry_test.sh
if [[ ! -e test_ioccc/mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/mkiocccentry_test.sh file not found" 1>&2
    exit 6
fi
if [[ ! -f test_ioccc/mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/mkiocccentry_test.sh is not a regular file" 1>&2
    exit 6
fi
if [[ ! -x test_ioccc/mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/mkiocccentry_test.sh is not executable" 1>&2
    exit 6
fi
# jstr_test.sh
if [[ ! -e jparse/test_jparse/jstr_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jstr_test.sh file not found" 1>&2
    exit 7
fi
if [[ ! -f jparse/test_jparse/jstr_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jstr_test.sh is not a regular file" 1>&2
    exit 7
fi
if [[ ! -x jparse/test_jparse/jstr_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jstr_test.sh is not executable" 1>&2
    exit 7
fi
# jnum_chk
if [[ ! -e jparse/test_jparse/jnum_chk ]]; then
    echo "$0: ERROR: jparse/test_jparse/jnum_chk file not found" 1>&2
    exit 8
fi
if [[ ! -f jparse/test_jparse/jnum_chk ]]; then
    echo "$0: ERROR: jparse/test_jparse/jnum_chk is not a regular file" 1>&2
    exit 8
fi
if [[ ! -x jparse/test_jparse/jnum_chk ]]; then
    echo "$0: ERROR: jparse/test_jparse/jnum_chk is not executable" 1>&2
    exit 8
fi
# dyn_test
if [[ ! -e dyn_array/dyn_test ]]; then
    echo "$0: ERROR: dyn_array/dyn_test file not found" 1>&2
    exit 9
fi
if [[ ! -f dyn_array/dyn_test ]]; then
    echo "$0: ERROR: dyn_array/dyn_test is not a regular file" 1>&2
    exit 9
fi
if [[ ! -x dyn_array/dyn_test ]]; then
    echo "$0: ERROR: dyn_array/dyn_test is not executable" 1>&2
    exit 9
fi
# jparse_test.sh
if [[ ! -e jparse/test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh file not found" 1>&2
    exit 10
fi
if [[ ! -f jparse/test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh is not a regular file" 1>&2
    exit 10
fi
if [[ ! -x jparse/test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh is not executable" 1>&2
    exit 10
fi
# json_teststr.txt: for jparse_test.sh
if [[ ! -e jparse/test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/json_teststr.txt file not found" 1>&2
    exit 11
fi
if [[ ! -f jparse/test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh is not a regular file" 1>&2
    exit 11
fi
if [[ ! -r jparse/test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/json_teststr.txt is not readable" 1>&2
    exit 11
fi
# txzchk
if [[ ! -e txzchk ]]; then
    echo "$0: ERROR: txzchk file not found" 1>&2
    exit 12
fi
if [[ ! -f txzchk ]]; then
    echo "$0: ERROR: txzchk is not a regular file" 1>&2
    exit 12
fi
if [[ ! -x txzchk ]]; then
    echo "$0: ERROR: txzchk is not executable" 1>&2
    exit 12
fi
# chkentry_test.sh
if [[ ! -e test_ioccc/chkentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/chkentry_test.sh file not found" 1>&2
    exit 13
fi
if [[ ! -f test_ioccc/chkentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/chkentry_test.sh is not a regular file" 1>&2
    exit 13
fi
if [[ ! -x test_ioccc/chkentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/chkentry_test.sh is not executable" 1>&2
    exit 13
fi
# test_JSON
if [[ ! -e ./test_ioccc/test_JSON ]]; then
    echo "$0: ERROR: ./test_JSON file not found" 1>&2
    exit 14
fi
if [[ ! -d ./test_ioccc/test_JSON ]]; then
    echo "$0: ERROR: ./test_ioccc/test_JSON is not a directory" 1>&2
    exit 14
fi
if [[ ! -r ./test_ioccc/test_JSON ]]; then
    echo "$0: ERROR: ./test_ioccc/test_JSON is not readable directory" 1>&2
    exit 14
fi
# chkentry
if [[ ! -e chkentry ]]; then
    echo "$0: ERROR: chkentry file not found" 1>&2
    exit 15
fi
if [[ ! -f chkentry ]]; then
    echo "$0: ERROR: chkentry is not a regular file" 1>&2
    exit 15
fi
if [[ ! -x chkentry ]]; then
    echo "$0: ERROR: chkentry is not executable" 1>&2
    exit 15
fi
# test_txzchk
if [[ ! -e test_ioccc/test_txzchk ]]; then
    echo "$0: ERROR: test_ioccc/test_txzchk file not found" 1>&2
    exit 16
fi
if [[ ! -d test_ioccc/test_txzchk ]]; then
    echo "$0: ERROR: test_ioccc/test_txzchk is not a directory" 1>&2
    exit 16
fi
if [[ ! -r test_ioccc/test_txzchk ]]; then
    echo "$0: ERROR: test_ioccc/test_txzchk is not readable directory" 1>&2
    exit 16
fi

# clear log file
#
rm -f "$LOGFILE"

# try creating a new log file
#
touch "$LOGFILE"
if [[ ! -e "$LOGFILE" ]]; then
    echo "$0: ERROR: couldn't create log file: $LOGFILE" 1>&2
    exit 17
fi
if [[ ! -w "$LOGFILE" ]]; then
    echo "$0: ERROR: log file is not writable: $LOGFILE" 1>&2
    exit 17
fi

# start the test suite
#
echo "Start test suite" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"

# iocccsize_test.sh
#
echo "RUNNING: test_ioccc/iocccsize_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "test_ioccc/iocccsize_test.sh -v 1" | tee -a -- "$LOGFILE"
test_ioccc/iocccsize_test.sh -v 1 | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_ioccc/iocccsize_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_ioccc/iocccsize_test.sh non-zero exit code: $status"
    EXIT_CODE="20"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: test_ioccc/iocccsize_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: test_ioccc/iocccsize_test.sh" | tee -a -- "$LOGFILE"
fi

# dbg_test
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: dbg_test" | tee -a -- "$LOGFILE"
rm -f dbg_test.out
echo | tee -a -- "$LOGFILE"
echo "./dbg/dbg_test -e 2 foo bar baz >dbg_test.out 2>&1" | tee -a -- "$LOGFILE"
./dbg/dbg_test -e 2 foo bar baz >dbg_test.out 2>&1 | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 5 ]]; then
    echo "exit status of dbg_test: $status != 5";
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    exit status of dbg_test: $status != 5";
    EXIT_CODE="21"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: dbg_test" | tee -a -- "$LOGFILE"
else
    grep -q '^ERROR\[5\]: main: simulated error, foo: foo bar: bar: errno\[2\]: No such file or directory$' dbg_test.out
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: did not find the correct dbg_test error message" 1>&2 | tee -a -- "$LOGFILE"
	echo "$0: ERROR: beginning dbg_test.out contents" 1>&2 | tee -a -- "$LOGFILE"
	< dbg_test.out tee -a -- "$LOGFILE"
	echo "$0: ERROR: dbg_test.out contents complete" 1>&2 | tee -a -- "$LOGFILE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
    did not find the correct dbg_test error message"
	EXIT_CODE="22"
	echo | tee -a -- "$LOGFILE"
	echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
	echo | tee -a -- "$LOGFILE"
	echo "FAILED: dbg_test" | tee -a -- "$LOGFILE"
    else
	echo | tee -a -- "$LOGFILE"
	echo "PASSED: dbg_test" | tee -a -- "$LOGFILE"
	rm -f dbg_test.out
    fi
fi

# mkiocccentry_test.sh
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: test_ioccc/mkiocccentry_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "test_ioccc/mkiocccentry_test.sh -Z $TOPDIR" | tee -a -- "$LOGFILE"
test_ioccc/mkiocccentry_test.sh -Z "$TOPDIR" | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_ioccc/mkiocccentry_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_ioccc/mkiocccentry_test.sh non-zero exit code: $status"
    EXIT_CODE="23"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: test_ioccc/mkiocccentry_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: test_ioccc/mkiocccentry_test.sh" | tee -a -- "$LOGFILE"
fi

# jstr_test.sh
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: jparse/test_jparse/jstr_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "jparse/test_jparse/jstr_test.sh -Z $TOPDIR -e $TOPDIR/jparse/jstrencode -d $TOPDIR/jparse/jstrdecode" | tee -a -- "$LOGFILE"
jparse/test_jparse/jstr_test.sh -Z "$TOPDIR" -e "$TOPDIR/jparse/jstrencode" -d "$TOPDIR/jparse/jstrdecode" | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jparse/test_jparse/jstr_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jparse/test_jparse/jstr_test.sh non-zero exit code: $status"
    EXIT_CODE="24"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: jparse/test_jparse/jstr_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: jparse/test_jparse/jstr_test.sh" | tee -a -- "$LOGFILE"
fi

# jnum_chk
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: jparse/test_jparse/jnum_chk" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "jparse/test_jparse/jnum_chk" | tee -a -- "$LOGFILE"
jparse/test_jparse/jnum_chk -J "${J_FLAG}" | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jparse/test_jparse/jnum_chk non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jparse/test_jparse/jnum_chk non-zero exit code: $status"
    EXIT_CODE="25"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: jparse/test_jparse/jnum_chk" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: jparse/test_jparse/jnum_chk" | tee -a -- "$LOGFILE"
fi

# dyn_test
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: dyn_array/dyn_test" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "dyn_array/dyn_test" | tee -a -- "$LOGFILE"
dyn_array/dyn_test | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: dyn_array/dyn_test non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    dyn_array/dyn_test non-zero exit code: $status"
    EXIT_CODE="26"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: dyn_array/dyn_test" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: dyn_array/dyn_test" | tee -a -- "$LOGFILE"
fi

# jparse_test.sh for test_jparse/test_JSON
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: jparse/test_jparse/jparse_test.sh for test_jparse/test_JSON" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "jparse/test_jparse/jparse_test.sh -J $V_FLAG -d ./jparse/test_jparse/test_JSON -s . -j jparse/jparse jparse/test_jparse/json_teststr.txt" | tee -a -- "$LOGFILE"
jparse/test_jparse/jparse_test.sh -J "$V_FLAG" -d ./jparse/test_jparse/test_JSON -s . -j jparse/jparse jparse/test_jparse/json_teststr.txt | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh for test_jparse/test_JSON non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jparse/test_jparse/jparse_test.sh for test_jparse/test_JSON non-zero exit code: $status"
    EXIT_CODE="27"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: jparse/test_jparse/jparse_test.sh for test_jparse/test_JSON" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: jparse/test_jparse/jparse_test.sh for test_jparse/test_JSON" | tee -a -- "$LOGFILE"
fi

# jparse_test.sh for general.json
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: jparse/test_jparse/jparse_test.sh for general.json" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "jparse/test_jparse/jparse_test.sh -J $V_FLAG -d ./test_ioccc/test_JSON -s general.json -j jparse/jparse jparse/test_jparse/json_teststr.txt" | tee -a -- "$LOGFILE"
jparse/test_jparse/jparse_test.sh -J "$V_FLAG" -d ./test_ioccc/test_JSON -s general.json -j jparse/jparse jparse/test_jparse/json_teststr.txt | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh for general.json non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jparse/test_jparse/jparse_test.sh for general.json non-zero exit code: $status"
    EXIT_CODE="28"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: jparse/test_jparse/jparse_test.sh for general.json" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: jparse/test_jparse/jparse_test.sh for general.json" | tee -a -- "$LOGFILE"
fi

# jparse_test.sh for info.json
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: jparse/test_jparse/jparse_test.sh for info.json" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "jparse/test_jparse/jparse_test.sh -J $V_FLAG -d ./test_ioccc/test_JSON -s info.json -j jparse/jparse jparse/test_jparse/json_teststr.txt" | tee -a -- "$LOGFILE"
jparse/test_jparse/jparse_test.sh -J "$V_FLAG" -d ./test_ioccc/test_JSON -s info.json -j jparse/jparse jparse/test_jparse/json_teststr.txt | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh for info.json non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jparse/test_jparse/jparse_test.sh for info.json non-zero exit code: $status"
    EXIT_CODE="29"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: jparse/test_jparse/jparse_test.sh for info.json" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: jparse/test_jparse/jparse_test.sh for info.json" | tee -a -- "$LOGFILE"
fi

# jparse_test.sh for auth.json
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: jparse/test_jparse/jparse_test.sh for auth.json" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "jparse/test_jparse/jparse_test.sh -J $V_FLAG -d ./test_ioccc/test_JSON -s auth.json -j jparse/jparse jparse/test_jparse/json_teststr.txt" | tee -a -- "$LOGFILE"
jparse/test_jparse/jparse_test.sh -J "$V_FLAG" -d ./test_ioccc/test_JSON -s auth.json -j jparse/jparse jparse/test_jparse/json_teststr.txt | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh for auth.json non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jparse/test_jparse/jparse_test.sh for auth.json non-zero exit code: $status"
    EXIT_CODE="30"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: jparse/test_jparse/jparse_test.sh for auth.json" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: jparse/test_jparse/jparse_test.sh for auth.json" | tee -a -- "$LOGFILE"
fi

# txzchk_test.sh
#
# NOTE: we have to include ./ before the test_txzchk because the error files in
# the test_txzchk/bad subdirectory all have that form. This is a stylistic
# choice that can be changed if so desired but I have the ./ to match the rest
# of the command line.
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: test_ioccc/txzchk_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "test_ioccc/txzchk_test.sh -t ./txzchk -F ./test_ioccc/fnamchk -d test_ioccc/test_txzchk -Z $TOPDIR" | tee -a -- "$LOGFILE"
test_ioccc/txzchk_test.sh -t ./txzchk -F ./test_ioccc/fnamchk -d ./test_ioccc/test_txzchk -Z "$TOPDIR" | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_ioccc/txzchk_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_ioccc/txzchk_test.sh non-zero exit code: $status"
    EXIT_CODE="31"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: test_ioccc/txzchk_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: test_ioccc/txzchk_test.sh" | tee -a -- "$LOGFILE"
fi

# chkentry_test.sh
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: test_ioccc/chkentry_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "test_ioccc/chkentry_test.sh -v 1 -d ./test_ioccc/test_JSON -c ./chkentry" | tee -a -- "$LOGFILE"
test_ioccc/chkentry_test.sh -v 1 -d ./test_ioccc/test_JSON -c ./chkentry | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_ioccc/chkentry_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_ioccc/chkentry_test.sh non-zero exit code: $status"
    EXIT_CODE="32"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: test_ioccc/chkentry_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: test_ioccc/chkentry_test.sh" | tee -a -- "$LOGFILE"
fi

# report overall status
#
if [[ $EXIT_CODE -ne 0 ]]; then
    echo | tee -a -- "$LOGFILE"
    echo "These test(s) failed:" | tee -a -- "$LOGFILE"
    echo "$FAILURE_SUMMARY" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "About to exit: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo
    echo "See $LOGFILE for more details"
    echo
fi
exit "$EXIT_CODE"
