#!/usr/bin/env bash
#
# run_jparse_tests.sh - perform the complete suite of tests for the jparse repo
#
# Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
# rights reserved.
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
# THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
# ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
# DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
#
# This JSON parser and tool were co-developed in 2022-205 by Cody Boone
# Ferguson and Landon Curt Noll:
#
#  @xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# "Share and Enjoy!"
#     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
#



# setup
#
export RUN_JPARSE_TESTS_VERSION="2.0.0 2025-02-28"
export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-J json_level] [-j jparse] [-p pr_jparse_test] [-c jnum_chk] [-Z topdir]

    -h			print help and exit
    -V			print version and exit
    -v level		set debug level for this script (def: 0)
    -D dbg_level	set debug level for jparse_test.sh (def: 0)
    -J json_level	set json debug level (def: 0)
    -j jparse		path to jparse
    -p pr_jparse_test	path to pr_jparse_test
    -c jnum_chk		path to jnum_chk
    -Z topdir		top level build directory (def: try . or ..)

Exit codes:
     0   all OK
     2   -h and help string printed or -V and version string printed
     3   command line usage error
     4	 could not create a writable log file
     5   something not found, not the right file type, or not executable/readable
 >= 20   some test failed

run_jparse_tests.sh version: $RUN_JPARSE_TESTS_VERSION"

export V_FLAG="0"
export D_FLAG="0"
export J_FLAG="0"
export EXIT_CODE="0"
export FAILURE_SUMMARY=
export LOGFILE="./jparse_test.log"
export TOPDIR=
export JPARSE="./jparse"
export PR_JPARSE_TEST="./test_jparse/pr_jparse_test"
export JNUM_CHK="./test_jparse/jnum_chk"
export UTIL_TEST="./util_test"

# parse args
#
while getopts :hVv:D:J:j:p:c:Z: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    D)	D_FLAG="$OPTARG";
	;;
    j)	JPARSE="$OPTARG";
	;;
    p)	PR_JPARSE_TEST="$OPTARG";
	;;
    c)	JNUM_CHK="$OPTARG";
	;;
    J)	J_FLAG="$OPTARG";
	;;
    V)	echo "$RUN_JPARSE_TESTS_VERSION"
	exit 2
	;;
    Z)  TOPDIR="$OPTARG";
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

# check args
#
shift $(( OPTIND - 1 ));
if [[ $# -gt 0 ]]; then
    echo "$0: ERROR: expected 0 args, found: $#" 1>&2
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
    cd ../..
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

# clear log file
#
rm -f "$LOGFILE"

# try creating a new log file
#
touch "$LOGFILE"
if [[ ! -e "$LOGFILE" ]]; then
    echo "$0: ERROR: couldn't create log file: $LOGFILE" 1>&2
    exit 4
fi
if [[ ! -w "$LOGFILE" ]]; then
    echo "$0: ERROR: log file is not writable: $LOGFILE" 1>&2
    exit 4
fi


# firewall - verify we have the required executables and needed data file(s)
#
# NOTE: we check for everything before exiting so that the user can look at the
# log file (or stdout) and know which files are missing. This way they don't
# have to repeatedly run it to get all the error messages.
echo "Checking for required files and directories" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"

# jparse
#
if [[ -z "$JPARSE" ]]; then
    echo "$0: ERROR: \$JPARSE empty, try a non-empty string for -j option" | tee -a -- "${LOGFILE}"
    EXIT_CODE="5"
elif [[ ! -e "$JPARSE" ]]; then
    echo "$0: ERROR: $JPARSE file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f "$JPARSE" ]]; then
    echo "$0: ERROR: $JPARSE is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x "$JPARSE" ]]; then
    echo "$0: ERROR: $JPARSE is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi

# pr_jparse_test
#
if [[ -z "$PR_JPARSE_TEST" ]]; then
    echo "$0: ERROR: \$PR_JPARSE_TEST empty, try a non-empty string for -p option" | tee -a -- "${LOGFILE}"
    EXIT_CODE="5"
elif [[ ! -e "$PR_JPARSE_TEST" ]]; then
    echo "$0: ERROR: $PR_JPARSE_TEST file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f "$PR_JPARSE_TEST" ]]; then
    echo "$0: ERROR: $PR_JPARSE_TEST is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x "$PR_JPARSE_TEST" ]]; then
    echo "$0: ERROR: $PR_JPARSE_TEST is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi

# jnum_chk
#
if [[ -z "$JNUM_CHK" ]]; then
    echo "$0: ERROR: \$JNUM_CHK empty, try a non-empty string for -c option" | tee -a -- "${LOGFILE}"
    EXIT_CODE="5"
elif [[ ! -e "$JNUM_CHK" ]]; then
    echo "$0: ERROR: $JNUM_CHK file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f "$JNUM_CHK" ]]; then
    echo "$0: ERROR: $JNUM_CHK is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x "$JNUM_CHK" ]]; then
    echo "$0: ERROR: $JNUM_CHK is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi

# test_jparse/jparse_test.sh
if [[ ! -e test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: test_jparse/jparse_test.sh file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: test_jparse/jparse_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: test_jparse/jparse_test.sh is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# jstr_test.sh
if [[ ! -e test_jparse/jstr_test.sh ]]; then
    echo "$0: ERROR: test_jparse/jstr_test.sh file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f test_jparse/jstr_test.sh ]]; then
    echo "$0: ERROR: test_jparse/jstr_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x test_jparse/jstr_test.sh ]]; then
    echo "$0: ERROR: test_jparse/jstr_test.sh is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# json_teststr.txt: for test_jparse/jparse_test.sh
if [[ ! -e test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: test_jparse/json_teststr.txt file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: test_jparse/jparse_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -r test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: test_jparse/json_teststr.txt is not readable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# json_teststr_fail.txt: for test_jparse/jparse_test.sh
if [[ ! -e test_jparse/json_teststr_fail.txt ]]; then
    echo "$0: ERROR: test_jparse/json_teststr_fail.txt file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f test_jparse/json_teststr_fail.txt ]]; then
    echo "$0: ERROR: test_jparse/jparse_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -r test_jparse/json_teststr_fail.txt ]]; then
    echo "$0: ERROR: test_jparse/json_teststr_fail.txt is not readable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# util_test
if [[ ! -e test_jparse/util_test ]]; then
    echo "$0: ERROR: test_jparse/util_test file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f test_jparse/util_test ]]; then
    echo "$0: ERROR: test_jparse/util_test is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x test_jparse/util_test ]]; then
    echo "$0: ERROR: test_jparse/util_test is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi

# test_JSON
if [[ ! -e ./test_jparse/test_JSON ]]; then
    echo "$0: ERROR: ./test_jparse/test_JSON file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -d ./test_jparse/test_JSON ]]; then
    echo "$0: ERROR: ./test_jparse/test_JSON is not a directory" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -r ./test_jparse/test_JSON ]]; then
    echo "$0: ERROR: ./test_jparse/test_JSON is not readable directory" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi

if [[ "$EXIT_CODE" -ne 0 ]]; then
    echo | tee -a -- "$LOGFILE"
    echo "$0: ERROR: cannot continue" | tee -a -- "$LOGFILE"
    exit "$EXIT_CODE"
fi

# start the test suite
#
echo "Start test suite" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"

# test_jparse/jparse_test.sh
#
echo "RUNNING: ./test_jparse/jparse_test.sh -D ${D_FLAG} -v ${V_FLAG} -j $JPARSE -p $PR_JPARSE_TEST -d test_jparse/test_JSON -s . test_jparse/json_teststr.txt" | tee -a -- "${LOGFILE}"
./test_jparse/jparse_test.sh -D "${V_FLAG}" -v "${V_FLAG}" -j "$JPARSE" -p "$PR_JPARSE_TEST" -d test_jparse/test_JSON \
    -s . test_jparse/json_teststr.txt | tee -a -- "${LOGFILE}"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_jparse/jparse_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_jparse/jparse_test.sh non-zero exit code: $status"
    EXIT_CODE="23"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo "FAILED: ./jparse_test.sh -D $V_FLAG -v ${V_FLAG} -j $JPARSE -p $PR_JPARSE_TEST -d test_jparse/test_JSON -s . test_jparse/json_teststr.txt" \
	| tee -a -- "${LOGFILE}"
else
    echo "PASSED: ./jparse_test.sh -D $V_FLAG -v ${V_FLAG} -j $JPARSE -p $PR_JPARSE_TEST -d test_jparse/test_JSON -s . test_jparse/json_teststr.txt" | tee -a -- "${LOGFILE}"
fi

echo | tee -a -- "${LOGFILE}"
echo "RUNNING: ./test_jparse/jparse_test.sh -D $V_FLAG -v ${V_FLAG} -j $JPARSE -F jparse.json" | tee -a -- "${LOGFILE}"
./test_jparse/jparse_test.sh -D "$V_FLAG" -v "${V_FLAG}" -j "$JPARSE" -F jparse.json | tee -a -- "${LOGFILE}"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_jparse/jparse_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_jparse/jparse_test.sh non-zero exit code: $status"
    EXIT_CODE="24"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo "FAILED: ./jparse_test.sh -D $V_FLAG -v ${V_FLAG} -j $JPARSE -F jparse.json" | tee -a -- "${LOGFILE}"
else
    echo "PASSED: ./jparse_test.sh -D $V_FLAG -v ${V_FLAG} -j $JPARSE -F jparse.json" | tee -a -- "${LOGFILE}"
fi
echo | tee -a -- "${LOGFILE}"
echo "RUNNING: ./test_jparse/jparse_test.sh -D $V_FLAG -v ${V_FLAG} -f -j $JPARSE test_jparse/json_teststr_fail.txt" | tee -a -- "${LOGFILE}"
./test_jparse/jparse_test.sh -D "$V_FLAG" -v "${V_FLAG}" -f -j "$JPARSE" test_jparse/json_teststr_fail.txt | tee -a -- "${LOGFILE}"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_jparse/jparse_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_jparse/jparse_test.sh non-zero exit code: $status"
    EXIT_CODE="25"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo "FAILED: ./jparse_test.sh -D $V_FLAG -v ${V_FLAG} -f -j $JPARSE test_jparse/json_teststr_fail.txt" | tee -a -- "${LOGFILE}"
else
    echo "PASSED: ./jparse_test.sh -D $V_FLAG -v ${V_FLAG} -j $JPARSE -F jparse.json" | tee -a -- "${LOGFILE}"
fi


# jstr_test.sh
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: test_jparse/jstr_test.sh" | tee -a -- "$LOGFILE"
echo "test_jparse/jstr_test.sh -Z $TOPDIR -v $V_FLAG -e $TOPDIR/jstrdecode -d $TOPDIR/jstrencode" | tee -a -- "$LOGFILE"
test_jparse/jstr_test.sh -Z "$TOPDIR" -v "$V_FLAG" -e "$TOPDIR/jstrdecode" -d "$TOPDIR/jstrencode" | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_jparse/jstr_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_jparse/jstr_test.sh non-zero exit code: $status"
    EXIT_CODE="26"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo "FAILED: test_jparse/jstr_test.sh -Z $TOPDIR -v $V_FLAG -e $TOPDIR/jstrdecode -d $TOPDIR/jstrencode" | tee -a -- "$LOGFILE"
else
    echo "PASSED: test_jparse/jstr_test.sh -Z $TOPDIR -v $V_FLAG -e $TOPDIR/jstrdecode -d $TOPDIR/jstrencode" | tee -a -- "$LOGFILE"
fi

# util_test
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: test_jparse/util_test" | tee -a -- "$LOGFILE"
echo "test_jparse/util_test" | tee -a -- "$LOGFILE"
test_jparse/util_test | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_jparse/util_test non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_jparse/util_test non-zero exit code: $status"
    EXIT_CODE="27"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo "FAILED: test_jparse/util_test" | tee -a -- "$LOGFILE"
else
    echo "PASSED: test_jparse/util_test" | tee -a -- "$LOGFILE"
fi


# jnum_chk
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: $JNUM_CHK" -J "$J_FLAG" -v "$V_FLAG" | tee -a -- "$LOGFILE"
echo "$JNUM_CHK" -J "$J_FLAG" -v "$V_FLAG" | tee -a -- "$LOGFILE"
"$JNUM_CHK" -J "${J_FLAG}" -v "$V_FLAG" | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: $JNUM_CHK non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    $JNUM_CHK non-zero exit code: $status"
    EXIT_CODE="28"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo "FAILED: $JNUM_CHK -J ${J_FLAG}" -v "$V_FLAG" | tee -a -- "$LOGFILE"
else
    echo "PASSED: $JNUM_CHK -J ${J_FLAG}" -v "$V_FLAG" | tee -a -- "$LOGFILE"
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
