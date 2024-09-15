#!/usr/bin/env bash
#
# is_available.sh - check if a tool is available
#
# This script was written in 2024 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# Share and enjoy! :-)
#

export IS_AVAILABLE_VERSION="1.0.0 2024-09-15"

export USAGE="usage: $0 [-h] [-V] [-v level] tool_name

    -h			    print help and exit
    -V			    print version and exit
    -v level		    set verbosity level for this script: (def level: 0)

Exit codes:
     0   all OK
     1   tool not found
     2   -h and help string printed or -V and version string printed
     3   invalid command line
 >= 10   internal error or missing file or directory

is_available.sh version: $IS_AVAILABLE_VERSION"

# parse args
#
export V_FLAG="0"
while getopts :hVv: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$IS_AVAILABLE_VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
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
if [[ $# -ne 1 ]]; then
    echo "$0: ERROR: expected one arg, got $#" 1>&2
    echo 1>&2
    echo "$USAGE" 1>&2
    exit 3
elif [[ -z "$1" ]]; then
    echo "$0: ERROR: expected one non-empty arg" 1>&2
    echo 1>&2
    echo "$USAGE" 1>&2
    exit 3
fi

TOOL="$(type -P "$1")"
if [[ -n "$TOOL" ]]; then
    if [[ "$V_FLAG" -gt 0 ]]; then
	echo "$TOOL" 1>&2
    fi
    exit 0
else
    if [[ "$V_FLAG" -gt 0 ]]; then
	echo "\"$1\" not found" 1>&2
    fi
    exit 1
fi
