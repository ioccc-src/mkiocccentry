#!/usr/bin/env bash
#
# jparse_test.sh - test jparse on simple one-line JSON files

# setup
#
export USAGE="usage: $0 [-h] [-v level] [-D dbg_level.sh] [-J level] [-q] [-p jparse] [file ..]

    -h			print help and exit 2
    -v level		set verbosity level for this script: (def level: 0)
    -D dbg_level	set verbosity level for tests (def: level: 0)
    -J level		set JSON parser verbosity level (def level: 0)
    -q			quiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)
    -D dbg_level	set verbosity level for tests (def: level: 0)
    -p /path/to/jparse	path to jparse tool (def: ./jparse)

    [file ...]		read JSON documents, one per line, from these files (def: from stdin)

exit codes:
    0 - all is well
    1 - at least one test failed
    2 - help mode exit
    3 - invalid command line
    >= 4 - internal error"
export V_FLAG="0"
export DBG_LEVEL="0"
export JSON_DBG_LEVEL="0"
export Q_FLAG=""
export JPARSE="./jparse"
export LOGFILE="./jparse_test.log"
export EXIT_CODE=0

# parse args
#
while getopts :hv:D:J:qp: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    D) DBG_LEVEL="$OPTARG";
       ;;
    J) JSON_DBG_LEVEL="$OPTARG";
       ;;
    q) Q_FLAG="-q";
       ;;
    p) JPARSE="$OPTARG";
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
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: -v: $V_FLAG" 1>&2
    echo "$0: debug[1]: -D: $DBG_LEVEL" 1>&2
    echo "$0: debug[1]: -J: $JSON_DBG_LEVEL" 1>&2
    if [[ -z $Q_FLAG ]]; then
	echo "$0: debug[1]: -q: false" 1>&2
    else
	echo "$0: debug[1]: -q: true" 1>&2
    fi
    echo "$0: debug[1]: jparse: $JPARSE" 1>&2
fi

# check args
#
shift $(( OPTIND - 1 ));

# firewall
#
if [[ ! -e $JPARSE ]]; then
    echo "$0: jparse not found: $JPARSE"
    exit 4
fi
if [[ ! -f $JPARSE ]]; then
    echo "$0: jparse not a regular file: $JPARSE"
    exit 4
fi
if [[ ! -x $JPARSE ]]; then
    echo "$0: jparse not executable: $JPARSE"
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

# run_test - run a single jparse test
#
# usage:
#	run_test jparse dbg_level json_dbg_level quiet_mode json_doc_string
#
#	jparse			path to the jparse program
#	dbg_level		internal test debugging level to use as in: jparse -v dbg_level
#	json_dbg_level		JSON parser debug level to use in: jparse -J json_dbg_level
#	quiet_mode		quiet mode to use in: jparse -q
#	json_doc_string		JSON document as a string to give to jparse
#
run_test()
{
    # parse args
    #
    if [[ $# -ne 5 ]]; then
	echo "$0: ERROR: expected 5 args to run_test, found $#" 1>&2
	exit 4
    fi
    typeset jparse="$1"
    typeset dbg_level="$2"
    typeset json_dbg_level="$3"
    typeset quiet_mode="$4"
    typeset json_doc_string="$5"

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_test: jparse: $jparse" 1>&2
	echo "$0: debug[9]: in run_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_test: json_dbg_level: $json_dbg_level" 1>&2
	echo "$0: debug[9]: in run_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_test: json_doc_string: $json_doc_string" 1>&2
    fi

    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run: $jparse -v $dbg_level -J $json_dbg_level -s $json_doc_string >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run: $jparse -v $dbg_level -J $json_dbg_level -s $json_doc_string >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$jparse" -v "$dbg_level" -J "$json_dbg_level" -s "$json_doc_string" >> "${LOGFILE}" 2>&1
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run: $jparse -v $dbg_level -J $json_dbg_level -q -s $json_doc_string >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run: $jparse -v $dbg_level -J $json_dbg_level -q -s $json_doc_string >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$jparse" -v "$dbg_level" -J "$json_dbg_level" -q -s "$json_doc_string" >> "${LOGFILE}" 2>&1
    fi
    status="$?"

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
	echo "$0: jparse OK, exit code 0" 1>&2 >> "${LOGFILE}"
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: jparse OK, exit code 0" 1>&2
	fi
    else
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: jparse FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]: in run_test: jparse exit code: $status" 1>&2
	    fi
	fi
	EXIT_CODE=1
    fi
    echo >> "${LOGFILE}"

    # return
    #
    return
}

# case: process stdin
#
if [[ $# -eq 0 ]]; then

    # note input source
    #
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: reading JSON documents from stdin" 1>&2
    fi

    # read JSON document lines from stdin
    #
    while read -r JSON_DOC; do
	run_test "$JPARSE" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$JSON_DOC"
    done

else

    while [[ $# -gt 0 ]]; do

	# note input source
	#
	JSON_TEST_FILE="$1"
	shift
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: reading JSON documents from: $JSON_TEST_FILE" 1>&2
	fi

	# firewall - check test file
	#
	if [[ ! -e $JSON_TEST_FILE ]]; then
	    echo "$0: test file not found: $JSON_TEST_FILE"
	    exit 4
	fi
	if [[ ! -f $JSON_TEST_FILE ]]; then
	    echo "$0: test file not a regular file: $JSON_TEST_FILE"
	    exit 4
	fi
	if [[ ! -r $JSON_TEST_FILE ]]; then
	    echo "$0: test file not readable: $JSON_TEST_FILE"
	    exit 4
	fi

	# process all lines in test file
	#
	while read -r JSON_DOC; do
	    run_test "$JPARSE" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$JSON_DOC"
	done < "$JSON_TEST_FILE"

    done
fi

# All Done!!! -- Jessica Noll, Age 2
#
if [[ $V_FLAG -ge 1 ]]; then
    if [[ $EXIT_CODE -eq 0 ]]; then
	echo "$0: debug[1]: all tests PASSED" 1>&2
	echo "$0: debug[1]: all tests PASSED" >> "${LOGFILE}"
	echo 1>&2
	echo "$0: see jparse_test.log for details" 1>&2
    else
	echo "$0: debug[1]: some tests FAILED" 1>&2
	echo "$0: debug[1]: some tests FAILED" >> "${LOGFILE}"
	echo 1>&2
	echo "$0: see jparse_test.log for details" 1>&2
    fi
fi
exit "$EXIT_CODE"
