#!/usr/bin/env bash
#
# vermod - modify version strings (and others) under ./test_JSON
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
export USAGE="usage: $0 [-h] [-v level] [-d test_dir] [-i limit_ioccc.sh.sh]
	[-o] [-F] [-Q] [-n] [-l] [-L] old_ver new_ver

    -h            Print help and exit
    -v level      Set the debugging level (def: 0)

    -d test_dir   Process '*.json' files below dir (def: ./test_JSON)
    -i limit_ioccc.sh   Use limit.sh to verify new version (def: ./limit_ioccc.sh)

    -o            Verify old version (def: verify new version)
    -F            Force change, even if it cannot be verified (def: reject an unverified change)

    -Q            Do not double quite string (def: add double quotes around old_ver and new_ver when making changes)

    -n            No op: change no files (def: change files)
    -l            List files that change/will change (def: don not list)
                     NOTE: Useful with -n to show what would change
    -L            List files that do not/will not change (def: don not list)
                     NOTE: Useful with -n to show what will not change

    old_ver       Old version string to change from
    new_ver       New version string to change to

exit codes:
    0 - all is well
    1 - rpl exited non-zero
    2 - rpl command not found
    3 - no limit.sh file found, or is not readable
    4 - no test_dir directory found, or is not readable
    5 - usage message printed due to -h
    6 - command line error
    7 - no *.json files found under test_dir
    8 - new_ver (or old_ver if -o) not found in limit_ioccc.sh
    >= 9 - internal error"
export V_FLAG="0"
export JSON_TREE="./test_JSON"
export LIMIT_SH="./limit_ioccc.sh"
export OLD_FLAG=
export FORCE=
export QUOTE='"'
export NOOP=
export LIST_CHANGE=
export LIST_NOCHANGE=

# parse args
#
while getopts :hv:d:i:oFQnlL flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 5
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    d) JSON_TREE="$OPTARG";
       ;;
    i) LIMIT_SH="$OPTARG";
       ;;
    o) OLD_FLAG="-o"
       ;;
    F) FORCE="-f"
       ;;
    Q) QUOTE=
       ;;
    n) NOOP="-n"
       ;;
    l) LIST_CHANGE="-l"
       ;;
    L) LIST_NOCHANGE="-L"
       ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
       exit 6
       ;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
       exit 6
       ;;
   *)
       ;;
    esac
done

# check args
#
shift $(( OPTIND - 1 ));
if [[ $# -ne 2 ]]; then
    echo "$0: ERROR: expected 2 arguments, found $#" 1>&2
    exit 6
fi
OLD_VER="$QUOTE$1$QUOTE"
NEW_VER="$QUOTE$2$QUOTE"
export OLD_VER NEW_VER

# firewall
#
if [[ ! -e $JSON_TREE ]]; then
    echo "$0: ERROR: test_dir does not exist: $JSON_TREE" 1>&2
    exit 4
fi
if [[ ! -d $JSON_TREE ]]; then
    echo "$0: ERROR: test_dir is not a directory: $JSON_TREE" 1>&2
    exit 4
fi
if [[ ! -r $JSON_TREE ]]; then
    echo "$0: ERROR: test_dir is not readable: $JSON_TREE" 1>&2
    exit 4
fi
if [[ ! -e $LIMIT_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh does not exist: $LIMIT_SH" 1>&2
    exit 3
fi
if [[ ! -f $LIMIT_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh is not a file: $LIMIT_SH" 1>&2
    exit 3
fi
if [[ ! -r $LIMIT_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh is not a readable file: $LIMIT_SH" 1>&2
    exit 3
fi
RPL_CMD=$(type -P rpl)
if [[ -z "$RPL_CMD" ]]; then
    echo "$0: ERROR: rpl command not found" 1>&2
    exit 2
fi
if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: V_FLAG: $V_FLAG" 1>&2
    echo "$0: debug[5]: test_dir: $JSON_TREE" 1>&2
    echo "$0: debug[5]: limit_ioccc.sh: $LIMIT_SH" 1>&2
    echo "$0: debug[5]: rpm: $RPL_CMD" 1>&2
    echo "$0: debug[5]: OLD_FLAG: <$OLD_FLAG>" 1>&2
    echo "$0: debug[5]: FORCE: <$FORCE>" 1>&2
    echo "$0: debug[5]: QUOTE: <$QUOTE>" 1>&2
    echo "$0: debug[5]: NOOP: <$NOOP>" 1>&2
    echo "$0: debug[5]: LIST_CHANGE: <$LIST_CHANGE>" 1>&2
    echo "$0: debug[5]: LIST_NOCHANGE: <$LIST_NOCHANGE>" 1>&2
    echo "$0: debug[5]: OLD_VER: <$OLD_VER>" 1>&2
    echo "$0: debug[5]: NEW_VER: <$NEW_VER>" 1>&2
fi

# count *.json files under test_dir
#
COUNT=$(find "$JSON_TREE" -type f -name '*.json' -print 2>/dev/null | wc -l)
export COUNT
if [[ $COUNT -le 0 ]]; then
    echo "$0: ERROR: no .json files found under test_dir: $JSON_TREE" 1>&2
    exit 7
fi
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: .json file count: $COUNT" 1>&2
fi

# check if a pattern is found in the limit_ioccc.sh file
#
if [[ -n $OLD_FLAG ]]; then
    if ! grep -F -q "$OLD_VER" "$LIMIT_SH" >/dev/null 2>&1; then
	if [[ -z $FORCE ]]; then
	    echo "$0: ERROR: old_ver: <$OLD_VER> not found in limit_ioccc.sh: $LIMIT_SH" 1>&2
	    exit 8
	else
	    echo "$0: Warning: old_ver: <$OLD_VER> not found in limit_ioccc.sh: $LIMIT_SH" 1>&2
	fi
    elif [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: old_ver: <$OLD_VER> found in limit_ioccc.sh: $LIMIT_SH" 1>&2
    fi
else
    if ! grep -F -q "$NEW_VER" "$LIMIT_SH" >/dev/null 2>&1; then
	if [[ -z $FORCE ]]; then
	    echo "$0: ERROR: new_ver: <$NEW_VER> not found in limit_ioccc.sh: $LIMIT_SH" 1>&2
	    exit 8
	else
	    echo "$0: Warning: new_ver: <$NEW_VER> not found in limit_ioccc.sh: $LIMIT_SH" 1>&2
	fi
    elif [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: new_ver: <$NEW_VER> found in limit_ioccc.sh: $LIMIT_SH" 1>&2
    fi
fi

# listing files
#
# case: -l and -L (list all .json files)
#
if [[ -n $LIST_CHANGE && -n $LIST_NOCHANGE ]]; then
    find "$JSON_TREE" -type f -name '*.json' -print 2>/dev/null | sort -u

# case: -l (list .json files that will change)
#
elif [[ -n $LIST_CHANGE ]]; then
     grep -F -R --include '*.json' -l -- "$OLD_VER" "$JSON_TREE" 2>/dev/null | sort -u

# case: -L (list .json files that will NOT change)
#
elif [[ -n $LIST_NOCHANGE ]]; then
     grep -F -R --include '*.json' -L -- "$OLD_VER" "$JSON_TREE" 2>/dev/null | sort -u
fi

# replace string in files
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: rpl -q -x'.json' -R -- $OLD_VER $NEW_VER $JSON_TREE" 1>&2
fi
status=0
if [[ -z $NOOP ]]; then
    rpl -q -x'.json' -R -- "$OLD_VER" "$NEW_VER" "$JSON_TREE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: rpl non-zero exit status: $status" 1>&2
	exit 1
    fi
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
