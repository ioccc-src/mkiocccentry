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
	      ./vermod.sh ./jsemcgen.sh ./jsemtblgen"

export BUG_REPORT_VERSION="0.3 2022-10-18"
export FAILURE_SUMMARY=
export WARNING_SUMMARY=
export DBG_LEVEL="0"
export V_FLAG="0"
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
	echo "$0: ERROR: function expects 2 args, found $#" 1>&2
	exit 3
    fi
    local COMMAND="$1"

    echo "## RUNNING: $COMMAND" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    command ${COMMAND} 2>&1 | tee -a -- "$LOG_FILE"

    status=${PIPESTATUS[0]}
    if [[ "$status" -ne 0 ]]; then
	echo "$0: WARNING: $COMMAND FAILED WITH EXIT CODE $status" | tee -a -- "$LOG_FILE"
	WARNING_SUMMARY="$WARNING_SUMMARY
	$COMMAND non-zero exit code: $status"
	echo "### POSSIBLE ISSUE DETECTED: $COMMAND returned $status" | tee -a -- "$LOG_FILE"
    fi

    echo "## OUTPUT OF ${COMMAND} ABOVE" | tee -a -- "$LOG_FILE"
    echo | tee -a -- "$LOG_FILE"
    return 0;
}

# get_shell - get the user shell :-)
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
    echo | tee -a -- "$LOG_FILE"
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


if [[ $V_FLAG -gt 1 ]]; then
    echo "Will write contents to $LOG_FILE" 1>&2
fi

echo "# TIME OF REPORT: $(date)" | tee -a -- "$LOG_FILE"
echo "# BUG_REPORT_VERSION: $BUG_REPORT_VERSION" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
# Section 0: environment and system information.
echo "# SECTION 0: ENVIRONMENT AND SYSTEM INFORMATION" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
# echo $SHELL: we need to know their default shell
get_shell
# bash --version: get bash version
run_check 11 "bash --version"
# uname -a: get system information
run_check 12 "uname -a"
# which tar: find the path to tar
#
# NOTE: we don't need to check if tar accepts the correct options in this script
# because txzchk_test.sh will do that later on.
run_check 13 "which tar"
# tar --version: find out what version tar is
run_check 14 "tar --version"
# which awk: get awk path
run_check 15 "which awk"
# awk --version | head -n 1: get awk version
run_check 16 "awk --version | head -n 1"
# which sed: get sed path
run_check 17 "which sed"
# sed --version | head -n 1: get sed version
# NOTE: this will likely fail in some BSD Unices including macOS as these do not
# have a --version option and no other way to get version via the command
# itself. However we can try doing:
#
#	strings /usr/bin/sed | head -n 1
#
# which seems to have a version string of a kind. Under macOS this gives:
#
#	$ strings /usr/bin/sed | head -n 1
#	@(#)PROGRAM:sed  PROJECT:text_cmds-138.100.3
#
# It seems though that this formatting is not consistent across BSDs. For
# example under FreeBSD it appears to be:
#
#	#ifndef lint
#	static const char sccsid[] = "@(#)main.c	8.2 (Berkeley) 1/3/94";
#	#endif
#
# which would suggest that it probably would be:
#
#	@(#)main.c        8.2 (Berkeley) 1/3/94
#
# but unfortunately it's not the first string in the source code for sed in
# FreeBSD.
#
# These Unices have the command sw_vers which might help but this will fail on
# linux. It might be worth checking with something like:
#
#	which sw_vers 2>/dev/null && sw_vers
#
# but this will require a bit more work to get the formatting right if it's
# worth adding.
#
# So how to go about it? We could try running first sed --version and if that
# fails we could try extracting the path of sed by `which' and then use strings
# on that path. But this appears that it might not be enough all the time.
#
# There are a number of tools we attempt to get the version from and these are
# all a warning if it fails rather than making it an error. But in a future
# commit we will look at a number of ways to try and get version information.
run_check_warn "sed --version | head -n 1"
# which basename: get path to basename tool
run_check 18 "which basename"
# basename --version
run_check_warn "basename --version | head -n 1"
# which cut: get path to cut tool
run_check 19 "which cut"
# cut --version
run_check_warn "cut --version | head -n 1"
# which cp: get path to cp tool
run_check 20 "which cp"
# cp --version
run_check_warn "cp --version | head -n 1"
# which grep: get path to grep tool
run_check 21 "which grep"
# grep --version
run_check_warn "grep --version | head -n 1"




echo "# SECTION 0 ABOVE: ENVIRONMENT AND SYSTEM INFORMATION" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# Section 1: C environment
echo "# SECTION 1: C ENVIRONMENT" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
# which cc: get all paths for cc
run_check 22 "which cc"
# cc -v: get compiler version
run_check 23 "cc -v"
# which make: get path to make tool
run_check 24 "which -a make"
# make -v: get make version
run_check 25 "make -v"
# cpp -dM /dev/null: get predefined macros
run_check 26 "cpp -dM /dev/null"
echo "# SECTION 1 ABOVE: C ENVIRONMENT" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# Section 2: compilation checks.
echo "# SECTION 2: COMPILATION CHECKS" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
# make clobber: start clean
run_check 27 "make clobber"
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
run_check 28 "make all"
# make test: run the IOCCC toolkit test suite
run_check 29 "make test"
# hostchk.sh -v 3: we need to run some checks to make sure the system can
# compile things and so on
run_check 30 "./hostchk.sh -v 3"
echo "# SECTION 2 ABOVE: COMPILATION CHECKS" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# Section 3: JSON scanner and parser checks.
echo "# SECTION 3: BISON AND FLEX CHECKS" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
# run_bison.sh -v 7: check if bison will work
run_check 31 "./run_bison.sh -v 7"
# run_flex.sh -v 7: check if flex will work
run_check 32 "./run_flex.sh -v 7"
# run make all again: run_bison.sh and run_flex.sh will likely cause a need for
# recompilation
echo "## RUNNING make all a second time" | tee -a -- "$LOG_FILE"
run_check 33 "make all"
echo "# SECTION 3 ABOVE: BISON AND FLEX CHECKS" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# Section 4: IOCCC environment like version information and making sure that
# everything is executable.
echo "# SECTION 4: IOCCC ENVIRONMENT" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
# See that every tool is executable and run -V on each one that is.
#
# If any tool is not executable the exit code will be set to 34.
for f in $TOOLS; do
    if is_exec "$f"; then
	echo "## Checking if $f is executable" | tee -a -- "$LOG_FILE"
	echo "## $f IS EXECUTABLE" | tee -a -- "$LOG_FILE"
	echo "## RUNNING: $f -V" | tee -a -- "$LOG_FILE"
	echo "$f version $($f -V)" | tee -a -- "$LOG_FILE"
	echo | tee -a -- "$LOG_FILE"
    else
	EXIT_CODE=34
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
echo "# SECTION 4 ABOVE: IOCCC ENVIRONMENT" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"


# Section 5: check for any user modifications to the tools
echo "# SECTION 5: USER MODIFICATIONS TO ENVIRONMENT" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# check for makefile.local to see if user is overriding any rules.
#
# NOTE: we don't use run_check for this because it's not an actual error whether
# or not the user has a makefile.local file. What matters is the contents of it
# if they do have one.
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
echo "## RUNNING: git diff to check for local modifications to the code" | tee -a -- "$LOG_FILE"
git --no-pager diff | tee -a -- "$LOG_FILE"
echo "## git diff ABOVE" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"
echo "# SECTION 5 ABOVE: USER MODIFICATIONS TO ENVIRONMENT" | tee -a -- "$LOG_FILE"
echo | tee -a -- "$LOG_FILE"

# final report
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
    echo "instead email the Judges." | tee -a -- "$LOG_FILE"

fi
echo 1>&2
echo "NOTE: $LOG_FILE contains various information about" 1>&2
echo "your environment including things such as hostname, login name, operating system" 1>&2
echo "information, paths and versions of various tools. Although not encouraged," 1>&2
echo "you are free to edit this file if you feel so inclined. This information is" 1>&2
echo "added to the file in case it proves useful in debugging a problem, and therefore" 1>&2
echo "we kindly request that you provide it to us when you report a problem with this" 1>&2
echo "code." 1>&2


if [[ "$V_FLAG" -gt 1 ]]; then
    echo "About to exit with exit code: $EXIT_CODE" 1>&2
fi
exit "$EXIT_CODE"
