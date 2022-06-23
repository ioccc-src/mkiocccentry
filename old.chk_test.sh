#!/usr/bin/env bash
#
# chk_test.sh - test chkauth and chkinfo on JSON test files

# setup
#
export USAGE="usage: $0 [-h] [-v level] [-D dbg_level] [-j chkinfo] [-J chkauth]
		      [-C chkcode.sh] [-t info_only | -t auth_only] [-d json_tree]

    -h				print help and exit 2
    -v level			set verbosity level for this script: (def level: 0)
    -D dbg_level		set verbosity level for tests (def: level: 0)
    -j /path/to/chkinfo	path to chkinfo executable (def: ./chkinfo)
    -J /path/to/chkauth	path to chkauth executable (def: ./chkauth)
    -C /path/to/chkcode.sh	path to the chkcode.sh executable (def: ./chkcode.sh)
    -t info_only		run only chkinfo tests (def: run both)
    -t auth_only		run only chkauth tests (def: run both)
    -d json_tree		tree where JSON test files are to be found (def: ./test_JSON)
				  These sub-directories are expected:
				    json_tree/author.json/bad
				    json_tree/author.json/good
				    json_tree/info.json/bad
				    json_tree/info.json/good

exit codes:
    0 - all is well
    1 - at least one test failed
    2 - help mode exit
    3 - invalid command line
    >= 4 - internal error"
export CHKINFO="./chkinfo"
export CHKAUTH="./chkauth"
export CHKCODE="./chkcode.sh"
export RUN_CHKINFO="true"
export RUN_CHKAUTH="true"
export EXIT_CODE=0
export JSON_TREE="./test_JSON"
export LOGFILE="./chk_test.log"

# parse args
#
export V_FLAG="0"
export DBG_LEVEL="0"
while getopts :hv:D:j:J:C:t:d: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    D) DBG_LEVEL="$OPTARG";
       ;;
    j) CHKINFO="$OPTARG";
       ;;
    J) CHKAUTH="$OPTARG";
       ;;
    C) CHKCODE="$OPTARG";
       ;;
    t) if [[ $OPTARG == info_only ]]; then
	    RUN_CHKAUTH=
       elif [[ $OPTARG == auth_only ]]; then
       	    RUN_CHKINFO=
       else
	    echo "$0: ERROR: unknown -t flag" 1>&2
	    exit 3
       fi
       ;;
    d) JSON_TREE="$OPTARG"
        ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
       exit 3
       ;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
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
    echo "$0: ERROR: expected 1 argument, found $#" 1>&2
    exit 3
fi
if [[ $# -eq 1 ]]; then
    JSON_TREE="$1"
fi
export JSON_INFO_TREE="$JSON_TREE/info.json"
export JSON_AUTH_TREE="$JSON_TREE/author.json"

# form the temporary exit code file
#
export EXIT_CODE_FILE MKTEMP_TEMPLATE
MKTEMP_TEMPLATE=".exit_code.$(basename "$0").XXXXXXXXXX"
EXIT_CODE_FILE=$(mktemp "$MKTEMP_TEMPLATE")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp $MKTEMP_TEMPLATE exit code: $status" 1>&2
    exit 55
fi
if [[ ! -e $EXIT_CODE_FILE ]]; then
    echo "$0: ERROR: EXIT_CODE_FILE does not exist: $EXIT_CODE_FILE" 1>&2
    exit 55
fi
if [[ ! -f $EXIT_CODE_FILE ]]; then
    echo "$0: ERROR: EXIT_CODE_FILE not a regular file: $EXIT_CODE_FILE" 1>&2
    exit 55
fi
if [[ ! -r $EXIT_CODE_FILE ]]; then
    echo "$0: ERROR: EXIT_CODE_FILE file not readable: $EXIT_CODE_FILE" 1>&2
    exit 55
fi
if [[ ! -w $EXIT_CODE_FILE ]]; then
    echo "$0: ERROR: EXIT_CODE_FILE file not writable: $EXIT_CODE_FILE" 1>&2
    exit 55
fi

# check that json_tree is a readable directory
#
if [[ ! -e $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not found: $JSON_TREE" 1>&2
    exit 4
fi
if [[ ! -d $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not a directory: $JSON_TREE" 1>&2
    exit 4
fi
if [[ ! -r $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not readable directory: $JSON_TREE" 1>&2
    exit 4
fi

# check that we can run at least some tests
#
if [[ -z $RUN_CHKINFO && -z $RUN_CHKAUTH ]]; then
    echo "$0: ERROR: cannot specify both -t info_only and -t auth_only" 1>&2
    exit 3
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

# chkinfo test checks
#
if [[ -n $RUN_CHKINFO ]]; then
    if [[ ! -e $CHKINFO ]]; then
	echo "$0: ERROR: chkinfo not found: $CHKINFO" 1>&2
	exit 4
    fi
    if [[ ! -f $CHKINFO ]]; then
	echo "$0: ERROR: chkinfo not a regular file: $CHKINFO" 1>&2
	exit 4
    fi
    if [[ ! -x $CHKINFO ]]; then
	echo "$0: ERROR: chkinfo not executable: $CHKINFO" 1>&2
	exit 4
    fi
    if [[ ! -e $JSON_INFO_TREE ]]; then
	echo "$0: ERROR: json_tree for chkinfo directory not found: $JSON_INFO_TREE" 1>&2
	exit 4
    fi
    if [[ ! -d $JSON_INFO_TREE ]]; then
	echo "$0: ERROR: json_tree for chkinfo not a directory: $JSON_INFO_TREE" 1>&2
	exit 4
    fi
    if [[ ! -r $JSON_INFO_TREE ]]; then
	echo "$0: ERROR: json_tree for chkinfo not readable directory: $JSON_INFO_TREE" 1>&2
	exit 4
    fi
    for subdir in bad good; do
	if [[ ! -e $JSON_INFO_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for chkinfo subdir not found: $JSON_INFO_TREE/$subdir" 1>&2
	    exit 4
	fi
	if [[ ! -d $JSON_INFO_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for chkinfo subdir not a directory: $JSON_INFO_TREE/$subdir" 1>&2
	    exit 4
	fi
	if [[ ! -r $JSON_INFO_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for chkinfo subdir not readable directory: $JSON_INFO_TREE/$subdir" 1>&2
	    exit 4
	fi
    done
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: will run chkinfo from: $CHKINFO" 1>&2
    fi
fi

# chkauth test checks
#
if [[ -n $RUN_CHKAUTH ]]; then
    if [[ ! -e $CHKAUTH ]]; then
	echo "$0: ERROR: chkauth not found: $CHKAUTH" 1>&2
	exit 4
    fi
    if [[ ! -f $CHKAUTH ]]; then
	echo "$0: ERROR: chkauth not a regular file: $CHKAUTH" 1>&2
	exit 4
    fi
    if [[ ! -x $CHKAUTH ]]; then
	echo "$0: ERROR: chkauth not executable: $CHKAUTH" 1>&2
	exit 4
    fi
    if [[ ! -e $JSON_AUTH_TREE ]]; then
	echo "$0: ERROR: json_tree for chkauth directory not found: $JSON_AUTH_TREE" 1>&2
	exit 4
    fi
    if [[ ! -d $JSON_AUTH_TREE ]]; then
	echo "$0: ERROR: json_tree for chkauth not a directory: $JSON_AUTH_TREE" 1>&2
	exit 4
    fi
    if [[ ! -r $JSON_AUTH_TREE ]]; then
	echo "$0: ERROR: json_tree for chkauth not readable directory: $JSON_AUTH_TREE" 1>&2
	exit 4
    fi
    for subdir in bad good; do
	if [[ ! -e $JSON_AUTH_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for chkauth subdir not found: $JSON_AUTH_TREE/$subdir" 1>&2
	    exit 4
	fi
	if [[ ! -d $JSON_AUTH_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for chkauth subdir not a directory: $JSON_AUTH_TREE/$subdir" 1>&2
	    exit 4
	fi
	if [[ ! -r $JSON_AUTH_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for chkauth subdir not readable directory: $JSON_AUTH_TREE/$subdir" 1>&2
	    exit 4
	fi
    done
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: will run chkauth from: $CHKAUTH" 1>&2
    fi
fi

# check that the chkcode.sh tool is available
#
if [[ ! -e $CHKCODE ]]; then
    echo "$0: ERROR: chkcode.sh not found: $CHKCODE" 1>&2
    exit 4
fi
if [[ ! -f $CHKCODE ]]; then
    echo "$0: ERROR: chkcode.sh not a regular file: $CHKCODE" 1>&2
    exit 4
fi
if [[ ! -x $CHKCODE ]]; then
    echo "$0: ERROR: chkcode.sh not executable: $CHKCODE" 1>&2
    exit 4
fi

# run_test - run a single test
#
# usage:
#	run_test test_prog debug_level {fail|pass} chk_test_file
#
#	run_test	- our function name
#	test_prog	- path to a test program
#	debug_level	- the -v debug level to pass to test_prog
#	fail		- test must fail - error if passes
#	pass		- test must pass - error if fails
#	chk_test_file	- the JSON file to give to test_prog
#
run_test()
{
    # parse args
    #
    if [[ $# -ne 4 ]]; then
	echo "$0: ERROR: expected 5 args to run_test, found $#" 1>&2
	exit 4
    fi
    typeset test_prog="$1"
    typeset debug_level="$2"
    typeset pass_fail="$3"
    typeset chk_test_file="$4"
    typeset json_code="$chk_test_file.code"
    if [[ ! -e $test_prog ]]; then
	echo "$0: in run_test: test_prog not found: $test_prog"
	exit 4
    fi
    if [[ ! -f $test_prog ]]; then
	echo "$0: in run_test: test_prog not a regular file: $test_prog"
	exit 4
    fi
    if [[ ! -x $test_prog ]]; then
	echo "$0: in run_test: test_prog not executable: $test_prog"
	exit 4
    fi
    if [[ $pass_fail != pass && $pass_fail != fail ]]; then
	echo "$0: in run_test: pass_fail neither 'pass' nor 'fail': $pass_fail" 1>&2
	exit 4
    fi
    if [[ ! -e $test_prog ]]; then
	echo "$0: in run_test: chk_test_file not found: $chk_test_file"
	exit 4
    fi
    if [[ ! -f $test_prog ]]; then
	echo "$0: in run_test: chk_test_file not a regular file: $chk_test_file"
	exit 4
    fi
    if [[ ! -r $test_prog ]]; then
	echo "$0: in run_test: chk_test_file not readable: $chk_test_file"
	exit 4
    fi

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_test: test_prog: $test_prog" 1>&2
	echo "$0: debug[9]: in run_test: debug_level: $debug_level" 1>&2
	echo "$0: debug[9]: in run_test: pass or fail: $pass_fail" 1>&2
	echo "$0: debug[9]: in run_test: chk_test_file: $chk_test_file" 1>&2
    fi

    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: in run_test: about to run: $test_prog -v $debug_level -- $chk_test_file" 1>&2
    fi
    if [[ $debug_level -gt 0 ]]; then
	"$test_prog" -t -v "$debug_level" -- "$chk_test_file"
	status="$?"
    else
	"$test_prog" -t -v 0 -q -- "$chk_test_file"
	status="$?"
    fi
    if [[ $V_FLAG -ge 7 ]]; then
	echo "$0: debug[7]: in run_test: test_prog exit code: $status" 1>&2
    fi

    # examine test result
    #
    if [[ $pass_fail == pass ]]; then
	if [[ $status -ne 0 ]]; then
	    echo "$0: Warning: in run_test: FAIL: $test_prog -v $debug_level $chk_test_file exit code: $status != 0" | tee -a "$LOGFILE" 1>&2
	    echo "$0: Warning: the above mentioned test FAILED when it should have PASSED" | tee -a "$LOGFILE" 1>&2
	    if [[ $V_FLAG -lt 3 ]]; then
		echo "$0: Warning: for more details try: $test_prog -v 3 -- $chk_test_file" | tee -a "$LOGFILE" 1>&2
	    else
		echo "$0: Warning: for more details try: $test_prog -v $V_FLAG -- $chk_test_file" | tee -a "$LOGFILE" 1>&2
	    fi
	    echo | tee -a "${LOGFILE}" 1>&2
	    EXIT_CODE=1
	    echo "$EXIT_CODE" > "$EXIT_CODE_FILE"
	    status="$?"
	    if [[ $status -ne 0 || ! -s $EXIT_CODE_FILE ]]; then
		echo "$0: FATAL: failed to write $EXIT_CODE into EXIT_CODE_FILE: $EXIT_CODE_FILE" 1>&2
		exit 55
	    fi
	elif [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: in run_test: PASS: $test_prog -v $debug_level $chk_test_file" 1>&2
	fi
    elif [[ $pass_fail == fail ]]; then
	if [[ $status -eq 0 ]]; then
	    echo "$0: Warning: in run_test: FAIL: $test_prog -v $debug_level $chk_test_file exit code: $status == 0" | tee -a "$LOGFILE" 1>&2
	    echo "$0: Warning: the above mentioned test PASSED when it should have FAILED" | tee -a "$LOGFILE" 1>&2
	    if [[ $V_FLAG -lt 3 ]]; then
		echo "$0: Warning: for more details try: $test_prog -v 3 -- $chk_test_file" | tee -a "$LOGFILE" 1>&2
	    else
		echo "$0: Warning: for more details try: $test_prog -v $V_FLAG -- $chk_test_file" | tee -a "$LOGFILE" 1>&2
	    fi
	    echo | tee -a "${LOGFILE}" 1>&2
	    EXIT_CODE=1
	    echo "$EXIT_CODE" > "$EXIT_CODE_FILE"
	    status="$?"
	    if [[ $status -ne 0 || ! -s $EXIT_CODE_FILE ]]; then
		echo "$0: FATAL: failed to write $EXIT_CODE into EXIT_CODE_FILE: $EXIT_CODE_FILE" 1>&2
		exit 55
	    fi
	elif [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: in run_test: PASS: $test_prog -v $debug_level $chk_test_file" 1>&2
	fi
    fi

    # If an expected code file exists, use chkcode.sh to check the codes
    #
    # We run chkcode.sh at a -v 3 minimum so the log file might capture code difference information
    #
    if [[ -f $json_code ]]; then
	if [[ $V_FLAG -lt 3 ]]; then
	    "$CHKCODE" -v 3 -D "$debug_level" -- "$test_prog" "$chk_test_file" 2>&1 | tee -a "$LOGFILE" 1>&2
	    status="$?"
	else
	    if [[ $V_FLAG -ge 5 ]]; then
		echo "$0: debug[5]: about run: $CHKCODE -v $V_FLAG 3 -D $debug_level -- $test_prog $chk_test_file" 1>&2
	    fi
	    "$CHKCODE" -v "$V_FLAG" -D "$debug_level" -- "$test_prog" "$chk_test_file" 2>&1 | tee -a "$LOGFILE" 1>&2
	    status="$?"
	fi
	case "$status" in
	0) if [[ $V_FLAG -ge 5 ]]; then
		echo "$0: debug[5]: PASS: chkcode.sh matched JSON codes" | tee -a "$LOGFILE" 1>&2
	   fi
	   ;;
	1) echo "$0: Warning: FAIL: JSON codes for $chk_test_file do not match expected code list in $json_code" | tee -a "$LOGFILE" 1>&2
	   EXIT_CODE=1
	   echo "$EXIT_CODE" > "$EXIT_CODE_FILE"
	   status="$?"
	   if [[ $status -ne 0 || ! -s $EXIT_CODE_FILE ]]; then
		echo "$0: FATAL: failed to write $EXIT_CODE into EXIT_CODE_FILE: $EXIT_CODE_FILE" 1>&2
		exit 55
	   fi
	   ;;
	*) echo "$0: ERROR: FAIL: chkcode.sh exit status: $status > 1" | tee -a "$LOGFILE" 1>&2
	   EXIT_CODE=1
	   echo "$EXIT_CODE" > "$EXIT_CODE_FILE"
	   status="$?"
	   if [[ $status -ne 0 || ! -s $EXIT_CODE_FILE ]]; then
		echo "$0: FATAL: failed to write $EXIT_CODE into EXIT_CODE_FILE: $EXIT_CODE_FILE" 1>&2
		exit 55
	   fi
	   ;;
	esac
    elif [[ $V_FLAG -ge 7 ]]; then
	echo "$0: debug[7]: skipping chkcode.sh, code list not found: $json_code" 1>&2
    fi

    # return
    #
    return
}

# run chkinfo tests
#
if [[ -n $RUN_CHKINFO ]]; then

    # run tests that must pass
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run chkinfo tests that must pass" 1>&2
    fi
    find "$JSON_INFO_TREE/good" -type f -print | while read -r file; do
        run_test "$CHKINFO" "$DBG_LEVEL" pass "$file"
    done

    # run tests that must fail
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run chkinfo tests that must fail" 1>&2
    fi
    find "$JSON_INFO_TREE/bad" -type f -print | while read -r file; do
        run_test "$CHKINFO" "$DBG_LEVEL" fail "$file"
    done
fi

# run chkauth tests
#
if [[ -n $RUN_CHKAUTH ]]; then

    # run tests that must pass
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run chkauth tests that must pass" 1>&2
    fi
    find "$JSON_AUTH_TREE/good" -type f -print | while read -r file; do
        run_test "$CHKAUTH" "$DBG_LEVEL" pass "$file"
    done

    # run tests that must fail
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run chkauth tests that must fail" 1>&2
    fi
    find "$JSON_AUTH_TREE/bad" -type f -print | while read -r file; do
        run_test "$CHKAUTH" "$DBG_LEVEL" fail "$file"
    done
fi

# Check for numeric exit code from EXIT_CODE_FILE as a possible EXIT_CODE
#
if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: near final top level EXIT_CODE: $EXIT_CODE" 1>&2
fi
if [[ -s $EXIT_CODE_FILE ]]; then
    NEW_EXIT_CODE=$(< "$EXIT_CODE_FILE")
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: found non-empty EXIT_CODE_FILE: $EXIT_CODE_FILE" 1>&2
    fi
    if [[ -z $NEW_EXIT_CODE ]]; then
	echo "$0: FATAL: NEW_EXIT_CODE empty but non-empty EXIT_CODE_FILE exists: $EXIT_CODE_FILE" 1>&2
	exit 55
    fi
    if [[ $NEW_EXIT_CODE =~ ^[0-9]+$ ]]; then
	if [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: NEW_EXIT_CODE: $NEW_EXIT_CODE" 1>&2
	fi
    else
	echo "$0: FATAL: NEW_EXIT_CODE is not an integer: $NEW_EXIT_CODE" 1>&2
	exit 55
    fi
    if [[ $NEW_EXIT_CODE -lt 0 || $NEW_EXIT_CODE -gt 255 ]]; then
	echo "$0: FATAL: NEW_EXIT_CODE not in range [0,255]: $NEW_EXIT_CODE" 1>&2
	exit 55
    fi
    if [[ $EXIT_CODE -eq 0 ]]; then
	EXIT_CODE="$NEW_EXIT_CODE"
	if [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: top level EXIT_CODE has been set to: $EXIT_CODE" 1>&2
	fi
    elif [[ $NEW_EXIT_CODE -eq 0 ]]; then
	if [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: while NEW_EXIT_CODE is 0, top level EXIT_CODE will remain: $EXIT_CODE" 1>&2
	fi
    fi
fi
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: top level EXIT_CODE: $EXIT_CODE" 1>&2
fi
rm -f "$EXIT_CODE_FILE"

# All Done!!! -- Jessica Noll, Age 2
#
if [[ $V_FLAG -ge 1 ]]; then
    if [[ $EXIT_CODE -eq 0 ]]; then
	echo "$0: debug[1]: all tests PASSED" 1>&2
    else
	echo "$0: debug[1]: some tests FAILED" 1>&2
    fi
fi
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: final EXIT_CODE: $EXIT_CODE" 1>&2
fi
exit "$EXIT_CODE"
