#!/usr/bin/env bash
#
# reset_tstamp.sh - reset the minimum timestamp: MIN_TIMESTAMP
#
# Only run this script when you wish to invalidate all timestamps
# prior to now, such as when you make a fundamental change to a
# critical JSON format, or make a fundamental change the compressed
# tarball file structure, or make a critical change to limit_ioccc.h
# that is MORE restrictive.
#
# DO NOT run this script simply for a new IOCCC!
#
# Yes, we make it very hard to run this script for good reason.
# Only IOCCC judges can perform ALL the steps needed to complete this action.
#
# Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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
# LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
# EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
# USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#
# chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# Share and enjoy! :-)


# setup
#
export USAGE="usage: $0 [-h] [-v level] [-V] [-l limit_ioccc.h]

    -h              print help and exit 8
    -v level        set debug level (def: 0)
    -V              print version and exit 8
    -l limit_ioccc.h   limit file (def: ./limit_ioccc.h)

Exit codes:
    0    timestamp updated

    1    failed Phase 0 of verification
    2    failed Phase 1 of verification
    3    failed Phase 2 of verification
    4    failed Phase 3 of verification
    5    failed Phase 4 of verification

    6    rpl failed to modify limit_ioccc.h

    7    limit_ioccc.h not found or not readable and writable

    8    -h and help string printed or -V and version string printed
    9    Command line usage error

    >=10  internal error"
export RESET_TSTAMP_VERSION="0.4 2022-04-23"
export V_FLAG="0"
export LIMIT_IOCCC_H="./limit_ioccc.h"

# parse args
#
while getopts :hv:Vl: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 8
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    V) echo "$RESET_TSTAMP_VERSION"
       exit 8
       ;;
    l) LIMIT_IOCCC_H="$OPTARG";
       ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
       exit 9
       ;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
       exit 9
       ;;
   *)
       ;;
    esac
done
if [[ -z $LIMIT_IOCCC_H ]]; then
    echo "$0: ERROR: -b $LIMIT_IOCCC_H name cannot be empty" 1>&2
    exit 9
fi

# check args
#
shift $(( OPTIND - 1 ));
if [[ $# -gt 0 ]]; then
    echo "$0: ERROR: Expected 0 args, found: $#" 1>&2
    exit 9
fi

# firewall
#
if [[ ! -e $LIMIT_IOCCC_H ]]; then
    echo "$0: ERROR: limit_ioccc.h file not found: $LIMIT_IOCCC_H" 1>&2
    exit 7
fi
if [[ ! -f $LIMIT_IOCCC_H ]]; then
    echo "$0: ERROR: limit_ioccc.h not a regular file: $LIMIT_IOCCC_H" 1>&2
    exit 7
fi
if [[ ! -r $LIMIT_IOCCC_H ]]; then
    echo "$0: ERROR: limit_ioccc.h not a readable file: $LIMIT_IOCCC_H" 1>&2
    exit 7
fi
if [[ ! -w $LIMIT_IOCCC_H ]]; then
    echo "$0: ERROR: limit_ioccc.h not a writable file: $LIMIT_IOCCC_H" 1>&2
    exit 7
fi

# debug
#
if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: RESET_TSTAMP_VERSION=$RESET_TSTAMP_VERSION" 1>&2
    echo "$0: debug[5]: LIMIT_IOCCC_H=$LIMIT_IOCCC_H" 1>&2
fi

# verify that we have the rpl command
#
RPL_CMD=$(type -P rpl)
if [[ -z "$RPL_CMD" ]]; then
    echo "$0: ERROR: rpl not found" 1>&2
    echo "$0: ERROR: If do not have the rpl tool, then you may not perform this action." 1>&2
    exit 1
fi
GDATE_CMD=$(type -P gdate)
if [[ -z "$GDATE_CMD" ]]; then
    echo "$0: ERROR: date not found" 1>&2
    echo "$0: ERROR: If do not have the rpl tool, then you may not perform this action." 1>&2
    exit 1
fi

# Phase 0 of verification
#
echo
echo 'Yes, we make it very hard to run this script for good reason.'
echo 'Only IOCCC judges can perform the ALL the steps needed to complete this action.'
echo
echo 'WARNING: This script will invalidate all timestamps prior to now.'
echo 'WARNING: Only run this script when you wish to invalidate all timestamps'
echo 'WARNING: because you made a fundamental change to a critical JSON format,'
echo 'WARNING: made a fundamental change to the compressed tarball file structure,'
echo 'WARNING: or made a critical change to limit_ioccc.h that is MORE restrictive.'
echo
echo 'WARNING: DO NOT run this script simply for a new IOCCC!'
echo
echo 'WARNING: If you wish to do this, please enter the following phrase:'
echo
echo '    Please invalidate all IOCCC timestamps prior to now'
echo
echo -n 'Enter the phrase: '
read -r ANSWER
if [[ "$ANSWER" != 'Please invalidate all IOCCC timestamps prior to now' ]]; then
    echo "$0: notice: Wise choice, MIN_TIMESTAMP was not changed." 1>&2
    exit 2
fi

# Phase 1 of verification
#
echo
echo 'WARNING: Are you sure you want to invalidate all existing compressed tarballs'
echo 'WARNING: and all prior .info.JSON files that have been made,'
echo 'WARNING: and all prior .author.JSON files that have been made,'
echo 'WARNING: forcing everyone to rebuild their compressed tarball entries?'
echo
echo 'WARNING: If you really wish to do this, please enter the following phrase:'
echo
echo '    I understand this'
echo
echo 'WARNING: followed by the phrase:'
echo
echo '    and apologise to those with existing IOCCC compressed tarballs'
echo
echo -n 'Enter that phrase: '
read -r ANSWER
if [[ "$ANSWER" != 'I understand this and apologise to those with existing IOCCC compressed tarballs' ]]; then
    echo "$0: notice: Wise choice, MIN_TIMESTAMP was not changed." 1>&2
    exit 3
fi

# Phase 2 of verification
#
echo
echo 'Enter the phrase that is required (even if you are from that very nice place).'
echo
echo -n 'Enter the required phrase: '
read -r ANSWER
if [[ "$ANSWER" != 'Sorry (tm Canada) :=)' ]]; then
    echo "$0: notice: Wise choice, MIN_TIMESTAMP was not changed." 1>&2
    exit 4
fi

# Phase 3 of verification
#
echo
echo -n 'WARNING: Enter the existing value of MIN_TIMESTAMP: '
read -r OLD_MIN_TIMESTAMP
FORMED_OLD_NOW="$($GDATE_CMD -u --date=@"$OLD_MIN_TIMESTAMP" '+%a %b %d %H:%M:%S %Y UTC')"
if [[ -z $FORMED_OLD_NOW ]]; then
    echo "$0: ERROR: cannot convert OLD_MIN_TIMESTAMP to time string" 1>&2
    exit 5
fi
export FORMED_OLD_NOW
export NOW
NOW="$(date '+%s')"
if grep -q "$OLD_MIN_TIMESTAMP" "$LIMIT_IOCCC_H" >/dev/null 2>&1; then
    echo
    echo 'We guess that you do really really do want to change MIN_TIMESTAMP.'
    echo 'Enter Y if you really want to change the MIN_TIMESTAMP!'
    echo
else
    echo "$0: ERROR: Invalid value of MIN_TIMESTAMP" 1>&2
    exit 5
fi
FORMED_NOW="$($GDATE_CMD -u --date=@"$NOW" '+%a %b %d %H:%M:%S %Y UTC')"
if [[ -z $FORMED_NOW ]]; then
    echo "$0: ERROR: cannot convert MIN_TIMESTAMP to time string" 1>&2
    exit 5
fi
export FORMED_NOW

# Change the timestamp
#
"$RPL_CMD" -w -p \
   "#define MIN_TIMESTAMP ((time_t)$OLD_MIN_TIMESTAMP)" \
   "#define MIN_TIMESTAMP ((time_t)$NOW)" "$LIMIT_IOCCC_H"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: rpl failed to modify $LIMIT_IOCCC_H: error code: $status" 1>&2
    exit 6
fi
echo
echo "FYI:"
echo
echo "OLD_MIN_TIMESTAMP=$OLD_MIN_TIMESTAMP"
echo "FORMED_OLD_NOW=$FORMED_OLD_NOW"
echo "NOW=$NOW"
echo "FORMED_NOW=$FORMED_NOW"
echo
echo "This line in $LIMIT_IOCCC_H as it exists now, is:"
echo
grep '^#define MIN_TIMESTAMP' "$LIMIT_IOCCC_H"
echo
echo "$0: notice: You still need to:"
echo
echo '    make clobber all test'
echo
echo 'then:'
echo
echo "	  make ./limit_ioccc.sh"
echo "    ./vermod.sh -v 1 -n -Q $OLD_MIN_TIMESTAMP $NOW"
echo "    $RPL_CMD -s -x'.json' -R -- '$FORMED_OLD_NOW' '$FORMED_NOW' ./test_JSON"
echo
echo 'If all is well, then:'
echo
echo "    ./vermod.sh -v 1 -Q $OLD_MIN_TIMESTAMP $NOW"
echo "    $RPL_CMD -x'.json' -R -- '$FORMED_OLD_NOW' '$FORMED_NOW' ./test_JSON"
echo
echo "$0: notice: And if all is well, commit and push the change to the GitHub repo!"
echo

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
