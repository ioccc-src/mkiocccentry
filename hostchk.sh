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

export WORK_DIR="./hostchk"
export HOSTCHK_VERSION="0.3 2022-10-27"
export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-c cc] [-w work_dir] [-f]

    -h			    print help and exit
    -V			    print version and exit
    -v level		    set verbosity level for this script: (def level: 0)
    -D dbg_level	    set verbosity level for tests (def: level: 0)
    -c cc		    path to compiler (def: $CC)
    -w work_dir		    change work directory (def: $WORK_DIR)
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
while getopts :hv:VD:c:w:f flag; do
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
    w)	WORK_DIR="$OPTARG";
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
    exit 10
fi
if [[ ! -f $CC ]]; then
    echo "$0: ERROR: cc not a regular file: $CC" 1>&2
    exit 10
fi
if [[ ! -x $CC ]]; then
    echo "$0: ERROR: cc not executable: $CC" 1>&2
    exit 10
fi

# create an empty working directory
#
rm -rf  "$WORK_DIR"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: rm -rf $WORK_DIR failed, error code: $status" 1>&2
    exit 11
fi
mkdir -p "$WORK_DIR"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mkdir -p $WORK_DIR failed, error code: $status" 1>&2
    exit 11
fi


# set up for compile test
#
export INCLUDE_TEST_SUCCESS="true"
RUN_INCLUDE_TEST="true"
PROG_FILE=$(mktemp -u .hostchk.prog.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    EXIT_CODE=41
    echo "$0: ERROR: mktemp -u $PROG_FILE exit code: $status: new exit code: $EXIT_CODE" 1>&2
    RUN_INCLUDE_TEST=
fi

# trap to remove prog and also work directory
trap "rm -rf \$PROG_FILE \$WORK_DIR; exit" 1 2 3 15

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
	EXIT_CODE=42
	echo "$0: ERROR: unable to compile test file with all necessary system include files $h: new exit code: $EXIT_CODE" 1>&2
	INCLUDE_TEST_SUCCESS="false"
    elif [[ -s "$PROG_FILE" && -x "$PROG_FILE" ]]; then
	./"$PROG_FILE"
	status="$?"
	if [[ $status -ne 0 ]]; then
	    EXIT_CODE=43
	    echo "$0: ERROR: unable to run executable compiled with all necessary system include files: new exit code: $EXIT_CODE" 1>&2
	    INCLUDE_TEST_SUCCESS="false"
	else
	    if [[ $V_FLAG -gt 1 ]]; then
		echo "$0: Running executable compiled with all necessary system include files: success" 1>&2
	    fi
	fi
    else
	EXIT_CODE=44
	echo "$0: ERROR: unable to form an executable compiled using all necessary system include files: new exit code: $EXIT_CODE" 1>&2
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
		EXIT_CODE=42
		echo "$0: ERROR: unable to compile with $h: new exit code: $EXIT_CODE" 1>&2
		INCLUDE_TEST_SUCCESS="false"
	    elif [[ -s "$PROG_FILE" && -x "$PROG_FILE" ]]; then
		./"$PROG_FILE"
		status="$?"
		if [[ $status -ne 0 ]]; then
		    EXIT_CODE=43
		    echo "$0: ERROR: unable to run executable compiled using: $h: new exit code: $EXIT_CODE" 1>&2
		    INCLUDE_TEST_SUCCESS="false"
		else
		    if [[ $V_FLAG -gt 1 ]]; then
			echo "$0: Running executable compiled using: $h: success"
		    fi
		fi
	    else
		EXIT_CODE=44
		echo "$0: ERROR: unable to form an executable compiled using: $h: new exit code: $EXIT_CODE" 1>&2
		INCLUDE_TEST_SUCCESS="false"
	    fi

	    # clean up after compile test
	    #
	    rm -f "$PROG_FILE"
	done < <(grep -h -o '#include.*<.*>' ./*.c ./*.h|sort -u)
    else
	EXIT_CODE=45
	echo "$0: notice: include test disabled due to test set up error(s): new exit code: $EXIT_CODE" 1>&2
	INCLUDE_TEST_SUCCESS="false"
    fi
fi

# now make and test compile and run some C files

# compile_test	- compile test source file, checking that an executable was
#		  created
#
# usage:
#	compile exit_code source output
#
#	exit_code   - new exit code of rule fails
#	source	    - source file
#	output	    - output file of compiler
#
compile_test() {

    # parse args
    #
    if [[ $# -ne 3 ]]; then
	echo "$0: ERROR: function expects 3 args, found $#" 1>&2
	return 1
    fi
    local CODE="$1"
    local SOURCE="$2"
    local OUTPUT="$3"

    if [[ ! -e "$SOURCE" ]]; then
	echo "$0: ERROR: source code does not exist: $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
	return 1
    fi

    if [[ ! -r "$SOURCE" ]]; then
	echo "$0: ERROR: source file not readable: $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
	return 1
    fi

    $CC -o "$OUTPUT" "$SOURCE"
    if [[ ! -e "$OUTPUT" ]]; then
	echo "$0: ERROR: could not create executable from $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
	return 1
    fi
    if [[ ! -x "$WORK_FILE" ]]; then
	echo "$0: ERROR: could not create executable from $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
    fi

    return 0
}


# pre-errno test: get value of ENOENT
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating source that should print the value of ENOENT" 1>&2
fi
cat <<EOF>"$WORK_DIR/pre-errno.c"
#include <errno.h>
#include <stdio.h>
int
main(void)
{
    printf("%d", ENOENT);

    return 0;
}
EOF
WORK_FILE="$WORK_DIR/pre-errno"
if compile_test 46 "$WORK_DIR/pre-errno.c" "$WORK_FILE"; then
    ENOENT=$("$WORK_FILE")
    if [[ $V_FLAG -gt 1 ]]; then
	echo "$0: got: ENOENT == $ENOENT" 1>&2
    fi
fi
# errno test 0: test printf of errno == ENOENT. If program does not print:
#
#   errno: $ENOENT
#
# where $ENOENT was obtained above then there's an inconsistency with the errno
# value.
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating source that should print: errno: $ENOENT" 1>&2
fi
cat <<EOF>"$WORK_DIR/errno0.c"
#include <errno.h>
#include <stdio.h>
int
main(void)
{
    errno = ENOENT;

    (void) printf("errno: %d\n", errno);
    return 0;
}
EOF
WORK_FILE="$WORK_DIR/errno0"
if compile_test 47 "$WORK_DIR/errno0.c" "$WORK_FILE"; then
    if ! "$WORK_FILE" | grep -q "errno: $ENOENT"; then
	# XXX should we not use -q to grep so we can print the actual value?
	EXIT_CODE=48
	echo "$0: ERROR: inconsistent ENOENT value: did not get: \"errno: $ENOENT\": new exit code: $EXIT_CODE" 1>&2
    elif [[ $V_FLAG -gt 1 ]]; then
	echo "$0: got: errno: $ENOENT" 1>&2
    fi
fi

# errno1: create source file that when compiled should return 1 (errno is set to
# ENOENT so errno == ENOENT should be 1)
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating source that should verify errno == $ENOENT, returning 1" 1>&2
fi
cat<<EOF>"$WORK_DIR/errno1.c"
#include <errno.h>
int
main(void)
{
    errno = ENOENT;

    return errno == ENOENT;
}
EOF
WORK_FILE="$WORK_DIR/errno1"
if compile_test 49 "$WORK_DIR/errno1.c" "$WORK_FILE"; then
    "$WORK_FILE"
    status=$?
    if [[ "$status" -ne 1 ]] ; then
	EXIT_CODE=50
	echo "$0: ERROR: errno != ENOENT: new exit code: $EXIT_CODE" 1>&2
    elif [[ "$V_FLAG" -gt 1 ]]; then
	echo "$0: got return value 1: errno == ENOENT"
    fi
fi

# hello.c: print "Hello, world".
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating source that should print: "Hello, world"" 1>&2
fi
cat <<EOF>"$WORK_DIR/hello.c"
#include <stdio.h>
int
main(void)
{
    (void) printf("Hello, world\n");
    return 0;
}
EOF
WORK_FILE="$WORK_DIR/hello"
if compile_test 51 "$WORK_DIR/hello.c" "$WORK_FILE"; then
    if ! "$WORK_FILE" | grep -q "Hello, world"; then
	EXIT_CODE=52
	echo "$0: ERROR: expected string "Hello, world" not found: new exit code: $EXIT_CODE" 1>&2
    elif [[ "$V_FLAG" -gt 1 ]]; then
	echo "$0: got: "Hello, world"" 1>&2
    fi
fi

# main0: create source file that should return 0
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating source that should simply return 0" 1>&2
fi
cat <<EOF>"$WORK_DIR/main0.c"
int
main(void)
{
    return 0;
}
EOF
WORK_FILE="$WORK_DIR/main0"
if compile_test 52 "$WORK_DIR/main0.c" "$WORK_FILE"; then
    if ! "$WORK_FILE"; then
	EXIT_CODE=53
	echo "$0: ERROR: expected return value 0 not returned: got $?: new exit code: $EXIT_CODE" 1>&2
    elif [[ "$V_FLAG" -gt 1 ]]; then
	echo "$0: got return value 0" 1>&2
    fi
fi


# main1: create source file that should return 1
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating source that should simply return 1" 1>&2
fi
cat <<EOF>"$WORK_DIR/main1.c"
int
main(void)
{
    return 1;
}
EOF
WORK_FILE="$WORK_DIR/main1"
if compile_test 54 "$WORK_DIR/main1.c" "$WORK_FILE"; then
    if "$WORK_FILE"; then
	EXIT_CODE=55
	echo "$0: ERROR: expected return value 1 not returned: got $?: new exit code: $EXIT_CODE" 1>&2
    elif [[ "$V_FLAG" -gt 1 ]]; then
	echo "$0: got return value 1" 1>&2
    fi
fi

# remove work directory
rm -rf "$WORK_DIR"

# All Done!!! -- Jessica Noll, Age 2
#
exit "$EXIT_CODE"
