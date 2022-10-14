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

export BUG_REPORT_VERSION="0.1 2022-10-14"

export DBG_LEVEL="0"
export V_FLAG="0"
export USAGE="usage: $0 [-h] [-V] [-v level] [-D level]

    -h			    print help and exit 2
    -V			    print version and exit 2
    -v level		    set verbosity level for this script: (def level: $V_FLAG)
    -D level		    set verbosity level for tests (def: $DBG_LEVEL)

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


# assume all tests will pass
EXIT_CODE=0

# NOTE: log file does not have an underscore in the name because we want to
# distinguish it from this script which does have an underscore in it.
LOG_FILE="bug-report.$(/bin/date +%Y%m%d.%H%M%S).txt"

# attempt to create log file
rm -f "$LOG_FILE"
touch "$LOG_FILE"

if [[ ! -e "$LOG_FILE" ]]; then
    echo "$0: could not create log file: $LOG_FILE"
    exit 1
fi
if [[ ! -w "$LOG_FILE" ]]; then
    echo "$0: log file not writable: $LOG_FILE"
    exit 1
fi

if [[ $V_FLAG -gt 1 ]]; then
    echo "Will write contents to $LOG_FILE" 1>&2
fi

echo "## TIME OF REPORT: $(date)" | tee -a -- "$LOG_FILE"
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
    EXIT_CODE=10
    echo "make clobber failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
fi
echo '' >> "$LOG_FILE" 1>&2
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
make all | tee -a -- "$LOG_FILE"  1>&2
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    EXIT_CODE=11
    echo "make all failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
fi
echo '' >> "$LOG_FILE" 1>&2
echo "## RUNNING make test: " | tee -a -- "$LOG_FILE" 1>&2
make test | tee -a -- "$LOG_FILE"  1>&2
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    EXIT_CODE=12
    echo "make test failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
fi

echo '' >> "$LOG_FILE" 1>&2
echo "## RUNNING hostchk.sh -v 3: " | tee -a -- "$LOG_FILE" 1>&2
./hostchk.sh -v 3 | tee -a -- "$LOG_FILE"  1>&2
status=${PIPESTATUS[0]}
if [[ "$status" -ne 0 ]]; then
    EXIT_CODE=13
    echo "hostchk.sh failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
fi


if [[ "$EXIT_CODE" -ne 0 ]]; then
    echo "Found one or more issues. Please file an issue on the GitHub issues page:" 1>&2
    echo '' 1>&2
    echo "	https://github.com/ioccc-src/mkiocccentry/issues" 1>&2
    echo '' 1>&2
    echo "making sure to attach $LOG_FILE with your report. You may" 1>&2
    echo "instead email the Judges." 1>&2
else
    echo "All tests PASSED" 1>&2
    echo '' 1>&2
    echo "A log of the above tests was saved to $LOG_FILE." 1>&2
    echo "If you feel everything is in order you may safely delete that file." 1>&2
    echo "Otherwise you may report the issue at the GitHub issue page:" 1>&2
    echo '' 1>&2
    echo "	https://github.com/ioccc-src/mkiocccentry/issues" 1>&2
    echo '' 1>&2
    echo "making sure to attach $LOG_FILE with your report. You may" 1>&2
    echo "instead email the Judges." 1>&2

fi

if [[ "$V_FLAG" -gt 1 ]]; then
    echo "About to exit with exit code: $EXIT_CODE" 1>&2
fi
exit "$EXIT_CODE"
