#!/usr/bin/env bash
#
# bug_report.sh - produce a file suitable for filing a bug report
#
# Collect system information to help user report bugs and issues
# using the mkiocccentry tools.
#
# When you run this script without any arguments:
#
#	./bug-report.sh
#
# After printing logs of stuff on the terminal, a file of the form:
#
#	bug-report.YYYYMMDD.HHMMSS.txt
#
# will be created in the current directory, where YYYYMMDD.HHMMSS is the
# date and time in your default local time zone.
#
# If you are reporting a bug, the bug-report.YYYYMMDD.HHMMSS.txt file should
# be uploaded as part of your bug report.  Report / create a GitHub issue
# by going to:
#
#	https://github.com/ioccc-src/mkiocccentry/issues
#
# Please upload the bug-report.YYYYMMDD.HHMMSS.txt file as part of your report.
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
# Do NOT put this tool (bug_report.sh) in the list, it will
# cause an infinite loop.
#
export TOOLS="
    ./chkentry
    ./dbg/dbg_test
    ./dyn_array/dyn_test
    ./iocccsize
    ./jparse/test_jparse/jnum_gen
    ./jparse/jparse
    ./jparse/jsemcgen.sh
    ./jparse/jsemtblgen
    ./jparse/jstrdecode
    ./jparse/jstrencode
    ./jparse/run_bison.sh
    ./jparse/run_flex.sh
    ./jparse/test_jparse/jnum_chk
    ./jparse/test_jparse/jparse_test.sh
    ./jparse/test_jparse/jstr_test.sh
    ./jparse/verge
    ./mkiocccentry
    ./soup/all_sem_ref.sh
    ./soup/reset_tstamp.sh
    ./soup/run_usage.sh
    ./soup/vermod.sh
    ./test_ioccc/chkentry_test.sh
    ./test_ioccc/fnamchk
    ./test_ioccc/hostchk.sh
    ./test_ioccc/ioccc_test.sh
    ./test_ioccc/iocccsize_test.sh
    ./test_ioccc/mkiocccentry_test.sh
    ./test_ioccc/prep.sh
    ./test_ioccc/txzchk_test.sh
    ./test_ioccc/utf8_test
    ./txzchk
    "

# we need this to find overriding Makefile.local in all directories to see if
# the user is overriding any Makefile. As well, we check if the directory even
# is searchable and has a Makefile.
export SUBDIRS="
    ./dbg
    ./dyn_array
    ./jparse
    ./jparse/test_jparse
    ./soup
    ./test_ioccc
    "

# we need to determine if the system has gmake first
MAKE="$(type -P gmake)"
if [[ -z "$MAKE" ]]; then
	MAKE="$(type -P make)"
fi
export MAKE
export BUG_REPORT_VERSION="1.0.7 2025-01-18"
export FAILURE_SUMMARY=
export NOTICE_SUMMARY=
export DBG_LEVEL="0"
export V_FLAG="0"
export T_FLAG=""
export X_FLAG=""
export L_FLAG=""
export EXIT_CODE=0
export MAKE_FLAGS="V=@ S=@ Q= E=@ I= Q_V_OPTION=1 INSTALL_V= MAKE_CD_Q="
export USAGE="usage: $0 [-h] [-V] [-v level] [-D level] [-t] [-x] [-l] [-L logfile] [-m make] [-M make_flags]

    -h              print help and exit
    -V              print version and exit
    -v level        set verbosity level for this script: (def level: $V_FLAG)
    -D level        set verbosity level for tests (def: $DBG_LEVEL)
    -t              disable make actions (def: run make actions)
    -x              remove bug report if no problems detected
    -l              only write to log file
    -L logfile      specify logfile name (def: based on date and time)
    -m make         specify path to make(1) (def: $MAKE)
    -M make_flags   set any make flags (def: $MAKE_FLAGS)

Exit codes:
     0	    all tests OK
     1	    failed to create a bug report file
     2	    help mode exit or print version mode exit
     3	    invalid command line
     4	    error in function call
 >= 10	    at least one check failed

bug_report.sh version: $BUG_REPORT_VERSION"

# Determine the name of the log file
#
# NOTE: log file does not have an underscore in the name because we want to
# distinguish it from this script which does have an underscore in it.
#
LOGFILE="bug-report.$(date +%Y%m%d.%H%M%S).txt"

# parse args
#
export V_FLAG="0"
while getopts :hVv:D:txlL:m:M: flag; do
    case "$flag" in
        h)  echo "$USAGE" 1>&2
            exit 2
            ;;
        V)  echo "$BUG_REPORT_VERSION" 1>&2
            exit 2
            ;;
        v)  V_FLAG="$OPTARG";
            ;;
        D)  DBG_LEVEL="$OPTARG";
            ;;
        t)  T_FLAG="-t"
            ;;
        x)  X_FLAG="-x"
            ;;
        l)  L_FLAG="-l"
            ;;
        L)  LOGFILE="$OPTARG"
            ;;
        m)  MAKE="$OPTARG"
            ;;
        M)  MAKE_FLAGS="$OPTARG"
            ;;
        \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
            echo 1>&2
            echo "$USAGE" 1>&2
            exit 3
                    ;;
        :)  echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
            echo 1>&2
            echo "$USAGE" 1>&2
            exit 3
            ;;
        *)
            ;;
    esac
done

# make sure that make is an executable file
#
# First, if the user for some reason provided an empty string, try an correct it
# for them:
if [[ -z "$MAKE" ]]; then
	# try for gmake first
	MAKE="$(type -P gmake)"
	if [[ -z "$MAKE" ]]; then
		MAKE="$(type -P make)"
	fi
fi
if [[ -z "$MAKE" ]]; then
	echo "make variable MAKE unset! Try -m make option." 1>&2
	exit 3
fi

if [[ ! -e "$MAKE" ]]; then
	echo "make does not exist: $MAKE" 1>&2
	echo "Use -m make option to set path." 1>&2
	exit 3
fi
if [[ ! -f "$MAKE" ]]; then
	echo "make is not a regular file: $MAKE" 1>&2
	echo "Use -m make option to set path." 1>&2
	exit 3
fi
if [[ ! -x "$MAKE" ]]; then
	echo "make is not an executable file: $MAKE" 1>&2
	echo "Use -m make option to set path." 1>&2
	exit 3
fi


# the LOGFILE name will have been either generated or specified by the -L option
# but now we have to export it and create it.
export LOGFILE
# attempt to create a writable log file
#
rm -f "$LOGFILE"
touch "$LOGFILE"
if [[ ! -e "$LOGFILE" ]]; then
    echo "$0: ERROR: could not create log file: $LOGFILE"
    exit 1
fi
if [[ ! -w "$LOGFILE" ]]; then
    echo "$0: ERROR: log file not writable: $LOGFILE"
    exit 1
fi

# write_echo - write a message to either the log file or both the log file and
# stdout
#
write_echo()
{
    local MSG="$*"

    if [[ -z "$L_FLAG" ]]; then
	echo "$MSG" | tee -a -- "$LOGFILE"
    else
	echo "$MSG" >> "$LOGFILE"
    fi
}
# exec_command - invoke command redirecting output only to the log file or to
# both stdout and the log file
exec_command()
{
    local COMMAND=$*
    if [[ -z "$L_FLAG" ]]; then
	# SC2086 (info): Double quote to prevent globbing and word splitting.
	# https://www.shellcheck.net/wiki/SC2086
	# shellcheck disable=SC2086
	command ${COMMAND} 2>&1 | tee -a -- "$LOGFILE"
	return "${PIPESTATUS[0]}"
    else
	# SC2086 (info): Double quote to prevent globbing and word splitting.
	# https://www.shellcheck.net/wiki/SC2086
	# shellcheck disable=SC2086
	command ${COMMAND} >> "$LOGFILE" 2>&1
	return $?
    fi

}
# exec_command_lines - invoke command redirecting output only to the log file or
# to both stdout and the log file but filter through head to show only N lines.
exec_command_lines()
{
    local LINES="$1"
    local COMMAND="${*:2}"
    if [[ -z "$L_FLAG" ]]; then
	# SC2086 (info): Double quote to prevent globbing and word splitting.
	# https://www.shellcheck.net/wiki/SC2086
	# shellcheck disable=SC2086
	command ${COMMAND} 2>&1 | head -n "$LINES" | tee -a -- "$LOGFILE"
	return "${PIPESTATUS[0]}"
    else
	# SC2086 (info): Double quote to prevent globbing and word splitting.
	# https://www.shellcheck.net/wiki/SC2053
	# shellcheck disable=SC2086
	command ${COMMAND} | head -n "$LINES" >> "$LOGFILE" 2>&1
	return "${PIPESTATUS[0]}"
    fi
}

# is_exec   - determine if arg exists, is a regular file and is executable
#
# NOTE: don't exit if the file is not an executable
is_exec()
{
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: expected 1 arg to is_exec, found $#" | tee -a -- "$LOGFILE"
	exit 4
    else
	declare f="$1"
	if [[ ! -e "$f" ]]; then
	    write_echo "$0: ERROR: $1 does not exist"
	    return 1
	fi
	if [[ ! -f "$f" ]]; then
	    write_echo "$0: ERROR: $1 is not a regular file"
	    return 1
	fi
	if [[ ! -x "$f" ]]; then
	    write_echo "$0: ERROR: $1 is not executable"
	    return 1
	fi
	return 0
    fi
}

# is_exec_quiet   - determine if arg exists, is a regular file and is executable
#
# NOTE: don't exit if the file is not an executable
is_exec_quiet()
{
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: expected 1 arg to is_exec_quiet, found $#" | tee -a -- "$LOGFILE"
	exit 4
    else
	declare f="$1"
	if [[ ! -e "$f" ]]; then
	    return 1
	fi
	if [[ ! -f "$f" ]]; then
	    return 1
	fi
	if [[ ! -x "$f" ]]; then
	    return 1
	fi
	return 0
    fi
}

# is_read_exec_dir   - determine if arg exists, is a directory and is searchable
#
# NOTE: don't exit if the directory is not searchable
is_read_exec_dir()
{
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: expected 1 arg to is_read_exec_dir, found $#" | tee -a -- "$LOGFILE"
	exit 4
    else
	declare f="$1"
	if [[ ! -e "$f" ]]; then
	    write_echo "$0: ERROR: $1 does not exist"
	    return 1
	fi
	if [[ ! -d "$f" ]]; then
	    write_echo "$0: ERROR: $1 is not a directory"
	    return 1
	fi
	if [[ ! -r "$f" ]]; then
	    write_echo "$0: ERROR: $1 is not readable"
	    return 1
	fi
	if [[ ! -x "$f" ]]; then
	    write_echo "$0: ERROR: $1 is not searchable"
	    return 1
	fi
	return 0
    fi
}


# type_of - determine if a name is an alias, a path or a built-in
#
# NOTE: an alias is highly unlikely to be found in a script but if it something
# is aliased by some chance we'll hopefully know that from this function.
type_of()
{
    # parse args
    #
    if [[ $# -ne 2 ]]; then
	echo "$0: ERROR: function expects 2 args, found $#" | tee -a -- "$LOGFILE"
	exit 4
    fi

    local CODE="$1"
    local COMMAND=$2

    write_echo
    write_echo "## CHECKING TYPE OF: \"$COMMAND\""
    TYPE_OF="$(type -a "$COMMAND" 2>/dev/null)"
    status=$?
    if [[ -n "$TYPE_OF" ]]; then
	write_echo "$TYPE_OF"
    elif [[ "$status" -ne 0 ]]; then
	EXIT_CODE="$CODE"
	write_echo "$0: ERROR: \"type -a $COMMAND\" FAILED WITH EXIT CODE $status: NEW EXIT_CODE: $EXIT_CODE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	$COMMAND non-zero exit code: $status"
	write_echo "### ISSUE DETECTED: \"$COMMAND\" returned $status"
    fi
    write_echo "## TYPE OF \"$COMMAND\" ABOVE"
}

# type_of_optional - determine if a name is an alias, a path or a built-in
#
# NOTE: an alias is highly unlikely to be found in a script but if it something
# is aliased by some chance we'll hopefully know that from this function.
type_of_optional()
{
    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 2 args, found $#" | tee -a -- "$LOGFILE"
	exit 4
    fi

    local COMMAND=$1

    write_echo
    write_echo "## CHECKING TYPE OF: \"$COMMAND\""
    TYPE_OF="$(type -a "$COMMAND" 2>/dev/null)"
    status=$?

    if [[ -n "$TYPE_OF" ]]; then
	write_echo "$TYPE_OF"
    elif [[ "$status" -ne 0 ]]; then
	write_echo "$0: OPTIONAL COMMAND \"$COMMAND\" NOT FOUND: type -a returned $status"
    fi
    write_echo "## TYPE OF \"$COMMAND\" ABOVE"
}


# get path to tools we might need for get_version and get_version_optional
# functions below
#
WHAT="$(type -P what)"
IDENT="$(type -P ident)"
STRINGS="$(type -P strings)" # this should always exist but we check anyway

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
get_version_optional()
{

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" | tee -a -- "$LOGFILE"
	exit 4
    fi
    local COMMAND
    local EXIT=0
    local IS_EXEC=1
    COMMAND="$(type -P "$1")"
    if ! is_exec_quiet "$COMMAND"; then
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
	IS_EXEC=0
    fi

    write_echo "## VERSION CHECK FOR: \"$1\""

    # try --version
    #
    command "${COMMAND}" --version >/dev/null 2>&1 </dev/null
    status=$?
    if [[ "$status" -eq 0 ]]; then
	exec_command "${COMMAND}" --version </dev/null
	write_echo "## \"$COMMAND --version\" ABOVE"
	write_echo ""
	return
    fi

    # try -v
    #
    command "${COMMAND}" -v >/dev/null 2>&1 </dev/null
    status=$?
    if [[ "$status" -eq 0 ]]; then
	exec_command "${COMMAND}" -v </dev/null
	write_echo "## \"$COMMAND -v\" ABOVE"
	write_echo ""
	return
    fi

    # try -V
    #
    command "${COMMAND}" -V >/dev/null 2>&1 </dev/null
    status=$?
    if [[ "$status" -eq 0 ]]; then
	exec_command "${COMMAND}" -V </dev/null
	write_echo "## \"$COMMAND -V\" ABOVE"
	write_echo ""
	return
    fi

    if [[ "$IS_EXEC" -eq 1 ]]; then
	# try what(1) if available
	#
	# An important note is that what(1) might not get the correct information.
	# For instance running it on bmake(1) I see:
	#
	#   $ what ./bmake
	#   ./bmake:
	#   Copyright (c) 1988, 1989, 1990, 1993 The Regents of the University of California.  All rights reserved.
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
	# Looking at the Apple website this is indeed the version (at least at
        # the time this was written). Thus because it's not something that will
        # work in all cases instead we will try ident(1) as well even if what(1)
        # succeeds. If either succeeds we will not try strings(1).
	#
	if [[ -n "$WHAT" ]]; then
	    $WHAT "${COMMAND}"  >/dev/null 2>&1 </dev/null
	    status=$?
	    if [[ "$status" -eq 0 ]]; then
		exec_command "$WHAT" "${COMMAND}" </dev/null
		write_echo "## OUTPUT OF \"what $COMMAND\" ABOVE"
		write_echo ""
		EXIT=1
	    fi
	fi

	# try ident(1) if available
	#
	# The same or similar caveats for what(1) might apply here too but I have no
	# way to test this.
	#
	if [[ -n "$IDENT" ]]; then
	    $IDENT "${COMMAND}"  >/dev/null 2>&1 </dev/null
	    status=$?
	    if [[ "$status" -eq 0 ]]; then
		exec_command "$IDENT" "${COMMAND}" </dev/null
		write_echo "## OUTPUT OF \"ident $COMMAND\" ABOVE"
		write_echo ""
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
	if [[ -n "$STRINGS" ]]; then
	    write_echo "$0: unknown version for \"$COMMAND\": trying \"strings\""
	    NOTICE_SUMMARY="$NOTICE_SUMMARY
	    Notice: unknown version for \"$COMMAND\": trying \"strings\""
	    $STRINGS "${COMMAND}" | head -n 15 >/dev/null 2>&1
	    status=${PIPESTATUS[0]}
	    if [[ "$status" -eq 0 ]]; then
		exec_command_lines 15 "$STRINGS" "${COMMAND}"
		write_echo "## OUTPUT OF \"strings $COMMAND\" ABOVE"
		write_echo ""
		return
	    fi
	fi

	# report unknown version
	#
	write_echo "Notice: unknown version for optional command: \"$COMMAND\""
	NOTICE_SUMMARY="$NOTICE_SUMMARY
	$0: Notice: unknown version for optional command: \"$COMMAND\""
	write_echo ""

    # mention that an optional command is not found or not executable
    #
    else
	write_echo "Optional command is not found or not executable: \"$COMMAND\""
	write_echo ""
    fi
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
get_version()
{

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" | tee -a -- "$LOGFILE"
	exit 4
    fi
    local COMMAND
    local EXIT=0
    local IS_EXEC=1
    COMMAND="$(type -P "$1")"
    if ! is_exec_quiet "$COMMAND"; then
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
	IS_EXEC=0
    fi
    write_echo "## VERSION CHECK FOR: \"$1\""

    # First check if this is bash. If it is we can do something special for
    # version information.
    if [[ "$(basename "${COMMAND}")" = "bash" ]]; then
	write_echo "## RUNNING: \"echo \$BASH_VERSION\""
	exec_command "echo $BASH_VERSION"
	write_echo "## \"\$BASH_VERSION\" ABOVE"
	# get bash MAJOR version
	write_echo "## RUNNING: \"echo \${BASH_VERSINFO[0]}\""
	exec_command "echo ${BASH_VERSINFO[0]}"
        write_echo "## BASH \"\${BASH_VERSINFO[0]}\" (MAJOR VERSION) ABOVE"
	# get bash MINOR version
	write_echo "## RUNNING: \"echo \${BASH_VERSINFO[1]}\""
	exec_command "echo ${BASH_VERSINFO[1]}"
        write_echo "## BASH MINOR \"\${BASH_VERSINFO[1]}\" (MINOR VERSION) ABOVE"
	# get bash PATCH LEVEL
	write_echo "## RUNNING: \"echo \${BASH_VERSINFO[2]}\""
	exec_command "echo ${BASH_VERSINFO[2]}"
        write_echo "## BASH \"\${BASH_VERSINFO[2]}\" (PATCH LEVEL) ABOVE"
	# get bash BUILD version
	write_echo "## RUNNING: \"echo \${BASH_VERSINFO[3]}\""
	exec_command "echo ${BASH_VERSINFO[3]}"
        write_echo "## BASH \"\${BASH_VERSINFO[3]}\" (BUILD VERSION) ABOVE"
	# get bash RELEASE STATUS (e.g. beta)
	write_echo "## RUNNING: \"echo \${BASH_VERSINFO[4]}\""
	exec_command "echo ${BASH_VERSINFO[4]}"
        write_echo "## BASH \"\${BASH_VERSINFO[4]}\" (RELEASE) STATUS ABOVE"
	# get MACHTYPE
	#
	# We use $MACHTYPE instead of ${BASH_VERSINFO[5]}. Why? Just in case
	# some versions of bash do not have it and since it's meant to be the
	# same value the name here can serve as additional documentation. Of
	# course we could get that elsewhere but this is a bash variable so it
	# seems fitting that it is put here.
	write_echo "## RUNNING: \"echo \$MACHTYPE\""
	exec_command "echo $MACHTYPE"
        write_echo "## \"\$MACHTYPE\" (BASH SYSTEM TYPE) ABOVE"

	# Don't return from this function even after all this because trying
	# --version on bash might prove useful in some way even though we should
	# have got the information above.
    fi



    # try --version
    #
    command "${COMMAND}" --version >/dev/null 2>&1 </dev/null
    status=$?
    if [[ "$status" -eq 0 ]]; then
	exec_command "${COMMAND}" --version </dev/null
	write_echo "## OUTPUT OF \"$COMMAND --version\" ABOVE"
	write_echo ""
	return
    fi

    # try -v
    #
    command "${COMMAND}" -v >/dev/null 2>&1 </dev/null
    status=$?
    if [[ "$status" -eq 0 ]]; then
	exec_command "${COMMAND}" -v </dev/null
	write_echo "## OUTPUT OF \"$COMMAND -v\" ABOVE"
	write_echo ""
	return
    fi

    # try -V
    #
    command "${COMMAND}" -V >/dev/null 2>&1 </dev/null
    status=$?
    if [[ "$status" -eq 0 ]]; then
	exec_command "${COMMAND}" -V </dev/null
	write_echo "## OUTPUT OF \"$COMMAND -V\" ABOVE"
	write_echo ""
	return
    fi

    if [[ "$IS_EXEC" -eq 1 ]]; then
	# try what(1) if available
	#
	# An important note is that what(1) might not get the correct information.
	# For instance running it on bmake(1) I see:
	#
	#   $ what ./bmake
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
	# Looking at the Apple website this is indeed the version (at least at
        # the time of writing this). Thus because it's not something that will
        # work in all cases instead we will try ident(1) as well even if this
        # succeeds. If either succeeds we will not try strings(1).
	#
	if [[ -n "$WHAT" ]]; then
	    $WHAT "${COMMAND}"  >/dev/null 2>&1 </dev/null
	    status=$?
	    if [[ "$status" -eq 0 ]]; then
		exec_command "$WHAT" "${COMMAND}" </dev/null
		write_echo "## OUTPUT OF \"what $COMMAND\" ABOVE"
		write_echo ""
		EXIT=1
	    fi
	fi

	# try ident(1) if available
	#
	# The same or similar caveats for what(1) might apply here too but I have no
	# way to test this.
	#
	if [[ -n "$IDENT" ]]; then
	    $IDENT "${COMMAND}"  >/dev/null 2>&1 </dev/null
	    status=$?
	    if [[ "$status" -eq 0 ]]; then
		exec_command "$IDENT" "${COMMAND}" </dev/null
		write_echo "## OUTPUT OF \"ident $COMMAND\" ABOVE"
		write_echo ""
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
	if [[ -n "$STRINGS" ]]; then
	    write_echo "Notice: unknown version for \"$COMMAND\": trying \"strings\""
	    NOTICE_SUMMARY="$NOTICE_SUMMARY
	    Notice: unknown version for \"$COMMAND\": trying \"strings\""
	    $STRINGS "${COMMAND}" | head -n 15 >/dev/null 2>&1
	    status=${PIPESTATUS[0]}
	    if [[ "$status" -eq 0 ]]; then
		exec_command_lines 15 "$STRINGS" "${COMMAND}"
		write_echo "## OUTPUT OF \"strings $COMMAND\" ABOVE"
		write_echo ""
		return
	    fi
	fi

	# report unknown version
	#
	write_echo "Notice: unknown version for required command: \"$COMMAND\""
	NOTICE_SUMMARY="$NOTICE_SUMMARY
	$0: Notice: unknown version for required command: \"$COMMAND\""
	write_echo ""

    # notice that a required command is not found or not executable
    #
    else
	write_echo "Notice: required command is not found or is not executable: \"$COMMAND\""
	NOTICE_SUMMARY="$NOTICE_SUMMARY
	Notice: required command is not found or is not executable: \"$COMMAND\""
	write_echo ""
    fi
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
get_version_minimal()
{
    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" | tee -a -- "$LOGFILE"
	exit 4
    fi
    local COMMAND
    local EXIT=0
    local IS_EXEC=1
    COMMAND="$(type -P "$1")"
    if ! is_exec_quiet "$COMMAND"; then
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
	IS_EXEC=0
    fi
    write_echo "## VERSION CHECK FOR: \"$1\""

    # try --version
    #
    command "${COMMAND}" --version >/dev/null 2>&1 </dev/null
    status=$?
    if [[ "$status" -eq 0 ]]; then
	exec_command "${COMMAND}" --version </dev/null
	write_echo "## OUTPUT OF \"$COMMAND --version\" ABOVE"
	write_echo ""
	return
    fi

    if [[ "$IS_EXEC" -eq 1 ]]; then
	# try what(1) if available
	#
	# An important note is that what(1) might not get the correct information.
	# For instance running it on bmake(1) I see:
	#
	#   $ what ./bmake
	#   ./bmake:
	#   Copyright (c) 1988, 1989, 1990, 1993 The Regents of the University of California.  All rights reserved.
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
	# Looking at the Apple website this is indeed the version (at least at
        # the time of writing this). Thus because it's not something that will
        # work in all cases instead we will try ident(1) as well even if this
        # succeeds.
	#
	if [[ -n "$WHAT" ]]; then
	    $WHAT "${COMMAND}" >/dev/null 2>&1 </dev/null
	    status=$?
	    if [[ "$status" -eq 0 ]]; then
		exec_command "$WHAT" "${COMMAND}" </dev/null
		write_echo "## OUTPUT OF \"what $COMMAND\" ABOVE"
		write_echo ""
		EXIT=1
	    fi
	fi

	# try ident(1) if available
	#
	# The same or similar caveats for what(1) might apply here too but I have no
	# way to test this.
	#
	if [[ -n "$IDENT" ]]; then
	    $IDENT "${COMMAND}"  >/dev/null 2>&1 </dev/null
	    status=$?
	    if [[ "$status" -eq 0 ]]; then
		exec_command "$IDENT" "${COMMAND}" </dev/null
		write_echo "## OUTPUT OF \"ident $COMMAND\" ABOVE"
		write_echo ""
		EXIT=1
	    fi
	fi

	# don't try strings as this will clutter up the bug reports as there are
	# some commands that we simply cannot rely on having versions and we
	# probably don't even need the version of those tools.

	# We also don't report that the version is unknown because this might
	# lead to false warnings.
    fi
}


# run_optional_check    -   run check but don't report any problems
#
# usage:
#	run_optional_check command
#
#	command	    - check to run
#
run_optional_check()
{
    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: function expects 1 arg, found $#" | tee -a -- "$LOGFILE"
	exit 4
    fi
    local COMMAND=$1

    write_echo "## RUNNING: \"$COMMAND\""

    # We have to disable the warning to quote COMMAND: shellcheck is totally
    # wrong about quoting this. If one does quote the below variable COMMAND it
    # will cause the command to fail making sure that the script is worthless.
    #
    # SC2086 (info): Double quote to prevent globbing and word splitting.
    # https://www.shellcheck.net/wiki/SC2086
    # shellcheck disable=SC2086
    exec_command ${COMMAND} </dev/null
    status=$?
    if [[ "$status" -ne 0 ]]; then
	write_echo "$0: OPTIONAL COMMAND \"$COMMAND\" FAILED WITH EXIT CODE $status"
    else
	write_echo "## OUTPUT OF \"${COMMAND}\" ABOVE"
    fi

    write_echo ""
    return 0;
}

# get_shell - get the user shell :-)
#
get_shell()
{
    write_echo "## RUNNING: \"echo \$SHELL\""
    write_echo "$SHELL"
    write_echo "## USER SHELL ABOVE"
    write_echo ""
}

# get_path - get the user path :-)
#
get_path()
{
    write_echo "## RUNNING: \"echo \$PATH\""
    write_echo "$PATH"
    write_echo "## USER PATH ABOVE"
    write_echo ""
}

# run_check
#
# usage:
#	run_check exit_code command
#
#	exit_code   - new exit code of rule fails
#	command	    - check to run
#
run_check()
{
    # parse args
    #
    if [[ $# -ne 2 ]]; then
	echo "$0: ERROR: function expects 2 args, found $#" | tee -a -- "$LOGFILE"
	exit 4
    fi
    local CODE="$1"
    local COMMAND=$2
    write_echo "## RUNNING: \"$COMMAND\""

    exec_command "${COMMAND}"

    # Once upon a time there was a bug in this script in this function.
    # PIPESTATUS was needed to fix a bug introduced in commit
    # 8343c4b8cb97e52df64fe8973e68f0d83c6090e1 where the exit status of each
    # command was not checked properly which meant that even if a test failed it
    # would not be reported as an issue which rather defeated the purpose of
    # this script.
    #
    # As amusing as the thought is that there's a bug in a script to help report
    # bugs and issues, this bug was not in fact intentional. :-) I had thought of it
    # earlier on but I neglected to address it before the commit as I moved on with
    # adding the tests. However since the world definitely needs more reasons to
    # laugh I'm keeping this comment here for the sake of humour and the irony that
    # I caused. You're welcome! :-)
    #
    # But this function now actually does need to use '$?' so it makes this less
    # fun. :-(
    #
    status=$?
    if [[ "$status" -ne 0 ]]; then
	EXIT_CODE="$CODE"
	write_echo "$0: ERROR: \"$COMMAND\" FAILED WITH EXIT CODE $status: NEW EXIT_CODE: $EXIT_CODE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	\"$COMMAND\" non-zero exit code: $status"
	write_echo "### ISSUE DETECTED: \"$COMMAND\" returned $status"
    fi
    write_echo "## OUTPUT OF \"${COMMAND}\" ABOVE"
    write_echo ""
    return 0;
}

#############################
# Pre-section of the report #
#############################

if [[ $V_FLAG -gt 1 ]]; then
    write_echo "Will write report to $LOGFILE"
fi
write_echo "# TIME OF REPORT: \"$(date)\""
write_echo "# BUG_REPORT_VERSION: \"$BUG_REPORT_VERSION\""

#################################################
# Section 0: environment and system information #
#################################################

write_echo ""
write_echo "#################################################"
write_echo "# SECTION 0: ENVIRONMENT AND SYSTEM INFORMATION #"
write_echo "#################################################"
write_echo ""

# Required tool checks
#
# These tools are required in order to compile tools that are needed
# by someone submitting an submission to the IOCCC.
#
# If your system cannot compile and/or fails in this section, you may
# still consider submitting an IOCCC submission from this system.  However
# you will have to use a more modern and/or more capable system in which
# to compile this tool set OR add missing tools your system.
#
# echo $SHELL: we need to know their default shell
get_shell

# echo $PATH: we need to know their default path
get_path

# shopt -s: get shell options
run_optional_check "shopt -s"

# uname -a: get system information
run_check 10 "uname -a"

# sw_vers: if this exists on the system we should use it. This is a BSD/macOS
# thing
SW_VERS=$(type -P sw_vers)
if [[ -n "$SW_VERS" ]]; then
    run_check 11 "sw_vers"
fi

# type -a awk: get all types of awk
type_of 12 "awk"

# awk --version | head -n 1: get awk version
get_version "awk"

# type -a basename: get all types of basename
type_of 13 "basename"

# try getting basename version
get_version "basename"

# type -a bash: find which party to bash (okay - all types of bash)
type_of 14 "bash"

# bash --version: get bash version
get_version "bash"

# type -a cat: find which cat the user owns :-) (okay - types of cats)
#
#   'Don't hurt us! Don't let them hurt us, precious! They won't hurt us will
#   they, nice little hobbitses? We didn't mean no harm, but they jumps on us
#   like cats on poor mices, they did, precious. And we're so lonely, *gollum*.
#   We'll be nice to them, very nice, if they'll be nice to us, won't we, yes,
#   yess'.
#
type_of 15 "cat"

# try getting version of cat but in a limited way so that it does not block.
# Don't warn if version cannot be found.
get_version_minimal "cat"

# type -a cmp: get all types of cmp
type_of 16 "cmp"

# try getting version of cmp
get_version "cmp"

# type -a cp: get all types of cp
type_of 17 "cp"

# try getting version of cp
get_version "cp"

# type -a cut: get all types of cut
type_of 18 "cut"

# try getting cut version
get_version "cut"

# type -a date: find the bug reporter a date :-) (okay - get all types of date
# command)
type_of 19 "date"

# try getting version of date
get_version "date"

# type -a echo: get all types of echo
type_of 20 "echo"

# NOTE: we don't try getting version of echo because in some implementations
# (all I've tested in fact) it'll just echo --version etc. despite the fact
# the man page suggests it should show a version. Well actually doing /bin/echo
# --version does work (though command echo does not for some reason) but not all
# implementations have this and will simply print "--version" etc. which is
# pointless clutter. Now we could try what(1) or ident(1) but it's probably not
# necessary and it would require an even simpler version of get_version_minimal.
# We have to draw a line somewhere and this is where it is:
#
#	----------
#
# :-)
#

# type -a find: find all the users since they appear to be lost :-)
# Or to be more specific: what kind of find did the user find ? :-) (or actually
# just find all types of find :-) )
type_of 21 "find"

# try getting version of find
get_version "find"

# type -a getopts: get all types of getopts
#
# NOTE: this might not be necessary but it was suggested anyway. I might be
# wrong but I suspect in shell scripts it would use the built-in unless given
# the command command or a path but using type -a should give us everything.
# will give the path under both linux and macOS.
type_of 22 "getopts"
# NOTE: no need to try and get version as it's a built-in under both macOS and
# linux and neither have even a --version option.

# type -a grep: get all types of grep
type_of 23 "grep"

# try getting grep version
get_version "grep"

# type -a head: determine if the user has a head :-) (on my behalf I hope they do:
# I lost my head years ago and it's a real problem I can tell you! :-) ) Okay,
# actually we just want to find all types of heads (or head) :-)
type_of 24 "head"

# try getting version of head
get_version "head"

# type -a mktemp: get all types of mktemp (well not the C library :-) )
type_of 25 "mktemp"

# try getting version of mktemp
get_version "mktemp"

# type -a mv: find where user will be moving :-) (or actually just all types of mv :-) )
type_of 26 "mv"

# try getting version of mv
get_version "mv"

# type -a printf: get all types of printf (well not the C library :-) )
type_of 27 "printf"

# try getting printf version
get_version "printf"

# type -a rm: get all types of rm
type_of 28 "rm"

# try getting version of rm
get_version "rm"

# type -a sed: record everything the user has said :-) (or rather get all types of sed)
type_of 29 "sed"

# try getting sed version
get_version "sed"

# type -a tar: get all types of tar (okay - actually tar in the system ... we
# don't care about the dark, flammable liquid :-) )
#
# NOTE: we don't need to check if tar accepts the correct options in this script
# because hostchk.sh will do that later on.
type_of 30  "tar"

# tar --version: find out what version tar is
get_version "tar"

# type -a tee: get all types of tee (none of it will be as good as a cup of
# black tea but we still need to find the tees here :-) )
type_of 31 "tee"

# try getting version of tee
get_version "tee"

# type -a touch: get all types of touch (not counting physical or mental or
# emotional touch of humans, other animals and things :-) )
type_of 32 "touch"

# try getting version of touch
get_version "touch"

# type -a tr: get all types of tr
type_of 33 "tr"

# try getting version of tr
get_version "tr"

# type -a true: get all types of true (this is not false :-) )
type_of 34 "true"

# make sure true is true :-)
run_check 35 "true"

# try getting version of true
get_version "true"

# type -a yes: get all yesses (yes Preciouss, yesses is a valid spelling just as yeses is :-) )
type_of 36 "yes"

# don't try getting version of yes because it will just try printing the args
# over and over again as it is designed to do

write_echo "#-----------------------------------------------------#"
write_echo "# SECTION 0 ABOVE: ENVIRONMENT AND SYSTEM INFORMATION #"
write_echo "#-----------------------------------------------------#"

#############################
# Section 1: optional tools #
#############################

write_echo ""
write_echo "#############################"
write_echo "# SECTION 1: OPTIONAL TOOLS #"
write_echo "#############################"
write_echo ""

# optional tool checks
#
# If is OK to not have an optional tool in that such optional tools
# are NOT mandatory to compile the core tools that someone needs
# to submit an submission to the IOCCC.
#
# These optional tools are used by the maintainers and/or
# there are workarounds for not having these tools.

# type -a checknr: get all types of checknr
type_of_optional "checknr"

# try getting version of checknr
get_version_optional "checknr"

# type -a ctags: get all types of ctags
type_of_optional "ctags"

# try getting version of ctags
get_version_optional "ctags"

# type -a fmt: get all types of fmt
type_of_optional "fmt"

# try getting version of fmt
get_version_optional "fmt"

# type -a gdate: get all types of gdate
type_of_optional "gdate"

# try getting version of gdate
get_version_optional "gdate"

# type -a install: which install are we using ? :-) (that is find all types of install :-) )
type_of_optional "install"

# try getting version of install
get_version_optional "install"

# type -a man: find a good man for the females here :-)
type_of_optional "man"

# try getting version of man (is that the age ? :-) )
get_version_optional "man"

# type -a picky: find how picky the bug reporter is (or is that how picky we are ? :) - actually
# just find all types of picky)
type_of_optional "picky"

# don't try getting version of picky as it'll block

# type -a rpl: get all types of rpl
type_of_optional "rpl"

# try getting version of rpl
get_version_optional "rpl"

# type -a seqcexit: get all types of seqcexit
type_of_optional "seqcexit"

# try getting version of seqcexit
get_version_optional "seqcexit"

# type -a shellcheck: get all types of shellcheck(s)
type_of_optional "shellcheck"

# try getting version of shellcheck
get_version_optional "shellcheck"

write_echo "#---------------------------------#"
write_echo "# SECTION 1 ABOVE: OPTIONAL TOOLS #"
write_echo "#---------------------------------#"

############################
# Section 2: C environment #
############################

write_echo ""
write_echo "############################"
write_echo "# SECTION 2: C ENVIRONMENT #"
write_echo "############################"
write_echo ""

# type -a cc: get all types of cc
type_of 37 "cc"

# cc -v: get compiler version
run_check 38 "cc -v"

# type -a make: get all makes :-)
type_of 39 "make"

# type -a of gmake: although we prefer GNU make it might not be installed as
# gmake so it's an optional tool.
if [[ $(basename "$MAKE") == gmake ]]; then
    type_of_optional "gmake"
fi

# try and get version of make
get_version "make"

# try and get version of gmake
if [[ $(basename "$MAKE") == gmake ]]; then
    get_version "gmake"
fi

# cpp -dM /dev/null: get predefined macros
run_check 40 "cpp -dM /dev/null"

write_echo "#--------------------------------#"
write_echo "# SECTION 2 ABOVE: C ENVIRONMENT #"
write_echo "#--------------------------------#"

#################################
# Section 3: compilation checks #
#################################

write_echo ""
write_echo "#################################"
write_echo "# SECTION 3: COMPILATION CHECKS #"
write_echo "#################################"
write_echo ""

if [[ -z "$T_FLAG" ]]; then
    # make clobber: start clean
    run_check 41 "$MAKE $MAKE_FLAGS clobber"

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
    run_check 42 "$MAKE $MAKE_FLAGS all" # the answer to life, the universe and everything conveniently makes all :-)

    # make test: run the IOCCC toolkit test suite
    run_check 43 "$MAKE $MAKE_FLAGS test"
fi

# hostchk.sh -v 3: we need to run some checks to make sure the system can
# compile things and so on
run_check 44 "./test_ioccc/hostchk.sh -v 3"

write_echo "#-------------------------------------#"
write_echo "# SECTION 3 ABOVE: COMPILATION CHECKS $"
write_echo "#-------------------------------------#"

#############################################
# Section 4: JSON scanner and parser checks #
#############################################

write_echo ""
write_echo "####################################"
write_echo "# SECTION 4: BISON AND FLEX CHECKS #"
write_echo "####################################"
write_echo ""

# pre-clean
rm -f lex.yy.c

# type -a bison: get all types of bison (be careful it doesn't ram you :- ) )
type_of_optional "bison"

# try getting version of bison
get_version_optional "bison"

# type -a flex: get all types of flexes :-) (okay - flex)
type_of_optional "flex"

# try getting version of flex
get_version_optional "flex"

# NOTE: we do want to check that run_bison.sh and run_flex.sh do not exit
# non-zero because if they do it means also backup files could not be used which
# would mean the repo could not be used properly.
#
# run_bison.sh -v 7: check if bison will work
run_check 45 "$MAKE -C jparse ${MAKE_FLAGS} run_bison-v7"

# run_flex.sh -v 7: check if flex will work
run_check 46 "$MAKE -C jparse ${MAKE_FLAGS} run_flex-v7"

if [[ -z "$T_FLAG" ]]; then
    # run make all again: run_bison.sh and run_flex.sh will likely cause a need for
    # recompilation
    write_echo "## RUNNING \"$MAKE all\" a second time"
    run_check 47 "$MAKE $MAKE_FLAGS all"
fi

# post-clean
rm -f lex.yy.c

write_echo "#----------------------------------------#"
write_echo "# SECTION 4 ABOVE: BISON AND FLEX CHECKS #"
write_echo "#----------------------------------------#"

####################################################################
# Section 5: IOCCC environment like version info executable checks #
####################################################################

write_echo ""
write_echo "################################"
write_echo "# SECTION 5: IOCCC ENVIRONMENT #"
write_echo "################################"
write_echo ""

# See that every tool is executable and run -V on each one that is.
#
# If any tool is not executable the exit code will be set to 48.
#
for f in $TOOLS; do
    write_echo "## CHECKING: IF \"$f\" IS EXECUTABLE"
    if is_exec "$f"; then
	write_echo "## \"$f\" IS EXECUTABLE"
	write_echo "## RUNNING: \"$f -V\""
	write_echo "$f version $($f -V)"
	write_echo ""
    else
	EXIT_CODE=48
	write_echo "$0: ERROR: \"$f\" IS NOT EXECUTABLE: new exit code: $EXIT_CODE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	\"$f\" cannot be executed"
	write_echo "### ISSUE DETECTED: \"$f\" process execution was botched: not even a zombie process was produced"
    fi
done

# See that every subdirectory exists.
#
# If any does not exist or is not readable we report this.
for d in $SUBDIRS; do
    write_echo "## CHECKING: IF \"$d\" IS A SEARCHABLE DIRECTORY"
    if is_read_exec_dir "$d"; then
	write_echo "## \"$d\" IS A SEARCHABLE DIRECTORY"
	write_echo ""
    else
	EXIT_CODE=49
	write_echo "$0: ERROR: \"$d\" IS NOT A SEARCHABLE DIRECTORY: new exit code: $EXIT_CODE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	$d is not a searchable directory"
	write_echo "### ISSUE DETECTED: \"$d\" is not a searchable directory"
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
run_optional_check "$MAKE -C soup ${MAKE_FLAGS} limit_ioccc.sh"
write_echo "## Checking for \"limit_ioccc.sh\""
if [[ -e "./soup/limit_ioccc.sh" ]]; then
    if [[ -r "./soup/limit_ioccc.sh" ]]; then
	write_echo "## Found \"limit_ioccc.sh\" file:"
	write_echo "--"
	write_echo "cat ./soup/limit_ioccc.sh"
	# tee -a -- "$LOGFILE" < ./soup/limit_ioccc.sh
	if [[ -z "$L_FLAG" ]]; then
	    < ./soup/limit_ioccc.sh tee -a -- "$LOGFILE"
	else
	    cat ./soup/limit_ioccc.sh >> "$LOGFILE"
	fi
	write_echo "--" | tee -a -- "$LOGFILE"
    else
	write_echo "### Notice: found unreadable \"limit_ioccc.sh\""
	NOTICE_SUMMARY="$NOTICE_SUMMARY
	Notice: found unreadable \"limit_ioccc.sh\""
    fi
else
    write_echo "### Notice: no \"limit_ioccc.sh\" file found"
    NOTICE_SUMMARY="$NOTICE_SUMMARY
    Notice: no \"limit_ioccc.sh\" file found"
fi
write_echo ""

write_echo "#------------------------------------#"
write_echo "# SECTION 5 ABOVE: IOCCC ENVIRONMENT #"
write_echo "#------------------------------------#"

############################################################
# Section 6: check for any user modifications to the tools #
############################################################

write_echo ""
write_echo "################################################"
write_echo "# SECTION 6: USER MODIFICATIONS TO IOCCC TOOLS #"
write_echo "################################################"
write_echo ""

# check that Makefile exists
#
# If it does not the script would have already reported an issue but we want to
# be as complete as possible.
write_echo "## CHECKING: IF \"Makefile\" EXISTS"
if [[ -e "./Makefile" ]]; then
    if [[ -r "./Makefile" ]]; then
	write_echo "\"Makefile\" exists"
    else
	EXIT_CODE=50
	write_echo "ERROR: \"Makefile\" NOT READABLE: new exit code: $EXIT_CODE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	\"Makefile\" not readable"
	write_echo "### ISSUE DETECTED: \"Makefile\" not readable"
    fi
else
    EXIT_CODE=51
    write_echo "ERROR: \"Makefile\" DOES NOT EXIST: new exit code: $EXIT_CODE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    \"Makefile\" does not exist"
    write_echo "### ISSUE DETECTED: \"Makefile\" does not exist"
fi

# now do the same for subdirectories
for d in $SUBDIRS; do
    write_echo "## CHECKING: IF \"$d/Makefile\" EXISTS"
    if [[ -f "$d/Makefile" ]]; then
	if [[ -r "$d/Makefile" ]]; then
	    write_echo "\"$d/Makefile\" exists"
	else
	    EXIT_CODE=52
	    write_echo "ERROR: \"$d/Makefile\" NOT READABLE: new exit code: $EXIT_CODE"
	    FAILURE_SUMMARY="$FAILURE_SUMMARY
	    \"$d/Makefile\" not readable"
	    write_echo "### ISSUE DETECTED: \"$d/Makefile\" not readable"
	fi
    else
	EXIT_CODE=53
	write_echo "$0: ERROR: \"$d/Makefile\" DOES NOT EXIST: new exit code: $EXIT_CODE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
	\"$d/Makefile\" does not exist"
	write_echo "### ISSUE DETECTED: \"$d/Makefile\" does not exist"
    fi
done
write_echo ""

# check for Makefile.local files to see if user is overriding any rules or variables.
#
# NOTE: we don't use run_check for this because it's not an actual error whether
# or not the user has a Makefile.local file. What matters is the contents of it
# if they do have one.
#
write_echo "## CHECKING IF \"Makefile.local\" EXISTS"
if [[ -e "./Makefile.local" ]]; then
    if [[ -r "./Makefile.local" ]]; then
	write_echo "### Warning: found \"Makefile\" overriding file \"Makefile.local\":"
	write_echo "cat ./Makefile.local"
	write_echo "--"
	if [[ -z "$L_FLAG" ]]; then
	    # tee -a -- "$LOGFILE" < Makefile.local
	    < Makefile.local tee -a -- "$LOGFILE"
	else
	    cat Makefile.local >> "$LOGFILE"
	fi
	write_echo "--"
    else
	write_echo "### Warning: found unreadable \"Makefile.local\""
    fi
else
    write_echo "# Makefile has no overriding \"Makefile.local\""
fi
write_echo ""

# now do the same for subdirectories
for d in $SUBDIRS; do
    if [[ -e "$d/Makefile.local" ]]; then
	if [[ -r "$d/Makefile.local" ]]; then
	    write_echo "### Warning: found \"$d/Makefile\" overriding file \"$d/Makefile.local\":"
	    write_echo "--"
	    if [[ -z "$L_FLAG" ]]; then
		# tee -a -- "$LOGFILE" < Makefile.local
		< "$d/Makefile.local" tee -a -- "$LOGFILE"
	    else
		cat "$d/Makefile.local" >> "$LOGFILE"
	    fi
	    write_echo "--"
	else
	    write_echo "### Warning: found unreadable \"$d/Makefile.local\""
	fi
    else
	write_echo "# \"$d/Makefile\" has no overriding \"$d/Makefile.local\""
    fi
done
write_echo ""


# check if there are any local modifications to the code
#
# NOTE: We don't use run_check for this because if git does not exist for some
# reason then the shell might exit with an error code even though we're not
# after that (as in it might not be an error). Additionally whether there are
# differences or not git will return 0. It will return non-zero in the case that
# it's not in a git repo but we don't explicitly check for this. All we care
# about is whether or not the user has changes that might be causing a problem.

write_echo "## RUNNING: \"git status\" on the code"
exec_command git --no-pager status
write_echo ""
write_echo "## \"git status\" ABOVE"
write_echo ""

write_echo "## RUNNING: \"git diff\" to check for local modifications to the code"
exec_command git --no-pager diff
write_echo "## \"git diff\" ABOVE"
write_echo ""

write_echo "## RUNNING: git diff --staged to check for local modifications to the code"
exec_command git --no-pager diff --staged
write_echo "## git diff --staged ABOVE"
write_echo ""

write_echo "#----------------------------------------------------#"
write_echo "# SECTION 6 ABOVE: USER MODIFICATIONS TO IOCCC TOOLS #"
write_echo "#----------------------------------------------------#"
write_echo ""

################
# final report #
################

if [[ -n "$NOTICE_SUMMARY" ]]; then
    write_echo 1>&2
    write_echo "One or more POSSIBLE issues noticed:"
    write_echo ""
    write_echo "Begin notice list"
    write_echo "$NOTICE_SUMMARY"
    write_echo "End of notice list"
    write_echo ""
    # make it so log file is not deleted even if -x specified
    # let the calling tool remove or not remove the logfile
    X_FLAG=""
fi
if [[ "$EXIT_CODE" -ne 0 ]]; then
    write_echo 1>&2
    write_echo "One or more problems occurred:"
    write_echo "$FAILURE_SUMMARY"
    write_echo ""
    write_echo "Final exit code: $EXIT_CODE"
    write_echo ""
    write_echo "Please file an issue on the GitHub issues page:"
    write_echo ""
    write_echo "	https://github.com/ioccc-src/mkiocccentry/issues"
    write_echo ""
    write_echo "making sure to attach $LOGFILE with your report. You may"
    write_echo "instead email the Judges but you're encouraged to file a"
    write_echo "report instead. This is because not all tools were written by"
    write_echo "the Judges."
    X_FLAG=""
elif [[ -z "$X_FLAG" ]]; then
    write_echo "All tests PASSED"
    write_echo ""
    write_echo "A log of the above tests was saved to $LOGFILE."
    write_echo "If you feel everything is in order you may safely delete that file."
    write_echo "Otherwise you may report the issue at the GitHub issue page:"
    write_echo ""
    write_echo "	https://github.com/ioccc-src/mkiocccentry/issues"
    write_echo ""
    write_echo "making sure to attach $LOGFILE with your report. You may"
    write_echo "instead email the Judges but you're encouraged to file a"
    write_echo "report instead. This is because not all tools were written by"
    write_echo "the Judges."
fi

if [[ -z "$X_FLAG" ]]; then
    write_echo ""
    write_echo "NOTE: $LOGFILE contains various information about"
    write_echo "your environment including things such as hostname, login name, operating system"
    write_echo "information, paths and versions of various tools. Although not encouraged,"
    write_echo "you are free to edit this file if you feel so inclined. This information is"
    write_echo "added to the file in case it proves useful in debugging a problem, and therefore"
    write_echo "we kindly request that you please provide it to us when you report a problem with this"
    write_echo "code."
else
    rm -f "$LOGFILE"
fi

# final cleanup
#
rm -f jparse/lex.yy.c jparse/lex.jparse_.c

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
if [[ "$V_FLAG" -gt 1 ]]; then
    write_echo "About to exit with exit code: $EXIT_CODE"
fi
exit "$EXIT_CODE"
