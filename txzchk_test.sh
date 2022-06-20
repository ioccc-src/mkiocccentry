#!/usr/bin/env bash
#
# txzchk_test.sh - test txzchk with good and bad tarballs (as text files)
#
# Written in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# "Because sometimes even the IOCCC Judges need some help." :-)

# setup
#

TAR="$(type -P tar)"
export USAGE="usage: $0 [-h] [-v level] [-D dbg_level] [-t txzchk] [-T tar]
		     [-F fnamchk] [-d txzchk_tree]

    -h			    print help and exit 2
    -v level		    set verbosity level for this script: (def level: 0)
    -D dbg_level	    set verbosity level for tests (def: level: 0)
    -t txzchk		    path to txzchk executable (def: ./txzchk)
    -T tar		    path to tar that accepts -J option (def: $TAR)
    -F fnamchk	            path to fnamchk (def: ./fnamchk)

    -d txzchk_tree	    tree where txzchk test files are to be found (def: ./test_txzchk)
    			      These subdirectories are expected:
				txzchk_tree/bad
				txzchk_tree/good

exit codes:
    0 - all is well
    1 - at least one test failed
    2 - help mode exit
    3 - invalid command line
    >= 4 - internal error"
export TXZCHK="./txzchk"
export EXIT_CODE=0
export TXZCHK_TREE="./test_txzchk"
export LOGFILE="./txzchk_test.log"
export FNAMCHK="./fnamchk"

# parse args
#
export V_FLAG="0"
export DBG_LEVEL="0"
while getopts :hv:D:t:d:T:F: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    D) DBG_LEVEL="$OPTARG";
       ;;
    t) TXZCHK="$OPTARG";
       ;;
    d) TXZCHK_TREE="$OPTARG";
	;;
    F) FNAMCHK="$OPTARG";
	;;
    T) TAR="$OPTARG";
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
    TXZCHK_TREE="$1"
fi
export TXZCHK_GOOD_TREE="$TXZCHK_TREE/good"
export TXZCHK_BAD_TREE="$TXZCHK_TREE/bad"

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

# check that txzchk_tree is a readable directory
#
if [[ ! -e $TXZCHK_TREE ]]; then
    echo "$0: ERROR: txzchk_tree not found: $TXZCHK_TREE" 1>&2
    exit 4
fi
if [[ ! -d $TXZCHK_TREE ]]; then
    echo "$0: ERROR: txzchk_tree not a directory: $TXZCHK_TREE" 1>&2
    exit 4
fi
if [[ ! -r $TXZCHK_TREE ]]; then
    echo "$0: ERROR: txzchk_tree not readable directory: $TXZCHK_TREE" 1>&2
    exit 4
fi

# good tree
#
if [[ ! -e $TXZCHK_GOOD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/good for txzchk directory not found: $TXZCHK_GOOD_TREE" 1>&2
    exit 4
fi
if [[ ! -d $TXZCHK_GOOD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/good for txzchk not a directory: $TXZCHK_GOOD_TREE" 1>&2
    exit 4
fi
if [[ ! -r $TXZCHK_GOOD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/good for txzchk not readable directory: $TXZCHK_GOOD_TREE" 1>&2
    exit 4
fi

# bad tree
#
if [[ ! -e $TXZCHK_BAD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/bad for txzchk directory not found: $TXZCHK_BAD_TREE" 1>&2
    exit 4
fi
if [[ ! -d $TXZCHK_BAD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/bad for txzchk not a directory: $TXZCHK_BAD_TREE" 1>&2
    exit 4
fi
if [[ ! -r $TXZCHK_BAD_TREE ]]; then
    echo "$0: ERROR: txzchk_tree/bad for txzchk not readable directory: $TXZCHK_BAD_TREE" 1>&2
    exit 4
fi

# check for txzchk
#
if [[ ! -e $TXZCHK ]]; then
    echo "$0: ERROR: txzchk not found: $TXZCHK" 1>&2
    exit 4
fi
if [[ ! -f $TXZCHK ]]; then
    echo "$0: ERROR: txzchk not a regular file: $TXZCHK" 1>&2
    exit 4
fi
if [[ ! -x $TXZCHK ]]; then
    echo "$0: ERROR: txzchk not executable: $TXZCHK" 1>&2
    exit 4
fi

# check for fnamchk
#
if [[ ! -e $FNAMCHK ]]; then
    echo "$0: ERROR: fnamchk not found: $FNAMCHK" 1>&2
    exit 4
fi
if [[ ! -f $FNAMCHK ]]; then
    echo "$0: ERROR: fnamchk not a regular file: $FNAMCHK" 1>&2
    exit 4
fi
if [[ ! -x $FNAMCHK ]]; then
    echo "$0: ERROR: fnamchk not executable: $FNAMCHK" 1>&2
    exit 4
fi

# check for tar
#
if [[ ! -e $TAR ]]; then
    echo "$0: ERROR: tar not found: $TAR" 1>&2
    exit 4
fi
if [[ ! -f $TAR ]]; then
    echo "$0: ERROR: tar not a regular file: $TAR" 1>&2
    exit 4
fi
if [[ ! -x $TAR ]]; then
    echo "$0: ERROR: tar not executable: $TAR" 1>&2
    exit 4
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

if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: will run txzchk from: $TXZCHK" 1>&2
fi

# run_test - run a single test
#
# usage:
#	run_test debug_level {fail|pass} txzchk_test_file
#
#	run_test	    - our function name
#	debug_level	    - the -v debug level to pass to txzchk
#	fail		    - test must fail - error if passes
#	pass		    - test must pass - error if fails
#	txzchk_test_file    - the txzchk text file to give to txzchk
#
run_test()
{
    # parse args
    #
    if [[ $# -ne 3 ]]; then
	echo "$0: ERROR: expected 3 args to run_test, found $#" 1>&2
	exit 4
    fi
    typeset debug_level="$1"
    typeset pass_fail="$2"
    typeset txzchk_test_file="$3"

    if [[ ! -e $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not found: $txzchk_test_file"
	exit 4
    fi
    if [[ ! -f $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not a regular file: $txzchk_test_file"
	exit 4
    fi
    if [[ ! -r $txzchk_test_file ]]; then
	echo "$0: in run_test: txzchk_test_file not readable: $txzchk_test_file"
	exit 4
    fi
    if [[ $pass_fail != pass && $pass_fail != fail ]]; then
	echo "$0: in run_test: pass_fail neither 'pass' nor 'fail': $pass_fail" 1>&2
	exit 4
    fi

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_test: txzchk: $TXZCHK" 1>&2
	echo "$0: debug[9]: in run_test: tar: $TAR" 1>&2
	echo "$0: debug[9]: in run_test: fnamchk: $FNAMCHK" 1>&2
	echo "$0: debug[9]: in run_test: debug_level: $debug_level" 1>&2
	echo "$0: debug[9]: in run_test: pass or fail: $pass_fail" 1>&2
	echo "$0: debug[9]: in run_test: txzchk_test_file: $txzchk_test_file" 1>&2
    fi

    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: in run_test: about to run: $TXZCHK -v $debug_level -t $TAR -F $FNAMCHK -T -E txt -e -- $txzchk_test_file" 1>&2
    fi
    if [[ $debug_level -gt 0 ]]; then
	"$TXZCHK" -v "$debug_level" -t "$TAR" -F "$FNAMCHK" -T -E txt -- "$txzchk_test_file" 1>&2
	status="$?"
    else
	"$TXZCHK" -v 0 -q -F "$FNAMCHK" -t "$TAR" -T -E txt -e -- "$txzchk_test_file" 1>&2
	status="$?"
    fi
    if [[ $V_FLAG -ge 7 ]]; then
	echo "$0: debug[7]: in run_test: txzchk exit code: $status" 1>&2
    fi

    # examine test result
    #
    if [[ $pass_fail == pass ]]; then
	if [[ $status -ne 0 ]]; then
	    echo "$0: Warning: in run_test: FAIL: $TXZCHK -v $debug_level -t $TAR -F $FNAMCHK -T -E txt $txzchk_test_file exit code: $status != 0" | tee -a "$LOGFILE" 1>&2
	    echo "$0: Warning: the above mentioned test FAILED when it should have PASSED" | tee -a "$LOGFILE" 1>&2
	    if [[ $V_FLAG -lt 3 ]]; then
		echo "$0: Warning: for more details try: $TXZCHK -v 3 -t $TAR -F $FNAMCHK -T -E txt -- $txzchk_test_file" | tee -a "$LOGFILE" 1>&2
	    else
		echo "$0: Warning: for more details try: $TXZCHK -v $V_FLAG -t $TAR -F $FNAMCHK -T -E txt -- $txzchk_test_file" | tee -a "$LOGFILE" 1>&2
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
	    echo "$0: debug[5]: in run_test: PASS: $TXZCHK -v $debug_level -t $TAR -F $FNAMCHK -T -E txt $debug_level $txzchk_test_file" 1>&2
	fi
    elif [[ $pass_fail == fail ]]; then
	if [[ $status -eq 0 ]]; then
	    echo "$0: Warning: in run_test: FAIL: $TXZCHK -v $debug_level -F $FNAMCHK -T -E txt $txzchk_test_file exit code: $status == 0" | tee -a "$LOGFILE" 1>&2
	    echo "$0: Warning: the above mentioned test PASSED when it should have FAILED" | tee -a "$LOGFILE" 1>&2
	    if [[ $V_FLAG -lt 3 ]]; then
		echo "$0: Warning: for more details try: $TXZCHK -v 3 -F $FNAMCHK -T -E txt -- $txzchk_test_file" | tee -a "$LOGFILE" 1>&2
	    else
		echo "$0: Warning: for more details try: $TXZCHK -v $V_FLAG -F $FNAMCHK -T -E txt -- $txzchk_test_file" | tee -a "$LOGFILE" 1>&2
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
	    echo "$0: debug[5]: in run_test: PASS: $TXZCHK -v $debug_level -F $FNAMCHK -T -E txt $txzchk_test_file" 1>&2
	fi
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
find "$TXZCHK_GOOD_TREE" -type f -name '*.txt' -print | while read -r file; do
    run_test "$DBG_LEVEL" pass "$file"
done


# run tests that must fail
#

# now the text files
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run txzchk tests that must fail: text files" 1>&2
fi
find "$TXZCHK_BAD_TREE" -type f -name '*.txt' -print | while read -r file; do
    run_test "$DBG_LEVEL" fail "$file"
done

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
exit "$EXIT_CODE"
