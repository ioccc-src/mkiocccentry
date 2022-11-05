#!/usr/bin/env bash
#
# txzchk_test.sh - test txzchk with good and bad tarballs (as text files)
#
# Dedicated to:
#
#	The many poor souls who have been tarred and feathered:
#
#	    "Because sometimes people throw feathers on tar :-( and because
#	    sometimes people try hiding the fact that they're planning on
#	    throwing feathers on the tar." :-)
#
#	And to my wonderful Mum and my dear cousin and friend Dani:
#
#	    "Because even feathery balls of tar need some love." :-)
#
# Written in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# "Because sometimes even the IOCCC Judges need some help." :-)

# setup

# attempt to fetch system specific path to the tools we need
#
# get tar path
TAR="$(type -P tar 2>/dev/null)"
# Make sure TAR is set:
#
# It's possible that the path could not be obtained so we set it to the default
# in this case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# could make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${TAR:=/usr/bin/tar} 2>/dev/null
#
# but due to the reasons cited above we must rely on the more complicated form:
if [[ -z "$TAR" ]]; then
    TAR="/usr/bin/tar"
fi

export TXZCHK_TEST_VERSION="0.5 2022-11-04"
export USAGE="usage: $0 [-h] [-V] [-v level] [-t txzchk] [-T tar] [-F fnamchk] [-d txzchk_tree] [-Z topdir]

    -h			    print help and exit
    -V			    print version and exit
    -v level		    set verbosity level for this script: (def level: 0)
    -t txzchk		    path to txzchk executable (def: ./txzchk)
    -T tar		    path to tar that accepts -J option (def: $TAR)
    -F fnamchk	            path to fnamchk (def: ./fnamchk)

    -d txzchk_tree	    tree where txzchk test files are to be found (def: ./test_txzchk)
			      These subdirectories are expected:
				txzchk_tree/bad
				txzchk_tree/good
			      NOTE: the contents of the error files in the bad subdir
				    are not flexible as the check for errors reported by
				    txzchk and the error file is done as an exact match
    -Z topdir		    top level build directory (def: try . or ..)

Exit codes:
     0   all is well
     1   at least one test failed
     2   -h and help string printed or -V and version string printed
     3   invalid command line
 >= 10   internal error

$0 version: $TXZCHK_TEST_VERSION"

export TXZCHK="./txzchk"
export EXIT_CODE=0
export TXZCHK_TREE="test/test_txzchk"
export LOGFILE="test/txzchk_test.log"
export FNAMCHK="./fnamchk"
export TOPDIR=

# parse args
#
export V_FLAG="0"
while getopts :hVv:t:d:T:F:Z: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$TXZCHK_TEST_VERSION" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    t)	TXZCHK="$OPTARG";
	;;
    d)	TXZCHK_TREE="$OPTARG";
	;;
    F)	FNAMCHK="$OPTARG";
	;;
    T)	TAR="$OPTARG";
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
if [[ $# -gt 1 ]]; then
    echo "$0: ERROR: expected no more than 1 argument, found $#" 1>&2
    exit 3
fi
if [[ $# -eq 1 ]]; then
    TXZCHK_TREE="$1"
fi
export TXZCHK_GOOD_TREE="$TXZCHK_TREE/good"
export TXZCHK_BAD_TREE="$TXZCHK_TREE/bad"

# change to the top level directory as needed
#
if [[ -n $TOPDIR ]]; then
    if [[ ! -d $TOPDIR ]]; then
	echo "$0: ERROR: -T $TOPDIR given: not a directory: $TOPDIR" 1>&2
	exit 3
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: -T $TOPDIR given, about to cd $TOPDIR" 1>&2
    fi
    # warning: Use 'cd ... || exit' or 'cd ... || return' in case cd fails. [SC2164]
    # shellcheck disable=SC2164
    cd "$TOPDIR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: -T $TOPDIR given: cd $TOPDIR exit code: $status" 1>&2
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
    echo "$0: ERROR: cannot determine TOPDIR, use -T topdir" 1>&2
    exit 3
fi
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: TOPDIR is the current directory: $TOPDIR" 1>&2
fi

# make the default all set
#
make all 2>&1 | grep -v 'Nothing to be done for'

# check that txzchk_tree is a readable directory
#
if [[ ! -e $TXZCHK_TREE ]]; then
    echo "$0: ERROR: txzchk_tree not found: $TXZCHK_TREE" 1>&2
    exit 10
fi
if [[ ! -d $TXZCHK_TREE ]]; then
    echo "$0: ERROR: txzchk_tree not a directory: $TXZCHK_TREE" 1>&2
    exit 11
fi
if [[ ! -r $TXZCHK_TREE ]]; then
    echo "$0: ERROR: txzchk_tree not readable directory: $TXZCHK_TREE" 1>&2
    exit 12
fi

# good tree
#
if [[ ! -e $TXZCHK_GOOD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/good for txzchk directory not found: $TXZCHK_GOOD_TREE" 1>&2
    exit 13
fi
if [[ ! -d $TXZCHK_GOOD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/good for txzchk not a directory: $TXZCHK_GOOD_TREE" 1>&2
    exit 14
fi
if [[ ! -r $TXZCHK_GOOD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/good for txzchk not readable directory: $TXZCHK_GOOD_TREE" 1>&2
    exit 15
fi

# bad tree
#
if [[ ! -e $TXZCHK_BAD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/bad for txzchk directory not found: $TXZCHK_BAD_TREE" 1>&2
    exit 16
fi
if [[ ! -d $TXZCHK_BAD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/bad for txzchk not a directory: $TXZCHK_BAD_TREE" 1>&2
    exit 17
fi
if [[ ! -r $TXZCHK_BAD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/bad for txzchk not readable directory: $TXZCHK_BAD_TREE" 1>&2
    exit 18
fi

# check for txzchk
#
if [[ ! -e $TXZCHK ]]; then
    echo "$0: ERROR: txzchk not found: $TXZCHK" 1>&2
    exit 19
fi
if [[ ! -f $TXZCHK ]]; then
    echo "$0: ERROR: txzchk not a regular file: $TXZCHK" 1>&2
    exit 20
fi
if [[ ! -x $TXZCHK ]]; then
    echo "$0: ERROR: txzchk not executable: $TXZCHK" 1>&2
    exit 21
fi

# check for fnamchk
#
if [[ ! -e $FNAMCHK ]]; then
    echo "$0: ERROR: fnamchk not found: $FNAMCHK" 1>&2
    exit 22
fi
if [[ ! -f $FNAMCHK ]]; then
    echo "$0: ERROR: fnamchk not a regular file: $FNAMCHK" 1>&2
    exit 23
fi
if [[ ! -x $FNAMCHK ]]; then
    echo "$0: ERROR: fnamchk not executable: $FNAMCHK" 1>&2
    exit 24
fi

# check for tar
#
if [[ ! -e $TAR ]]; then
    echo "$0: ERROR: tar not found: $TAR" 1>&2
    exit 25
fi
if [[ ! -f $TAR ]]; then
    echo "$0: ERROR: tar not a regular file: $TAR" 1>&2
    exit 26
fi
if [[ ! -x $TAR ]]; then
    echo "$0: ERROR: tar not executable: $TAR" 1>&2
    exit 27
fi

# remove logfile so that each run starts out with an empty file
#
rm -f "$LOGFILE"
touch "$LOGFILE"
if [[ ! -f "${LOGFILE}" ]]; then
    echo "$0: ERROR: couldn't create log file" 1>&2
    exit 28
fi
if [[ ! -w "${LOGFILE}" ]]; then
    echo "$0: ERROR: log file not writable" 1>&2
    exit 29
fi

# set up for the different tests
# set up for tar test
#
RUN_TAR_TEST="true"
TEST_FILE=$(mktemp .txzchk_test.test_file.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp .txzchk_test.test_file.XXXXXXXXXX exit code: $status" 1>&2
    EXIT_CODE=30
    RUN_TAR_TEST=
fi
date > "$TEST_FILE"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: date > $TEST_FILE exit code: $status" 1>&2
    EXIT_CODE=31
    RUN_TAR_TEST=
fi
if [[ ! -r $TEST_FILE ]]; then
    echo "$0: ERROR: not a readable TEST_FILE: $TEST_FILE" 1>&2
    EXIT_CODE=32
    RUN_TAR_TEST=
fi
TAR_ERROR=$(mktemp -u .txzchk_test.tar_err.XXXXXXXXXX.out)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $TAR_ERROR exit code: $status" 1>&2
    EXIT_CODE=33
    RUN_TAR_TEST=
fi
TARBALL=$(mktemp -u .txzchk_test.tarball.XXXXXXXXXX.txz)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $TARBALL exit code: $status" 1>&2
    EXIT_CODE=34
    RUN_TAR_TEST=
fi

# We need a file to write the output of txzchk to in order to compare it
# with any error file. This is needed for the files that are supposed to
# fail but it's possible that there could be a use for good files too.
STDERR=$(mktemp -u .txzchk_test.stderr.XXXXXXXXXX)
# delete the temporary file in the off chance it already exists
rm -f "$STDERR"
# now let's make sure we can create it as well: if we can't or it's not
# writable there's an issue.
#
touch "$STDERR"
if [[ ! -e "$STDERR" ]]; then
    echo "$0: could not create output file: $STDERR"
    exit 30
fi
if [[ ! -w "$STDERR" ]]; then
    echo "$0: output file not writable: $STDERR"
    exit 31
fi

trap "rm -f \$TARBALL \$TEST_FILE \$TAR_ERROR \$STDERR; exit" 0 1 2 3 15

# run tar test
#
TAR_TEST_SUCCESS="true"
if [[ -n $RUN_TAR_TEST ]]; then
    "${TAR}" --format=v7 -cJf "$TARBALL" "$TEST_FILE" 2>"$TAR_ERROR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: $TAR --format=v7 -cJf $TARBALL $TEST_FILE 2>$TAR_ERROR exit code: $status" 1>&2
	EXIT_CODE=35
	TAR_TEST_SUCCESS=
    fi
    if [[ ! -s $TARBALL ]]; then
	echo "$0: ERROR: did not find a non-empty tarball: $TARBALL" 1>&2
	EXIT_CODE=36
	TAR_TEST_SUCCESS=
    fi
    if [[ -s $TAR_ERROR ]]; then
	echo "$0: notice: tar stderr follows:" 1>&2
	cat "$TAR_ERROR" 1>&2
	echo "$0: notice: end of tar stderr" 1>&2
	EXIT_CODE=37
	TAR_TEST_SUCCESS=
    fi
else
    echo "$0: notice: tar test disabled due to test set up error(s)" 1>&2
    TAR_TEST_SUCCESS=
fi

# tar test clean up
#
if [[ -n $TAR_TEST_SUCCESS ]]; then
    rm -f "$TEST_FILE" "$TAR_ERROR" "$TARBALL"
fi

if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: will run txzchk from: $TXZCHK" 1>&2
fi

# run_test - run a single test
#
# usage:
#	run_test {fail|pass} txzchk_test_file
#
#	run_test	    - our function name
#	fail		    - test must fail - error if passes
#	pass		    - test must pass - error if fails
#	txzchk_test_file    - the txzchk text file to give to txzchk
#
run_test()
{
    # parse args
    #
    if [[ $# -ne 2 ]]; then
	echo "$0: ERROR: expected 2 args to run_test, found $#" 1>&2
	exit 32
    fi
    declare pass_fail="$1"
    declare txzchk_test_file="$2"
    declare txzchk_err_file="$txzchk_test_file.err"

    if [[ ! -e $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not found: $txzchk_test_file"
	exit 33
    fi
    if [[ ! -f $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not a regular file: $txzchk_test_file"
	exit 34
    fi
    if [[ ! -r $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not readable: $txzchk_test_file"
	exit 35
    fi

    # if pass_fail is fail then there has to be an error file
    if [[ $pass_fail = fail ]]; then
	if [[ ! -e $txzchk_err_file ]]; then
	    echo "$0: in run_test: txzchk_err_file not found for test that must fail: $txzchk_err_file"
	    exit 36
	fi
	if [[ ! -f $txzchk_err_file ]]; then
	    echo "$0: in run_test: txzchk_err_file not a regular file for test that must fail: $txzchk_err_file"
	    exit 37
	fi
	if [[ ! -r $txzchk_err_file ]]; then
	    echo "$0: in run_test: txzchk_err_file not readable for test that must fail: $txzchk_err_file"
	    exit 38
	fi
    elif [[ $pass_fail = pass ]]; then
	if [[ -e $txzchk_err_file ]]; then
	    echo "$0: in run_test: txzchk_err_file exists for test that must not fail: $txzchk_err_file"
	    exit 39
	fi
    else
	echo "$0: pass_fail neither 'pass' nor 'fail': $pass_fail"
	exit 4
    fi
    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_test: test must $pass_fail"
	echo "$0: debug[9]: in run_test: txzchk: $TXZCHK" 1>&2
	echo "$0: debug[9]: in run_test: tar: $TAR" 1>&2
	echo "$0: debug[9]: in run_test: fnamchk: $FNAMCHK" 1>&2
	echo "$0: debug[9]: in run_test: txzchk_test_file: $txzchk_test_file" 1>&2
	if [[ $pass_fail = fail ]]; then
	    echo "$0: debug[9]: in run-test: txzchk_err_file: $txzchk_err_file" 1>&2
	fi
    fi

    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: in run_test: about to run: $TXZCHK -w -v 0 -q -t $TAR -F $FNAMCHK -T -E txt -- $txzchk_test_file 2>$STDERR"
    fi

    "$TXZCHK" -w -v 0 -q -F "$FNAMCHK" -t "$TAR" -T -E txt -- "$txzchk_test_file" 2>"$STDERR"
    status="$?"

    if [[ $V_FLAG -ge 7 ]]; then
	echo "$0: debug[7]: in run_test: txzchk exit code: $status" 1>&2
    fi

    # examine test result
    #
    # if we have an error file (expected errors) and the output of the above
    # command does not match it is a fail.
    if [[ -e "$txzchk_err_file" ]]; then
	if ! cmp -s "$txzchk_err_file" "$STDERR"; then
	    echo "$0: Warning: in run_test: FAIL: $TXZCHK -w -v 0 -t $TAR -F $FNAMCHK -T -E txt $txzchk_test_file" | tee -a -- "$LOGFILE" 1>&2
	    echo "$0: Warning: in run_test: expected errors do not match result of test" 1>&2
	    if [[ $V_FLAG -lt 3 ]]; then
		echo "$0: Warning: for more details try: $TXZCHK -w -v 3 -t $TAR -F $FNAMCHK -T -E txt -- $txzchk_test_file" | tee -a -- "$LOGFILE" 1>&2
	    else
		echo "$0: Warning: for more details try: $TXZCHK -w -v $V_FLAG -t $TAR -F $FNAMCHK -T -E txt -- $txzchk_test_file" | tee -a -- "$LOGFILE" 1>&2
	    fi
	    echo | tee -a -- "${LOGFILE}" 1>&2
	    EXIT_CODE=42
	fi
    # Otherwise if there was output written to stderr it indicates that one or
    # more unexpected errors have occurred. This won't be because of a new test
    # file added to the bad subdirectory and the command to generate the error
    # file was not run because we specifically check this but it's possible it
    # occurs because of some other issue. We do that check by checking for
    # pass_fail to be fail and if it is fail and the error file does not exist
    # it is an error. Perhaps this check should also check that pass_fail is not
    # 'pass' but this would further complicate the script when the fact there's
    # stderr output is an error that should be addressed.
    elif [[ -s "$STDERR" ]]; then
	echo "$0: Warning: in run_test: FAIL: $TXZCHK -w -v 0 -t $TAR -F $FNAMCHK -T -E txt $txzchk_test_file" | tee -a -- "$LOGFILE" 1>&2
	echo "$0: Warning: in run_test: unexpected errors found for file that should have passed:" | tee -a "$LOGFILE" 1>&2
	echo cat "$STDERR"
	# tee -a -- "$LOGFILE < "$STDERR"
	< "$STDERR" tee -a -- "$LOGFILE"
	echo | tee -a -- "${LOGFILE}" 1>&2
	exit
	EXIT_CODE=43
    # all is okay if we get here
    elif [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: in run_test: PASS: $TXZCHK -w -v 0 -t $TAR -F $FNAMCHK -T -E txt $txzchk_test_file" 1>&2
    fi

    # return
    #
    return
}

# run txzchk tests
#

# run tests that must pass
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run txzchk tests that must pass: text files" 1>&2
fi
while read -r file; do
    run_test pass "$file"
done < <(find "$TXZCHK_GOOD_TREE" -type f -name '*.txt' -print)


# run tests that must fail
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run txzchk tests that must fail: text files" 1>&2
fi
while read -r file; do
    run_test fail "$file"
done < <(find "$TXZCHK_BAD_TREE" -type f -name '*.txt' -print)

# explicitly delete the temporary files

rm -f "$TARBALL" "$TEST_FILE" "$TAR_ERROR" "$STDERR"

# All Done!!! -- Jessica Noll, Age 2
#
if [[ $V_FLAG -ge 1 ]]; then
    if [[ $EXIT_CODE -eq 0 ]]; then
	echo "$0: debug[1]: all tests PASSED" 1>&2
    else
	echo "$0: debug[1]: some tests FAILED" 1>&2
    fi
fi
exit "$EXIT_CODE"
