#!/usr/bin/env bash
#
# ioccc_test.sh - perform the complete suite of tests for the mkiocccentry repo
#
# This script was co-developed in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# Share and enjoy! :-)


# setup
#
export IOCCC_TEST_VERSION="1.0.4 2025-0l-11"

# attempt to fetch system specific path to the tools we need
#

# get tar path
TAR="$(type -P tar 2>/dev/null)"
# Make sure TAR is set:
#
# It's possible that the path could not be obtained so we set it to the default
# in this case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# could make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${TAR:=/usr/bin/tar} 2>/dev/null
#
# but due to the reasons cited above we must rely on the more complicated form:
if [[ -z "$TAR" ]]; then
    TAR="/usr/bin/tar"
fi

# get make path
MAKE="$(type -P make 2>/dev/null)"
# Make sure MAKE is set:
#
# It's possible that the path could not be obtained so we set it to the default
# in this case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# could make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${MAKE:=/usr/bin/make} 2>/dev/null
#
# but due to the reasons cited above we must rely on the more complicated form:
if [[ -z "$MAKE" ]]; then
    MAKE="/usr/bin/make"
fi


export USAGE="usage: $0 [-h] [-v level] [-J json_level] [-t tar] [-m make] [-V] [-Z topdir]

    -h              print help and exit
    -v level        set debug level (def: 0)
    -J json_level   set json debug level (def: 0)
    -t tar	    path to tar that accepts -J option (def: $TAR)
    -m make         path to GNU compatible make (def: $MAKE)
    -V              print version and exit
    -Z topdir	    top level build directory (def: try . or ..)

Exit codes:
     0   all OK
     2   -h and help string printed or -V and version string printed
     3   command line usage error
     4	 could not create a writable log file
     5   something not found, not the right file type, or not executable
 >= 20   some test failed

ioccc_test.sh version: $IOCCC_TEST_VERSION"

export V_FLAG="0"
export J_FLAG="0"
export EXIT_CODE="0"
export FAILURE_SUMMARY=
export LOGFILE="test_ioccc/test_ioccc.log"
export TOPDIR=

# parse args
#
while getopts :hv:J:VZ:t:m: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    J)	J_FLAG="$OPTARG";
	;;
    V)	echo "$IOCCC_TEST_VERSION"
	exit 2
	;;
    Z)  TOPDIR="$OPTARG";
        ;;
    t)	TAR="$OPTARG";
	;;
    m)  MAKE="$OPTARG";
        ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
    :)	echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
   *)
	;;
    esac
done

# check args
#
shift $(( OPTIND - 1 ));
if [[ $# -gt 0 ]]; then
    echo "$0: ERROR: expected 0 args, found: $#" 1>&2
    exit 3
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
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: TOPDIR is the current directory: $TOPDIR" 1>&2
fi

# clear log file
#
rm -f "$LOGFILE"

# try creating a new log file
#
touch "$LOGFILE"
if [[ ! -e "$LOGFILE" ]]; then
    echo "$0: ERROR: couldn't create log file: $LOGFILE" 1>&2
    exit 4
fi
if [[ ! -w "$LOGFILE" ]]; then
    echo "$0: ERROR: log file is not writable: $LOGFILE" 1>&2
    exit 4
fi


# firewall - verify we have the required executables and needed data file(s)
#
# NOTE: we check for everything before exiting so that the user can look at the
# log file (or stdout) and know which files are missing. This way they don't
# have to repeatedly run it to get all the error messages.
echo "Checking for required files and directories" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"


# iocccsize_test.sh
if [[ ! -e test_ioccc/iocccsize_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/iocccsize_test.sh file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f test_ioccc/iocccsize_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/iocccsize_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x test_ioccc/iocccsize_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/iocccsize_test.sh is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# dbg_test
if [[ ! -e dbg/dbg_test ]]; then
    echo "$0: ERROR: dbg_test file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f dbg/dbg_test ]]; then
    echo "$0: ERROR: dbg_test is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x dbg/dbg_test ]]; then
    echo "$0: ERROR: dbg_test is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# mkiocccentry_test.sh
if [[ ! -e test_ioccc/mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/mkiocccentry_test.sh file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f test_ioccc/mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/mkiocccentry_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x test_ioccc/mkiocccentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/mkiocccentry_test.sh is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# jnum_chk
if [[ ! -e jparse/test_jparse/jnum_chk ]]; then
    echo "$0: ERROR: jparse/test_jparse/jnum_chk file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f jparse/test_jparse/jnum_chk ]]; then
    echo "$0: ERROR: jparse/test_jparse/jnum_chk is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x jparse/test_jparse/jnum_chk ]]; then
    echo "$0: ERROR: jparse/test_jparse/jnum_chk is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# dyn_test
if [[ ! -e dyn_array/dyn_test ]]; then
    echo "$0: ERROR: dyn_array/dyn_test file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f dyn_array/dyn_test ]]; then
    echo "$0: ERROR: dyn_array/dyn_test is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x dyn_array/dyn_test ]]; then
    echo "$0: ERROR: dyn_array/dyn_test is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# jparse_test.sh
if [[ ! -e jparse/test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f jparse/test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x jparse/test_jparse/jparse_test.sh ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# json_teststr.txt: for jparse_test.sh
if [[ ! -e jparse/test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/json_teststr.txt file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f jparse/test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -r jparse/test_jparse/json_teststr.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/json_teststr.txt is not readable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# json_teststr_fail.txt: for jparse_test.sh
if [[ ! -e jparse/test_jparse/json_teststr_fail.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/json_teststr_fail.txt file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f jparse/test_jparse/json_teststr_fail.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -r jparse/test_jparse/json_teststr_fail.txt ]]; then
    echo "$0: ERROR: jparse/test_jparse/json_teststr_fail.txt is not readable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# txzchk
if [[ ! -e txzchk ]]; then
    echo "$0: ERROR: txzchk file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f txzchk ]]; then
    echo "$0: ERROR: txzchk is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x txzchk ]]; then
    echo "$0: ERROR: txzchk is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# chkentry_test.sh
if [[ ! -e test_ioccc/chkentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/chkentry_test.sh file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f test_ioccc/chkentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/chkentry_test.sh is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x test_ioccc/chkentry_test.sh ]]; then
    echo "$0: ERROR: test_ioccc/chkentry_test.sh is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# test_JSON
if [[ ! -e ./test_ioccc/test_JSON ]]; then
    echo "$0: ERROR: ./test_JSON file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -d ./test_ioccc/test_JSON ]]; then
    echo "$0: ERROR: ./test_ioccc/test_JSON is not a directory" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -r ./test_ioccc/test_JSON ]]; then
    echo "$0: ERROR: ./test_ioccc/test_JSON is not readable directory" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# chkentry
if [[ ! -e chkentry ]]; then
    echo "$0: ERROR: chkentry file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f chkentry ]]; then
    echo "$0: ERROR: chkentry is not a regular file" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x chkentry ]]; then
    echo "$0: ERROR: chkentry is not executable" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi
# tar
if [[ ! -e "$TAR" ]]; then
    echo "$0: ERROR: tar file not found: $TAR" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -f "$TAR" ]]; then
    echo "$0: ERROR: tar is not a regular file: $TAR" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -x "$TAR" ]]; then
    echo "$0: ERROR: tar is not executable: $TAR" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi

# test_txzchk
if [[ ! -e test_ioccc/test_txzchk ]]; then
    echo "$0: ERROR: test_ioccc/test_txzchk file not found" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -d test_ioccc/test_txzchk ]]; then
    echo "$0: ERROR: test_ioccc/test_txzchk is not a directory" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
elif [[ ! -r test_ioccc/test_txzchk ]]; then
    echo "$0: ERROR: test_ioccc/test_txzchk is not readable directory" | tee -a -- "$LOGFILE"
    EXIT_CODE="5"
fi

if [[ "$EXIT_CODE" -ne 0 ]]; then
    echo | tee -a -- "$LOGFILE"
    echo "$0: ERROR: cannot continue" | tee -a -- "$LOGFILE"
    exit "$EXIT_CODE"
fi

# start the test suite
#
echo "Start test suite" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"

# iocccsize_test.sh
#
echo "RUNNING: test_ioccc/iocccsize_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "test_ioccc/iocccsize_test.sh -v 1" | tee -a -- "$LOGFILE"
test_ioccc/iocccsize_test.sh -v 1 | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_ioccc/iocccsize_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_ioccc/iocccsize_test.sh non-zero exit code: $status"
    EXIT_CODE="20"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: test_ioccc/iocccsize_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: test_ioccc/iocccsize_test.sh" | tee -a -- "$LOGFILE"
fi

# dbg_test
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: dbg_test" | tee -a -- "$LOGFILE"
rm -f dbg_test.out
echo | tee -a -- "$LOGFILE"
echo "./dbg/dbg_test -e 2 foo bar baz >dbg_test.out 2>&1" | tee -a -- "$LOGFILE"
./dbg/dbg_test -e 2 foo bar baz >dbg_test.out 2>&1 | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 5 ]]; then
    echo "exit status of dbg_test: $status != 5";
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    exit status of dbg_test: $status != 5";
    EXIT_CODE="21"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: dbg_test" | tee -a -- "$LOGFILE"
else
    grep -q '^ERROR\[5\]: main: simulated error, foo: foo bar: bar: errno\[2\]: No such file or directory$' dbg_test.out
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: did not find the correct dbg_test error message" 1>&2 | tee -a -- "$LOGFILE"
	echo "$0: ERROR: beginning dbg_test.out contents" 1>&2 | tee -a -- "$LOGFILE"
	< dbg_test.out tee -a -- "$LOGFILE"
	echo "$0: ERROR: dbg_test.out contents complete" 1>&2 | tee -a -- "$LOGFILE"
	FAILURE_SUMMARY="$FAILURE_SUMMARY
    did not find the correct dbg_test error message"
	EXIT_CODE="22"
	echo | tee -a -- "$LOGFILE"
	echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
	echo | tee -a -- "$LOGFILE"
	echo "FAILED: dbg_test" | tee -a -- "$LOGFILE"
    else
	echo | tee -a -- "$LOGFILE"
	echo "PASSED: dbg_test" | tee -a -- "$LOGFILE"
	rm -f dbg_test.out
    fi
fi

# mkiocccentry_test.sh
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: test_ioccc/mkiocccentry_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "test_ioccc/mkiocccentry_test.sh -m $MAKE -Z $TOPDIR -t $TAR" | tee -a -- "$LOGFILE"
test_ioccc/mkiocccentry_test.sh -m "$MAKE" -Z "$TOPDIR" -t "$TAR" | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_ioccc/mkiocccentry_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_ioccc/mkiocccentry_test.sh non-zero exit code: $status"
    EXIT_CODE="23"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: test_ioccc/mkiocccentry_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: test_ioccc/mkiocccentry_test.sh" | tee -a -- "$LOGFILE"
fi

# we do not run jstr_test.sh because make test in jparse/test_jparse will run
# this and it has the correct paths (this is a workaround for the fact that
# jparse/ is a subdirectory here but obviously not in the jparse repo).

# we also do not run jnum_chk because make test in jparse/test_jparse will run
# this and it has the correct paths (this is a workaround for the fact that
# jparse/ is a subdirectory here but obviously not in the jparse repo).

# we also do not run jparse_test.sh for jparse/test_jparse/test_JSON because
# make test in jparse/test_jparse will run this and it has the correct paths
# (this is a workaround for the fact that jparse/ is a subdirectory here but
# obviously not in the jparse repo).

# dyn_test
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: dyn_array/dyn_test" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "dyn_array/dyn_test" | tee -a -- "$LOGFILE"
dyn_array/dyn_test | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: dyn_array/dyn_test non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    dyn_array/dyn_test non-zero exit code: $status"
    EXIT_CODE="24"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: dyn_array/dyn_test" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: dyn_array/dyn_test" | tee -a -- "$LOGFILE"
fi

# jparse_test.sh for general.json
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: jparse/test_jparse/jparse_test.sh for general.json" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "jparse/test_jparse/jparse_test.sh -J $V_FLAG -Z $TOPDIR -L -d ./test_ioccc/test_JSON -s general.json -j jparse/jparse jparse/test_jparse/json_teststr.txt" | tee -a -- "$LOGFILE"
jparse/test_jparse/jparse_test.sh -J "$V_FLAG" -Z "$TOPDIR" -L -d ./test_ioccc/test_JSON -s general.json -j jparse/jparse jparse/test_jparse/json_teststr.txt | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: jparse/test_jparse/jparse_test.sh for general.json non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    jparse/test_jparse/jparse_test.sh for general.json non-zero exit code: $status"
    EXIT_CODE="25"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: jparse/test_jparse/jparse_test.sh for general.json" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: jparse/test_jparse/jparse_test.sh for general.json" | tee -a -- "$LOGFILE"
fi

# NOTE: do NOT test the test_ioccc/test_JSON/{auth,info}.json/ files with
# jparse_test.sh because these files are valid JSON files. The bad files are
# meant to be valid JSON but invalid per the chkentry rules. Thus the
# chkentry_test.sh later in this file addresses these files.


# txzchk_test.sh
#
# NOTE: we have to include ./ before the test_txzchk because the error files in
# the test_txzchk/bad subdirectory all have that form. This is a stylistic
# choice that can be changed if so desired but I have the ./ to match the rest
# of the command line.
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: test_ioccc/txzchk_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "test_ioccc/txzchk_test.sh -t $TAR -T ./txzchk -F ./test_ioccc/fnamchk -d test_ioccc/test_txzchk -Z $TOPDIR" | tee -a -- "$LOGFILE"
test_ioccc/txzchk_test.sh -t "$TAR" -T ./txzchk -F ./test_ioccc/fnamchk -d ./test_ioccc/test_txzchk -Z "$TOPDIR" | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_ioccc/txzchk_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_ioccc/txzchk_test.sh non-zero exit code: $status"
    EXIT_CODE="26"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: test_ioccc/txzchk_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: test_ioccc/txzchk_test.sh" | tee -a -- "$LOGFILE"
fi

# chkentry_test.sh
#
echo | tee -a -- "$LOGFILE"
echo "RUNNING: test_ioccc/chkentry_test.sh" | tee -a -- "$LOGFILE"
echo | tee -a -- "$LOGFILE"
echo "test_ioccc/chkentry_test.sh -v 1 -d ./test_ioccc/test_JSON -c ./chkentry" | tee -a -- "$LOGFILE"
test_ioccc/chkentry_test.sh -v 1 -d ./test_ioccc/test_JSON -c ./chkentry | tee -a -- "$LOGFILE"
status="${PIPESTATUS[0]}"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: test_ioccc/chkentry_test.sh non-zero exit code: $status" 1>&2 | tee -a -- "$LOGFILE"
    FAILURE_SUMMARY="$FAILURE_SUMMARY
    test_ioccc/chkentry_test.sh non-zero exit code: $status"
    EXIT_CODE="27"
    echo | tee -a -- "$LOGFILE"
    echo "EXIT_CODE set to: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "FAILED: test_ioccc/chkentry_test.sh" | tee -a -- "$LOGFILE"
else
    echo | tee -a -- "$LOGFILE"
    echo "PASSED: test_ioccc/chkentry_test.sh" | tee -a -- "$LOGFILE"
fi

# report overall status
#
if [[ $EXIT_CODE -ne 0 ]]; then
    echo | tee -a -- "$LOGFILE"
    echo "These test(s) failed:" | tee -a -- "$LOGFILE"
    echo "$FAILURE_SUMMARY" | tee -a -- "$LOGFILE"
    echo | tee -a -- "$LOGFILE"
    echo "About to exit: $EXIT_CODE" | tee -a -- "$LOGFILE"
    echo
    echo "See $LOGFILE for more details"
    echo
fi
exit "$EXIT_CODE"
