#!/usr/bin/env bash
#
# cpath - canonicalize paths
#
# "Not all those who wander are lost."
#
#      -- J.R.R. Tolkien
#
# Copyright (c) 1991-2025 by Landon Curt Noll.  All Rights Reserved.
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby granted,
# provided that the above copyright, this permission notice and text
# this comment, and the disclaimer below appear in all of the following:
#
#       supporting documentation
#       source copies
#       source works derived from this source
#       binaries derived from this source or from derived source
#
# THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
# ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
# DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# This code was developed between 1991-2025 by Landon Curt Noll:
#
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# Share and enjoy! :-)


# setup
#
export VERSION="1.0.0 2025-10-15"
NAME=$(basename "$0")
export NAME
#
export V_FLAG=0
#
export NOOP=
export DO_NOT_PROCESS=
#
if [[ -x ./cpath ]]; then
    CPATH="./cpath"
else
    CPATH=$(type -P cpath)
    export CPATH
    if [[ -z $CPATH ]]; then
	echo "$0: FATAL: cpath tool is not installed or not in \$PATH" 1>&2
	exit 5
    fi
fi
export CPATH
export IN="in"
export OUT="out"
export OUT_LOW="out.low"
export OUT_R="out.r"


# usage
#
export USAGE="usage: $0 [-h] [-v level] [-V] [-n] [-N] [-c cpath] [-i in] [-o out] [-l out.low] [-r out.r]

    -h          print help message and exit
    -v level    set verbosity level (def level: $V_FLAG)
    -V          print version string and exit

    -n          go thru the actions, but do not update any files (def: do the action)
    -N          do not process anything, just parse arguments (def: process something)

    -c cpath	path to cpath (def: $CPATH)
    -i in	cpath data input file (def: $IN)
    -o out	cpath data output comparison file (def: $OUT)
    -l out.low	cpath data output comparison file (def: $OUT_LOW)
    -r out.r	cpath data output comparison file (def: $OUT_R)

Exit codes:
     0         all OK
     1	       some test failed
     2         -h and help string printed or -V and version string printed
     3         command line error
     5	       some internal tool or file is not found, or is not an executable file
 >= 10         internal error

$NAME version: $VERSION"


# parse command line
#
while getopts :hv:VnNc:i:o:l: flag; do
  case "$flag" in
    h) echo "$USAGE"
	exit 2
	;;
    v) V_FLAG="$OPTARG"
	;;
    V) echo "$VERSION"
	exit 2
	;;
    n) NOOP="-n"
	;;
    N) DO_NOT_PROCESS="-N"
	;;
    c) CPATH="$OPTARG"
        ;;
    i) IN="$OPTARG"
        ;;
    o) OUT="$OPTARG"
        ;;
    l) OUT_LOW="$OPTARG"
        ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
    *) echo "$0: ERROR: unexpected value from getopts: $flag" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
  esac
done
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: debug level: $V_FLAG" 1>&2
fi
#
# remove the options
#
shift $(( OPTIND - 1 ));
#
# verify arg count
#
if [[ $# -ne 0 ]]; then
    echo "$0: ERROR: expected 0 args, found: $#" 1>&2
    echo "$USAGE" 1>&2
    exit 3
fi


# verify that the cpath tool is executable
#
if [[ ! -e $CPATH ]]; then
    echo  "$0: ERROR: cpath does not exist: $CPATH" 1>&2
    exit 5
fi
if [[ ! -f $CPATH ]]; then
    echo  "$0: ERROR: cpath is not a regular file: $CPATH" 1>&2
    exit 5
fi
if [[ ! -x $CPATH ]]; then
    echo  "$0: ERROR: cpath is not an executable file: $CPATH" 1>&2
    exit 5
fi


# verify in and out files are readable
#
if [[ ! -e $IN ]]; then
    echo  "$0: ERROR: in does not exist: $IN" 1>&2
    exit 5
fi
if [[ ! -f $IN ]]; then
    echo  "$0: ERROR: in is not a regular file: $IN" 1>&2
    exit 5
fi
if [[ ! -r $IN ]]; then
    echo  "$0: ERROR: in is not a readable file: $IN" 1>&2
    exit 5
fi
if [[ ! -e $OUT ]]; then
    echo  "$0: ERROR: out does not exist: $OUT" 1>&2
    exit 5
fi
if [[ ! -f $OUT ]]; then
    echo  "$0: ERROR: out is not a regular file: $OUT" 1>&2
    exit 5
fi
if [[ ! -r $OUT ]]; then
    echo  "$0: ERROR: out is not a readable file: $OUT" 1>&2
    exit 5
fi
if [[ ! -e $OUT_LOW ]]; then
    echo  "$0: ERROR: out.low does not exist: $OUT_LOW" 1>&2
    exit 5
fi
if [[ ! -f $OUT_LOW ]]; then
    echo  "$0: ERROR: out.low is not a regular file: $OUT_LOW" 1>&2
    exit 5
fi
if [[ ! -r $OUT_LOW ]]; then
    echo  "$0: ERROR: out.low is not a readable file: $OUT_LOW" 1>&2
    exit 5
fi
if [[ ! -e $OUT_R ]]; then
    echo  "$0: ERROR: out.r does not exist: $OUT_R" 1>&2
    exit 5
fi
if [[ ! -f $OUT_R ]]; then
    echo  "$0: ERROR: out.r is not a regular file: $OUT_R" 1>&2
    exit 5
fi
if [[ ! -r $OUT_R ]]; then
    echo  "$0: ERROR: out.r is not a readable file: $OUT_R" 1>&2
    exit 5
fi


# form temporary file to capture cpath stdout
#
export TMP_STDOUT=".tmp.$NAME.stdout.$$.tmp"
if [[ $V_FLAG -ge 3 ]]; then
    echo  "$0: debug[3]: temporary stdout file: $TMP_STDOUT" 1>&2
fi
trap 'rm -f $TMP_STDOUT; exit' 0 1 2 3 15
rm -f "$TMP_STDOUT"
if [[ -e $TMP_STDOUT ]]; then
    echo "$0: ERROR: cannot remove temporary stdout file: $TMP_STDOUT" 1>&2
    exit 10
fi
:> "$TMP_STDOUT"
if [[ ! -e $TMP_STDOUT ]]; then
    echo "$0: ERROR: cannot create temporary stdout file: $TMP_STDOUT" 1>&2
    exit 11
fi


# form temporary file to capture cpath stderr
#
export TMP_STDERR=".tmp.$NAME.stderr.$$.tmp"
if [[ $V_FLAG -ge 3 ]]; then
    echo  "$0: debug[3]: temporary stderr file: $TMP_STDERR" 1>&2
fi
trap 'rm -f $TMP_STDOUT $TMP_STDERR; exit' 0 1 2 3 15
rm -f "$TMP_STDERR"
if [[ -e $TMP_STDERR ]]; then
    echo "$0: ERROR: cannot remove temporary stderr file: $TMP_STDERR" 1>&2
    exit 12
fi
:> "$TMP_STDERR"
if [[ ! -e $TMP_STDERR ]]; then
    echo "$0: ERROR: cannot create temporary stderr file: $TMP_STDERR" 1>&2
    exit 13
fi


# print running info if verbose
#
# If -v 3 or higher, print exported variables in order that they were exported.
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: VERSION=$VERSION" 1>&2
    echo "$0: debug[3]: NAME=$NAME" 1>&2
    echo "$0: debug[3]: V_FLAG=$V_FLAG" 1>&2
    echo "$0: debug[3]: NOOP=$NOOP" 1>&2
    echo "$0: debug[3]: DO_NOT_PROCESS=$DO_NOT_PROCESS" 1>&2
    echo "$0: debug[3]: CPATH=$CPATH" 1>&2
    echo "$0: debug[3]: IN=$IN" 1>&2
    echo "$0: debug[3]: OUT=$OUT" 1>&2
    echo "$0: debug[3]: OUT_LOW=$OUT_LOW" 1>&2
    echo "$0: debug[3]: OUT_R=$OUT_R" 1>&2
    echo "$0: debug[3]: TMP_STDOUT=$TMP_STDOUT" 1>&2
    echo "$0: debug[3]: TMP_STDERR=$TMP_STDERR" 1>&2
fi


# -N stops early before any processing is performed
#
if [[ -n $DO_NOT_PROCESS ]]; then
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: arguments parsed, -N given, exiting 0" 1>&2
    fi
    exit 0
fi


# test input file
#
export TST_STR
export EXPECTED_CODE
TST_STR="$CPATH < $IN"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" < "$IN" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    if ! cmp -s "$OUT" "$TMP_STDOUT"; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	diff -u "$OUT" "$TMP_STDOUT" 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test input file with lower case
#
TST_STR="$CPATH -l < $IN"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -l < "$IN" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    if ! cmp -s "$OUT_LOW" "$TMP_STDOUT"; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	diff -u "$OUT_LOW" "$TMP_STDOUT" 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test input file with -r (relative only) errors
#
TST_STR="$CPATH -r < $IN"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=1
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -r < "$IN" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    if ! cmp -s "$OUT_R" "$TMP_STDOUT"; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	diff -u "$OUT_R" "$TMP_STDOUT" 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test input file, which is all safe, for safe paths
#
TST_STR="$CPATH -s < $IN"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -s < "$IN" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    if ! cmp -s "$OUT" "$TMP_STDOUT"; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	diff -u "$OUT" "$TMP_STDOUT" 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# single arg test
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    echo "$P_OUT" | cmp -s "$TMP_STDOUT" -
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	echo "$P_OUT" | diff -u "$TMP_STDOUT" - 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_path for a short path
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -m 99 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -m 99 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    echo "$P_OUT" | cmp -s "$TMP_STDOUT" -
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	echo "$P_OUT" | diff -u "$TMP_STDOUT" - 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_file for a short path component
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -M 99 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -M 99 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    echo "$P_OUT" | cmp -s "$TMP_STDOUT" -
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	echo "$P_OUT" | diff -u "$TMP_STDOUT" - 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_depth for a shallow path depth
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -d 99 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -d 99 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    echo "$P_OUT" | cmp -s "$TMP_STDOUT" -
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	echo "$P_OUT" | diff -u "$TMP_STDOUT" - 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_path for a too long path
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -m 7 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=4
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -m 7 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_file for too long path components
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -M 2 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=5
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -M 2 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_depth for too deep paths
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -d 2 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=6
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -d 2 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test unsafe path #0
#
export P_STR
export P_OUT
P_STR="a/-bb/ccc"
P_OUT="a/bb/ccc"
TST_STR="$CPATH -s $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=7
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -s "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test unsafe path #1
#
export P_STR
export P_OUT
P_STR="a/b%b/ccc"
P_OUT="a/bb/ccc"
TST_STR="$CPATH -s $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=7
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -s "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_path for a too long path
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -m 7 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=4
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -m 7 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_file for too long path components
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -M 2 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=5
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -M 2 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test max_depth for too deep paths
#
export P_STR
export P_OUT
P_STR="./a/bb//ccc/./dddd/../."
P_OUT="a/bb/ccc"
TST_STR="$CPATH -d 2 $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=6
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -d 2 "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test unsafe path #0
#
export P_STR
export P_OUT
P_STR="a/-bb/ccc"
P_OUT="a/bb/ccc"
TST_STR="$CPATH -s $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=7
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -s "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test unsafe path #1
#
export P_STR
export P_OUT
P_STR="a/b%b/ccc"
P_OUT="a/bb/ccc"
TST_STR="$CPATH -s $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=7
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -s "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test alphanumeric safely check
#
export P_STR
export P_OUT
P_STR="./a/b2////c333/Four"
P_OUT="a/b2/c333/Four"
TST_STR="$CPATH -S '^[0-9A-Za-z]+$' $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -S '^[0-9A-Za-z]+$' "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    echo "$P_OUT" | cmp -s "$TMP_STDOUT" -
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	echo "$P_OUT" | diff -u "$TMP_STDOUT" - 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test alphanumeric safely check and lower case
#
export P_STR
export P_OUT
P_STR="./a/B2////C333/Four"
P_OUT="a/b2/c333/four"
TST_STR="$CPATH -S '^[0-9A-Za-z]+$' -l $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -S '^[0-9A-Za-z]+$' -l "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    echo "$P_OUT" | cmp -s "$TMP_STDOUT" -
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	echo "$P_OUT" | diff -u "$TMP_STDOUT" - 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# test alphanumeric safely check failure
#
export P_STR
export P_OUT
P_STR="./a/B2////C-333/Four"
P_OUT="a/b2/C-333/four"
TST_STR="$CPATH -S '^[0-9A-Za-z]+$' -l $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=7
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -S '^[0-9A-Za-z]+$' -l "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# single arg .. test #0
#
export P_STR
export P_OUT
P_STR="./../../a/bb/../../../ccc/./dddd/../."
P_OUT="../../../ccc"
TST_STR="$CPATH $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    echo "$P_OUT" | cmp -s "$TMP_STDOUT" -
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	echo "$P_OUT" | diff -u "$TMP_STDOUT" - 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# single arg .. test #1
#
export P_STR
export P_OUT
P_STR="/../../a/bb/../../../ccc/./dddd/../."
P_OUT="/ccc"
TST_STR="$CPATH $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=0
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ -s $TMP_STDERR ]]; then
	echo "$0: ERROR: unexpected output on stderr for: $TST_STR" 1>&2
	echo "$0: ERROR: unexpected stderr starts below:" 1>&2
	cat "$TMP_STDERR" 1>&2
	echo "$0: ERROR: unexpected stderr ends above:" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDOUT ]]; then
	echo "$0: ERROR: no output on stdout for: $TST_STR" 1>&2
	exit 1
    fi
    echo "$P_OUT" | cmp -s "$TMP_STDOUT" -
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: unexpected output for: $TST_STR" 1>&2
	echo "$0: ERROR: output difference starts below:" 1>&2
	echo "$P_OUT" | diff -u "$TMP_STDOUT" - 1>&2
	echo "$0: ERROR: output difference ends above:" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# single arg .. with -D test #0 failure
#
export P_STR
export P_OUT
P_STR="./../../a/bb/../../../ccc/./dddd/../."
P_OUT="../../../ccc"
TST_STR="$CPATH -D $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=8
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -D "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# single arg .. with -D test #1 failure
#
export P_STR
export P_OUT
P_STR="/../../a/bb/../../../ccc/./dddd/../."
P_OUT="/ccc"
TST_STR="$CPATH -D $P_STR"
if [[ -z $NOOP ]]; then

    # test setup
    #
    EXPECTED_CODE=8
    :>"$TMP_STDOUT"
    :>"$TMP_STDERR"
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: about to run: $TST_STR"
    fi

    # run test
    #
    "$CPATH" -D "$P_STR" > "$TMP_STDOUT" 2> "$TMP_STDERR"
    status="$?"

    # verify test results
    #
    if [[ $status -ne $EXPECTED_CODE ]]; then
	echo "$0: ERROR: $TST_STR failed, error: $status != $EXPECTED_CODE" 1>&2
	exit 1
    fi
    if [[ ! -s $TMP_STDERR ]]; then
	echo "$0: ERROR: no output on stderr for: $TST_STR" 1>&2
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: test OK: expected status: $EXPECTED_CODE $TST_STR"
    fi

elif [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: due to -n, the following test was bypassed: $TST_STR" 1>&2
fi


# All Done!!! -- Jessica Noll, Age 2
#
rm -f "$TMP_STDOUT" "$TMP_STDERR"
exit 0
