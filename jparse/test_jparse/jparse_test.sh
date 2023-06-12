#!/usr/bin/env bash
#
# jparse_test.sh - test JSON parser on valid and invalid JSON file(s)
#
# Run jparse on simple one-line JSON files or on whole JSON documents to test the
# JSON parser.
#
# When used with "-d json_tree -s subdir", two directories are expected:
#
#	json_tree/tree/subdir/good
#	json_tree/tree/subdir/bad
#
# All files under json_tree/tree/subdir/good must be valid JSON.  If any file under
# that directory tests as an invalid JSON file, this script will exit non-zero.
#
# All files under json_tree/tree/subdir/bad must be valid JSON.  If any file under
# that directory tests as a valid JSON file, this script will exit non-zero.
#
# Without "-d json_tree -s subdir" a list of files in the command line are
# tested to see if they are valid JSON files.  With no arguments, or - the
# contents of stdin is tested.
#
# "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
#
# This JSON parser and the test suite were co-developed in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# "Share and Enjoy!"
#     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)


# setup
#
export JPARSE_TEST_VERSION="1.0.1 2023-06-12"
export CHK_TEST_FILE="./jparse/test_jparse/json_teststr.txt"
export JPARSE="./jparse/jparse"
export JSON_TREE="./jparse/test_jparse/test_JSON"
export SUBDIR="."
export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-J level] [-q] [-j jparse] [-d json_tree] [-s subdir] [file ..]

    -h			print help and exit
    -V			print version and exit
    -v level		set verbosity level for this script: (def level: 0)
    -D dbg_level	set verbosity level for tests (def: level: 0)
    -J level		set JSON parser verbosity level (def level: 0)
    -q			quiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)
    -j /path/to/jparse	path to jparse tool (def: $JPARSE)
    -d json_tree	read files under json_tree/subdir/good and json_tree/subdir/bad (def: $JSON_TREE)
			    These subdirectories are expected:
				json_tree/tree/subdir/bad
				json_tree/tree/subdir/good
    -s subdir		subdirectory under json_tree to find the good and bad subdirectories (def: $SUBDIR)
    [file ...]		read JSON documents, one per line, from these files, - means stdin (def: $CHK_TEST_FILE)
			NOTE: To use stdin, end the command line with: -- -

Exit codes:
     0   all tests are OK
     1   at least one test failed
     2	 -h and help string printed or -V and version string printed
     3   invalid command line
     4	 jparse not a regular executable file
     5	 couldn't create writable log file
     6	 missing directory or directories
     7	 missing JSON file
  >= 10  internal error

jparse_test.sh version: $JPARSE_TEST_VERSION"

export V_FLAG="0"
export DBG_LEVEL="0"
export JSON_DBG_LEVEL="0"
export Q_FLAG=""
export LOGFILE="./jparse_test.log"
export EXIT_CODE=0
export FILE_FAILURE_SUMMARY=""
export STRING_FAILURE_SUMMARY=""

# parse args
#
while getopts :hVv:D:J:qj:d:s: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$JPARSE_TEST_VERSION"
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
    j)	JPARSE="$OPTARG";
	;;
    d)	JSON_TREE="$OPTARG"
	;;
    s)  SUBDIR="$OPTARG"
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
    echo "$0: debug[1]: -s: $SUBDIR" 1>&2
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
export JSON_GOOD_TREE="$JSON_TREE/$SUBDIR/good"
export JSON_BAD_TREE="$JSON_TREE/$SUBDIR/bad"

# firewall
#
if [[ ! -e $JPARSE ]]; then
    echo "$0: ERROR: jparse not found: $JPARSE"
    exit 4
fi
if [[ ! -f $JPARSE ]]; then
    echo "$0: ERROR: jparse not a regular file: $JPARSE"
    exit 4
fi
if [[ ! -x $JPARSE ]]; then
    echo "$0: ERROR: jparse not executable: $JPARSE"
    exit 4
fi

# check that json_tree is a readable directory
#
if [[ ! -e $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not found: $JSON_TREE" 1>&2
    exit 6
fi
if [[ ! -d $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not a directory: $JSON_TREE" 1>&2
    exit 6
fi
if [[ ! -r $JSON_TREE ]]; then
    echo "$0: ERROR: json_tree not readable directory: $JSON_TREE" 1>&2
    exit 6
fi

# good tree
#
if [[ ! -e $JSON_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/good for jparse directory not found: $JSON_GOOD_TREE" 1>&2
    exit 6
fi
if [[ ! -d $JSON_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/good for jparse not a directory: $JSON_GOOD_TREE" 1>&2
    exit 6
fi
if [[ ! -r $JSON_GOOD_TREE ]]; then
    echo "$0: ERROR: json_tree/good for jparse not readable directory: $JSON_GOOD_TREE" 1>&2
    exit 6
fi

# bad tree
#
if [[ ! -e $JSON_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/bad for jparse directory not found: $JSON_BAD_TREE" 1>&2
    exit 6
fi
if [[ ! -d $JSON_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/bad for jparse not a directory: $JSON_BAD_TREE" 1>&2
    exit 6
fi
if [[ ! -r $JSON_BAD_TREE ]]; then
    echo "$0: ERROR: json_tree/bad for jparse not readable directory: $JSON_BAD_TREE" 1>&2
    exit 6
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

# run_file_test - run a single jparse test on a file
#
# usage:
#	run_file_test jparse dbg_level json_dbg_level quiet_mode json_doc_file pass|fail
#
#	jparse			path to the jparse program
#	dbg_level		internal test debugging level to use as in: jparse -v dbg_level
#	json_dbg_level		JSON parser debug level to use in: jparse -J json_dbg_level
#	quiet_mode		quiet mode to use in: jparse -q
#	json_doc_file		JSON document as a string to give to jparse
#	pass|fail		string saying if jparse must return valid json or invalid json
#
run_file_test()
{
    # parse args
    #
    if [[ $# -ne 6 ]]; then
	echo "$0: ERROR: expected 6 args to run_file_test, found $#" 1>&2
	exit 10
    fi
    declare jparse="$1"
    declare dbg_level="$2"
    declare json_dbg_level="$3"
    declare quiet_mode="$4"
    declare json_doc_file="$5"
    declare pass_fail="$6"

    if [[ "$pass_fail" != "pass" && "$pass_fail" != "fail" ]]; then
	echo "$0: ERROR: in run_file_test: pass_fail neither 'pass' nor 'fail'" 1>&2
	EXIT_CODE=11
	return
    fi

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_file_test: jparse: $jparse" 1>&2
	echo "$0: debug[9]: in run_file_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_file_test: json_dbg_level: $json_dbg_level" 1>&2
	echo "$0: debug[9]: in run_file_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_file_test: json_doc_file: $json_doc_file" 1>&2
	echo "$0: debug[9]: in run_file_test: pass_fail: $pass_fail" 1>&2
    fi

    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $jparse -v $dbg_level -J $json_dbg_level -- $json_doc_file >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run test that must $pass_fail: $jparse -v $dbg_level -J $json_dbg_level -- $json_doc_file >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$jparse" -v "$dbg_level" -J "$json_dbg_level" -- "$json_doc_file" >> "${LOGFILE}" 2>&1
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must $pass_fail: $jparse -v $dbg_level -J $json_dbg_level -q -- $json_doc_file >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run test that must $pass_fail: $jparse -v $dbg_level -J $json_dbg_level -q -- $json_doc_file >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$jparse" -v "$dbg_level" -J "$json_dbg_level" -q -- "$json_doc_file" >> "${LOGFILE}" 2>&1
    fi
    status="$?"

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
	if [[ $pass_fail = pass ]]; then
	    echo "$0: in test that must pass: jparse OK, exit code 0" 1>&2 >> "${LOGFILE}"
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]: jparse OK, exit code 0" 1>&2 >> "${LOGFILE}"
	    fi
	else
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must fail: jparse FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_file_test: jparse exit code: $status" 1>&2 >> "${LOGFILE}"
		fi
	    fi
	    FILE_FAILURE_SUMMARY="$FILE_FAILURE_SUMMARY
	    FILE: $json_doc_file"
	    EXIT_CODE=1
	fi
    else
	if [[ $pass_fail = pass ]]; then
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must pass: jparse FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_file_test: jparse exit code: $status" 1>&2 >> "${LOGFILE}"
		fi
	    fi
	    FILE_FAILURE_SUMMARY="$FILE_FAILURE_SUMMARY
	    FILE: $json_doc_file"
	    EXIT_CODE=1
	else
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: in test that must fail: jparse OK, exit code: $status" 1>&2 >> "${LOGFILE}"
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]: in run_file_test: jparse exit code: $status" 1>&2 >> "${LOGFILE}"
		fi
	    fi
	fi
    fi
    echo >> "${LOGFILE}"

    # return
    #
    return
}


# run_string_test - run a single jparse test on a string
#
# usage:
#	run_string_test jparse dbg_level json_dbg_level quiet_mode json_doc_string
#
#	jparse			path to the jparse program
#	dbg_level		internal test debugging level to use as in: jparse -v dbg_level
#	json_dbg_level		JSON parser debug level to use in: jparse -J json_dbg_level
#	quiet_mode		quiet mode to use in: jparse -q
#	json_doc_string		JSON document as a string to give to jparse
#
run_string_test()
{
    # parse args
    #
    if [[ $# -ne 5 ]]; then
	echo "$0: ERROR: expected 5 args to run_string_test, found $#" 1>&2
	exit 12
    fi
    declare jparse="$1"
    declare dbg_level="$2"
    declare json_dbg_level="$3"
    declare quiet_mode="$4"
    declare json_doc_string="$5"

    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_string_test: jparse: $jparse" 1>&2
	echo "$0: debug[9]: in run_string_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_string_test: json_dbg_level: $json_dbg_level" 1>&2
	echo "$0: debug[9]: in run_string_test: quiet_mode: $quiet_mode" 1>&2
	echo "$0: debug[9]: in run_string_test: json_doc_string: $json_doc_string" 1>&2
    fi

    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run: $jparse -v $dbg_level -J $json_dbg_level -s -- $json_doc_string >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run: $jparse -v $dbg_level -J $json_dbg_level -s -- $json_doc_string >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$jparse" -v "$dbg_level" -J "$json_dbg_level" -s -- "$json_doc_string" >> "${LOGFILE}" 2>&1
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run: $jparse -v $dbg_level -J $json_dbg_level -q -s -- $json_doc_string >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run: $jparse -v $dbg_level -J $json_dbg_level -q -s -- $json_doc_string >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$jparse" -v "$dbg_level" -J "$json_dbg_level" -q -s -- "$json_doc_string" >> "${LOGFILE}" 2>&1
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
		echo "$0: debug[3]: in run_string_test: jparse exit code: $status" 1>&2 >> "${LOGFILE}"
	    fi
	fi
	STRING_FAILURE_SUMMARY="$STRING_FAILURE_SUMMARY
	STRING: $json_doc_string"
	EXIT_CODE=1
    fi
    echo >> "${LOGFILE}"

    # return
    #
    return
}

# case: process stdin
#
if [[ $# -gt 0 ]]; then

    while [[ $# -gt 0 ]]; do

	# note input source
	#
	CHK_TEST_FILE="$1"
	shift
	if [[ $V_FLAG -ge 1 && "$CHK_TEST_FILE" != "-" ]]; then
	    echo "$0: debug[1]: reading JSON documents from: $CHK_TEST_FILE" 1>&2 >> "${LOGFILE}"
	fi

	# firewall - check test file
	#
	if [[ "$CHK_TEST_FILE" = "-" ]]; then
	    # note input source
	    #
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: reading JSON documents from stdin" 1>&2 >> "${LOGFILE}"
	    fi

	    # read JSON document lines from stdin
	    #
	    while read -r JSON_DOC; do
		run_string_test "$JPARSE" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$JSON_DOC"
	    done
	else
	    if [[ ! -e $CHK_TEST_FILE ]]; then
		echo "$0: ERROR: test file not found: $CHK_TEST_FILE"
		exit 7
	    fi
	    if [[ ! -f $CHK_TEST_FILE ]]; then
		echo "$0: ERROR: test file not a regular file: $CHK_TEST_FILE"
		exit 7
	    fi
	    if [[ ! -r $CHK_TEST_FILE ]]; then
		echo "$0: ERROR: test file not readable: $CHK_TEST_FILE"
		exit 7
	    fi
	    # process all lines in test file
	    #
	    while read -r JSON_DOC; do
		run_string_test "$JPARSE" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$JSON_DOC"
	    done < "$CHK_TEST_FILE"
	fi
    done
elif [[ -n "$CHK_TEST_FILE" ]]; then
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: reading JSON documents from: $CHK_TEST_FILE" 1>&2 >> "${LOGFILE}"
	fi

	# firewall - check test file
	#
	if [[ ! -e $CHK_TEST_FILE ]]; then
	    echo "$0: ERROR: test file not found: $CHK_TEST_FILE"
	    exit 7
	fi
	if [[ ! -f $CHK_TEST_FILE ]]; then
	    echo "$0: ERROR: test file not a regular file: $CHK_TEST_FILE"
	    exit 7
	fi
	if [[ ! -r $CHK_TEST_FILE ]]; then
	    echo "$0: ERROR: test file not readable: $CHK_TEST_FILE"
	    exit 7
	fi

	# process all lines in test file
	#
	while read -r JSON_DOC; do
	    run_string_test "$JPARSE" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$JSON_DOC"
	done < "$CHK_TEST_FILE"
fi

# run tests that must pass
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run jparse tests that must pass: JSON files" 1>&2 >> "${LOGFILE}"
fi
while read -r file; do
    run_file_test "$JPARSE" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$file" pass
done < <(find "$JSON_GOOD_TREE" -type f -name '*.json' -print)


# run tests that must fail
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to run jparse tests that must fail: JSON files" 1>&2 >> "${LOGFILE}"
fi
while read -r file; do
    run_file_test "$JPARSE" "$DBG_LEVEL" "$JSON_DBG_LEVEL" "$Q_FLAG" "$file" fail
done < <(find "$JSON_BAD_TREE" -type f -name '*.json' -print)


if [[ -n "$FILE_FAILURE_SUMMARY" ]]; then
    echo "The following files failed: " | tee -a -- "${LOGFILE}"
    echo "--" | tee -a -- "${LOGFILE}"
    echo "$FILE_FAILURE_SUMMARY" | tee -a -- "${LOGFILE}"
    echo "--" | tee -a -- "${LOGFILE}"
fi
if [[ -n "$STRING_FAILURE_SUMMARY" ]]; then
    echo "The following strings failed: " | tee -a -- "${LOGFILE}"
    echo "--" | tee -a -- "${LOGFILE}"
    echo "$STRING_FAILURE_SUMMARY" | tee -a -- "${LOGFILE}"
    echo "--" | tee -a -- "${LOGFILE}"
fi

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
