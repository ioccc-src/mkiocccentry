#!/usr/bin/env bash
#
# json-test.sh - test jauthchk and jinfochk on JSON test fileds

# setup
#
export USAGE="usage: $0 [-h] [-v level] [-D dbg_level] [-j jinfochk] [-J jauthchk]
		      [-t jinfo_only | -t jauth_only] [-d json_tree]

    -h				print help and exit 2
    -v level			set verbosity level for this script: (def level: 0)
    -D dbg_level		set verbosity level for tests (def: level: 0)
    -j /path/to/jinfochk	path to jinfochk executable (def: ./jinfochk)
    -J /path/to/jauthchk	path to jauthchk executable (def: ./jauthchk)
    -t jinfo_only		run only jinfochk tests (def: run both)
    -t jauth_only		run only jauthchk tests (def: run both)
    -d json_tree		tree where JSON test files are to be found (def: ./test_JSON)
    				  These sub-directories are expected:
				    json_tree/author.json/bad
				    json_tree/author.json/strict-good
				    json_tree/author.json/strict-bad
				    json_tree/author.json/good
				    json_tree/info.json/bad
				    json_tree/info.json/strict-good
				    json_tree/info.json/strict-bad
				    json_tree/info.json/good

exit codes:
    0 - all is well
    1 - at least one test failed
    2 - help mode exit
    3 - invalid command line
    >= 4 - internal error"
export JINFOCHK="./jinfochk"
export JAUTHCHK="./jauthchk"
export RUN_JINFOCHK="true"
export RUN_JAUTHCHK="true"
export EXIT_CODE=0
export JSON_TREE="./test_JSON"

# parse args
#
export V_FLAG="0"
export DBG_LEVEL="0"
while getopts :hv:D:j:J:t:d: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    D) DBG_LEVEL="$OPTARG";
       ;;
    j) JINFOCHK="$OPTARG";
       ;;
    J) JAUTHCHK="$OPTARG";
       ;;
    t) if [[ $OPTARG == jinfo_only ]]; then
	    RUN_JAUTHCHK=
       elif [[ $OPTARG == jauth_only ]]; then
       	    RUN_JINFOCHK=
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
if [[ -z $RUN_JINFOCHK && -z $RUN_JAUTHCHK ]]; then
    echo "$0: ERROR: cannot specify both -t jinfo_only and -t jauth_only" 1>&2
    exit 3
fi

# jinfochk test checks
#
if [[ -n $RUN_JINFOCHK ]]; then
    if [[ ! -e $JINFOCHK ]]; then
	echo "$0: ERROR: jinfochk not found: $JINFOCHK" 1>&2
	exit 4
    fi
    if [[ ! -f $JINFOCHK ]]; then
	echo "$0: ERROR: jinfochk not a file: $JINFOCHK" 1>&2
	exit 4
    fi
    if [[ ! -x $JINFOCHK ]]; then
	echo "$0: ERROR: jinfochk not executable: $JINFOCHK" 1>&2
	exit 4
    fi
    if [[ ! -e $JSON_INFO_TREE ]]; then
	echo "$0: ERROR: json_tree for jinfochk directory not found: $JSON_INFO_TREE" 1>&2
	exit 4
    fi
    if [[ ! -d $JSON_INFO_TREE ]]; then
	echo "$0: ERROR: json_tree for jinfochk not a directory: $JSON_INFO_TREE" 1>&2
	exit 4
    fi
    if [[ ! -r $JSON_INFO_TREE ]]; then
	echo "$0: ERROR: json_tree for jinfochk not readable directory: $JSON_INFO_TREE" 1>&2
	exit 4
    fi
    for subdir in bad good strict-bad strict-good; do
	if [[ ! -e $JSON_INFO_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for jinfochk subdir not found: $JSON_INFO_TREE/$subdir" 1>&2
	    exit 4
	fi
	if [[ ! -d $JSON_INFO_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for jinfochk subdir not a directory: $JSON_INFO_TREE/$subdir" 1>&2
	    exit 4
	fi
	if [[ ! -r $JSON_INFO_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for jinfochk subdir not readable directory: $JSON_INFO_TREE/$subdir" 1>&2
	    exit 4
	fi
    done
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: will run jinfochk from: $JINFOCHK" 1>&2
    fi
fi

# jauthchk test checks
#
if [[ -n $RUN_JAUTHCHK ]]; then
    if [[ ! -e $JAUTHCHK ]]; then
	echo "$0: ERROR: jauthchk not found: $JAUTHCHK" 1>&2
	exit 4
    fi
    if [[ ! -f $JAUTHCHK ]]; then
	echo "$0: ERROR: jauthchk not a file: $JAUTHCHK" 1>&2
	exit 4
    fi
    if [[ ! -x $JAUTHCHK ]]; then
	echo "$0: ERROR: jauthchk not executable: $JAUTHCHK" 1>&2
	exit 4
    fi
    if [[ ! -e $JSON_AUTH_TREE ]]; then
	echo "$0: ERROR: json_tree for jauthchk directory not found: $JSON_AUTH_TREE" 1>&2
	exit 4
    fi
    if [[ ! -d $JSON_AUTH_TREE ]]; then
	echo "$0: ERROR: json_tree for jauthchk not a directory: $JSON_AUTH_TREE" 1>&2
	exit 4
    fi
    if [[ ! -r $JSON_AUTH_TREE ]]; then
	echo "$0: ERROR: json_tree for jauthchk not readable directory: $JSON_AUTH_TREE" 1>&2
	exit 4
    fi
    for subdir in bad good strict-bad strict-good; do
	if [[ ! -e $JSON_AUTH_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for jauthchk subdir not found: $JSON_AUTH_TREE/$subdir" 1>&2
	    exit 4
	fi
	if [[ ! -d $JSON_AUTH_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for jauthchk subdir not a directory: $JSON_AUTH_TREE/$subdir" 1>&2
	    exit 4
	fi
	if [[ ! -r $JSON_AUTH_TREE/$subdir ]]; then
	    echo "$0: ERROR: json_tree for jauthchk subdir not readable directory: $JSON_AUTH_TREE/$subdir" 1>&2
	    exit 4
	fi
    done
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: will run jauthchk from: $JAUTHCHK" 1>&2
    fi
fi

# run_test - run a single test
#
# usage:
#	run_test test_prog debug_level {must_fail|must_pass} json_test_file
#
#	run_test	- our function name
#	test_prog	- path to a test program
#	debug_level	- the -v debug level to pass to test_prog
#	must_fail	- test must fail - error if passes
#	must_pass	- test must pass - error if fails
#	json_test_file	- the JSON file to give to test_prog
#
run_test()
{
    # parse args
    #
    if [[ $# -ne 5 ]]; then
	echo "$0: ERROR: expected 5 args to run_test, founbd $#" 1>&2
	exit 4
    fi
    typeset test_prog="$1"
    typeset debug_level="$2"
    typeset pass_fail="$3"
    typeset strict="$4"
    typeset json_test_file="$5"
    if [[ ! -e $test_prog ]]; then
	echo "$0: in run_test: test_prog not found: $test_prog"
	exit 4
    fi
    if [[ ! -f $test_prog ]]; then
	echo "$0: in run_test: test_prog not a file: $test_prog"
	exit 4
    fi
    if [[ ! -x $test_prog ]]; then
	echo "$0: in run_test: test_prog not executable: $test_prog"
	exit 4
    fi
    if [[ $pass_fail != pass && $pass_fail != fail ]]; then
	echo "$0: in run_test: pass_fail neither pass nor fail: $pass_fail" 1>&2
	exit 4
    fi
    if [[ $strict != strict && $strict != notstrict ]]; then
	echo "$0: in run_test: strict neither strict nor notstrict: $strict" 1>&2
	exit 4
    fi
    if [[ ! -e $test_prog ]]; then
	echo "$0: in run_test: json_test_file not found: $json_test_file"
	exit 4
    fi
    if [[ ! -f $test_prog ]]; then
	echo "$0: in run_test: json_test_file not a file: $json_test_file"
	exit 4
    fi
    if [[ ! -r $test_prog ]]; then
	echo "$0: in run_test: json_test_file not readabke: $json_test_file"
	exit 4
    fi

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_test: test_prog: $test_prog" 1>&2
	echo "$0: debug[9]: in run_test: debug_level: $debug_level" 1>&2
	echo "$0: debug[9]: in run_test: pass or fail: $pass_fail" 1>&2
	echo "$0: debug[9]: in run_test: strict or notstrict: $strict" 1>&2
	echo "$0: debug[9]: in run_test: json_test_file: $json_test_file" 1>&2
    fi

    # case: run a strict test
    #
    if [[ $strict == strict ]]; then
	if [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: in run_test: about to run: $test_prog -v $debug_level -s -- $json_test_file" 1>&2
	fi
	if [[ $debug_level -gt 0 ]]; then
	    "$test_prog" -v "$debug_level" -s -- "$json_test_file"
	    status="$?"
	else
	    "$test_prog" -v 0 -q -s -- "$json_test_file"
	    status="$?"
	fi
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: in run_test: test_prog exit code: $status" 1>&2
	fi

    # case: run a notstrict test
    #
    else
	if [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: in run_test: about to run: $test_prog -v $debug_level -- $json_test_file" 1>&2
	fi
	if [[ $debug_level -gt 0 ]]; then
	    "$test_prog" -v "$debug_level" -- "$json_test_file"
	    status="$?"
	else
	    "$test_prog" -v 0 -q -- "$json_test_file"
	    status="$?"
	fi
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: in run_test: test_prog exit code: $status" 1>&2
	fi
    fi

    # examine test result
    #
    if [[ $pass_fail == pass ]]; then
	if [[ $status -ne 0 ]]; then
	    echo "$0: Warning: in run_test: FAIL: $test_prog -v $debug_level $json_test_file exit code: $status != 0" 1>&2
	    EXIT_CODE=1
	elif [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: in run_test: PASS: $test_prog -v $debug_level $json_test_file" 1>&2
	fi
    elif [[ $pass_fail == fail ]]; then
	if [[ $status -eq 0 ]]; then
	    echo "$0: Warning: in run_test: FAIL: $test_prog -v $debug_level $json_test_file exit code: $status == 0" 1>&2
	    EXIT_CODE=1
	elif [[ $V_FLAG -ge 5 ]]; then
	    echo "$0: debug[5]: in run_test: PASS: $test_prog -v $debug_level $json_test_file" 1>&2
	fi
    fi

    # return
    #
    return
}

# run jinfochk tests
#
if [[ -n $RUN_JINFOCHK ]]; then

    # run notstrict tests that must pass
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run jinfochk notstrict tests that must pass" 1>&2
    fi
    find "$JSON_INFO_TREE/good" -type f -print | while read -r file; do
        run_test "$JINFOCHK" "$DBG_LEVEL" pass notstrict "$file"
    done

    # run notstrict tests that must fail
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run jinfochk notstrict tests that must fail" 1>&2
    fi
    find "$JSON_INFO_TREE/bad" -type f -print | while read -r file; do
        run_test "$JINFOCHK" "$DBG_LEVEL" fail notstrict "$file"
    done

    # run strict tests that must pass
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run jinfochk strict tests that must pass" 1>&2
    fi
    find "$JSON_INFO_TREE/strict-good" -type f -print | while read -r file; do
        run_test "$JINFOCHK" "$DBG_LEVEL" pass strict "$file"
    done

    # run strict tests that must fail
    #
    if [[ $V_FLAG -ge 3 ]]; then
	"$0: debug[3]: about to run strict jinfochk tests that must fail" 1>&2
    fi
    find "$JSON_INFO_TREE/strict-bad" -type f -print | while read -r file; do
        run_test "$JINFOCHK" "$DBG_LEVEL" fail strict "$file"
    done
fi

# run jauthchk tests
#
if [[ -n $RUN_JAUTHCHK ]]; then

    # run notstrict tests that must pass
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run jauthchk notstrict tests that must pass" 1>&2
    fi
    find "$JSON_AUTH_TREE/good" -type f -print | while read -r file; do
        run_test "$JAUTHCHK" "$DBG_LEVEL" pass notstrict "$file"
    done

    # run notstrict tests that must fail
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run jauthchk notstrict tests that must fail" 1>&2
    fi
    find "$JSON_AUTH_TREE/bad" -type f -print | while read -r file; do
        run_test "$JAUTHCHK" "$DBG_LEVEL" fail notstrict "$file"
    done

    # run strict tests that must pass
    #
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run jauthchk strict tests that must pass" 1>&2
    fi
    find "$JSON_AUTH_TREE/strict-good" -type f -print | while read -r file; do
        run_test "$JAUTHCHK" "$DBG_LEVEL" pass strict "$file"
    done

    # run strict tests that must fail
    #
    if [[ $V_FLAG -ge 3 ]]; then
	"$0: debug[3]: about to run strict jauthchk tests that must fail" 1>&2
    fi
    find "$JSON_AUTH_TREE/strict-bad" -type f -print | while read -r file; do
        run_test "$JAUTHCHK" "$DBG_LEVEL" fail strict "$file"
    done
fi

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
