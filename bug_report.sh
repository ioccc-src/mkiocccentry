#!/usr/bin/env bash
#
# bug-report.sh - collect system information to help user report bugs and issues
# using the mkiocccentry tools
#
# NOTE: This is a work in progress.

export BUG_REPORT_VERSION="0.0 2022-10-13"
export USAGE="usage: $0 [-h] [-V] [-v level]

    -h			    print help and exit 2
    -V			    print version and exit 2
    -v level		    set verbosity level for this script: (def level: 0)

exit codes:
    0 - all is well
    1 - failed to create a bug report file
    2 - help mode exit or print version mode exit
    3 - invalid command line
    >= 10 - internal error

$0 version: $BUG_REPORT_VERSION"


# parse args
#
export V_FLAG="0"
while getopts :hVv:t:d:T:F: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$BUG_REPORT_VERSION" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
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
status=$?
if [[ "$status" -ne 0 ]]; then
    EXIT_CODE=10
    echo "make clobber failed: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
fi
echo '' >> "$LOG_FILE" 1>&2
echo "## RUNNING make all: " | tee -a -- "$LOG_FILE" 1>&2
make all | tee -a -- "$LOG_FILE"  1>&2
status=$?
if [[ "$status" -ne 0 ]]; then
    EXIT_CODE=11
    echo "make all failed: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
fi
echo '' >> "$LOG_FILE" 1>&2
echo "## RUNNING make test: " | tee -a -- "$LOG_FILE" 1>&2
make test | tee -a -- "$LOG_FILE"  1>&2
status=$?
if [[ "$status" -ne 0 ]]; then
    EXIT_CODE=12
    echo "make test failed with exit code $status: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
fi

if [[ "$EXIT_CODE" -ne 0 ]]; then
    echo "Found one more issues. Please file an issue on the GitHub issues page and attach" 1>&2
    echo "the file $LOG_FILE in your report" 1>&2
else
    echo "All tests PASSED" 1>&2
    echo "A log of the above tests was saved to $LOG_FILE" 1>&2
    echo "If you feel everything is in order you may safely delete the file." 1>&2
fi

if [[ "$V_FLAG" -gt 1 ]]; then
    echo "About to exit with exit code: $EXIT_CODE" 1>&2
fi
exit "$EXIT_CODE"
