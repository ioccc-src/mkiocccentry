#!/usr/bin/env bash
#
# have_timegm.sh - see if we can compile have_timegm.c
#
# If we can compile have_timegm.c then we will output nothing and exit 0.
#
# If we encounter a compile warning/error while trying to compile have_timegm.c
# or if the output file is not executable or returns an error / does not exit 0
# we will output -DTIMEGM_PROBLEM and exit 0.
#
# All other errors will result in a non-zero exit.

# setup
#
export C_SRC="have_timegm.c"
export PROG="./have_timegm"
export V_FLAG="0"
export HAVE_TIMEGM_VERSION="0.0 2022-08-17"

export USAGE="usage: $0 [-h] [-V] [-v level]

    -h			    print help and exit 2
    -V			    print version and exit 2
    -v level		    set verbosity level for this script: (def level: 0)

exit codes:
    0	- all is well
    1	- invalid command line
    2	- strptime() returned NULL
    3	- strptime() return not '\\0'
    4	- strftime() failed
    5	- original time string and conversion mismatch
    100	- help or version mode used
    101 - invalid option or option missing an argument
    102 - missing have_timegm.c
    103 - have_timegm.c not a regular file
    104 - have_timegm.c not a readable file

$0 version: $HAVE_TIMEGM_VERSION"


# parse args
#
while getopts :hv:V flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 100
	;;
    v)	V_FLAG="$OPTARG";
	;;
    V)	echo "$0 version $HAVE_TIMEGM_VERSION" 1>&2
	exit 100
	;;
    \?)	echo "invalid option: -$OPTARG" 1>&2
	exit 101
	;;
    :)	echo "option -$OPTARG requires an argument" 1>&2
	exit 101
	;;
    *)
	;;
    esac
done

# verify we have source to compile
#
if [[ ! -e $C_SRC ]]; then
    echo "$0: missing have_timegm.c: $C_SRC" 1>&2
    exit 102
fi
if [[ ! -f $C_SRC ]]; then
    echo "$0: have_timegm.c is not a regular file: $C_SRC" 1>&2
    exit 103
fi
if [[ ! -r $C_SRC ]]; then
    echo "$0: have_timegm.c is not a readable file: $C_SRC" 1>&2
    exit 104
fi

# prep for compile
#
rm -f "$PROG"

# compile test code
#
cc -std=gnu11 -Wall -Wextra -Werror -pedantic "$C_SRC" -o "$PROG" >/dev/null 2>&1
status="$?"
if [[ $status -ne 0 ]]; then
   echo "-DTIMEGM_PROBLEM"
elif [[ ! -x "$PROG" ]]; then
   echo "-DTIMEGM_PROBLEM"

# execute test code
#
else
    "$PROG" 'Mon Aug 15 21:51:43 2022 UTC' >/dev/null 2>&1
    status="$?"
    if [[ $status -ne 0 ]]; then
       echo "-DTIMEGM_PROBLEM"
    fi
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit "$status"
