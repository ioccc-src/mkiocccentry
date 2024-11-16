#!/usr/bin/env bash
#
# chkentry_test.sh - chkentry test on good and bad files under a directory
#
# Use chkentry to test all the files under test_JSON{info,auth}.json/good/ and
# verify there are no JSON semantic errors.  If any JSON semantic errors are
# detected, this script will exit non-zero.
#
# Use chkentry to test all the files under test_JSON{info,auth}.json/bad/ and
# verify there are JSON semantic errors.  If any file is found to be free of
# JSON semantic errors, this script will exit non-zero.
#
# IMPORTANT NOTE: All JSON files (under both good and bad) MUST to be valid JSON.
#		  This script focuses testing JSON semantic errors only.
#
# This script was written in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# with some minor improvements by:
#
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
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
export JSON_TREE="./test_ioccc/test_JSON"

export CHKENTRY_TEST_VERSION="1.0.1 2024-03-02"

export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-J level] [-q] [-c chkentry] [-d json_tree]

    -h			print help and exit
    -V			print version and exit
    -v level		set verbosity level for this script: (def level: 0)
    -D dbg_level	set verbosity level for tests (def: level: 0)
    -J level		set JSON parser verbosity level (def level: 0)
    -q			quiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )
    -c chkentry		path to chkentry tool (def: $CHKENTRY)
    -d json_tree	test files json_tree/{info,auth}.json/{good,bad}/*.json (def: $JSON_TREE)

			NOTE: these subdirectories are required under json_tree:

			json_tree/info.json/good/	valid JSON syntax that is a proper .info.json semantic file
			json_tree/info.json/bad/	valid JSON syntax that has 1 or more .info.json semantic errors

			json_tree/auth.json/good/	valid JSON syntax that is a proper .auth.json semantic file
			json_tree/auth.json/bad/	valid JSON syntax that has 1 or more .auth.json semantic errors

Exit codes:
     0   all OK
     1   all JSON files were valid JSON; however some chkentry semantic tests failed
     2   -h and help string printed or -V and version string printed
     3   invalid command line
     4	 missing or non-executable chkentry
     5	 missing or non-readable json_tree directory or subdirectory
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
    d)	JSON_TREE="$OPTARG"
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
export INFO_GOOD_TREE="$JSON_TREE/info.json/good"
export INFO_BAD_TREE="$JSON_TREE/info.json/bad"
export AUTH_GOOD_TREE="$JSON_TREE/auth.json/good"
export AUTH_BAD_TREE="$JSON_TREE/auth.json/bad"

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

# check that json_tree is a readable directory
#
if [[ ! -e $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not found: $JSON_TREE" 1>&2
    exit 5
fi
if [[ ! -d $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not a directory: $JSON_TREE" 1>&2
    exit 5
fi
if [[ ! -r $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not readable directory: $JSON_TREE" 1>&2
    exit 5
fi

# good tree for auth.json
#
if [[ ! -e $AUTH_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/good for chkentry directory not found: $AUTH_GOOD_TREE" 1>&2
    exit 5
fi
if [[ ! -d $AUTH_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/good for chkentry not a directory: $AUTH_GOOD_TREE" 1>&2
    exit 5
fi
if [[ ! -r $AUTH_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/good for chkentry not readable directory: $AUTH_GOOD_TREE" 1>&2
    exit 5
fi

# bad tree for auth.json
#
if [[ ! -e $AUTH_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/bad for chkentry directory not found: $AUTH_BAD_TREE" 1>&2
    exit 5
fi
if [[ ! -d $AUTH_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/bad for chkentry not a directory: $AUTH_BAD_TREE" 1>&2
    exit 5
fi
if [[ ! -r $AUTH_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/bad for chkentry not readable directory: $AUTH_BAD_TREE" 1>&2
    exit 5
fi

# good tree for info.json
#
if [[ ! -e $INFO_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/good for chkentry directory not found: $INFO_GOOD_TREE" 1>&2
    exit 5
fi
if [[ ! -d $INFO_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/good for chkentry not a directory: $INFO_GOOD_TREE" 1>&2
    exit 5
fi
if [[ ! -r $INFO_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/good for chkentry not readable directory: $INFO_GOOD_TREE" 1>&2
    exit 5
fi

# bad tree for info.json
#
if [[ ! -e $INFO_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/bad for chkentry directory not found: $INFO_BAD_TREE" 1>&2
    exit 5
fi
if [[ ! -d $INFO_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/bad for chkentry not a directory: $INFO_BAD_TREE" 1>&2
    exit 5
fi
if [[ ! -r $INFO_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/bad for chkentry not readable directory: $INFO_BAD_TREE" 1>&2
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

# run_auth_test - run a single chkentry test on a file
#
# usage:
#	run_auth_test chkentry dbg_level json_dbg_level quiet_mode auth_json pass|fail
#
#	chkentry		path to the chkentry program
#	dbg_level		internal test debugging level to use as in: chkentry -v dbg_level
#	json_dbg_level		JSON parser debug level to use in: chkentry -J json_dbg_level
#	quiet_mode		quiet mode to use in: chkentry -q
#	auth_json		auth.json file to check with chkentry
#	pass|fail		string saying if chkentry must return valid json or invalid json
#
run_auth_test()
{
    # parse args
    #
    if [[ $# -ne 6 ]]; then
	echo "$0: ERROR: expected 6 args to run_auth_test, found $#" 1>&2
	exit 12
    fi
    declare chkentry="$1"
    declare dbg_level="$2"
    declare json_dbg_level="$3"
    declare quiet_mode="$4"
    declare auth_json="$5"
    declare pass_fail="$6"

    if [[ "$pass_fail" != "pass" && "$pass_fail" != "fail" ]]; then
	echo "$0: ERROR: in run_auth_test: pass_fail neither 'pass' nor 'fail'" 1>&2
	exit 13
    fi

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_auth_test: chkentry: $chkentry" 1>&2
	echo "$0: debug[9]: in run_auth_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_auth_test: json_dbg_level: $json_dbg_level" 1>&2
	echo "$0: debug[9]: in run_auth_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_auth_test: auth_json: $auth_json" 1>&2
	echo "$0: debug[9]: in run_auth_test: pass_fail: $pass_fail" 1>&2
    fi

    # perform the test
    #
    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -- . $auth_json >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -- . $auth_json >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -- . "$auth_json" >> "${LOGFILE}" 2>&1
	status="$?"
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -q -- . $auth_json >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -q -- . $auth_json >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -q -- . "$auth_json" >> "${LOGFILE}" 2>&1
	status="$?"
    fi

    # examine test result
    #
    if [[ $status -eq 4 ]]; then
	echo "$0: ERROR: file is not a valid JSON file for chkentry to test: $auth_json" >> "${LOGFILE}"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: ERROR: file is not a valid JSON file for chkentry to test: $auth_json" 1>&2
	fi
	INVALID_JSON_FOUND="true"
    elif [[ $status -eq 0 ]]; then
	if [[ $pass_fail = pass ]]; then
	    echo "$0: test $auth_json should PASS: chkentry PASS with exit code 0" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]: test $auth_json should PASS: chkentry passed with exit code 0" 1>&2
	    fi
	else
	    echo "$0: ERROR: test $auth_json should FAIL: chkentry incorrectly exited code 0" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: ERROR: test $auth_json should FAIL: chkentry incorrectly exited code 0" 1>&2
	    fi
	    SEMANTIC_ERROR_MISSED="true"
	fi
    else
	if [[ $pass_fail = pass ]]; then
	    echo "$0: ERROR: test $auth_json should PASS: chkentry FAIL with exit code: $status" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 1 ]]; then
		if [[ $V_FLAG -ge 1 ]]; then
		    echo "$0: debug[1]: ERROR: test $auth_json should pass: chkentry FAIL with exit code: $status" 1>&2
		fi
	    fi
	    UNEXPECTED_SEMANTIC_ERROR="true"
	else
	    echo "$0: test $auth_json should FAIL: chkentry FAIL correctly with exit code: $status" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 1 ]]; then
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: debug[3]: test $auth_json should FAIL: chkentry FAIL correctly with exit code: $status" 1>&2
		fi
	    fi
	fi
    fi
    echo >> "${LOGFILE}"

    # return
    #
    return
}

# run_info_test - run a single chkentry test on a file
#
# usage:
#	run_info_test chkentry dbg_level json_dbg_level quiet_mode info_json pass|fail
#
#	chkentry		path to the chkentry program
#	dbg_level		internal test debugging level to use as in: chkentry -v dbg_level
#	json_dbg_level		JSON parser debug level to use in: chkentry -J json_dbg_level
#	quiet_mode		quiet mode to use in: chkentry -q
#	info_json		info.json file to check with chkentry
#	pass|fail		string saying if chkentry must return valid json or invalid json
#
run_info_test()
{
    # parse args
    #
    if [[ $# -ne 6 ]]; then
	echo "$0: ERROR: expected 6 args to run_info_test, found $#" 1>&2
	exit 13
    fi
    declare chkentry="$1"
    declare dbg_level="$2"
    declare json_dbg_level="$3"
    declare quiet_mode="$4"
    declare info_json="$5"
    declare pass_fail="$6"

    if [[ "$pass_fail" != "pass" && "$pass_fail" != "fail" ]]; then
	echo "$0: ERROR: in run_info_test: pass_fail neither 'pass' nor 'fail'" 1>&2
	EXIT_CODE=14
	return
    fi

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_info_test: chkentry: $chkentry" 1>&2
	echo "$0: debug[9]: in run_info_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_info_test: json_dbg_level: $json_dbg_level" 1>&2
	echo "$0: debug[9]: in run_info_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_info_test: info_json: $info_json" 1>&2
	echo "$0: debug[9]: in run_info_test: pass_fail: $pass_fail" 1>&2
    fi

    # perform the test
    #
    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -- $info_json . >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -- $info_json . >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -- "$info_json" . >> "${LOGFILE}" 2>&1
	status="$?"
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -q -- $info_json . >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -q -- $info_json . >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -q -- "$info_json" . >> "${LOGFILE}" 2>&1
	status="$?"
    fi

    # examine test result
    #
    if [[ $status -eq 4 ]]; then
	echo "$0: ERROR: file is not a valid JSON file for chkentry to test: $info_json" >> "${LOGFILE}"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: ERROR: file is not a valid JSON file for chkentry to test: $info_json" 1>&2
	fi
	INVALID_JSON_FOUND="true"
    elif [[ $status -eq 0 ]]; then
	if [[ $pass_fail = pass ]]; then
	    echo "$0: test $info_json should PASS: chkentry PASS with exit code 0" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]: test $info_json should PASS: chkentry passed with exit code 0" 1>&2
	    fi
	else
	    echo "$0: ERROR: test $info_json should FAIL: chkentry incorrectly exited code 0" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: ERROR: test $info_json should FAIL: chkentry incorrectly exited code 0" 1>&2
	    fi
	    SEMANTIC_ERROR_MISSED="true"
	fi
    else
	if [[ $pass_fail = pass ]]; then
	    echo "$0: ERROR: test $info_json should PASS: chkentry FAIL with exit code: $status" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 1 ]]; then
		if [[ $V_FLAG -ge 1 ]]; then
		    echo "$0: debug[1]: ERROR: test $info_json should pass: chkentry FAIL with exit code: $status" 1>&2
		fi
	    fi
	    UNEXPECTED_SEMANTIC_ERROR="true"
	else
	    echo "$0: test $info_json should FAIL: chkentry FAIL correctly with exit code: $status" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 1 ]]; then
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: debug[3]: test $info_json should FAIL: chkentry FAIL correctly with exit code: $status" 1>&2
		fi
	    fi
	fi
    fi
    echo >> "${LOGFILE}"

    # return
    #
    return
}

# run tests that must pass: auth.json
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run chkentry tests that must pass: auth.json files" 1>&2
fi
while read -r file; do
    run_auth_test "$CHKENTRY" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$file" pass
done < <(find "$AUTH_GOOD_TREE" -type f -name '*.json' -print)


# run tests that must fail: auth.json
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run chkentry tests that must fail: auth.json files" 1>&2
fi
while read -r file; do
    run_auth_test "$CHKENTRY" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$file" fail
done < <(find "$AUTH_BAD_TREE" -type f -name '*.json' -print)


# run tests that must pass: info.json
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run chkentry tests that must pass: info.json files" 1>&2
fi
while read -r file; do
    run_info_test "$CHKENTRY" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$file" pass
done < <(find "$INFO_GOOD_TREE" -type f -name '*.json' -print)

# run tests that must fail: info.json
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run chkentry tests that must fail: info.json files" 1>&2
fi
while read -r file; do
    run_info_test "$CHKENTRY" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$file" fail
done < <(find "$INFO_BAD_TREE" -type f -name '*.json' -print)

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
