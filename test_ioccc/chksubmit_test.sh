#!/usr/bin/env bash
#
# chksubmit_test.sh - chksubmit test on good and bad files under a directory
#
# "Because grammar and syntax alone do not make a complete language." :-)
#
# Use chksubmit to test all the submission directories under
# test_ioccc/workdir/{good,bad}.
#
# Copyright (c) 2022-2025 by Landon Curt Noll and Cody Boone Ferguson.
# All Rights Reserved.
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
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# This script and the JSON parser were co-developed in 2022-2025 by Cody Boone
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
export V_FLAG="0"
export DBG_LEVEL="0"
export Q_FLAG=""
export CHKSUBMIT="./chksubmit"
export CHKENTRY="./chkentry"
export LOGFILE="./test_ioccc/chksubmit_test.log"
export EXIT_CODE=0
export INVALID_DIRECTORY_FOUND=""
export WORKDIR="./test_ioccc/workdir"

export CHKSUBMIT_TEST_VERSION="2.1.3 2025-11-08"

export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-q] [-c chksubmit] [-C chkentry] [-d workdir]

    -h			print help and exit
    -V			print version and exit
    -v level		set verbosity level for this script: (def level: 0)
    -D dbg_level	set verbosity level for tests (def: level: 0)
    -q			quiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )
    -c chksubmit	path to chksubmit tool (def: $CHKSUBMIT)
    -C chkentry		path to chkentry tool (def: $CHKENTRY)
    -d workdir		good and bad submission workdir trees (def: $WORKDIR)

			NOTE: These subdirectories are required under workdir:

			    workdir/good/	valid submission directories
			    workdir/bad/	invalid submission directories

			NOTE: The default $WORKDIR is created by first running the
			      test_ioccc/mkiocccentry_test.sh test script.

Exit codes:
     0   all OK
     1   one or more tests failed
     3   invalid command line
     4	 missing or non-executable chksubmit
     5	 missing or non-readable workdir directory or subdirectory
 >= 10   internal error

chksubmit_test.sh version: $CHKSUBMIT_TEST_VERSION"


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


# parse args
#
while getopts :hVv:D:qc:C:d: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$CHKSUBMIT_TEST_VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    D)	DBG_LEVEL="$OPTARG";
	;;
    q)	Q_FLAG="-q";
	;;
    c)	CHKSUBMIT="$OPTARG";
	;;
    C)	CHKENTRY="$OPTARG";
	;;
    d)	WORKDIR="$OPTARG"
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
if [[ $# -ne 0 ]]; then
    echo "$0: ERROR: expected 0 arguments, found $#" 1>&2
    echo "$USAGE" 1>&2
    exit 3
fi
export GOOD_TREE="$WORKDIR/good"
export BAD_TREE="$WORKDIR/bad"

# debug of critical values
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: -v: $V_FLAG" 1>&2
    echo "$0: debug[1]: -D: $DBG_LEVEL" 1>&2
    if [[ -z $Q_FLAG ]]; then
	echo "$0: debug[1]: -q: false" 1>&2
    else
	echo "$0: debug[1]: -q: true" 1>&2
    fi
    echo "$0: debug[1]: chksubmit: $CHKSUBMIT" 1>&2
    echo "$0: debug[1]: chkentry: $CHKENTRY" 1>&2
    echo "$0: debug[1]: WORKDIR: $WORKDIR" 1>&2
    echo "$0: debug[1]: GOOD_TREE: $GOOD_TREE" 1>&2
    echo "$0: debug[1]: BAD_TREE: $BAD_TREE" 1>&2
fi

# firewall
#
if [[ ! -e $CHKSUBMIT ]]; then
    echo "$0: ERROR: chksubmit not found: $CHKSUBMIT"
    exit 4
fi
if [[ ! -f $CHKSUBMIT ]]; then
    echo "$0: ERROR: chksubmit not a regular file: $CHKSUBMIT"
    exit 4
fi
if [[ ! -x $CHKSUBMIT ]]; then
    echo "$0: ERROR: chksubmit not executable: $CHKSUBMIT"
    exit 4
fi
if [[ ! -e $CHKENTRY ]]; then
    echo "$0: ERROR: chkentry not found: $CHKENTRY"
    exit 4
fi
if [[ ! -f $CHKENTRY ]]; then
    echo "$0: ERROR: chkentry not a regular file: $CHKENTRY"
    exit 4
fi
if [[ ! -x $CHKENTRY ]]; then
    echo "$0: ERROR: chkentry not executable: $CHKENTRY"
    exit 4
fi

# check that workdir is a readable directory
#
if [[ ! -e $WORKDIR ]]; then
    echo "$0: ERROR: workdir not found: $WORKDIR" 1>&2
    exit 5
fi
if [[ ! -d $WORKDIR ]]; then
    echo "$0: ERROR: workdir not a directory: $WORKDIR" 1>&2
    exit 5
fi
if [[ ! -r $WORKDIR ]]; then
    echo "$0: ERROR: workdir not a readable directory: $WORKDIR" 1>&2
    exit 5
fi

# good tree
#
if [[ ! -e $GOOD_TREE ]]; then
    echo "$0: ERROR: good workdir for chksubmit directory not found: $GOOD_TREE" 1>&2
    exit 5
fi
if [[ ! -d $GOOD_TREE ]]; then
    echo "$0: ERROR: good workdir for chksubmit not a directory: $GOOD_TREE" 1>&2
    exit 5
fi
if [[ ! -r $GOOD_TREE ]]; then
    echo "$0: ERROR: good workdir for chksubmit not a readable directory: $GOOD_TREE" 1>&2
    exit 5
fi

# bad tree
#
if [[ ! -e $BAD_TREE ]]; then
    echo "$0: ERROR: bad workdir for chksubmit directory not found: $BAD_TREE" 1>&2
    exit 5
fi
if [[ ! -d $BAD_TREE ]]; then
    echo "$0: ERROR: bad workdir for chksubmit not a directory: $BAD_TREE" 1>&2
    exit 5
fi
if [[ ! -r $BAD_TREE ]]; then
    echo "$0: ERROR: bad workdir for chksubmit not readable directory: $BAD_TREE" 1>&2
    exit 5
fi

# remove logfile so that each run starts out with an empty file
#
rm -f "$LOGFILE"
touch "$LOGFILE"
if [[ ! -f "${LOGFILE}" ]]; then
    echo "$0: ERROR: couldn't create log file" 1>&2
    exit 10
fi
if [[ ! -w "${LOGFILE}" ]]; then
    echo "$0: ERROR: log file not writable" 1>&2
    exit 11
fi

# run_good_test - run a single chksubmit test on a directory that must pass
#
# usage:
#	run_good_test chksubmit dbg_level quiet_mode directory
#
#	chksubmit		path to the chksubmit program
#	chkentry		path to the chkentry program
#	dbg_level		internal test debugging level to use as in: chksubmit -v dbg_level
#	quiet_mode		quiet mode to use in: chksubmit -q
#	workdir		        directory to check
#
# The return code of this function is non-zero if an internal error occurs OR if
# the return value of chksubmit is not 0.
#
run_good_test()
{
    # parse args
    #
    if [[ $# -ne 5 ]]; then
	echo "$0: ERROR: expected 6 args to run_good_test, found $#" 1>&2
	exit 12
    fi
    declare chksubmit="$1"
    declare chkentry="$2"
    declare dbg_level="$3"
    declare quiet_mode="$4"
    declare workdir="$5"

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_good_test: chksubmit: $chksubmit" 1>&2
	echo "$0: debug[9]: in run_good_test: chkentry: $chkentry" 1>&2
	echo "$0: debug[9]: in run_good_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_good_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_good_test: workdir: $workdir" 1>&2
    fi

    # workdir must exist
    #
    if [[ ! -d "$workdir" ]] || [[ ! -r "$workdir" ]]; then
        echo "$0: ERROR: workdir is not a directory or is unreadable: $workdir"
        exit 13
    fi

    # perform the test
    #
    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must pass: $chksubmit -C $chkentry -v $dbg_level -- $workdir | tee -a -- ${LOGFILE}" 1>&2
	fi
	echo "$0: about to run test that must pass: $chksubmit -v $dbg_level -- $workdir | tee -a -- ${LOGFILE}" 1>&2
	"$chksubmit" -C "$chkentry" -v "$dbg_level" -- "$workdir" | tee -a -- "${LOGFILE}"
        status="${PIPESTATUS[0]}"
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must pass: $chksubmit -C $chkentry -v $dbg_level $quiet_mode -- $workdir | tee -a -- ${LOGFILE}" 1>&2
	fi
	echo "$0: about to run test that must pass: $chksubmit -C $chkentry -v $dbg_level $quiet_mode -- $workdir | tee -a -- ${LOGFILE}" 1>&2
	"$chksubmit" -C "$chkentry" -v "$dbg_level" "$quiet_mode" -- "$workdir" | tee -a -- "${LOGFILE}"
        status="${PIPESTATUS[0]}"
    fi

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
        echo "$0: test $workdir should PASS: chksubmit PASS with exit code 0" | tee -a "${LOGFILE}"
        if [[ $V_FLAG -ge 3 ]]; then
            echo "$0: debug[3]: test $workdir should PASS: chksubmit passed with exit code 0" | tee -a -- "${LOGFILE}"
        fi
    else
        echo "$0: test $workdir should PASS: chksubmit FAILED with exit code: $status" | tee -a -- "${LOGFILE}"
        if [[ $V_FLAG -ge 1 ]]; then
            if [[ $V_FLAG -ge 3 ]]; then
                echo "$0: debug[3]: debug[3]: test $workdir should PASS: chksubmit FAILED with exit code: $status" | tee -a -- "${LOGFILE}"
            fi
        fi
        EXIT_CODE=1
    fi
    echo >> "${LOGFILE}"

    # return
    #
    return
}

# run_bad_test - run a single chksubmit test on a directory that must pass
#
# usage:
#	run_bad_test chksubmit dbg_level json_dbg_level quiet_mode directory
#
#	chksubmit		path to the chksubmit program
#	dbg_level		internal test debugging level to use as in: chksubmit -v dbg_level
#	json_dbg_level		JSON parser debug level to use in: chksubmit -J json_dbg_level
#	quiet_mode		quiet mode to use in: chksubmit -q
#	workdir		        directory to check
#
# The return code of this function is non-zero if an internal error occurs OR if
# the return value of chksubmit is 0.
#
run_bad_test()
{
    # parse args
    #
    if [[ $# -ne 5 ]]; then
	echo "$0: ERROR: expected 5 args to run_bad_test, found $#" 1>&2
	exit 12
    fi
    declare chksubmit="$1"
    declare chkentry="$2"
    declare dbg_level="$3"
    declare quiet_mode="$4"
    declare workdir="$5"

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_bad_test: chksubmit: $chksubmit" 1>&2
	echo "$0: debug[9]: in run_good_test: chkentry: $chkentry" 1>&2
	echo "$0: debug[9]: in run_bad_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_bad_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_bad_test: workdir: $workdir" 1>&2
    fi

    # workdir must exist
    #
    if [[ ! -d "$workdir" ]] || [[ ! -r "$workdir" ]]; then
        echo "$0: ERROR: workdir is not a directory or is unreadable: $workdir"
        exit 13
    fi

    # perform the test
    #
    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that MUST FAIL: $chksubmit -C $chkentry -v $dbg_level -- $workdir | tee -a -- ${LOGFILE}" 1>&2
	fi
	echo "$0: about to run test that MUST FAIL: $chksubmit -C $chkentry -v $dbg_level -- $workdir | tee -a -- ${LOGFILE}"
        "$chksubmit" -C "$chkentry" -v "$dbg_level" -- "$workdir" | tee -a -- "${LOGFILE}"
        status="${PIPESTATUS[0]}"
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that MUST FAIL: $chksubmit -C $chkentry -v $dbg_level $quiet_mode -- $workdir | tee -a -- ${LOGFILE}" 1>&2
	fi
	echo "$0: about to run test that MUST FAIL: $chksubmit -C $chkentry -v $dbg_level -q -- $workdir | tee -a -- ${LOGFILE}"
	"$chksubmit" -C "$chkentry" -v "$dbg_level" -q -- "$workdir" | tee -a -- "${LOGFILE}"
        status="${PIPESTATUS[0]}"
    fi

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
        echo "$0: test $workdir should FAIL: chksubmit incorrectly passed with exit code 0" | tee -a -- "${LOGFILE}"
        if [[ $V_FLAG -ge 3 ]]; then
            echo "$0: debug[3]: test $workdir: chksubmit should FAIL: chksubmit incorrectly passed with exit code 0" | tee -a -- "${LOGFILE}"
        fi
        EXIT_CODE=1
    else
        echo "$0: test $workdir should FAIL: chksubmit correctly failed with exit code: $status" | tee -a -- "${LOGFILE}"
        if [[ $V_FLAG -ge 1 ]]; then
            if [[ $V_FLAG -ge 3 ]]; then
                echo "$0: debug[3]: debug[3]: test $workdir should FAIL: chksubmit correctly failed with exit code: $status" | tee -a -- "${LOGFILE}"
            fi
        fi
    fi
    echo >> "${LOGFILE}"

    # return
    #
    return
}

# run tests that must pass: good
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run chksubmit tests that must pass: good/ files" | tee -a -- "${LOGFILE}"
fi
while read -r dir; do
    run_good_test "$CHKSUBMIT" "$CHKENTRY" "$DBG_LEVEL" "$Q_FLAG" "$dir"
done < <(find "$GOOD_TREE" -mindepth 1 -maxdepth 1 -type d -print)


# run tests that must fail: bad
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run chksubmit tests that must fail: bad/ files" | tee -a -- "${LOGFILE}"
fi
while read -r dir; do
    run_bad_test "$CHKSUBMIT" "$CHKENTRY" "$DBG_LEVEL" "$Q_FLAG" "$dir"
done < <(find "$BAD_TREE" -mindepth 1 -maxdepth 1 -type d -print)

# determine exit code
#
if [[ $EXIT_CODE -ne 0 ]]; then
    echo "$0: one or more tests FAILED" | tee -a -- "${LOGFILE}"
    if [[ $V_FLAG -ge 1 ]]; then
        echo "$0: debug[1]: one or more tests FAILED" 1>&2
        echo "$0: Notice: one or more directories failed chksubmit tests, see $LOGFILE for details" 1>&2
        echo "$0: ERROR: about to exit: $EXIT_CODE" 1>&2
    fi
    echo "$0: about to exit: $EXIT_CODE" | tee -a -- "${LOGFILE}"
else
    echo "$0: all tests PASSED" | tee -a -- "${LOGFILE}"
    if [[ $V_FLAG -ge 1 ]]; then
        echo "$0: debug[1]: all tests PASSED" 1>&2
        echo 1>&2
    fi
    echo "$0: about to exit: $EXIT_CODE" | tee -a -- "${LOGFILE}"
fi

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
exit "$EXIT_CODE"
