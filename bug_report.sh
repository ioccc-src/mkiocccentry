#!/usr/bin/env bash
#
# bug-report.sh - collect system information to help user report bugs and issues
# using the mkiocccentry tools
#
# This script is being written by:
#
#	@xexyl
# 	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# NOTE: This is a work in progress.
#

# All of our tools. Kept towards top of file in case the list needs to be
# changed.
export TOOLS="./run_bison.sh ./run_flex.sh ./hostchk.sh ./dbg ./dyn_test ./fnamchk
	      ./ioccc_test.sh ./iocccsize ./iocccsize_test.sh ./chkentry ./jnum_gen
	      ./jnum_chk ./jparse ./jparse_test.sh ./jstr_test.sh ./jstrencode
	      ./jstrdecode ./mkiocccentry ./mkiocccentry_test.sh ./prep.sh
	      ./reset_tstamp.sh ./txzchk ./txzchk_test.sh ./utf8_test ./verge
	      ./vermod.sh"

export BUG_REPORT_VERSION="0.2 2022-10-15"
export FAILURE_SUMMARY=
export DBG_LEVEL="0"
export V_FLAG="0"
export USAGE="usage: $0 [-h] [-V] [-v level] [-D level]

    -h			    print help and exit 2
    -V			    print version and exit 2
    -v level		    set verbosity level for this script: (def level: $V_FLAG)
    -D level		    set verbosity level for tests (def: $DBG_LEVEL)

Exit codes:
     0   all is well
     1   failed to create a bug report file
     2   help mode exit or print version mode exit
     3   invalid command line
 >= 10   internal error

$0 version: $BUG_REPORT_VERSION"


# parse args
#
export V_FLAG="0"
while getopts :hVv:D: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$BUG_REPORT_VERSION" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    D)  DBG_LEVEL="$OPTARG";
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


# Assume all tests will pass. We start at 10 but whenever any test fails it is
# incremented. This makes it easy to rearrange the tests. If at the end the
# EXIT_CODE is still 10 we know every test passed.
EXIT_CODE=10

# NOTE: log file does not have an underscore in the name because we want to
# distinguish it from this script which does have an underscore in it.
LOG_FILE="bug-report.$(/bin/date +%Y%m%d.%H%M%S).txt"

# attempt to create log file
rm -f "$LOG_FILE"
touch "$LOG_FILE"

if [[ ! -e "$LOG_FILE" ]]; then
    echo "$0: ERROR: could not create log file: $LOG_FILE"
    exit 1
fi
if [[ ! -w "$LOG_FILE" ]]; then
    echo "$0: ERROR: log file not writable: $LOG_FILE"
    exit 1
fi


# is_exec   - determine if arg exists, is a regular file and is executable
function is_exec()
{
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: expected 1 arg to is_exec, found $#" | tee -a -- "$LOG_FILE"
	return 1
    else
	typeset f="$1"
	if [[ ! -e "$f" ]]; then
	    echo "$0: ERROR: $1 does not exist" | tee -a -- "$LOG_FILE"
	    return 1
	fi
	if [[ ! -f "$f" ]]; then
	    echo "$0: ERROR: $1 is not a regular file" | tee -a -- "$LOG_FILE"
	    return 1
	fi
	if [[ ! -x "$f" ]]; then
	    echo "$0: ERROR: $1 is not executable" | tee -a -- "$LOG_FILE"
	    return 1
	fi
	return 0
    fi
}

if [[ $V_FLAG -gt 1 ]]; then
    echo "Will write contents to $LOG_FILE" 1>&2
fi

echo "## TIME OF REPORT: $(date)" | tee -a -- "$LOG_FILE"

# uname -a: get system information
echo "## RUNNING uname -a: " | tee -a -- "$LOG_FILE"
uname -a | tee -a -- "$LOG_FILE"
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    EXIT_CODE=10
    echo "$0: ERROR: uname -a failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    uname -a non-zero exit code: $status"
fi
echo | tee -a -- "$LOG_FILE"

# which cc: get all paths for cc
echo "## RUNNING which -a cc: " | tee -a -- "$LOG_FILE"
which cc | tee -a -- "$LOG_FILE"
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: which cc failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    which cc non-zero exit code: $status"
fi
echo | tee -a -- "$LOG_FILE"

# cc -v: get compiler version
echo "## RUNNING cc -v: " | tee -a -- "$LOG_FILE"
cc -v 2>&1 | tee -a -- "$LOG_FILE"
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: cc -v failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    cc -v non-zero exit code: $status"
fi
echo | tee -a -- "$LOG_FILE"

# which make: get path to make tool
echo "## RUNNING which make: " | tee -a -- "$LOG_FILE"
which -a make | tee -a -- "$LOG_FILE"
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: which -a make failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    which -a make non-zero exit code: $status"
fi
echo | tee -a -- "$LOG_FILE"

# make -v: get make version
echo "## RUNNING make -v: " | tee -a -- "$LOG_FILE"
make -v | tee -a -- "$LOG_FILE"
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: make -v failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    make -v non-zero exit code: $status"
fi
echo | tee -a -- "$LOG_FILE"



# make clobber: start clean
echo "## RUNNING make clobber: " | tee -a -- "$LOG_FILE"
make clobber | tee -a -- "$LOG_FILE"  1>&2
# The below references to PIPESTATUS (instead of $?) fix the bug introduced in commit
# 8343c4b8cb97e52df64fe8973e68f0d83c6090e1 where the exit status of each command
# was not checked properly which meant that even if a test failed it would not
# be reported as an issue which rather defeated the purpose of this script.
#
# As amusing as the thought is that there's a bug in a script to help report
# bugs and issues, this bug was not in fact intentional. :-) I had thought of it
# earlier on but I neglected to address it before the commit as I moved on with
# adding the tests. However since the world definitely needs more reasons to
# laugh I'm keeping this comment here for the sake of humour and the irony that
# I caused. You're welcome! :-)
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: make clobber failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    make clobber non-zero exit code: $status"
fi
echo '' | tee -a -- "$LOG_FILE"

# make all: compile everything before we do anything else
echo "## RUNNING make all: " | tee -a -- "$LOG_FILE" 1>&2
# NOTE: This will indirectly call make fast_hostchk which, if it reports an
# issue, will be reported here. However we also directly invoke hostchk.sh later
# which will also report an issue so that one would likely see the warning more
# than once. If you only see the warning once then there probably is also a
# problem. The warning issued will suggest that you can file the issue with
# this script and it also suggests that you can get more information with
# running hostchk.sh directly.
#
# Obviously one needn't run this script a second or third time just because it
# runs hostchk.sh which suggests that you run this script each time it exits
# non-zero! :-) Sorry in advance if this (to make use of an American English
# phrase which I will happily use for a pun for a pun not made is a wasted
# opportunity :-) ) throws you for a loop! :-) But now that you're in the loop
# it shouldn't even matter. :-)
#
# This might seem extra verbose or overkill but we feel that if there's an issue
# with hostchk.sh it really is an issue that will likely prevent a successful
# use of this repo so each time the script fails we report the issue since it
# really is likely that, if the script fails, you will be unable to successfully
# use the mkiocccentry repo to submit a correct IOCCC entry.
#
make all | tee -a -- "$LOG_FILE"
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: make all failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    make all non-zero exit code: $status"
fi
echo | tee -a -- "$LOG_FILE"

# make test: run the IOCCC toolkit test suite
echo "## RUNNING make test: " | tee -a -- "$LOG_FILE"
make test | tee -a -- "$LOG_FILE"  1>&2
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: make test failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    make test non-zero exit code: $status"
fi
echo '' >> "$LOG_FILE" 1>&2

# which tar: find the path to tar
echo "## RUNNING which tar: " | tee -a -- "$LOG_FILE"
which tar | tee -a -- "$LOG_FILE"
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: which tar failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    which tar non-zero exit code: $status"
fi
echo '' | tee -a -- "$LOG_FILE"
# NOTE: we don't need to check if tar accepts the correct options in this script
# because hostchk.sh will do that later on.
for f in $TOOLS; do
    if is_exec "$f"; then
	echo "## Checking if $f is executable" | tee -a -- "$LOG_FILE"
	echo "$0: $f is executable" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	echo "## RUNNING $f -h" | tee -a -- "$LOG_FILE"
	"$f" -h | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
    else
	((EXIT_CODE++))
	echo "$0: ERROR: $f is not executable: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	$f cannot be executed"
    fi
done


echo '' >> "$LOG_FILE" 1>&2
echo "## RUNNING hostchk.sh -v 3: " | tee -a -- "$LOG_FILE" 1>&2
./hostchk.sh -v 3 | tee -a -- "$LOG_FILE"  1>&2
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    ((EXIT_CODE++))
    echo "$0: ERROR: hostchk.sh failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    hostchk.sh test non-zero exit code: $status"
fi


# The reason we check for 10 is that's the default value and each time a test
# fails the exit code is incremented. This makes it easy to rearrange the tests
# without having to worry to update the exit codes.
if [[ "$EXIT_CODE" -ne 10 ]]; then
    echo 1>&2
    echo "One or more problems occurred:" | tee -a -- "$LOG_FILE"
    echo "$FAILURE_SUMMARY" | tee -a -- "$LOG_FILE"
    echo  | tee -a -- "$LOG_FILE"
    echo "Final exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    echo "Please file an issue on the GitHub issues page:" | tee -a -- "$LOG_FILE"
    echo  | tee -a -- "$LOG_FILE"
    echo "	https://github.com/ioccc-src/mkiocccentry/issues" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    echo "making sure to attach $LOG_FILE with your report. You may" | tee -a -- "$LOG_FILE"
    echo "instead email the Judges." | tee -a -- "$LOG_FILE"
else
    # here we do set the exit code back to 0 if all is okay so the exit status
    # of the script does not trigger a false failure.
    EXIT_CODE=0
    echo "All tests PASSED" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    echo "A log of the above tests was saved to $LOG_FILE." | tee -a -- "$LOG_FILE"
    echo "If you feel everything is in order you may safely delete that file." | tee -a -- "$LOG_FILE"
    echo "Otherwise you may report the issue at the GitHub issue page:" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    echo "	https://github.com/ioccc-src/mkiocccentry/issues" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    echo "making sure to attach $LOG_FILE with your report. You may" | tee -a -- "$LOG_FILE"
    echo "instead email the Judges." | tee -a -- "$LOG_FILE"

fi

if [[ "$V_FLAG" -gt 1 ]]; then
    echo "About to exit with exit code: $EXIT_CODE" 1>&2
fi
exit "$EXIT_CODE"
