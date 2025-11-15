#!/usr/bin/env bash
#
# txzchk_test.sh - test txzchk with good and bad tarballs (as text files)
#
# Copyright (c) 2022-2025 by Cody Boone Ferguson.  All Rights Reserved.
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
# CODY BOONE FERGUSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
# SHALL CODY BOONE FERGUSON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
# DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
# OF THIS SOFTWARE.
#
# This tool and script were written in 2022-2025 by Cody Boone Ferguson:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# Dedicated to:
#
#	The many poor souls who have been tarred and feathered:
#
#	    "Because sometimes people throw feathers on tar." :-(
#
#	And to my wonderful Mum and my dear cousin and friend Dani:
#
#	    "Because even feathery balls of tar need some love." :-)
#
# Disclaimer:
#
#	No pitman or coal mines were harmed in the making of this tool and
#	neither were any pine trees or birch trees. Whether the fact no coal
#	mines were harmed is a good or bad thing might be debatable but
#	nevertheless none were harmed. :-) More importantly, no tar pits -
#	including the La Brea Tar Pits - were disturbed in the making of this
#	tool. :-)
#
# Share and enjoy! :-)
#     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
#

# setup


# IOCCC requires use of C locale
#
export LANG="C"
export LC_CTYPE="C"
export LC_NUMERIC="C"
export LC_TIME="C"
export LC_COLLATE="C"
export LC_MONETARY="C"
export LC_MESSAGES="C"
export LC_PAPER="C"
export LC_NAME="C"
export LC_ADDRESS="C"
export LC_TELEPHONE="C"
export LC_MEASUREMENT="C"
export LC_IDENTIFICATION="C"
export LC_ALL="C"


# attempt to fetch system specific path to the tool (tar) we need
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
[[ -z "$TAR" ]] && TAR="/usr/bin/tar"

export TXZCHK_TEST_VERSION="2.0.2 2025-11-15"
export FNAMCHK="./test_ioccc/fnamchk"
export TXZCHK="./txzchk"
export TXZCHK_TREE="./test_ioccc/test_txzchk"
export USAGE="usage: $0 [-h] [-V] [-v level] [-t tar] [-T txzchk] [-F fnamchk] [-d txzchk_tree] [-Z topdir] [-k] [-B]

    -h			    print help and exit
    -V			    print version and exit
    -v level		    set verbosity level for this script: (def level: 0)
    -t tar		    path to tar that accepts -J option (def: $TAR)
    -T txzchk		    path to txzchk executable (def: $TXZCHK)
    -F fnamchk	            path to fnamchk (def: $FNAMCHK)

    -d txzchk_tree	    tree where txzchk test files are to be found (def: $TXZCHK_TREE)
			      These subdirectories are expected:
				txzchk_tree/bad
				txzchk_tree/good
			      NOTE: the contents of the error files in the bad subdir
				    are not flexible as the check for errors reported by
				    txzchk and the error file is done as an exact match
    -Z topdir		    top level build directory (def: try . or ..)
    -k			    keep temporary files on exit (def: remove temporary files before exiting)
    -B			    rebuild error files

Exit codes:
     0   all OK
     1   at least one test failed
     2   -h and help string printed or -V and version string printed
     3   invalid command line
 >= 10   internal error or missing file or directory

txzchk_test.sh version: $TXZCHK_TEST_VERSION"

export EXIT_CODE=0
export LOGFILE="./test_ioccc/txzchk_test.log"
export TOPDIR=

# parse args
#
export V_FLAG="0"
export K_FLAG=""
export B_FLAG=""
while getopts :hVv:t:d:T:F:Z:kB flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$TXZCHK_TEST_VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    t)	TAR="$OPTARG";
	;;
    d)	TXZCHK_TREE="$OPTARG";
	;;
    F)	FNAMCHK="$OPTARG";
	;;
    T)	TXZCHK="$OPTARG";
	;;
    Z)  TOPDIR="$OPTARG";
        ;;
    k)  K_FLAG="true";
        ;;
    B)	B_FLAG="true";
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
if [[ $# -gt 1 ]]; then
    echo "$0: ERROR: expected no more than 1 argument, found $#" 1>&2
    echo 1>&2
    echo "$USAGE" 1>&2
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

# if -B used we will just rebuild the test error files
#
if [[ -n "$B_FLAG" ]]; then
    echo "Are you POSITIVE that the *.err files are simply out of date and" 1>&2
    echo "that the difference between what is in the *.err files and" 1>&2
    read -r -p "what is being generated is correct (YES/NO)? "

    if [[ "$REPLY" != "YES" ]]; then
	echo "$0: did not get YES as a reply, not regenerating files." 1>&2
	exit 3
    fi

    while read -r file; do
        if [[ "$file" =~ submit.test* ]]; then
            echo "$TXZCHK -v 0 -x -w -T -E txt -F $FNAMCHK $file 2>$file.err"; \
            "$TXZCHK" -v 0 -x -w -T -E txt -F "$FNAMCHK" "$file" 2>"$file.err"; \
        else
            echo "$TXZCHK -v 0 -w -T -E txt -F $FNAMCHK $file 2>$file.err"; \
            "$TXZCHK" -v 0 -w -T -E txt -F "$FNAMCHK" "$file" 2>"$file.err"; \
        fi
    done < <(find "$TXZCHK_BAD_TREE" -type f -name '*.txt' -print)

    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: rebuilt test error files" 1>&2
    fi

    echo 1>&2
    echo "You should now run, from the top level directory:" 1>&2
    echo 1>&2
    echo "    ./test_ioccc/txzchk_test.sh -v $V_FLAG" 1>&2
    echo 1>&2
    echo "to verify that everything is in order. If it is and there are any file changes," 1>&2
    echo "you will need to then run:" 1>&2
    echo 1>&2
    echo "    git add test_ioccc/test_txzchk/bad/*.txt test_ioccc/test_txzchk/bad/*.err" 1>&2
    echo 1>&2
    echo "and then commit (and if necessary open a pull request)." 1>&2
    echo "Otherwise, if there is a problem you will have to fix any other issues first." 1>&2

    exit 0
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
TAR_ERROR=$(mktemp .txzchk_test.tar_err.out.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp .txzchk_test.tar_err.out.XXXXXXXXXX exit code: $status" 1>&2
    EXIT_CODE=33
    RUN_TAR_TEST=
fi
TARBALL=$(mktemp .txzchk_test.tarball.txz.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp .txzchk_test.tarball.txz.XXXXXXXXXX  exit code: $status" 0>&2
    EXIT_CODE=34
    RUN_TAR_TEST=
fi

# We need a file to write the output of txzchk to in order to compare it
# with any error file. This is needed for the files that are supposed to
# fail but it's possible that there could be a use for good files too.
TMP_STDERR_FILE=$(mktemp -u .txzchk_test.stderr.XXXXXXXXXX)
# delete the temporary file in the off chance it already exists
rm -f "$TMP_STDERR_FILE"
# now let's make sure we can create it as well: if we can't or it's not
# writable there's an issue.
#
touch "$TMP_STDERR_FILE"
if [[ ! -e "$TMP_STDERR_FILE" ]]; then
    echo "$0: could not create output file: $TMP_STDERR_FILE"
    exit 35
fi
if [[ ! -w "$TMP_STDERR_FILE" ]]; then
    echo "$0: output file not writable: $TMP_STDERR_FILE"
    exit 36
fi

# remove or keep (some) temporary files
#
if [[ -z $K_FLAG ]]; then
    trap "rm -f \$TARBALL \$TEST_FILE \$TAR_ERROR \$TMP_STDERR_FILE; exit" 0 1 2 3 15
else
    trap "rm -f \$TARBALL \$TEST_FILE \$TAR_ERROR \$TMP_STDERR_FILE; exit" 1 2 3 15
fi

# run tar test
#
TAR_TEST_SUCCESS="true"
if [[ -n $RUN_TAR_TEST ]]; then
    "${TAR}" --format=v7 -cJf "$TARBALL" "$TEST_FILE" 2>"$TAR_ERROR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: $TAR --format=v7 -cJf $TARBALL $TEST_FILE 2>$TAR_ERROR exit code: $status" 1>&2
	EXIT_CODE=1
	TAR_TEST_SUCCESS=
    fi
    if [[ ! -s $TARBALL ]]; then
	echo "$0: ERROR: did not find a non-empty tarball: $TARBALL" 1>&2
	EXIT_CODE=1
	TAR_TEST_SUCCESS=
    fi
    if [[ -s $TAR_ERROR ]]; then
	echo "$0: Notice: tar stderr follows:" 1>&2
	sed -e 's/^/Notice: /' < "$TAR_ERROR" 1>&2
	echo "$0: Notice: end of tar stderr" 1>&2
	EXIT_CODE=1
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
	exit 37
    fi
    declare pass_fail="$1"
    declare txzchk_test_file="$2"
    declare txzchk_err_file="$txzchk_test_file.err"

    if [[ ! -e $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not found: $txzchk_test_file"
	exit 38
    fi
    if [[ ! -f $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not a regular file: $txzchk_test_file"
	exit 39
    fi
    if [[ ! -r $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not readable: $txzchk_test_file"
	exit 40
    fi

   test_mode=""
   if [[ "$txzchk_test_file" =~ submit.test* ]]; then
       test_mode="-x"
   fi

    # if pass_fail is fail then there has to be an error file
    if [[ $pass_fail = fail ]]; then
	if [[ ! -e $txzchk_err_file ]]; then
	    echo "$0: in run_test: txzchk_err_file not found for test that must fail: $txzchk_err_file"
	    exit 42
	fi
	if [[ ! -f $txzchk_err_file ]]; then
	    echo "$0: in run_test: txzchk_err_file not a regular file for test that must fail: $txzchk_err_file"
	    exit 42
	fi
	if [[ ! -r $txzchk_err_file ]]; then
	    echo "$0: in run_test: txzchk_err_file not readable for test that must fail: $txzchk_err_file"
	    exit 42
	fi
    elif [[ $pass_fail = pass ]]; then
	if [[ -e $txzchk_err_file ]]; then
	    echo "$0: in run_test: txzchk_err_file exists for test that must not fail: $txzchk_err_file"
	    exit 42
	fi
    else
	echo "$0: pass_fail neither 'pass' nor 'fail': $pass_fail"
	exit 42
    fi
    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_test: test must $pass_fail"
	echo "$0: debug[9]: in run_test: txzchk: $TXZCHK" 1>&2
	echo "$0: debug[9]: in run_test: tar: $TAR" 1>&2
	echo "$0: debug[9]: in run_test: fnamchk: $FNAMCHK" 1>&2
	echo "$0: debug[9]: in run_test: test_mode: $test_mode" 1>&2
	echo "$0: debug[9]: in run_test: txzchk_test_file: $txzchk_test_file" 1>&2
	if [[ $pass_fail = fail ]]; then
	    echo "$0: debug[9]: in run_test: txzchk_err_file: $txzchk_err_file" 1>&2
	fi
    fi

    if [[ $V_FLAG -ge 5 ]]; then
        if [[ -n "$test_mode" ]]; then
    	    echo "$0: debug[5]: in run_test: about to run: $TXZCHK -x -w -v 0 -t $TAR -F $FNAMCHK -T -E txt -- $txzchk_test_file 2>$TMP_STDERR_FILE"
        else
    	    echo "$0: debug[5]: in run_test: about to run: $TXZCHK -w -v 0 -t $TAR -F $FNAMCHK -T -E txt -- $txzchk_test_file 2>$TMP_STDERR_FILE"
        fi
    fi

    if [[ -n "$test_mode" ]]; then
        "$TXZCHK" "-x" -w -v 0 -F "$FNAMCHK" -t "$TAR" -T -E txt -- "$txzchk_test_file" 2>"$TMP_STDERR_FILE"
    else
        "$TXZCHK" -w -v 0 -F "$FNAMCHK" -t "$TAR" -T -E txt -- "$txzchk_test_file" 2>"$TMP_STDERR_FILE"
    fi
    status="$?"

    if [[ $V_FLAG -ge 7 ]]; then
	echo "$0: debug[7]: in run_test: txzchk exit code: $status" 1>&2
    fi

    # examine test result
    #
    # if we have an error file (expected errors) and the output of the above
    # command does not match it is a fail.
    if [[ -e "$txzchk_err_file" ]]; then
	if ! cmp -s "$txzchk_err_file" "$TMP_STDERR_FILE"; then
	    echo "$0: Warning: in run_test: FAIL: $TXZCHK $test_mode -w -v 0 -t $TAR -F $FNAMCHK -T -E txt $txzchk_test_file" | tee -a -- "$LOGFILE" 1>&2
	    echo "$0: Warning: in run_test: expected errors: $txzchk_err_file do not match result of test: $TMP_STDERR_FILE" 1>&2
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: Warning: diff -u $txzchk_err_file $TMP_STDERR_FILE starts below" 1>&2
		diff -u "$txzchk_err_file" "$TMP_STDERR_FILE" 1>&2
		echo "$0: Warning: diff -u $txzchk_err_file $TMP_STDERR_FILE ends above" 1>&2
	    fi
	    if [[ $V_FLAG -lt 3 ]]; then
		echo | tee -a -- "${LOGFILE}" 1>&2
		echo "$0: Warning: for more details try: $TXZCHK $test_mode -w -v 3 -F $FNAMCHK -t $TAR -T -E txt -- $txzchk_test_file" | tee -a -- "$LOGFILE" 1>&2
	    else
		echo | tee -a -- "${LOGFILE}" 1>&2
		echo "$0: Warning: for more details try: $TXZCHK $test_mode -w -v $V_FLAG -F $FNAMCHK -t $TAR -T -E txt -- $txzchk_test_file" | tee -a -- "$LOGFILE" 1>&2
	    fi
	    echo | tee -a -- "${LOGFILE}" 1>&2
	    if [[ -n "$test_mode" ]]; then
		echo "$0: Warning: to see what is different try: $TXZCHK -x -w -v 0 -F $FNAMCHK -t $TAR -T -E txt -- $txzchk_test_file 2>&1 >/dev/null | diff -u $txzchk_err_file -" | tee -a -- "$LOGFILE" 1>&2
	    else
		echo "$0: Warning: to see what is different try: $TXZCHK -w -v 0 -F $FNAMCHK -t $TAR -T -E txt -- $txzchk_test_file 2>&1 >/dev/null | diff -u $txzchk_err_file -" | tee -a -- "$LOGFILE" 1>&2
	    fi
	    echo | tee -a -- "${LOGFILE}" 1>&2
	    echo | tee -a -- "${LOGFILE}" 1>&2
	    EXIT_CODE=1
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
    elif [[ -s "$TMP_STDERR_FILE" ]]; then
	echo "$0: Warning: in run_test: FAIL: $TXZCHK $test_mode -w -v 0 -t $TAR -F $FNAMCHK -T -E txt $txzchk_test_file" | tee -a -- "$LOGFILE" 1>&2
	echo "$0: Warning: in run_test: unexpected errors found for file that should have passed:" | tee -a "$LOGFILE" 1>&2
	echo cat "$TMP_STDERR_FILE"
	# tee -a -- "$LOGFILE < "$TMP_STDERR_FILE"
	< "$TMP_STDERR_FILE" tee -a -- "$LOGFILE"
	echo | tee -a -- "${LOGFILE}" 1>&2
	EXIT_CODE=1
    # all is okay if we get here
    elif [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: in run_test: PASS: $TXZCHK $test_mode -w -v 0 -t $TAR -F $FNAMCHK -T -E txt $txzchk_test_file" 1>&2
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

if [[ -z $K_FLAG ]]; then
    rm -f "$TARBALL" "$TEST_FILE" "$TAR_ERROR" "$TMP_STDERR_FILE"
else
    echo
    echo "$0: keeping temporary files due to use of -k"
    echo
    echo "$0: to remove the temporary files:"
    echo
    echo -n "rm -f"
    if [[ -e $TARBALL ]]; then
	echo -n " $TARBALL"
    fi
    if [[ -e $TEST_FILE ]]; then
	echo -n " $TEST_FILE"
    fi
    if [[ -e $TAR_ERROR ]]; then
	echo -n " $TAR_ERROR"
    fi
    if [[ -e $TMP_STDERR_FILE ]]; then
	echo -n " $TMP_STDERR_FILE"
    fi
    echo
fi

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
if [[ $V_FLAG -ge 1 ]]; then
    if [[ $EXIT_CODE -eq 0 ]]; then
	echo "$0: debug[1]: all tests PASSED" 1>&2
    else
	echo "$0: debug[1]: some tests FAILED" 1>&2
    fi
fi
exit "$EXIT_CODE"
