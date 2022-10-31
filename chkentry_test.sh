#!/usr/bin/env bash
#
# chkentry_test.sh - test chkentry on all the files in
# test_JSON{info,auth}.json/{good,bad}

# setup
#
export CHKENTRY_TEST_VERSION="0.0 2022-10-31"
export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-J level] [-q] [-c chkentry] [-d json_tree]

    -h			print help and exit
    -V			print version and exit
    -v level		set verbosity level for this script: (def level: 0)
    -D dbg_level	set verbosity level for tests (def: level: 0)
    -J level		set JSON parser verbosity level (def level: 0)
    -q			quiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)
    -j /path/to/chkentry	path to chkentry tool (def: ./chkentry)
    -d json_tree	read files from good and bad subdirectories of this directory
			    These subdirectories are expected:
				tree/info.json/bad
				tree/info.json/good
				tree/auth.json/bad
				tree/auth.json/good

Exit codes:
     0   all is well
     1   at least one test failed
     2   help mode exit
     3   invalid command line
  >= 4   internal error

$0 version: $CHKENTRY_TEST_VERSION"

export V_FLAG="0"
export DBG_LEVEL="0"
export JSON_DBG_LEVEL="0"
export Q_FLAG=""
export CHKENTRY="./chkentry"
export LOGFILE="./chkentry_test.log"
export EXIT_CODE=0
export JSON_TREE="./test_JSON"

# parse args
#
while getopts :hVv:D:J:qj:d: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$CHKENTRY_TEST_VERSION" 1>&2
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
    j)	CHKENTRY="$OPTARG";
	;;
    d)	JSON_TREE="$OPTARG"
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

eval make all 2>&1 | grep -v 'Nothing to be done for'

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
    exit 10
fi
if [[ ! -d $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not a directory: $JSON_TREE" 1>&2
    exit 11
fi
if [[ ! -r $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not readable directory: $JSON_TREE" 1>&2
    exit 12
fi

# good tree for auth.json
#
if [[ ! -e $AUTH_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/good for chkentry directory not found: $AUTH_GOOD_TREE" 1>&2
    exit 13
fi
if [[ ! -d $AUTH_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/good for chkentry not a directory: $AUTH_GOOD_TREE" 1>&2
    exit 14
fi
if [[ ! -r $AUTH_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/good for chkentry not readable directory: $AUTH_GOOD_TREE" 1>&2
    exit 15
fi

# bad tree for auth.json
#
if [[ ! -e $AUTH_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/bad for chkentry directory not found: $AUTH_BAD_TREE" 1>&2
    exit 16
fi
if [[ ! -d $AUTH_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/bad for chkentry not a directory: $AUTH_BAD_TREE" 1>&2
    exit 17
fi
if [[ ! -r $AUTH_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/auth.json/bad for chkentry not readable directory: $AUTH_BAD_TREE" 1>&2
    exit 18
fi


# good tree for info.json
#
if [[ ! -e $INFO_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/good for chkentry directory not found: $INFO_GOOD_TREE" 1>&2
    exit 19
fi
if [[ ! -d $INFO_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/good for chkentry not a directory: $INFO_GOOD_TREE" 1>&2
    exit 20
fi
if [[ ! -r $INFO_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/good for chkentry not readable directory: $INFO_GOOD_TREE" 1>&2
    exit 21
fi

# bad tree for info.json
#
if [[ ! -e $INFO_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/bad for chkentry directory not found: $INFO_BAD_TREE" 1>&2
    exit 22
fi
if [[ ! -d $INFO_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/bad for chkentry not a directory: $INFO_BAD_TREE" 1>&2
    exit 23
fi
if [[ ! -r $INFO_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/info.json/bad for chkentry not readable directory: $INFO_BAD_TREE" 1>&2
    exit 24
fi


# remove logfile so that each run starts out with an empty file
#
rm -f "$LOGFILE"
touch "$LOGFILE"
if [[ ! -f "${LOGFILE}" ]]; then
    echo "$0: ERROR: couldn't create log file" 1>&2
    exit 5
fi
if [[ ! -w "${LOGFILE}" ]]; then
    echo "$0: ERROR: log file not writable" 1>&2
    exit 5
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
	exit 4
    fi
    declare chkentry="$1"
    declare dbg_level="$2"
    declare json_dbg_level="$3"
    declare quiet_mode="$4"
    declare auth_json="$5"
    declare pass_fail="$6"

    if [[ "$pass_fail" != "pass" && "$pass_fail" != "fail" ]]; then
	echo "$0: ERROR: in run_auth_test: pass_fail neither 'pass' nor 'fail'" 1>&2
	EXIT_CODE=2
	return
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

    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -- . $auth_json >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -- . $auth_json >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -- . "$auth_json" >> "${LOGFILE}" 2>&1
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -q -- . $auth_json >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -q -- . $auth_json >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -q -- . "$auth_json" >> "${LOGFILE}" 2>&1
    fi
    status="$?"

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
	if [[ $pass_fail = pass ]]; then
	    echo "$0: in test that must pass: chkentry OK, exit code 0" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]: chkentry OK, exit code 0" 1>&2
	    fi
	else
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must fail: chkentry FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_auth_test: chkentry exit code: $status" 1>&2
		fi
	    fi
	    EXIT_CODE=1
	fi
    else
	if [[ $pass_fail = pass ]]; then
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must pass: chkentry FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_auth_test: chkentry exit code: $status" 1>&2
		fi
	    fi
	    EXIT_CODE=1
	else
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must fail: chkentry OK, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_auth_test: chkentry exit code: $status" 1>&2
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
	exit 4
    fi
    declare chkentry="$1"
    declare dbg_level="$2"
    declare json_dbg_level="$3"
    declare quiet_mode="$4"
    declare info_json="$5"
    declare pass_fail="$6"

    if [[ "$pass_fail" != "pass" && "$pass_fail" != "fail" ]]; then
	echo "$0: ERROR: in run_info_test: pass_fail neither 'pass' nor 'fail'" 1>&2
	EXIT_CODE=2
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

    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -- $info_json . >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -- $info_json . >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -- "$info_json" . >> "${LOGFILE}" 2>&1
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -q -- $info_json . >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run test that must $pass_fail: $chkentry -v $dbg_level -J $json_dbg_level -q -- $info_json . >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -q -- "$info_json" . >> "${LOGFILE}" 2>&1
    fi
    status="$?"

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
	if [[ $pass_fail = pass ]]; then
	    echo "$0: in test that must pass: chkentry OK, exit code 0" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]: chkentry OK, exit code 0" 1>&2
	    fi
	else
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must fail: chkentry FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_info_test: chkentry exit code: $status" 1>&2
		fi
	    fi
	    EXIT_CODE=1
	fi
    else
	if [[ $pass_fail = pass ]]; then
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must pass: chkentry FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_info_test: chkentry exit code: $status" 1>&2
		fi
	    fi
	    EXIT_CODE=1
	else
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must fail: chkentry OK, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_info_test: chkentry exit code: $status" 1>&2
		fi
	    fi
	fi
    fi
    echo >> "${LOGFILE}"

    # return
    #
    return
}


# run_string_test - run a single chkentry test on a string
#
# usage:
#	run_string_test chkentry dbg_level json_dbg_level quiet_mode json_doc_string
#
#	chkentry			path to the chkentry program
#	dbg_level		internal test debugging level to use as in: chkentry -v dbg_level
#	json_dbg_level		JSON parser debug level to use in: chkentry -J json_dbg_level
#	quiet_mode		quiet mode to use in: chkentry -q
#	json_doc_string		JSON document as a string to give to chkentry
#
run_string_test()
{
    # parse args
    #
    if [[ $# -ne 5 ]]; then
	echo "$0: ERROR: expected 5 args to run_string_test, found $#" 1>&2
	exit 4
    fi
    declare chkentry="$1"
    declare dbg_level="$2"
    declare json_dbg_level="$3"
    declare quiet_mode="$4"
    declare json_doc_string="$5"

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_string_test: chkentry: $chkentry" 1>&2
	echo "$0: debug[9]: in run_string_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_string_test: json_dbg_level: $json_dbg_level" 1>&2
	echo "$0: debug[9]: in run_string_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_string_test: json_doc_string: $json_doc_string" 1>&2
    fi

    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run: $chkentry -v $dbg_level -J $json_dbg_level -s -- $json_doc_string >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run: $chkentry -v $dbg_level -J $json_dbg_level -s -- $json_doc_string >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -s -- "$json_doc_string" >> "${LOGFILE}" 2>&1
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run: $chkentry -v $dbg_level -J $json_dbg_level -q -s -- $json_doc_string >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run: $chkentry -v $dbg_level -J $json_dbg_level -q -s -- $json_doc_string >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$chkentry" -v "$dbg_level" -J "$json_dbg_level" -q -s -- "$json_doc_string" >> "${LOGFILE}" 2>&1
    fi
    status="$?"

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
	echo "$0: chkentry OK, exit code 0" 1>&2 >> "${LOGFILE}"
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: chkentry OK, exit code 0" 1>&2
	fi
    else
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: chkentry FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]: in run_string_test: chkentry exit code: $status" 1>&2
	    fi
	fi
	EXIT_CODE=1
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


# All Done!!! -- Jessica Noll, Age 2
#
if [[ $V_FLAG -ge 1 ]]; then
    if [[ $EXIT_CODE -eq 0 ]]; then
	echo "$0: debug[1]: all tests PASSED" 1>&2
	echo "$0: debug[1]: all tests PASSED" >> "${LOGFILE}"
	echo 1>&2
	echo "$0: see $LOGFILE for details" 1>&2
    else
	echo "$0: ERROR: debug[1]: some tests FAILED" 1>&2
	echo "$0: ERROR: debug[1]: some tests FAILED" >> "${LOGFILE}"
	echo 1>&2
	echo "$0: Notice: see $LOGFILE for details" 1>&2
    fi
fi
exit "$EXIT_CODE"
