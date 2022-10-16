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

BUG_REPORT_VERSION="0.3 2022-10-16"

# All of our tools. Kept towards top of file in case the list needs to be
# changed.
export TOOLS="./run_bison.sh ./run_flex.sh ./hostchk.sh ./dbg ./dyn_test ./fnamchk
	      ./ioccc_test.sh ./iocccsize ./iocccsize_test.sh ./chkentry ./jnum_gen
	      ./jnum_chk ./jparse ./jparse_test.sh ./jstr_test.sh ./jstrencode
	      ./jstrdecode ./mkiocccentry ./mkiocccentry_test.sh ./prep.sh
	      ./reset_tstamp.sh ./txzchk ./txzchk_test.sh ./utf8_test ./verge
	      ./vermod.sh ./jsemcgen.sh ./jsemtblgen"

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


EXIT_CODE=0

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
	declare f="$1"
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

# Make array of all commands we need.
#
# This is done this way so that we can use each command's index as an offset for
# an exit code that will be added to BASE_ERR_EXIT_CODE (the first exit code
# that is an error beyond the one that does not have an individual code, see below).
#
# Since arrays also don't have to be assigned contiguously this will allow us to
# also spread commands out for exit codes. We shouldn't need more than the
# amount of exit codes available so this should be okay.
#
# There is an exception to the exit codes here and that's the check for if each
# of our tools is executable. For that we use a single exit code 10. This is not
# done until later on but with exit code 10 so that it's out of the way of the
# other exit codes.
#
# We assign the commands to the array one at a time so we have control of the
# index for the command which will also be part of the error code if it goes
# wrong.
#
# The 0th element will be exit code 0 + BASE_ERR_EXIT_CODE if it fails; the 1st
# element will be 1 + BASE_ERR_EXIT_CODE and so on.
#
# NOTE: make all will indirectly call make fast_hostchk which, if it reports an
# issue, will be reported here. However we also directly invoke hostchk.sh later
# which will also report an issue so that one would likely see the warning more
# than once. If you only see the warning once then there probably is also a
# problem. The warning issued will suggest that you can file the issue with this
# script and it also suggests that you can get more information with running
# hostchk.sh directly.
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
BASE_ERR_EXIT_CODE=11
COMMANDS[0]="uname -a"			# uname -a		get system information
COMMANDS[1]="which cc"			# which cc		get path for cc
COMMANDS[2]="cc -v"			# cc -v			get compiler version
COMMANDS[3]="which make"		# which make		get make path
COMMANDS[4]="make -v"			# make -v		get make version
COMMANDS[5]="make clobber"		# make clobber		start clean
COMMANDS[6]="make all"			# make all		compile everything
COMMANDS[7]="make test"			# make test		run mkiocccentry test suite
COMMANDS[8]="which tar"			# which tar		get path to tar
COMMANDS[9]="./hostchk.sh -v 3"		# ./hostchk.sh -v 3	various checks on the host
COMMANDS[10]="./run_bison.sh -v 1"	# ./run_bison.sh -v 1	get details about bison
COMMANDS[11]="./run_flex.sh -v 1"	# ./run_flex.sh -v 1	get details about flex

# NOTE: The below references to PIPESTATUS (instead of $?) fix the bug
# introduced in commit 8343c4b8cb97e52df64fe8973e68f0d83c6090e1 where the exit
# status of each command was not checked properly which meant that even if a
# test failed it would not be reported as an issue which rather defeated the
# purpose of this script.
#
# As amusing as the thought is that there's a bug in a script to help report
# bugs and issues, this bug was not in fact intentional. :-) I had thought of it
# earlier on but I neglected to address it before the commit as I moved on with
# adding the tests. However since the world definitely needs more reasons to
# laugh I'm keeping this comment here for the sake of humour and the irony that
# I caused. You're welcome! :-)

for i in "${!COMMANDS[@]}"; do
    echo "## RUNNING ${COMMANDS[$i]}: " | tee -a -- "$LOG_FILE"
    command ${COMMANDS[$i]} | tee -a -- "$LOG_FILE"
    status=${PIPESTATUS[0]}
    if [[ "$status" -ne 0 ]]; then
	EXIT_CODE=$((i + BASE_ERR_EXIT_CODE))
	echo "$0: ERROR: ${COMMANDS[$i]} failed with exit code $status: new exit code $EXIT_CODE" | tee -a -- "$LOG_FILE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	${COMMANDS[$i]} non-zero exit code: $status"
    fi
    echo '' | tee -a -- "$LOG_FILE"
done

for f in $TOOLS; do
    if is_exec "$f"; then
	echo "## Checking if $f is executable" | tee -a -- "$LOG_FILE"
	echo "$0: $f is executable" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	echo "## RUNNING $f -h" | tee -a -- "$LOG_FILE"
	"$f" -h 2>&1 | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
    else
	EXIT_CODE=10
	echo "$0: ERROR: $f is not executable: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	$f cannot be executed"
    fi
done

if [[ "$EXIT_CODE" -ne 0 ]]; then
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
