#!/usr/bin/env bash
#
# legacy_os.sh - JSON string encoding and decoding test

# setup
#
export C_SRC="legacy_os.c"
export PROG="./legacy_os"
export V_FLAG="0"

# parse args
#
while getopts :hv: flag; do
    case "$flag" in
    h) echo "usage: $0 [-h] [-v level]" 1>&2
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    \?) echo "invalid option: -$OPTARG" 1>&2
       exit 3
       ;;
    :) echo "option -$OPTARG requires an argument" 1>&2
       exit 4
       ;;
   *)
       ;;
    esac
done

# verify we have source to compile
#
if [[ ! -e $C_SRC ]]; then
    echo "$0: missing legacy_os.c: $C_SRC" 1>&2
    exit 100
fi
if [[ ! -f $C_SRC ]]; then
    echo "$0: legacy_os.c is not a file: $C_SRC" 1>&2
    exit 101
fi
if [[ ! -r $C_SRC ]]; then
    echo "$0: legacy_os.c is not a readable file: $C_SRC" 1>&2
    exit 102
fi

# prep for compile
#
rm -f "$PROG"

# compile test code
#
cc -std=gnu11 -Wall -Wextra -Werror -pedantic "$C_SRC" -o "$PROG" >/dev/null 2>&1
status="$?"
if [[ $status -ne 0 ]]; then
   echo "legacy_os"
elif [[ ! -x "$PROG" ]]; then
   echo "legacy_os"

# execute test code
#
else
    "$PROG" 'Mon Aug 15 21:51:43 2022 UTC' >/dev/null 2>&1
    status="$?"
    if [[ $status -ne 0 ]]; then
       echo "legacy_os"
    fi
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
