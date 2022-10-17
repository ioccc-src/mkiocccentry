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
# get tar path
TAR="$(type -P tar 2>/dev/null)"
# get cc path
CC="$(type -P cc 2>/dev/null)"
# It's possible that the paths could not be obtained so we set to defaults in
# this case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# would make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${CC:=/usr/bin/cc} 2>/dev/null
#   ${TAR:=/usr/bin/tar} 2>/dev/null
#
# make sure CC is set
if [[ -z "$CC" ]]; then
    CC="/usr/bin/cc"
fi
# make sure TAR is set
if [[ -z "$TAR" ]]; then
    TAR="/usr/bin/tar"
fi

export HOSTCHK_VERSION="0.2 2022-10-12"
export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-t tar] [-c cc] [-f]

    -h			    print help and exit
    -V			    print version and exit
    -v level		    set verbosity level for this script: (def level: 0)
    -D dbg_level	    set verbosity level for tests (def: level: 0)
    -t tar		    path to tar that accepts -J option (def: $TAR)
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
while getopts :hv:VD:t:c:f flag; do
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
    t)	TAR="$OPTARG";
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
# check for tar
#
if [[ ! -e $TAR ]]; then
    echo "$0: ERROR: tar not found: $TAR" 1>&2
    exit 4
fi
if [[ ! -f $TAR ]]; then
    echo "$0: ERROR: tar not a regular file: $TAR" 1>&2
    exit 4
fi
if [[ ! -x $TAR ]]; then
    echo "$0: ERROR: tar not executable: $TAR" 1>&2
    exit 4
fi

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


# set up for tar test
#
RUN_TAR_TEST="true"
TEST_FILE=$(mktemp .hostchk.test_file.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp $TEST_FILE exit code: $status" 1>&2
    EXIT_CODE=30
    RUN_TAR_TEST=
fi
date > "$TEST_FILE"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: date > $TEST_FILE exit code: $status" 1>&2
    EXIT_CODE=31
    RUN_TAR_TEST=
fi
if [[ ! -r $TEST_FILE ]]; then
    echo "$0: ERROR: not a readable TEST_FILE: $TEST_FILE" 1>&2
    EXIT_CODE=32
    RUN_TAR_TEST=
fi
TAR_ERROR=$(mktemp -u .hostchk.tar_err.XXXXXXXXXX.out)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $TAR_ERROR exit code: $status" 1>&2
    EXIT_CODE=33
    RUN_TAR_TEST=
fi
TARBALL=$(mktemp -u .hostchk.tarball.XXXXXXXXXX.txz)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $TARBALL exit code: $status" 1>&2
    EXIT_CODE=34
    RUN_TAR_TEST=
fi

# run tar test
#
TAR_TEST_SUCCESS="true"
if [[ -n $RUN_TAR_TEST ]]; then
    "${TAR}" --format=v7 -cJf "$TARBALL" "$TEST_FILE" 2>"$TAR_ERROR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: $TAR --format=v7 -cJf $TARBALL $TEST_FILE 2>$TAR_ERROR exit code: $status" 1>&2
	EXIT_CODE=35
	TAR_TEST_SUCCESS=
    fi
    if [[ ! -s $TARBALL ]]; then
	echo "$0: ERROR: did not find a non-empty tarball: $TARBALL" 1>&2
	EXIT_CODE=36
	TAR_TEST_SUCCESS=
    fi
    if [[ -s $TAR_ERROR ]]; then
	echo "$0: notice: tar stderr follows:" 1>&2
	cat "$TAR_ERROR" 1>&2
	echo "$0: notice: end of tar stderr" 1>&2
	EXIT_CODE=37
	TAR_TEST_SUCCESS=
    fi
else
    echo "$0: notice: tar test disabled due to test set up error(s)" 1>&2
    TAR_TEST_SUCCESS=
fi

# tar test clean up
#
if [[ -n $TAR_TEST_SUCCESS ]]; then
    rm -f "$TEST_FILE" "$TAR_ERROR" "$TARBALL"
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
