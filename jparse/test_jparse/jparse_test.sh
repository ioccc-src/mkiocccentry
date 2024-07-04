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
export JPARSE_TEST_VERSION="1.0.3 2023-08-01"
export CHK_TEST_FILE="./jparse/test_jparse/json_teststr.txt"
export JPARSE="./jparse/jparse"
export PRINT_TEST="./jparse/test_jparse/print_test"
export JSON_TREE="./jparse/test_jparse/test_JSON"
export SUBDIR="."
export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-J level] [-q] [-j jparse] [-p print_test] [-d json_tree] [-s subdir] [-k] [-L] [file ..]

    -h			print help and exit
    -V			print version and exit
    -v level		set verbosity level for this script: (def level: 0)
    -D dbg_level	set verbosity level for tests (def: level: 0)
    -J level		set JSON parser verbosity level (def level: 0)
    -q			quiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)
    -j jparse		path to jparse tool (def: $JPARSE)
    -p print_test	path to print_test tool (def: $PRINT_TEST)
    -d json_tree	read files under json_tree/subdir/good and json_tree/subdir/bad (def: $JSON_TREE)
			    These subdirectories are expected:
				json_tree/tree/subdir/bad
				json_tree/tree/subdir/good
    -s subdir		subdirectory under json_tree to find the good and bad subdirectories (def: $SUBDIR)
    -k			keep temporary files on exit (def: remove temporary files before exiting)
    -L			run error location reporting test
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
export PRINT_TEST_FLAG_USED=""
export PRINT_TEST_FAILURE=""
export K_FLAG=""
export L_FLAG=""

# parse args
#
while getopts :hVv:D:J:qj:p:d:s:kL flag; do
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
    p)	PRINT_TEST="$OPTARG";
	PRINT_TEST_FLAG_USED="1"
	;;
    d)	JSON_TREE="$OPTARG"
	;;
    s)  SUBDIR="$OPTARG"
	;;
    k)  K_FLAG="true";
        ;;
    L)  L_FLAG="true";
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
    if [[ -n "$PRINT_TEST_FLAG_USED" ]]; then
	echo "$0: debug[1]: print_test: $PRINT_TEST" 1>&2
    fi
fi

# check args
#
shift $(( OPTIND - 1 ));
export JSON_GOOD_TREE="$JSON_TREE/$SUBDIR/good"
export JSON_BAD_TREE="$JSON_TREE/$SUBDIR/bad"
export JSON_BAD_LOC_TREE="$JSON_TREE/$SUBDIR/bad_loc"

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


if [[ -n "$PRINT_TEST_FLAG_USED" ]]; then
    if [[ ! -e $PRINT_TEST ]]; then
	echo "$0: ERROR: print_test not found: $PRINT_TEST"
	exit 5
    fi
    if [[ ! -f $PRINT_TEST ]]; then
	echo "$0: ERROR: print_test not a regular file: $PRINT_TEST"
	exit 5
    fi
    if [[ ! -x $PRINT_TEST ]]; then
	echo "$0: ERROR: print_test not executable: $PRINT_TEST"
	exit 5
    fi
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

# bad location tree
#
if [[ -n "$L_FLAG" ]]; then
    if [[ ! -e $JSON_BAD_LOC_TREE ]]; then
	echo "$0: ERROR: json_tree/bad for jparse directory not found: $JSON_BAD_LOC_TREE" 1>&2
	exit 6
    fi
    if [[ ! -d $JSON_BAD_LOC_TREE ]]; then
	echo "$0: ERROR: json_tree/bad for jparse not a directory: $JSON_BAD_LOC_TREE" 1>&2
	exit 6
    fi
    if [[ ! -r $JSON_BAD_LOC_TREE ]]; then
	echo "$0: ERROR: json_tree/bad for jparse not readable directory: $JSON_BAD_LOC_TREE" 1>&2
	exit 6
    fi

    # We need a file to write the output of jparse to in order to compare it
    # with any error file. This is needed for the files that are supposed to
    # fail but it's possible that there could be a use for good files too.
    TMP_STDERR_FILE=$(mktemp -u .jparse_test.stderr.XXXXXXXXXX)
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

if [[ -n "$L_FLAG" ]]; then
    # remove or keep (some) temporary files
    #
    if [[ -z $K_FLAG ]]; then
	trap "rm -f \$TMP_STDERR_FILE; exit" 0 1 2 3 15
    else
	trap "rm -f \$TMP_STDERR_FILE; exit" 1 2 3 15
    fi
fi

# run_location_err_test - run a single test
#
# usage:
#	run_location_err_test jparse_test_file
#
#	run_location_err_test	    - our function name
#	jparse_test_file    - the jparse text file to give to jparse
#
run_location_err_test()
{
    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: expected 1 arg to run_location_err_test, found $#" 1>&2
	exit 37
    fi
    declare jparse_test_file="$1"
    declare jparse_err_file="$jparse_test_file.err"

    if [[ ! -e $jparse_test_file ]]; then
	echo "$0: in run_location_err_test: jparse_test_file not found: $jparse_test_file"
	exit 38
    fi
    if [[ ! -f $jparse_test_file ]]; then
	echo "$0: in run_location_err_test: jparse_test_file not a regular file: $jparse_test_file"
	exit 39
    fi
    if [[ ! -r $jparse_test_file ]]; then
	echo "$0: in run_location_err_test: jparse_test_file not readable: $jparse_test_file"
	exit 40
    fi

    if [[ ! -e $jparse_err_file ]]; then
	echo "$0: in run_location_err_test: jparse_err_file not found for test that must fail: $jparse_err_file"
	exit 41
    fi
    if [[ ! -f $jparse_err_file ]]; then
	echo "$0: in run_location_err_test: jparse_err_file not a regular file for test that must fail: $jparse_err_file"
	exit 42
    fi
    if [[ ! -r $jparse_err_file ]]; then
	echo "$0: in run_location_err_test: jparse_err_file not readable for test that must fail: $jparse_err_file"
	exit 43
    fi
    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_location_err_test: test must fail"
	echo "$0: debug[9]: in run_location_err_test: jparse: $JPARSE" 1>&2
	echo "$0: debug[9]: in run_location_err_test: jparse_test_file: $jparse_test_file" 1>&2
    fi

    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: about to run test that must fail: $JPARSE -- $jparse_test_file >> ${LOGFILE} 2>$TMP_STDERR_FILE" 1>&2
    fi

    "$JPARSE" -- "$jparse_test_file" 2>"$TMP_STDERR_FILE" | tee -a -- "${LOGFILE}"

    if [[ $V_FLAG -ge 7 ]]; then
	echo "$0: debug[7]: in run_location_err_test: jparse exit code: $status" 1>&2
    fi

    # examine test result
    #
    # if we have an error file (expected errors) and the output of the above
    # command does not match it is a fail.
    if ! cmp -s "$jparse_err_file" "$TMP_STDERR_FILE"; then
	echo "$0: Warning: in run_location_err_test: FAIL: $JPARSE -- $jparse_test_file 2>$TMP_STDERR_FILE | tee -a -- ${LOGFILE}" 2>&1
	echo "$0: Warning: in run_location_err_test: expected errors: $jparse_err_file do not match result of test: $TMP_STDERR_FILE" 1>&2

	if [[ $V_FLAG -lt 3 ]]; then
	    echo "$0: Warning: for more details try: $JPARSE -- $jparse_test_file" | tee -a -- "$LOGFILE" 1>&2
	fi

	echo | tee -a -- "${LOGFILE}" 1>&2
	EXIT_CODE=50
    fi

    # return
    #
    return
}


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

# run_print_test - run print_test tool, making sure it passes
#
# usage:
#	run_print_test print_test dbg_level quiet_mode
#
#	print_test		path to the print_test program
#	dbg_level		internal test debugging level to use as in: print_test -v dbg_level
#	quiet_mode		quiet mode to use in: print_test -q
#
run_print_test()
{
    # parse args
    #
    if [[ $# -ne 3 ]]; then
	echo "$0: ERROR: expected 3 args to run_print_test, found $#" 1>&2
	exit 10
    fi
    declare print_test="$1"
    declare dbg_level="$2"
    declare quiet_mode="$3"


    # debugging
    #
    if [[ $V_FLAG -ge 9 ]]; then
	echo "$0: debug[9]: in run_print_test: print_test: $print_test" 1>&2
	echo "$0: debug[9]: in run_print_test: dbg_level: $dbg_level" 1>&2
	echo "$0: debug[9]: in run_print_test: json_dbg_level: $json_dbg_level" 1>&2
	echo "$0: debug[9]: in run_print_test: quiet_mode: $quiet_mode" 1>&2
    fi

    if [[ -z $quiet_mode ]]; then
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must pass: $print_test -v $dbg_level >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run test that must pass: $print_test -v $dbg_level >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$print_test" -v "$dbg_level" >> "${LOGFILE}" 2>&1
    else
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: about to run test that must pass: $print_test -v $dbg_level -q >> ${LOGFILE} 2>&1" 1>&2
	fi
	echo "$0: debug[3]: about to run test that must pass: $print_test -v $dbg_level -q >> ${LOGFILE} 2>&1" >> "${LOGFILE}"
	"$print_test" -v "$dbg_level" -q >> "${LOGFILE}" 2>&1
    fi
    status="$?"

    # examine test result
    #
    if [[ $status -eq 0 ]]; then
	echo "$0: in test that must pass: print_test OK, exit code 0" 1>&2 >> "${LOGFILE}"
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: print_test OK, exit code 0" 1>&2 >> "${LOGFILE}"
	fi
    else
	echo "$0: in test that must pass: print_test FAIL, exit code: $status" 1>&2 >> "${LOGFILE}"
	PRINT_TEST_FAILURE="1"
	EXIT_CODE=2
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

if [[ -n "$L_FLAG" ]]; then
    while read -r file; do
	run_location_err_test "$file"
    done < <(find "$JSON_BAD_LOC_TREE" -type f -name '*.json' -print)
fi

# run print_test tool if -p used
if [[ -n "$PRINT_TEST_FLAG_USED" ]]; then
    run_print_test "$PRINT_TEST" "$DBG_LEVEL" "$Q_FLAG"
fi

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
if [[ -n "$PRINT_TEST_FAILURE" ]]; then
    echo "NOTE: the print_test test failed. See the log for details."
fi

# explicitly delete the temporary files
if [[ -n "$L_FLAG" ]]; then
    if [[ -z $K_FLAG ]]; then
	rm -f "$TMP_STDERR_FILE"
    else
	echo
	echo "$0: keeping temporary files due to use of -k"
	echo
	echo "$0: to remove the temporary files:"
	echo
	echo -n "rm -f"
	if [[ -e $TMP_STDERR_FILE ]]; then
	    echo -n " $TMP_STDERR_FILE"
	fi
	echo
    fi
fi

# All Done!!! All Done!!! -- Jessica Noll, Age 2
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
