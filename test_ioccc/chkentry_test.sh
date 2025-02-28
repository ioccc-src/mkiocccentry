#!/usr/bin/env bash
#
# chkentry_test.sh - chkentry test on good and bad files under a directory
#
# "Because grammar and syntax alone do not make a complete language." :-)
#
# Use chkentry to test all the files under test_JSON{info,auth}.json/good/ and
# verify there are no JSON semantic errors.  If any JSON semantic errors are
# detected, this script will exit non-zero.
#
# Use chkentry to test all the files under test_JSON{info,auth}.json/bad/ and
# verify there are JSON semantic errors.  If any file is found to be free of
# JSON semantic errors, this script will exit non-zero.
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
export JSON_DBG_LEVEL="0"
export Q_FLAG=""
export CHKENTRY="./chkentry"
export LOGFILE="./test_ioccc/chkentry_test.log"
export EXIT_CODE=0
export INVALID_JSON_FOUND=""
export UNEXPECTED_SEMANTIC_ERROR=""
export SEMANTIC_ERROR_MISSED=""
export SLOT_TREE="./test_ioccc/slot"

export CHKENTRY_TEST_VERSION="1.1.1 2025-02-28"

export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-J level] [-q] [-c chkentry] [-d slot_tree]

    -h			print help and exit
    -V			print version and exit
    -v level		set verbosity level for this script: (def level: 0)
    -D dbg_level	set verbosity level for tests (def: level: 0)
    -J level		set JSON parser verbosity level (def level: 0)
    -q			quiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )
    -c chkentry		path to chkentry tool (def: $CHKENTRY)
    -d slot_tree	test files slot_tree/{info,auth}.json/{good,bad}/*.json (def: $SLOT_TREE)

			NOTE: these subdirectories are required under slot_tree:

			    slot_tree/good/	valid submission directories
			    slot_tree/bad/	invalid submission directories

Exit codes:
     0   all OK
     1   all JSON files were valid JSON; however some chkentry semantic tests failed
     2   -h and help string printed or -V and version string printed
     3   invalid command line
     4	 missing or non-executable chkentry
     5	 missing or non-readable slot_tree directory or subdirectory
     6	 some files were invalid JSON; chkentry correctly tested all other files
     7	 some files were invalid JSON and some chkentry tests failed
 >= 10   internal error

chkentry_test.sh version: $CHKENTRY_TEST_VERSION"

# parse args
#
while getopts :hVv:D:J:qc:d: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$CHKENTRY_TEST_VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    D)	DBG_LEVEL="$OPTARG";
	;;
    J)	JSON_DBG_LEVEL="$OPTARG";
	;;
    q)	Q_FLAG="-q";
	;;
    c)	CHKENTRY="$OPTARG";
	;;
    d)	SLOT_TREE="$OPTARG"
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
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: -v: $V_FLAG" 1>&2
    echo "$0: debug[1]: -D: $DBG_LEVEL" 1>&2
    echo "$0: debug[1]: -J: $JSON_DBG_LEVEL" 1>&2
    if [[ -z $Q_FLAG ]]; then
	echo "$0: debug[1]: -q: false" 1>&2
    else
	echo "$0: debug[1]: -q: true" 1>&2
    fi
    echo "$0: debug[1]: chkentry: $CHKENTRY" 1>&2
fi

# check args
#
shift $(( OPTIND - 1 ));
if [[ $# -ne 0 ]]; then
    echo "$0: ERROR: expected 0 arguments, found $#" 1>&2
    echo "$USAGE" 1>&2
    exit 3
fi
export GOOD_TREE="$SLOT_TREE/good"
export BAD_TREE="$SLOT_TREE/bad"

# firewall
#
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

# check that slot_tree is a readable directory
#
if [[ ! -e $SLOT_TREE ]]; then
    echo "$0: ERROR: slot_tree not found: $SLOT_TREE" 1>&2
    exit 5
fi
if [[ ! -d $SLOT_TREE ]]; then
    echo "$0: ERROR: slot_tree not a directory: $SLOT_TREE" 1>&2
    exit 5
fi
if [[ ! -r $SLOT_TREE ]]; then
    echo "$0: ERROR: slot_tree not readable directory: $SLOT_TREE" 1>&2
    exit 5
fi

# good tree
#
if [[ ! -e $GOOD_TREE ]]; then
    echo "$0: ERROR: slot_tree/good for chkentry directory not found: $GOOD_TREE" 1>&2
    exit 5
fi
if [[ ! -d $GOOD_TREE ]]; then
    echo "$0: ERROR: slot_tree/good for chkentry not a directory: $GOOD_TREE" 1>&2
    exit 5
fi
if [[ ! -r $GOOD_TREE ]]; then
    echo "$0: ERROR: slot_tree/good for chkentry not readable directory: $GOOD_TREE" 1>&2
    exit 5
fi

# bad tree
#
if [[ ! -e $BAD_TREE ]]; then
    echo "$0: ERROR: slot_tree/bad for chkentry directory not found: $BAD_TREE" 1>&2
    exit 5
fi
if [[ ! -d $BAD_TREE ]]; then
    echo "$0: ERROR: slot_tree/bad for chkentry not a directory: $BAD_TREE" 1>&2
    exit 5
fi
if [[ ! -r $BAD_TREE ]]; then
    echo "$0: ERROR: slot_tree/bad for chkentry not readable directory: $BAD_TREE" 1>&2
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

# run_good_test - run a single chkentry test on a directory that must pass
#
# usage:
#	run_good_test chkentry dbg_level json_dbg_level quiet_mode directory
#
#	chkentry		path to the chkentry program
#	dbg_level		internal test debugging level to use as in: chkentry -v dbg_level
#	json_dbg_level		JSON parser debug level to use in: chkentry -J json_dbg_level
#	quiet_mode		quiet mode to use in: chkentry -q
#	directory		directory to check
#
# The return code of this function is non-zero if an internal error occurs OR if
# the return value of chkentry is not 0.
#
run_good_test()
{
    # parse args
    #
    if [[ $# -ne 5 ]]; then
	echo "$0: ERROR: expected 5 args to run_good_test, found $#" 1>&2
	exit 12
    fi
    declare chkentry="$1"
    declare dbg_level="$2"
    declare json_dbg_level="$3"
    declare quiet_mode="$4"
    declare directory="$5"

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_good_test: chkentry: $chkentry" 1>&2
	echo "$0: debug[9]: in run_good_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_good_test: json_dbg_level: $json_dbg_level" 1>&2
	echo "$0: debug[9]: in run_good_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_good_test: directory: $directory" 1>&2
    fi

    # directory must exist
    #
    if [[ ! -d "$directory" ]] || [[ ! -r "$directory" ]]; then
        echo "$0: ERROR: directory is not a directory or unreadable: $directory"
        exit 13
    fi

    # perform the test
    #
    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must pass: $chkentry -v $dbg_level -J $json_dbg_level -- $directory >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: about to run test that must pass: $chkentry -v $dbg_level -J $json_dbg_level -- $directory >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -- "$directory" >> "${LOGFILE}" 2>&1
	status="$?"
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must pass: $chkentry -v $dbg_level -J $json_dbg_level -q -- $directory >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: about to run test that must pass: $chkentry -v $dbg_level -J $json_dbg_level -q -- $directory >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -q -- "$directory" >> "${LOGFILE}" 2>&1
	status="$?"
    fi

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
        echo "$0: test $directory should PASS: chkentry PASS with exit code 0" 1>&2 >> "${LOGFILE}"
        if [[ $V_FLAG -ge 3 ]]; then
            echo "$0: debug[3]: test $directory should PASS: chkentry passed with exit code 0" 1>&2
        fi
    else
        echo "$0: test $directory should FAIL: chkentry FAIL correctly with exit code: $status" 1>&2 >> "${LOGFILE}"
        if [[ $V_FLAG -ge 1 ]]; then
            if [[ $V_FLAG -ge 3 ]]; then
                echo "$0: debug[3]: debug[3]: test $directory should FAIL: chkentry FAIL correctly with exit code: $status" 1>&2
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
    echo "$0: debug[3]: about to run chkentry tests that must pass: good/ files" 1>&2
fi
while read -r file; do
    run_good_test "$CHKENTRY" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$file"
done < <(find "$GOOD_TREE" -mindepth 2 -maxdepth 2 -type d -print)


# run tests that must fail: bad
#
#if [[ $V_FLAG -ge 3 ]]; then
#    echo "$0: debug[3]: about to run chkentry tests that must fail: bad/ files" 1>&2
#fi
#while read -r file; do
#    run_bad_test "$CHKENTRY" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$file"
#done < <(find "$BAD_TREE" -type d -print)

# determine exit code
#
if [[ -z $INVALID_JSON_FOUND ]]; then
    if [[ -z $UNEXPECTED_SEMANTIC_ERROR && -z $SEMANTIC_ERROR_MISSED ]]; then
	EXIT_CODE=0
	echo "$0: all tests PASSED" >> "${LOGFILE}"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: all tests PASSED" 1>&2
	fi
	echo "$0: about to exit: $EXIT_CODE" >> "${LOGFILE}"
    else
	EXIT_CODE=1
	echo "$0: all JSON files were valid JSON; however some chkentry semantic tests failed" >> "${LOGFILE}"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: all JSON files were valid JSON; however some chkentry semantic tests failed" 1>&2
	    echo 1>&2
	    echo "$0: Notice: chkentry semantic tests failed, see $LOGFILE for details" 1>&2
	    echo "$0: ERROR: about to exit: $EXIT_CODE" 1>&2
	fi
	echo "$0: ERROR: about to exit: $EXIT_CODE" >> "${LOGFILE}"
    fi
else
    if [[ -z $UNEXPECTED_SEMANTIC_ERROR && -z $SEMANTIC_ERROR_MISSED ]]; then
	EXIT_CODE=6
	echo "$0: some files were invalid JSON; chkentry correctly tested all other files" >> "${LOGFILE}"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: some files were invalid JSON; chkentry correctly tested all other files" 1>&2
	    echo 1>&2
	    echo "$0: Notice: some files were invalid JSON, see $LOGFILE for details" 1>&2
	    echo "$0: ERROR: about to exit: $EXIT_CODE" 1>&2
	fi
	echo "$0: ERROR: about to exit: $EXIT_CODE" >> "${LOGFILE}"
    else
	EXIT_CODE=7
	echo "$0: some files were invalid JSON and some chkentry tests failed" >> "${LOGFILE}"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: some files were invalid JSON and some chkentry tests failed" 1>&2
	    echo 1>&2
	    echo "$0: Notice: chkentry tests failed and some files were invalid JSON: see $LOGFILE for details" 1>&2
	    echo "$0: ERROR: about to exit: $EXIT_CODE" 1>&2
	fi
	echo "$0: ERROR: about to exit: $EXIT_CODE" >> "${LOGFILE}"
    fi
fi

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
exit "$EXIT_CODE"
