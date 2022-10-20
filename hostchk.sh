#!/usr/bin/env bash
#
# hostchk.sh - make various checks on a host to determine if it can use the
# mkiocccentry repo.
#
# NOTE: This is NOT yet finished. More needs to be discussed at
# https://github.com/ioccc-src/mkiocccentry/issues/250 (hostchk rule/script) and at
# https://github.com/ioccc-src/mkiocccentry/issues/248 (make bug-report
# rule/script).
#
# NOTE: This cannot be perfect and it cannot account for everything but along
# with bug_report.sh we hope to account for most things. One can always report
# an issue at the GitHub issues page
# https://github.com/ioccc-src/mkiocccentry/issues (the preferable method as
# others can help as well and in the case of tools - for example txzchk -
# written by someone other than the Judges they can also look at the problem) or
# email the Judges.
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
# NOTE: This script is called from make all and if it exits non-zero it will
# issue a warning, suggesting you report it via the bug_report.sh script. That
# script itself will call hostchk.sh which will reissue that warning (more than
# once in fact since make all calls hostchk.sh and bug_report.sh calls it via
# make all and running hostchk.sh directly) and each time it will suggest using
# bug_report.sh.
#
# Obviously one needn't run this script a second or third time just because the
# Makefile and bug_report.sh suggest you do! :-) Sorry in advance if this (to
# make use of an American English phrase which I will happily do for a pun for a
# pun not made is a wasted opportunity :-) ) throws you for a loop! :-) But now
# that you're in the loop it shouldn't even matter. :-)
#
# This might seem extra verbose or overkill but we feel that if there's an issue
# with hostchk.sh it really is an issue that will likely prevent a successful
# use of this repo so each time the script fails we report the issue since it
# really is likely that, if the script fails, you will be unable to successfully
# use the mkiocccentry repo to submit a correct IOCCC entry.
#

#

# set up
#

# attempt to fetch system specific paths to the tools we need
#
# get cc path
CC="$(type -P cc 2>/dev/null)"
# It's possible that a path could not be obtained so we set to defaults in this
# case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# would make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${CC:=/usr/bin/cc} 2>/dev/null
#
# make sure CC is set
if [[ -z "$CC" ]]; then
    CC="/usr/bin/cc"
fi

export HOSTCHK_VERSION="0.2 2022-10-12"
export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-c cc] [-f]

    -h			    print help and exit
    -V			    print version and exit
    -v level		    set verbosity level for this script: (def level: 0)
    -D dbg_level	    set verbosity level for tests (def: level: 0)
    -c cc		    path to compiler (def: $CC)
    -f			    faster check (def: run slower for better diagnostics)

Exit codes:
     0   all is well
     1   at least one test failed
     2   help mode and version mode exit
     3   invalid command line
 >= 10   internal error

$0 version: $HOSTCHK_VERSION"

export EXIT_CODE=0

# parse args
#
export V_FLAG="0"
export DBG_LEVEL="0"
export F_FLAG=
while getopts :hv:VD:c:f flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    V)	echo "$HOSTCHK_VERSION"
	exit 2
	;;
    D)	DBG_LEVEL="$OPTARG";
	;;
    c)	CC="$OPTARG";
	;;
    f)	F_FLAG="true";
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

# firewall checks
#
# check for cc
#
if [[ ! -e $CC ]]; then
    echo "$0: ERROR: cc not found: $CC" 1>&2
    exit 4
fi
if [[ ! -f $CC ]]; then
    echo "$0: ERROR: cc not a regular file: $CC" 1>&2
    exit 4
fi
if [[ ! -x $CC ]]; then
    echo "$0: ERROR: cc not executable: $CC" 1>&2
    exit 4
fi

# set up for compile test
#
export INCLUDE_TEST_SUCCESS="true"
RUN_INCLUDE_TEST="true"
PROG_FILE=$(mktemp -u .hostchk.prog.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $PROG_FILE exit code: $status" 1>&2
    EXIT_CODE=41
    RUN_INCLUDE_TEST=
fi

trap "rm -f \$PROG_FILE; exit" 1 2 3 15

# Previously, -f was so fast it did absolutely nothing! :-)
#
# Now, however, fast mode is actually slower! :-( It however can be forgiven,
# perhaps, because it actually does something. :-) Instead of trying to compile
# a program for each required system header file it compiles one source file
# with all of them included at once.
#
if [[ -n "$F_FLAG" ]]; then
    rm -f "$PROG_FILE"
    if [[ $V_FLAG -gt 1 ]]; then
	echo "$0: test compiling test file with all necessary system include files" 1>&2
    fi

    printf "%s\n%s\n" "$(grep -h -o '#include.*<.*>' ./*.c ./*.h|sort -u)" "int main(void) { return 0; }" |
	    "${CC}" -x c - -o "$PROG_FILE"
    # test compile
    #
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unable to compile test file with all necessary system include files $h" 1>&2
	EXIT_CODE=42
	INCLUDE_TEST_SUCCESS="false"
    elif [[ -s "$PROG_FILE" && -x "$PROG_FILE" ]]; then
	./"$PROG_FILE"
	status="$?"
	if [[ $status -ne 0 ]]; then
	    echo "$0: ERROR: unable to run executable compiled with all necessary system include files" 1>&2
	    EXIT_CODE=43
	    INCLUDE_TEST_SUCCESS="false"
	else
	    if [[ $V_FLAG -gt 1 ]]; then
		echo "$0: Running executable compiled with all necessary system include files: success" 1>&2
	    fi
	fi
    else
	echo "$0: ERROR: unable to form an executable compiled using all necessary system include files" 1>&2
	EXIT_CODE=44
	INCLUDE_TEST_SUCCESS="false"
    fi

    # clean up after compile test
    #
    rm -f "$PROG_FILE"
else
# run include tests one at a time
#
    if [[ -n $RUN_INCLUDE_TEST ]]; then

	# test each required system include file
	#
	while read -r h; do

	    if [[ $V_FLAG -ge 1 ]]; then
		echo "Compiling code with $h" 1>&2
	    fi

	    # form C prog
	    #
	    rm -f "$PROG_FILE"
	    printf "%s\n%s\n" "$h" "int main(void){ return 0; }" | "${CC}" -x c - -o "$PROG_FILE"

	    # test compile
	    #
	    status="$?"
	    if [[ $status -ne 0 ]]; then
		echo "$0: ERROR: unable to compile with $h" 1>&2
		EXIT_CODE=42
		INCLUDE_TEST_SUCCESS="false"
	    elif [[ -s "$PROG_FILE" && -x "$PROG_FILE" ]]; then
		./"$PROG_FILE"
		status="$?"
		if [[ $status -ne 0 ]]; then
		    echo "$0: ERROR: unable to run executable compiled using: $h" 1>&2
		    EXIT_CODE=43
		    INCLUDE_TEST_SUCCESS="false"
		else
		    if [[ $V_FLAG -gt 1 ]]; then
			echo "$0: Running executable compiled using: $h: success"
		    fi
		fi
	    else
		echo "$0: ERROR: unable to form an executable compiled using: $h" 1>&2
		EXIT_CODE=44
		INCLUDE_TEST_SUCCESS="false"
	    fi

	    # clean up after compile test
	    #
	    rm -f "$PROG_FILE"
	done < <(grep -h -o '#include.*<.*>' ./*.c ./*.h|sort -u)
    else
	echo "$0: notice: include test disabled due to test set up error(s)" 1>&2
	EXIT_CODE=45
	INCLUDE_TEST_SUCCESS="false"
    fi
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit "$EXIT_CODE"
