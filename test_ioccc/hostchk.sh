#!/usr/bin/env bash
#
# hostchk.sh - check to see host is ready to make/use the mkiocccentry repo
#
# Make various checks on a host to determine if the current host can
# can make and use the mkiocccentry repo.
#
# NOTE: This cannot be perfect and it cannot account for everything but along
# with bug_report.sh we hope to account for most things. One can always report
# an issue at the GitHub issues page:
#
#	https://github.com/ioccc-src/mkiocccentry/issues
#
# This script was written in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# with some improvements and fixes by:
#
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# NOTE: This script is called from make all and if it exits non-zero it will
# issue a warning, suggesting you report it via the bug_report.sh script. That
# script itself will call hostchk.sh which will reissue that warning (more than
# once in fact since make all calls hostchk.sh and bug_report.sh calls it via
# make all and running hostchk.sh directly) and each time it will suggest using
# bug_report.sh.
#
# Obviously one needn't run this script a second or third time just because the
# Makefile and bug_report.sh suggest you do! :-) Sorry (tm Canada) in advance if this
# (to make use of an American English phrase which I will happily do for a pun for a
# pun not made is a wasted opportunity :-) ) throws you for a loop! :-) But now
# that you're in the loop it shouldn't even matter. :-)
#
# This might seem extra verbose or overkill but we feel that if there's an issue
# with hostchk.sh it really is an issue that will likely prevent a successful
# use of this repo so each time the script fails we report the issue since it
# really is likely that, if the script fails, you will be unable to successfully
# use the mkiocccentry repo to submit a correct IOCCC entry.

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
#
if [[ -z $CC ]]; then
    CC="/usr/bin/cc"
fi
export HOSTCHK_VERSION="1.0.1 2023-02-22"
export USAGE="usage: $0 [-h] [-V] [-v level] [-D dbg_level] [-c cc] [-w work_dir] [-f] [-Z topdir]

    -h			    Print help and exit
    -V			    Print version and exit
    -v level		    Set verbosity level for this script: (def level: 0)
    -D dbg_level	    Set verbosity level for tests (def: level: 0)
    -c cc		    Path to compiler (def: $CC)
    -w work_dir		    Use an explicit work directory (def: use a temporary directory)
				NOTE: The work_dir cannot exist
    -f			    Faster check (def: run slower for better diagnostics)
    -Z topdir		    Top level build directory (def: try . or ..)

Exit codes:
     0   all OK
     1   at least one test failed
     2   help mode and version mode exit
     3   invalid command line
     4	 compiler not found or not executable file
 >= 10   internal error

hostchk.sh version: $HOSTCHK_VERSION"

export EXIT_CODE=0
export TOPDIR=

# parse args
#
export V_FLAG="0"
export DBG_LEVEL="0"
export F_FLAG=
export W_FLAG=
while getopts :hv:VD:c:w:fZ: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG"
	;;
    V)	echo "$HOSTCHK_VERSION"
	exit 2
	;;
    D)	DBG_LEVEL="$OPTARG"
	;;
    c)	CC="$OPTARG"
	;;
    w)	WORK_DIR="$OPTARG"
	W_FLAG="true"
	;;
    f)	F_FLAG="true"
	;;
    Z)  TOPDIR="$OPTARG";
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

# change to the top level directory as needed
#
if [[ -n $TOPDIR ]]; then
    if [[ ! -d $TOPDIR ]]; then
	echo "$0: ERROR: -Z $TOPDIR given: not a directory: $TOPDIR" 1>&2
	exit 3
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: -Z $TOPDIR given, about to cd $TOPDIR" 1>&2
    fi
    # SC2164 (warning): Use 'cd ... || exit' or 'cd ... || return' in case cd fails.
    # https://www.shellcheck.net/wiki/SC2164
    # shellcheck disable=SC2164
    cd "$TOPDIR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: -Z $TOPDIR given: cd $TOPDIR exit code: $status" 1>&2
	exit 3
    fi
elif [[ -f mkiocccentry.c ]]; then
    TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume TOPDIR is .: $TOPDIR" 1>&2
    fi
elif [[ -f ../mkiocccentry.c ]]; then
    cd ..
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cd .. exit code: $status" 1>&2
	exit 3
    fi
    TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume TOPDIR is ..: $TOPDIR" 1>&2
    fi
else
    echo "$0: ERROR: cannot determine TOPDIR, use -Z topdir" 1>&2
    exit 3
fi

# setup a working directory unless -w was given
#
if [[ -z $W_FLAG ]]; then
    WORK_DIR=$(mktemp -d .hostchk.XXXXXXXXXX.work)
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: mktemp -d .hostchk.work.XXXXXXXXXX exit code: $status" 1>&2
	exit 10
    fi
elif [[ -z $WORK_DIR ]]; then
    echo "$0: ERROR: -w dir was given, but $$WORK_DIR is empty" 1>&2
    exit 11
elif [[ -e $WORK_DIR ]]; then
    echo "$0: ERROR: -w $WORK_DIR exists" 1>&2
    exit 12
fi

# set up for compile test
#
export INCLUDE_TEST_SUCCESS="true"
RUN_INCLUDE_TEST="true"
PROG_FILE=$(mktemp -u "$WORK_DIR/hostchk.XXXXXXXXXX.prog")
status="$?"
if [[ $status -ne 0 ]]; then
    EXIT_CODE=13	# will exit 13 at the end unless EXIT_CODE is changed later on
    echo "$0: ERROR: mktemp -u $PROG_FILE exit code: $status: new exit code: $EXIT_CODE" 1>&2
    RUN_INCLUDE_TEST=
fi

# trap to remove prog and also work directory
#
trap "rm -rf \$WORK_DIR; exit" 1 2 3 15

# Previously, -f was so fast it did absolutely nothing! :-)
#
# Now, however, fast mode is actually slower! :-( It however can be forgiven,
# perhaps, because it actually does something. :-) Instead of trying to compile
# a program for each required system header file it compiles one source file
# with all of them included at once.
#
if [[ -n $F_FLAG ]]; then

    # setup
    #
    rm -f "$PROG_FILE"
    if [[ $V_FLAG -gt 1 ]]; then
	echo "$0: test compiling test file with all necessary system include files" 1>&2
    fi

    # test compile
    #
    printf "%s\\n%s\\n" "$(grep '#include.*<.*>' "$TOPDIR"/*.[hc] "$TOPDIR"/dbg/*.[hc] \
	"$TOPDIR"/dyn_array/*.[hc] "$TOPDIR"/test_ioccc/*.[ch] "$TOPDIR"/soup/*.[hc] "$TOPDIR"/jparse/*.[hcly] \
	"$TOPDIR"/jparse/test_jparse/*.[hc] |cut -f 2- -d:|sort -u)" "int main(void) { return 0; }" |
	    "${CC}" -x c - -o "$PROG_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	EXIT_CODE=14	# will exit 14 at the end unless EXIT_CODE is changed later on
	echo "$0: ERROR: unable to compile test file with all necessary system include files $h: new exit code: $EXIT_CODE" 1>&2
	INCLUDE_TEST_SUCCESS="false"
    elif [[ -s $PROG_FILE && -x $PROG_FILE ]]; then
	./"$PROG_FILE"
	status="$?"
	if [[ $status -ne 0 ]]; then
	    EXIT_CODE=15	# will exit 15 at the end unless EXIT_CODE is changed later on
	    echo "$0: ERROR: unable to run executable compiled with all necessary system include files: new exit code: $EXIT_CODE" 1>&2
	    INCLUDE_TEST_SUCCESS="false"
	else
	    if [[ $V_FLAG -gt 1 ]]; then
		echo "$0: Running executable compiled with all necessary system include files: success" 1>&2
	    fi
	fi
    else
	EXIT_CODE=16	# will exit 16 at the end unless EXIT_CODE is changed later on
	echo "$0: ERROR: unable to form an executable compiled using all necessary system include files: new exit code: $EXIT_CODE" 1>&2
	INCLUDE_TEST_SUCCESS="false"
    fi

    # clean up after compile test
    #
    rm -f "$PROG_FILE"

# case: run include tests one at a time
#
elif [[ -n $RUN_INCLUDE_TEST ]]; then

    # test each required system include file
    #
    grep '#include.*<.*>' "$TOPDIR"/*.[hc] "$TOPDIR"/dbg/*.[hc] "$TOPDIR"/dyn_array/*.[hc] \
        "$TOPDIR"/test_ioccc/*.[ch] "$TOPDIR"/soup/*.[hc] "$TOPDIR"/jparse/*.[hcly] "$TOPDIR"/jparse/test_jparse/*.[hc] | \
	cut -f 2- -d:|sort -u| while read -r h; do

	# form C prog
	#
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "Compiling code with \"$h\"" 1>&2
	fi
	rm -f "$PROG_FILE"
	printf "%s\\n%s\\n" "$h" "int main(void){ return 0; }" | "${CC}" -x c - -o "$PROG_FILE"

	# test compile
	#
	status="$?"
	if [[ $status -ne 0 ]]; then
	    EXIT_CODE=17	# will exit 17 at the end unless EXIT_CODE is changed later on
	    echo "$0: ERROR: unable to compile with $h: new exit code: $EXIT_CODE" 1>&2
	    INCLUDE_TEST_SUCCESS="false"
	elif [[ -s $PROG_FILE && -x $PROG_FILE ]]; then
	    ./"$PROG_FILE"
	    status="$?"
	    if [[ $status -ne 0 ]]; then
		EXIT_CODE=18	# will exit 18 at the end unless EXIT_CODE is changed later on
		echo "$0: ERROR: unable to run executable compiled using: $h: new exit code: $EXIT_CODE" 1>&2
		INCLUDE_TEST_SUCCESS="false"
	    else
		if [[ $V_FLAG -gt 1 ]]; then
		    echo "$0: Running executable compiled using: \"$h\": success"
		fi
	    fi
	else
	    EXIT_CODE=19	# will exit 19 at the end unless EXIT_CODE is changed later on
	    echo "$0: ERROR: unable to form an executable compiled using: $h: new exit code: $EXIT_CODE" 1>&2
	    INCLUDE_TEST_SUCCESS="false"
	fi

	# clean up after compile test
	#
	rm -f "$PROG_FILE"

    done

# case: neither -f nor run include tests one at a time
#
else
    EXIT_CODE=20	# will exit 20 at the end unless EXIT_CODE is changed later on
    echo "$0: notice: include test disabled due to test set up error(s): new exit code: $EXIT_CODE" 1>&2
    INCLUDE_TEST_SUCCESS="false"
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
compile_test()
{

    # parse args
    #
    if [[ $# -ne 3 ]]; then
	echo "$0: ERROR: function expects 3 args, found $#" 1>&2
	return 1
    fi
    local CODE="$1"
    local SOURCE="$2"
    local OUTPUT="$3"

    if [[ ! -e $SOURCE ]]; then
	echo "$0: ERROR: source file does not exist: $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
	return 1
    fi

    if [[ ! -f $SOURCE ]]; then
	echo "$0: ERROR: source file not a regular file: $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
	return 1
    fi

    if [[ ! -r $SOURCE ]]; then
	echo "$0: ERROR: source file not readable: $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
	return 1
    fi

    $CC -o "$OUTPUT" "$SOURCE"
    if [[ ! -e $OUTPUT ]]; then
	echo "$0: ERROR: could not create executable from $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
	return 1
    fi
    if [[ ! -x $WORK_FILE ]]; then
	echo "$0: ERROR: could not create executable from $SOURCE: new exit code: $CODE" 1>&2
	EXIT_CODE="$CODE"
    fi

    return 0
}


# pre-errno test: get value of ENOENT
#
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating binary that should print the value of ENOENT" 1>&2
fi
cat <<EOF >"$WORK_DIR/pre-errno.c"
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
if compile_test 21 "$WORK_DIR/pre-errno.c" "$WORK_FILE"; then	# compile failure will exit 21 unless EXIT_CODE is changed
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
    echo "$0: creating binary that should print: errno: $ENOENT" 1>&2
fi
cat <<EOF >"$WORK_DIR/errno0.c"
#include <errno.h>
#include <stdio.h>
int
main(void)
{
    errno = ENOENT;

    (void) printf("errno: %d\\n", errno);
    return 0;
}
EOF
WORK_FILE="$WORK_DIR/errno0"
if compile_test 22 "$WORK_DIR/errno0.c" "$WORK_FILE"; then	# compile failure will exit 22 unless EXIT_CODE is changed
    if ! "$WORK_FILE" | grep -q "errno: $ENOENT"; then
	EXIT_CODE=23	# will exit 23 at the end unless EXIT_CODE is changed later on
	echo "$0: ERROR: inconsistent ENOENT value: did not get: \"errno: $ENOENT\": new exit code: $EXIT_CODE" 1>&2
    elif [[ $V_FLAG -gt 1 ]]; then
	echo "$0: got: errno: $ENOENT" 1>&2
    fi
fi

# errno1: create source file that when compiled should return 1 (errno is set to
# ENOENT so errno == ENOENT should be 1)
#
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating binary that should verify errno == $ENOENT, returning 1" 1>&2
fi
cat<<EOF >"$WORK_DIR/errno1.c"
#include <errno.h>
int
main(void)
{
    errno = ENOENT;

    return errno == ENOENT;
}
EOF
WORK_FILE="$WORK_DIR/errno1"
if compile_test 24 "$WORK_DIR/errno1.c" "$WORK_FILE"; then	# compile failure will exit 24 unless EXIT_CODE is changed
    "$WORK_FILE"
    status=$?
    if [[ $status -ne 1 ]] ; then
	EXIT_CODE=25	# will exit 25 at the end unless EXIT_CODE is changed later on
	echo "$0: ERROR: errno != ENOENT: new exit code: $EXIT_CODE" 1>&2
    elif [[ $V_FLAG -gt 1 ]]; then
	echo "$0: got return value 1: errno == ENOENT"
    fi
fi

# hello.c: print "Hello, world".
#
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating binary that should print: "Hello, world"" 1>&2
fi
cat <<EOF >"$WORK_DIR/hello.c"
#include <stdio.h>
int
main(void)
{
    (void) printf("Hello, world\\n");
    return 0;
}
EOF
WORK_FILE="$WORK_DIR/hello"
if compile_test 26 "$WORK_DIR/hello.c" "$WORK_FILE"; then	# compile failure will exit 26 unless EXIT_CODE is changed
    if ! "$WORK_FILE" | grep -q "Hello, world"; then
	EXIT_CODE=27	# will exit 27 at the end unless EXIT_CODE is changed later on
	echo "$0: ERROR: expected string "Hello, world" not found: new exit code: $EXIT_CODE" 1>&2
    elif [[ $V_FLAG -gt 1 ]]; then
	echo "$0: got: "Hello, world"" 1>&2
    fi
fi

# main0: create binary file that should return 0
#
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating binary that should simply return 0" 1>&2
fi
cat <<EOF >"$WORK_DIR/main0.c"
int
main(void)
{
    return 0;
}
EOF
WORK_FILE="$WORK_DIR/main0"
if compile_test 28 "$WORK_DIR/main0.c" "$WORK_FILE"; then	# compile failure will exit 28 unless EXIT_CODE is changed
    if ! "$WORK_FILE"; then
	status=$?
	EXIT_CODE=29	# will exit 29 at the end unless EXIT_CODE is changed later on
	echo "$0: ERROR: expected return value 0 not returned: got $status: new exit code: $EXIT_CODE" 1>&2
    elif [[ $V_FLAG -gt 1 ]]; then
	echo "$0: got return value 0" 1>&2
    fi
fi


# main1: create binary file that should return 1
#
if [[ $V_FLAG -gt 1 ]]; then
    echo "$0: creating binary that should simply return 1" 1>&2
fi
cat <<EOF >"$WORK_DIR/main1.c"
int
main(void)
{
    return 1;
}
EOF
WORK_FILE="$WORK_DIR/main1"
if compile_test 30 "$WORK_DIR/main1.c" "$WORK_FILE"; then	# compile failure will exit 30 unless EXIT_CODE is changed
    if "$WORK_FILE"; then
	status=$?
	EXIT_CODE=31	# will exit 31 at the end unless EXIT_CODE is changed later on
	echo "$0: ERROR: expected return value 1 not returned: got $status: new exit code: $EXIT_CODE" 1>&2
    elif [[ $V_FLAG -gt 1 ]]; then
	echo "$0: got return value 1" 1>&2
    fi
fi

# remove work directory
#
rm -rf "$WORK_DIR"

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
exit "$EXIT_CODE"
