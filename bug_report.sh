#!/usr/bin/env bash
#
# bug-report.sh - collect system information to help user report bugs and issues
# using the mkiocccentry tools
#
# This script was written in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# with some minor improvements by:
#
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#

# setup
#
# Maintain this list towards the top of file, in sorted order.
#
# Do NOT put this tool (bug-report.sh) in the list, it you will
# cause an infinite loop.
#
export TOOLS="
    ./chkentry
    ./dbg/dbg_test
    ./fnamchk
    ./hostchk.sh
    ./iocccsize
    ./jnum_gen
    ./jparse
    ./jsemcgen.sh
    ./jsemtblgen
    ./jstrdecode
    ./jstrencode
    ./mkiocccentry
    ./prep.sh
    ./reset_tstamp.sh
    ./run_bison.sh
    ./run_flex.sh
    ./run_usage.sh
    ./soup/all_ref.sh
    ./test_ioccc/chkentry_test.sh
    ./test_ioccc/dyn_test
    ./test_ioccc/ioccc_test.sh
    ./test_ioccc/iocccsize_test.sh
    ./test_ioccc/jnum_chk
    ./test_ioccc/jparse_test.sh
    ./test_ioccc/jstr_test.sh
    ./test_ioccc/mkiocccentry_test.sh
    ./test_ioccc/txzchk_test.sh
    ./test_ioccc/utf8_test
    ./txzchk
    ./verge
    ./vermod.sh
    "

export BUG_REPORT_VERSION="0.7 2022-11-10"
export FAILURE_SUMMARY=
export WARNING_SUMMARY=
export DBG_LEVEL="0"
export V_FLAG="0"
export EXIT_CODE=0
export USAGE="usage: $0 [-h] [-V] [-v level] [-D level]

    -h			    print help and exit
    -V			    print version and exit
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

# Determine the name of the log file
#
# NOTE: log file does not have an underscore in the name because we want to
# distinguish it from this script which does have an underscore in it.
#
LOG_FILE="bug-report.$(/bin/date +%Y%m%d.%H%M%S).txt"
export LOG_FILE

# attempt to create a writable log file
#
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
#
is_exec()
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

# get path to tools we might need for get_version and get_version_optional
# functions below
#
WHAT="$(which what)"
IDENT="$(which ident)"
STRINGS="$(which strings)" # this should always exist but we check anyway

# get_version_optional    -	try and get version of an optional tool
#
# A question is how do to determine the version of a tool when there's no
# universal option to get the version of _all_ tools (and in some tools we
# cannot detect the version as it will block instead or in the case of echo or
# yes just print the args).
#
# This is a good question and there's no guarantee we can obtain a version. In
# that case we will report it as an unknown version. Nevertheless we go through
# a series of steps as follows. First (step 0) we attempt to get the path to the
# command and then we run the following on the path (if it appears to be a
# built-in we cannot use the path of course).
#
#   1)	use --version (note: under macOS and BSD this will fail on several
#	tools and in some tools it will fail under linux as well)
#   2)	use -V (note: this might fail due to multiple reasons)
#   3)	use -v (note: this might fail due to multiple reasons)
#   4)	try what(1) (note: this is a macOS and seems available under some BSDs
#	as well - but despite the man page saying it conforms to to IEEE Std
#	1003.1-2001 ("POSIX.1") it is not available under linux)
#   5)	try ident(1) (note: this appears to be a BSD command that's not
#	available under macOS either)
#   6)	try strings(1) with showing just the first 10 lines
#
# As soon as one of these returns a zero exit code (except for what(1) which is
# described in the function below) we will stop. If we get through all steps
# without any results we will mark it as an unknown version.  The first step to
# succeed we will run a second time to record the output.
#
# Unfortunately this is far from perfect but we hope that it will help in a lot
# of cases. A note about what(1) and ident(1) is that we will only check for it
# once and if it does not exist we won't try it again. We also use the command
# command -p in hopes to get the actual path. This also might not be perfect.
#
# usage:
#	get_version_optional command
#
#	command	    - command to try and obtain the version
#
# NOTE: we don't want the path to the tool in this function as we try
# determining that instead.
#
get_version_optional() {

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" 1>&2
	exit 3
    fi
    local COMMAND
    local EXIT=0
    COMMAND="$(which "$1")"
    if ! is_exec "$COMMAND"; then
	return
    fi

    echo "## VERSION CHECK FOR: $1" | tee -a -- "$LOG_FILE"

    # try --version
    #
    command "${COMMAND}" --version >/dev/null 2>&1
    status=$?
    if [[ "$status" -eq 0 ]]; then
	command "${COMMAND}" --version | tee -a -- "$LOG_FILE"
	echo "## $COMMAND --version ABOVE" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	return
    fi

    # try -v
    #
    command "${COMMAND}" -v >/dev/null 2>&1
    status=$?
    if [[ "$status" -eq 0 ]]; then
	command "${COMMAND}" -v | tee -a -- "$LOG_FILE"
	echo "## $COMMAND -v ABOVE" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	return
    fi

    # try -V
    #
    command "${COMMAND}" -V >/dev/null 2>&1
    status=$?
    if [[ "$status" -eq 0 ]]; then
	command "${COMMAND}" -V | tee -a -- "$LOG_FILE"
	echo "## $COMMAND -V ABOVE" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	return
    fi

    # try what(1) if available
    #
    # An important note is that what(1) might not get the correct information.
    # For instance running it on bmake(1) I see:
    #
    #	$ what ./bmake
    #   ./bmake:
    #	 Copyright (c) 1988, 1989, 1990, 1993 The Regents of the University of California.  All rights reserved.
    #
    # which(1) is entirely useless.
    #
    # The question is should ident(1) come first but the trouble is I don't
    # actually know what it looks like. Also if the tool in question does not
    # have the appropriate string it won't give us anything useful either.
    # Here's what what(1) looks like on cut(1) which as can be seen is useful:
    #
    #	$ what /usr/bin/cut
    #	/usr/bin/cut:
    #	PROGRAM:cut  PROJECT:text_cmds-154
    #	PROGRAM:cut  PROJECT:text_cmds-154
    #
    # Looking at the Apple website this is indeed the version. Thus because it's
    # not something that will work in all cases instead we will try ident(1) as
    # well even if what(1) succeeds. If either succeeds we will not try
    # strings(1).
    #
    if [[ ! -z "$WHAT" ]]; then
	$WHAT "${COMMAND}"  >/dev/null 2>&1
	status=$?
	if [[ "$status" -eq 0 ]]; then
	    $WHAT "${COMMAND}" | tee -a -- "$LOG_FILE"
	    echo "## OUTPUT OF what $COMMAND ABOVE" | tee -a -- "$LOG_FILE"
	    echo | tee -a -- "$LOG_FILE"
	    EXIT=1
	fi
    fi

    # try ident(1) if available
    #
    # The same or similar caveats for what(1) might apply here too but I have no
    # way to test this.
    #
    if [[ ! -z "$IDENT" ]]; then
	$IDENT "${COMMAND}"  >/dev/null 2>&1
	status=$?
	if [[ "$status" -eq 0 ]]; then
	    $IDENT "${COMMAND}" | tee -a -- "$LOG_FILE"
	    echo "## OUTPUT OF ident $COMMAND ABOVE" | tee -a -- "$LOG_FILE"
	    echo | tee -a -- "$LOG_FILE"
	    EXIT=1
	fi
    fi

    # if we got output from either what(1) or ident(1) then skip strings(1)
    #
    if [[ -n "$EXIT" ]]; then
	return
    fi

    # try strings(1) if available. The filter is arbitrarily selected. For some
    # tools it might not be enough lines but if we get here it's probably not
    # going to be what we want anyway.
    #
    # Now a question to be answered is should we even use strings? The reason to
    # ask such a question is it's likely to work which means that we might never
    # reach the unknown version and strings(1) probably won't actually give us
    # the version.
    #
    # This is why we warn that there's a possible unknown version and only if
    # strings fails do we report positively that the version is unknown.
    #
    if [[ ! -z "$STRINGS" ]]; then
	echo "$0: WARNING: UNKNOWN VERSION FOR $COMMAND: trying strings" | tee -a -- "$LOG_FILE"
	WARNING_SUMMARY="$WARNING_SUMMARY
	WARNING: UNKNOWN VERSION FOR $COMMAND: trying strings"
	$STRINGS "${COMMAND}" | head -n 15 >/dev/null 2>&1
	status=${PIPESTATUS[0]}
	if [[ "$status" -eq 0 ]]; then
	    $STRINGS "${COMMAND}" | head -n 15 | tee -a -- "$LOG_FILE"
	    echo "## OUTPUT OF strings $COMMAND ABOVE" | tee -a -- "$LOG_FILE"
	    echo | tee -a -- "$LOG_FILE"
	    return
	fi
    fi

    # report unknown version
    #
    echo "$0: WARNING: UNKNOWN VERSION FOR $COMMAND" | tee -a -- "$LOG_FILE"
    WARNING_SUMMARY="$WARNING_SUMMARY
    WARNING: $COMMAND VERSION UNKNOWN"
    echo | tee -a -- "$LOG_FILE"
    return 0;
}

# get_version    -	try and get version of a tool
#
# usage:
#	get_version command
#
#	command	    - command to try and obtain the version
#
# NOTE: we don't want the path to the tool in this function as we try
# determining that instead.
#
get_version() {

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" 1>&2
	exit 3
    fi
    local COMMAND
    local EXIT=0
    COMMAND="$(which "$1")"
    if ! is_exec "$COMMAND"; then
	# if not executable we can try doing it as a built-in. This might or
	# might not need to be a better check. Although some of the tools are
	# built-ins in say zsh it does not appear to be in bash so it's possibly
	# not a problem (since we use /usr/bin/env bash).
	#
	# Additionally trying to run command on a built-in in zsh (for example:
	# true) will work because it's also a file. Also if it fails to run we
	# will know there's a problem and likely due to something missing so the
	# below might be all that's necessary.
	COMMAND="$1"
    fi
    echo "## VERSION CHECK FOR: $1" | tee -a -- "$LOG_FILE"

    # try --version
    #
    command "${COMMAND}" --version >/dev/null 2>&1
    status=$?
    if [[ "$status" -eq 0 ]]; then
	command "${COMMAND}" --version | tee -a -- "$LOG_FILE"
	echo "## OUTPUT OF $COMMAND --version ABOVE" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	return
    fi

    # try -v
    #
    command "${COMMAND}" -v >/dev/null 2>&1
    status=$?
    if [[ "$status" -eq 0 ]]; then
	command "${COMMAND}" -v | tee -a -- "$LOG_FILE"
	echo "## OUTPUT OF $COMMAND -v ABOVE" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	return
    fi

    # try -V
    #
    command "${COMMAND}" -V >/dev/null 2>&1
    status=$?
    if [[ "$status" -eq 0 ]]; then
	command "${COMMAND}" -V | tee -a -- "$LOG_FILE"
	echo "## OUTPUT OF $COMMAND -V ABOVE" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	return
    fi

    # try what(1) if available
    #
    # An important note is that what(1) might not get the correct information.
    # For instance running it on bmake(1) I see:
    #
    #	$ what ./bmake
    #   ./bmake:
    #	 Copyright (c) 1988, 1989, 1990, 1993 The Regents of the University of California.  All rights reserved.
    #
    # which(1) is entirely useless.
    #
    # The question is should ident(1) come first but the trouble is I don't
    # actually know what it looks like. Also if the tool in question does not
    # have the appropriate string it won't give us anything useful either.
    # Here's what what looks like on cut(1) which as can be seen is useful:
    #
    #	$ what /usr/bin/cut
    #	/usr/bin/cut:
    #	PROGRAM:cut  PROJECT:text_cmds-154
    #	PROGRAM:cut  PROJECT:text_cmds-154
    #
    # Looking at the Apple website this is indeed the version. Thus because it's
    # not something that will work in all cases instead we will try ident(1) as
    # well even if this succeeds. If either succeeds we will not try strings(1).
    #
    if [[ ! -z "$WHAT" ]]; then
	$WHAT "${COMMAND}"  >/dev/null 2>&1
	status=$?
	if [[ "$status" -eq 0 ]]; then
	    $WHAT "${COMMAND}" | tee -a -- "$LOG_FILE"
	    echo "## OUTPUT OF what $COMMAND ABOVE" | tee -a -- "$LOG_FILE"
	    echo | tee -a -- "$LOG_FILE"
	    EXIT=1
	fi
    fi

    # try ident(1) if available
    #
    # The same or similar caveats for what(1) might apply here too but I have no
    # way to test this.
    #
    if [[ ! -z "$IDENT" ]]; then
	$IDENT "${COMMAND}"  >/dev/null 2>&1
	status=$?
	if [[ "$status" -eq 0 ]]; then
	    $IDENT "${COMMAND}" | tee -a -- "$LOG_FILE"
	    echo "## OUTPUT OF ident $COMMAND ABOVE" | tee -a -- "$LOG_FILE"
	    echo | tee -a -- "$LOG_FILE"
	    EXIT=1
	fi
    fi

    # if we got output from either what(1) or ident(1) then skip strings(1)
    #
    if [[ -n "$EXIT" ]]; then
	return
    fi

    # try strings(1) if available. The filter is arbitrarily selected. For some
    # tools it might not be enough lines but if we get here it's probably not
    # going to be what we want anyway.
    #
    # Now a question to be answered is should we even use strings? The reason to
    # ask such a question is it's likely to work which means that we might never
    # reach the unknown version and strings(1) probably won't actually give us
    # the version.
    #
    # This is why we warn that there's a possible unknown version and only if
    # strings fails do we report positively that the version is unknown.
    #
    if [[ ! -z "$STRINGS" ]]; then
	echo "$0: WARNING: UNKNOWN VERSION FOR $COMMAND: trying strings" | tee -a -- "$LOG_FILE"
	WARNING_SUMMARY="$WARNING_SUMMARY
	WARNING: UNKNOWN VERSION FOR $COMMAND: trying strings"
	$STRINGS "${COMMAND}" | head -n 15 >/dev/null 2>&1
	status=${PIPESTATUS[0]}
	if [[ "$status" -eq 0 ]]; then
	    $STRINGS "${COMMAND}" | head -n 15 | tee -a -- "$LOG_FILE"
	    echo "## strings $COMMAND ABOVE" | tee -a -- "$LOG_FILE"
	    echo | tee -a -- "$LOG_FILE"
	    return
	fi
    fi

    # report unknown version
    #
    echo "$0: WARNING: UNKNOWN VERSION FOR $COMMAND" | tee -a -- "$LOG_FILE"
    WARNING_SUMMARY="$WARNING_SUMMARY
    WARNING: $COMMAND VERSION UNKNOWN"
    echo | tee -a -- "$LOG_FILE"
    return 0;
}

# get_version_minimal    -  try and get version of a tool by a limited number of ways
#
# usage:
#	get_version_minimal command
#
#	command	    - command to try and obtain the version
#
# NOTE: we don't want the path to the tool in this function as we try
# determining that instead.
#
# NOTE: don't warn if we cannot get the version and don't use strings(1) either.
#
get_version_minimal() {

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" 1>&2
	exit 3
    fi
    local COMMAND
    local EXIT=0
    COMMAND="$(which "$1")"
    if ! is_exec "$COMMAND"; then
	# if not executable we can try doing it as a built-in. This might or
	# might not need to be a better check. Although some of the tools are
	# built-ins in say zsh it does not appear to be in bash so it's possibly
	# not a problem (since we use /usr/bin/env bash).
	#
	# Additionally trying to run command on a built-in in zsh (for example:
	# true) will work because it's also a file. Also if it fails to run we
	# will know there's a problem and likely due to something missing so the
	# below might be all that's necessary.
	COMMAND="$1"
    fi
    echo "## VERSION CHECK FOR: $1" | tee -a -- "$LOG_FILE"

    # try --version
    #
    command "${COMMAND}" --version >/dev/null 2>&1
    status=$?
    if [[ "$status" -eq 0 ]]; then
	command "${COMMAND}" --version | tee -a -- "$LOG_FILE"
	echo "## OUTPUT OF $COMMAND --version ABOVE" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
	return
    fi

    # try what(1) if available
    #
    # An important note is that what(1) might not get the correct information.
    # For instance running it on bmake(1) I see:
    #
    #	$ what ./bmake
    #   ./bmake:
    #	 Copyright (c) 1988, 1989, 1990, 1993 The Regents of the University of California.  All rights reserved.
    #
    # which(1) is entirely useless.
    #
    # The question is should ident(1) come first but the trouble is I don't
    # actually know what it looks like. Also if the tool in question does not
    # have the appropriate string it won't give us anything useful either.
    # Here's what what looks like on cut(1) which as can be seen is useful:
    #
    #	$ what /usr/bin/cut
    #	/usr/bin/cut:
    #	PROGRAM:cut  PROJECT:text_cmds-154
    #	PROGRAM:cut  PROJECT:text_cmds-154
    #
    # Looking at the Apple website this is indeed the version. Thus because it's
    # not something that will work in all cases instead we will try ident(1) as
    # well even if this succeeds. If either succeeds we will not try strings(1).
    #
    if [[ ! -z "$WHAT" ]]; then
	$WHAT "${COMMAND}"  >/dev/null 2>&1
	status=$?
	if [[ "$status" -eq 0 ]]; then
	    $WHAT "${COMMAND}" | tee -a -- "$LOG_FILE"
	    echo "## OUTPUT OF what $COMMAND ABOVE" | tee -a -- "$LOG_FILE"
	    echo | tee -a -- "$LOG_FILE"
	    EXIT=1
	fi
    fi

    # try ident(1) if available
    #
    # The same or similar caveats for what(1) might apply here too but I have no
    # way to test this.
    #
    if [[ ! -z "$IDENT" ]]; then
	$IDENT "${COMMAND}"  >/dev/null 2>&1
	status=$?
	if [[ "$status" -eq 0 ]]; then
	    $IDENT "${COMMAND}" | tee -a -- "$LOG_FILE"
	    echo "## OUTPUT OF ident $COMMAND ABOVE" | tee -a -- "$LOG_FILE"
	    echo | tee -a -- "$LOG_FILE"
	    EXIT=1
	fi
    fi

    # don't try strings as this will clutter up the bug reports as there are
    # some commands that we simply cannot rely on having versions and we
    # probably don't even need the version of these tools.

    # We also don't report that the version is unknown because this might lead
    # tend to lead to false warnings.
}


# run_check_optional    -   run check but don't report any problems
#
# usage:
#	run_check_optional command
#
#	command	    - check to run
#
run_check_optional() {

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" 1>&2
	exit 3
    fi
    local COMMAND="$1"

    echo "## RUNNING: $COMMAND" | tee -a -- "$LOG_FILE"

    # We have to disable the warning to quote COMMAND: shellcheck is totally
    # wrong about quoting this. If one does quote the below variable COMMAND it
    # will cause the command to fail making sure that the script is worthless.
    # shellcheck disable=SC2086
    command ${COMMAND} 2>&1 | tee -a -- "$LOG_FILE"

    status=${PIPESTATUS[0]}
    if [[ "$status" -ne 0 ]]; then
	echo "$0: OPTIONAL COMMAND $COMMAND FAILED WITH EXIT CODE $status" | tee -a -- "$LOG_FILE"
    else
	echo "## OUTPUT OF ${COMMAND} ABOVE" | tee -a -- "$LOG_FILE"
    fi

    echo | tee -a -- "$LOG_FILE"
    return 0;
}

# run_check_warn    -	don't make it an error if it fails, just warn
#
# usage:
#	run_check_warn command
#
#	command	    - check to run
#
run_check_warn() {

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" 1>&2
	exit 3
    fi
    local COMMAND="$1"
    echo "## RUNNING: $COMMAND" | tee -a -- "$LOG_FILE"

    # We have to disable the warning to quote COMMAND: shellcheck is totally
    # wrong about quoting this. If one does quote the below variable COMMAND it
    # will cause the command to fail making sure that the script is worthless.
    # shellcheck disable=SC2086
    #
    command ${COMMAND} 2>&1 | tee -a -- "$LOG_FILE"
    status=${PIPESTATUS[0]}
    if [[ "$status" -ne 0 ]]; then
	echo "$0: WARNING: $COMMAND FAILED WITH EXIT CODE $status" | tee -a -- "$LOG_FILE"
	WARNING_SUMMARY="$WARNING_SUMMARY
	$COMMAND non-zero exit code: $status"
	echo "### POSSIBLE ISSUE DETECTED: $COMMAND returned $status" | tee -a -- "$LOG_FILE"
    fi

    # report result
    #
    echo "## OUTPUT OF ${COMMAND} ABOVE" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    return 0;
}

# get_shell - get the user shell :-)
#
get_shell() {
    echo "## RUNNING: echo \$SHELL" | tee -a -- "$LOG_FILE"
    echo "Default shell: $SHELL" | tee -a -- "$LOG_FILE"
    echo "## DEFAULT SHELL ABOVE" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
}

# run_check
#
# usage:
#	run_check exit_code command
#
#	exit_code   - new exit code of rule fails
#	command	    - check to run
#
run_check() {

    # parse args
    #
    if [[ $# -ne 2 ]]; then
	echo "$0: ERROR: function expects 2 args, found $#" 1>&2
	exit 3
    fi
    local CODE="$1"
    local COMMAND="$2"
    echo "## RUNNING: $COMMAND" | tee -a -- "$LOG_FILE"

    # We have to disable the warning to quote COMMAND: shellcheck is totally
    # wrong about quoting this. If one does quote the below variable COMMAND it
    # will cause the command to fail making sure that the script is worthless.
    # shellcheck disable=SC2086
    #
    command ${COMMAND} 2>&1 | tee -a -- "$LOG_FILE"

    # The below reference to PIPESTATUS (instead of $?) fixes the bug introduced
    # in commit 8343c4b8cb97e52df64fe8973e68f0d83c6090e1 where the exit status
    # of each command was not checked properly which meant that even if a test
    # failed it would not be reported as an issue which rather defeated the
    # purpose of this script.
    #
    # As amusing as the thought is that there's a bug in a script to help report
    # bugs and issues, this bug was not in fact intentional. :-) I had thought of it
    # earlier on but I neglected to address it before the commit as I moved on with
    # adding the tests. However since the world definitely needs more reasons to
    # laugh I'm keeping this comment here for the sake of humour and the irony that
    # I caused. You're welcome! :-)
    #
    status=${PIPESTATUS[0]}
    if [[ "$status" -ne 0 ]]; then
	EXIT_CODE="$CODE"
	echo "$0: ERROR: $COMMAND FAILED WITH EXIT CODE $status: NEW EXIT_CODE: $EXIT_CODE" | tee -a -- "$LOG_FILE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	$COMMAND non-zero exit code: $status"
	echo "### ISSUE DETECTED: $COMMAND returned $status" | tee -a -- "$LOG_FILE"
    fi
    echo "## OUTPUT OF ${COMMAND} ABOVE" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    return 0;
}

#############################
# Pre-section of the report #
#############################

if [[ $V_FLAG -gt 1 ]]; then
    echo "Will write contents to $LOG_FILE" 1>&2
fi
echo "# TIME OF REPORT: $(date)" | tee -a -- "$LOG_FILE"
echo "# BUG_REPORT_VERSION: $BUG_REPORT_VERSION" | tee -a -- "$LOG_FILE"

#################################################
# Section 0: environment and system information #
#################################################

echo | tee -a -- "$LOG_FILE"
echo "#################################################" | tee -a -- "$LOG_FILE"
echo "# SECTION 0: ENVIRONMENT AND SYSTEM INFORMATION #" | tee -a -- "$LOG_FILE"
echo "#################################################" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# Required tool checks
#
# These tools are required in order to compile tools that are needed
# by someone submitting an entry to the IOCCC.
#
# If your system cannot compile and/or fails in this section, you may
# still consider submitting an IOCCC entry from this system.  However
# you will have to use a more modern and/or more capable system in which
# to compile this tool set OR add missing tools your system.
#
# echo $SHELL: we need to know their default shell
get_shell

# uname -a: get system information
run_check 10 "uname -a"

# sw_vers: if this exists on the system we should use it. This is a BSD/macOS
# thing
SW_VERS=$(which sw_vers)
if [[ -n "$SW_VERS" ]]; then
    run_check 11 "sw_vers"
fi

# which awk: get awk path
run_check 12 "which awk"

# awk --version | head -n 1: get awk version
get_version "awk"

# which basename: get path to basename tool
run_check 13 "which basename"

# try getting basename version
get_version "basename"

# which bash: find the party :-) (okay - path to bash)
run_check 14 "which bash"

# bash --version: get bash version
get_version "bash"

# which cat: find which cat the user owns :-) (okay - path to cat tool)
run_check 15 "which cat"
# try getting version of cat but in a limited way so that it does not block.
# Don't warn if version cannot be found.
get_version_minimal "cat"

# which cmp: get path to cmp
run_check 16 "which cmp"

# try getting version of cmp
get_version "cmp"

# which cp: get path to cp
run_check 17 "which cp"

# try getting version of cp
get_version "cp"

# which cut: get path to cut tool
run_check 18 "which cut"

# try getting cut version
get_version "cut"

# which date: find the bug reporter a date :-) (okay - path to date tool)
run_check 19 "which date"

# try getting version of date
get_version "date"

# which echo: try getting path to echo
run_check 20 "which echo"

# NOTE: we don't try getting version of echo because in some implementations
# (all I've tested in fact) it'll just echo the --version etc. despite the fact
# the man page suggests it should show a version. Well actually doing /bin/echo
# --version does work (though command echo does not for some reason) but not all
# implementations have this and will simply print "--version" etc. which is
# pointless clutter. Now we could try what(1) or ident(1) but it's probably not
# necessary and it would require an even simpler version of get_version_minimal.
# We have draw a line somewhere and this is where it is:
#
#	----------
#
# :-)
#

# which find: what kind of find did the user find ? :-) (or actually just find find :-) )
run_check 21 "which find"

# try getting version of find
get_version "find"

# which getopts: get the path to getopts
#
# NOTE: this might not be necessary but it was suggested anyway. I might be
# wrong but I suspect in shell scripts it would use the built-in but using which
# will give the path under both linux and macOS. The same
run_check 22 "which getopts"

# NOTE: no need to try and get version as it's a built-in under both macOS and
# linux and neither have even a --version option.

# which grep: get path to grep tool
run_check 23 "which grep"

# try getting grep version
get_version "grep"

# which head: determine if the user has a head :-) (on my behalf I hope they do:
# I lost my head years ago and it's a real problem I can tell you! :-) )
run_check 24 "which head"

# try getting version of head
get_version "head"

# which mktemp: get path to mktemp
run_check 25 "which mktemp"

# try getting version of mktemp
get_version "mktemp"

# which mv: find where user will be moving :-) (or actually just path to mv :-))
run_check 26 "which mv"

# try getting version of mv
get_version "mv"

# which printf: get path to printf tool
run_check 27 "which printf"

# try getting printf version
get_version "printf"

# which rm: get path to rm tool
run_check 28 "which rm"

# try getting version of rm
get_version "rm"

# which sed: get sed path
run_check 29 "which sed"

# try getting sed version
get_version "sed"

# which tar: find the path to tar
#
# NOTE: we don't need to check if tar accepts the correct options in this script
# because txzchk_test.sh will do that later on.
run_check 30  "which tar"

# tar --version: find out what version tar is
get_version "tar"

# which tee: get path to tee (I don't mind if I have a cup of tea either :-) )
run_check 31 "which tee"

# try getting version of tee
get_version "tee"

# which touch: get path to touch
run_check 32 "which touch"

# try getting version of touch
get_version "touch"

# which tr: get path to tr
run_check 33 "which tr"

# try getting version of tr
get_version "tr"

# which true: try getting path to true
run_check 34 "which true"

# make sure true is true :-)
run_check 35 "true"

# try getting version of true
get_version "true"

# which yes: get path to yes
run_check 36 "which yes"

# don't try getting version of yes because it will just try printing the args
# over and over again as it is designed to do

echo "#-----------------------------------------------------#" | tee -a -- "$LOG_FILE"
echo "# SECTION 0 ABOVE: ENVIRONMENT AND SYSTEM INFORMATION #" | tee -a -- "$LOG_FILE"
echo "#-----------------------------------------------------#" | tee -a -- "$LOG_FILE"

#############################
# Section 1: optional tools #
#############################

echo | tee -a -- "$LOG_FILE"
echo "#############################" | tee -a -- "$LOG_FILE"
echo "# SECTION 1: OPTIONAL TOOLS #" | tee -a -- "$LOG_FILE"
echo "#############################" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# optional tool checks
#
# If is OK to not have an optional tool in that such optional tools
# are NOT mandatory to compile the core tools that someone needs
# to submit an entry to the IOCCC.
#
# These optional tools are used by the maintainers and/or
# there are workarounds for not having these tools.

# which checknr: determine if checknr is installed
run_check_optional "which checknr"

# try getting version of checknr
get_version_optional "checknr"

# which ctags: get ctags path
run_check_optional "which ctags"

# try getting version of ctags
get_version_optional "ctags"

# which fmt: determine if fmt is installed
run_check_optional "which fmt"

# try getting version of fmt
get_version_optional "fmt"

# which gdate: try getting path to gdate
run_check_optional "which gdate"

# try getting version of gdate
get_version_optional "gdate"

# which install: which install are we using ? :-) (that is find the path to install :-) )
run_check_optional "which install"

# try getting version of install
get_version_optional "install"

# which man: which man are you ? :-) (that is find the path to the man :-) )
run_check_optional "which man"

# try getting version of man (is that the age ? :-) )
get_version_optional "man"

# which man2html: try getting path to man2html
run_check_optional "which man2html"

# don't try getting version of man2html because some implementations will print
# all the help string if an invalid option is given and this is unnecessary
# clutter especially for an optional tool (that we have discovered is actually
# does not seem to even work right).

# which picky: try getting path to picky tool
run_check_optional "which picky"

# don't try getting version of picky as it'll block

# which rpl: get path to rpl
run_check_optional "which rpl"

# try getting version of rpl
get_version_optional "rpl"

# which seqcexit: get path to seqcexit
run_check_optional "which seqcexit"

# try getting version of seqcexit
get_version_optional "seqcexit"

# which shellcheck: get path to shellcheck
run_check_optional "which shellcheck"

# try getting version of shellcheck
get_version_optional "shellcheck"

echo "#---------------------------------#" | tee -a -- "$LOG_FILE"
echo "# SECTION 1 ABOVE: OPTIONAL TOOLS #" | tee -a -- "$LOG_FILE"
echo "#---------------------------------#" | tee -a -- "$LOG_FILE"

############################
# Section 2: C environment #
############################

echo | tee -a -- "$LOG_FILE"
echo "############################" | tee -a -- "$LOG_FILE"
echo "# SECTION 2: C ENVIRONMENT #" | tee -a -- "$LOG_FILE"
echo "############################" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# which cc: get all paths for cc
run_check 37 "which cc"

# cc -v: get compiler version
run_check 38 "cc -v"

# which make: get path to make tool
run_check 39 "which -a make"

# try and get version of make
get_version "make"

# cpp -dM /dev/null: get predefined macros
run_check 40 "cpp -dM /dev/null"

echo "#--------------------------------#" | tee -a -- "$LOG_FILE"
echo "# SECTION 2 ABOVE: C ENVIRONMENT #" | tee -a -- "$LOG_FILE"
echo "#--------------------------------#" | tee -a -- "$LOG_FILE"

#################################
# Section 3: compilation checks #
#################################

echo | tee -a -- "$LOG_FILE"
echo "#################################" | tee -a -- "$LOG_FILE"
echo "# SECTION 3: COMPILATION CHECKS #" | tee -a -- "$LOG_FILE"
echo "#################################" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# make clobber: start clean
run_check 41 "make clobber"

# make all: compile everything before we do anything else
#
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
# use of this repo so each time the script fails we report the issue for that
# very reason.
#
run_check 42 "make all" # the answer to life, the universe and everything conveniently makes all :-)

# make test: run the IOCCC toolkit test suite
run_check 43 "make test"

# hostchk.sh -v 3: we need to run some checks to make sure the system can
# compile things and so on
run_check 44 "./hostchk.sh -v 3"

echo "#-------------------------------------#" | tee -a -- "$LOG_FILE"
echo "# SECTION 3 ABOVE: COMPILATION CHECKS $" | tee -a -- "$LOG_FILE"
echo "#-------------------------------------#" | tee -a -- "$LOG_FILE"

#############################################
# Section 4: JSON scanner and parser checks #
#############################################

echo | tee -a -- "$LOG_FILE"
echo "####################################" | tee -a -- "$LOG_FILE"
echo "# SECTION 4: BISON AND FLEX CHECKS #" | tee -a -- "$LOG_FILE"
echo "####################################" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# pre-clean
rm -f lex.yy.c

# which bison: get path to bison (be careful it doesn't ram you :- ) )
run_check_optional "which bison"

# try getting version of bison
get_version_optional "bison"

# which flex: get path to flex without flexing your system's resources :-)
run_check_optional "which flex"

# try getting version of flex
get_version_optional "flex"

# NOTE: we do want to check that run_bison.sh and run_flex.sh do not exit
# non-zero because if they do it means also backup files could not be used which
# would mean the repo could not be used properly.
#
# run_bison.sh -v 7: check if bison will work
run_check 45 "./run_bison.sh -v 7"

# run_flex.sh -v 7: check if flex will work
run_check 46 "./run_flex.sh -v 7"

# run make all again: run_bison.sh and run_flex.sh will likely cause a need for
# recompilation
echo "## RUNNING make all a second time" | tee -a -- "$LOG_FILE"
run_check 47 "make all"

# post-clean
rm -f lex.yy.c

echo "#----------------------------------------#" | tee -a -- "$LOG_FILE"
echo "# SECTION 4 ABOVE: BISON AND FLEX CHECKS #" | tee -a -- "$LOG_FILE"
echo "#----------------------------------------#" | tee -a -- "$LOG_FILE"

####################################################################
# Section 5: IOCCC environment like version info executable checks #
####################################################################

echo | tee -a -- "$LOG_FILE"
echo "################################" | tee -a -- "$LOG_FILE"
echo "# SECTION 5: IOCCC ENVIRONMENT #" | tee -a -- "$LOG_FILE"
echo "################################" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# See that every tool is executable and run -V on each one that is.
#
# If any tool is not executable the exit code will be set to 48.
#
for f in $TOOLS; do
    echo "## Checking if $f is executable" | tee -a -- "$LOG_FILE"
    if is_exec "$f"; then
	echo "## $f IS EXECUTABLE" | tee -a -- "$LOG_FILE"
	echo "## RUNNING: $f -V" | tee -a -- "$LOG_FILE"
	echo "$f version $($f -V)" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
    else
	EXIT_CODE=48
	echo "$0: ERROR: $f IS NOT EXECUTABLE: new exit code: $EXIT_CODE" | tee -a -- "$LOG_FILE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	$f cannot be executed"
	echo "### ISSUE DETECTED: $f process execution was botched: not even a zombie process was produced" | tee -a -- "$LOG_FILE"
    fi
done

# cat limit_ioccc.sh: this will give us many important variables
#
# NOTE: this file should be generated from make all so we should expect to have
# it and thus if the user does not have there's possibly a problem. However the
# problem isn't really likely to cause a bug on their end (the cause of this
# might indicate a problem but the file itself is not necessary for using the
# repo). Still it has a lot of important variables that we would benefit from
# having.
#
# A reason why this file might be incorrect: missing awk, sed or both. Ideally
# the user would have these tools but if one or both are not available this file
# will not be as useful. This is why we don't mark it as an error.
#
echo "## Checking limit_ioccc.sh" | tee -a -- "$LOG_FILE"
if [[ -e "./limit_ioccc.sh" ]]; then
    if [[ -r "./limit_ioccc.sh" ]]; then
	echo "## NOTICE: Found limit_ioccc.sh file:" | tee -a -- "$LOG_FILE"
	echo "--" | tee -a -- "$LOG_FILE"
	echo "cat ./limit_ioccc.sh" | tee -a -- "$LOG_FILE"
	# tee -a -- "$LOG_FILE" < ./limit_ioccc.sh
	< limit_ioccc.sh tee -a -- "$LOG_FILE"
	echo "--" | tee -a -- "$LOG_FILE"
    else
	echo "### NOTICE: Found unreadable limit_ioccc.sh" | tee -a -- "$LOG_FILE"
    fi
else
    echo "### No limit_ioccc.sh file found" | tee -a -- "$LOG_FILE"
fi
echo | tee -a -- "$LOG_FILE"

echo "#------------------------------------#" | tee -a -- "$LOG_FILE"
echo "# SECTION 5 ABOVE: IOCCC ENVIRONMENT #" | tee -a -- "$LOG_FILE"
echo "#------------------------------------#" | tee -a -- "$LOG_FILE"

############################################################
# Section 6: check for any user modifications to the tools #
############################################################

echo | tee -a -- "$LOG_FILE"
echo "################################################" | tee -a -- "$LOG_FILE"
echo "# SECTION 6: USER MODIFICATIONS TO IOCCC TOOLS #" | tee -a -- "$LOG_FILE"
echo "################################################" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# check for makefile.local to see if user is overriding any rules.
#
# NOTE: we don't use run_check for this because it's not an actual error whether
# or not the user has a makefile.local file. What matters is the contents of it
# if they do have one.
#
echo "## CHECKING: if makefile.local exists" | tee -a -- "$LOG_FILE"
if [[ -e "./makefile.local" ]]; then
    if [[ -r "./makefile.local" ]]; then
	echo "### WARNING: Found Makefile overriding file makefile.local:" | tee -a -- "$LOG_FILE"
	echo "cat ./makefile.local" | tee -a -- "$LOG_FILE"
	echo "--" | tee -a -- "$LOG_FILE"
	# tee -a -- "$LOG_FILE" < makefile.local
	< makefile.local tee -a -- "$LOG_FILE"
	echo "--" | tee -a -- "$LOG_FILE"
    else
	echo "### NOTICE: Found unreadable makefile.local" | tee -a -- "$LOG_FILE"
    fi
else
    echo "### NOTICE: Makefile has no overriding makefile.local" | tee -a -- "$LOG_FILE"
fi
echo | tee -a -- "$LOG_FILE"

# check if there are any local modifications to the code
#
# NOTE: We don't use run_check for this because if git does not exist for some
# reason then the shell might exit with an error code even though we're not
# after that (as in it might not be an error). Additionally whether there are
# differences or not git will return 0. It will return non-zero in the case that
# it's not in a git repo but we don't explicitly check for this. All we care
# about is whether or not the user has changes that might be causing a problem.

echo "## RUNNING: git status on the code" | tee -a -- "$LOG_FILE"
git --no-pager status | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
echo "## git status ABOVE" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

echo "## RUNNING: git diff to check for local modifications to the code" | tee -a -- "$LOG_FILE"
git --no-pager diff | tee -a -- "$LOG_FILE"
echo "## git diff ABOVE" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

echo "## RUNNING: git diff --staged to check for local modifications to the code" | tee -a -- "$LOG_FILE"
git --no-pager diff --staged | tee -a -- "$LOG_FILE"
echo "## git diff --staged ABOVE" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

echo "#----------------------------------------------------#" | tee -a -- "$LOG_FILE"
echo "# SECTION 6 ABOVE: USER MODIFICATIONS TO IOCCC TOOLS #" | tee -a -- "$LOG_FILE"
echo "#----------------------------------------------------#" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

################
# final report #
################

if [[ ! -z "$WARNING_SUMMARY" ]]; then
    echo 1>&2
    echo "One or more POSSIBLE issues detected:" | tee -a -- "$LOG_FILE"
    echo  | tee -a -- "$LOG_FILE"
    echo "$WARNING_SUMMARY" | tee -a -- "$LOG_FILE"
    echo  | tee -a -- "$LOG_FILE"
fi
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
    echo "instead email the Judges but you're encouraged to file a" | tee -a -- "$LOG_FILE"
    echo "report instead. This is because not all tools were written by" | tee -a -- "$LOG_FILE"
    echo "the Judges." | tee -a -- "$LOG_FILE"
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
    echo "instead email the Judges but you're encouraged to file a" | tee -a -- "$LOG_FILE"
    echo "report instead. This is because not all tools were written by" | tee -a -- "$LOG_FILE"
    echo "the Judges." | tee -a -- "$LOG_FILE"
fi
echo 1>&2
echo "NOTE: $LOG_FILE contains various information about" 1>&2
echo "your environment including things such as hostname, login name, operating system" 1>&2
echo "information, paths and versions of various tools. Although not encouraged," 1>&2
echo "you are free to edit this file if you feel so inclined. This information is" 1>&2
echo "added to the file in case it proves useful in debugging a problem, and therefore" 1>&2
echo "we kindly request that you provide it to us when you report a problem with this" 1>&2
echo "code." 1>&2

# All Done!!! -- Jessica Noll, Age 2
#
if [[ "$V_FLAG" -gt 1 ]]; then
    echo "About to exit with exit code: $EXIT_CODE" 1>&2
fi
exit "$EXIT_CODE"
