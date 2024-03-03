#!/usr/bin/env bash
#
# vermod.sh - modify version strings (and others) under ./test_ioccc/test_JSON
#
# "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
#
# The JSON parser was co-developed in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# "Share and Enjoy!"
#     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)

# setup
#
export JSON_TREE="test_ioccc/test_JSON"
export LIMIT_SH="soup/limit_ioccc.sh"
export VERMOD_VERSION="1.0.1 2024-03-02"
export USAGE="usage: $0 [-h] [-V] [-v level] [-d test_dir] [-i limit.sh]
	[-o] [-F] [-Q] [-n] [-l] [-L] old_ver new_ver

    -h			print help and exit
    -V			print version and exit
    -v level		set the debugging level (def: 0)

    -d test_dir		process '*.json' files below dir (def: $JSON_TREE)
    -i limit.sh		use limit.sh to verify new version (def: $LIMIT_SH)

    -o			verify old version (def: verify new version)
    -F			force change even if verification fails (def: reject unverified changes)

    -Q			do not double quote string (def: add double quotes)

    -n			no op: change no files (def: change files)
    -l			list files that change/will change (def: do not list)
			NOTE: useful with -n to show what would change
    -L			list files that do not/will not change (def: do not list)
			    NOTE: useful with -n to show what will not change

    old_ver		old version string to change from
    new_ver		new version string to change to

Exit codes:
     0	    all OK
     1	    rpl exited non-zero
     2	    -h and help string printed or -V and version string printed
     3	    command line error
     4	    no test_dir directory found or is not readable
     5	    rpl command not found
     6	    no limit.sh file found or is not readable
     7	    no *.json files found under test_dir
     8	    new_ver (or old_ver if -o) not found in limit.sh

vermod.sh version: $VERMOD_VERSION"

export V_FLAG="0"
export OLD_FLAG=
export FORCE=
export QUOTE='"'
export NOOP=
export LIST_CHANGE=
export LIST_NOCHANGE=

# parse args
#
while getopts :hVv:d:i:oFQnlL flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$VERMOD_VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    d)	JSON_TREE="$OPTARG";
	;;
    i)	LIMIT_SH="$OPTARG";
	;;
    o)	OLD_FLAG="-o"
	;;
    F)	FORCE="-f"
	;;
    Q)	QUOTE=
	;;
    n)	NOOP="-n"
	;;
    l)	LIST_CHANGE="-l"
	;;
    L)	LIST_NOCHANGE="-L"
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
if [[ $# -ne 2 ]]; then
    echo "$0: ERROR: expected 2 arguments, found $#" 1>&2
    echo "$USAGE" 1>&2
    exit 3
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
    echo "$0: ERROR: limit.sh does not exist: $LIMIT_SH" 1>&2
    exit 6
fi
if [[ ! -f $LIMIT_SH ]]; then
    echo "$0: ERROR: limit.sh is not a regular file: $LIMIT_SH" 1>&2
    exit 6
fi
if [[ ! -r $LIMIT_SH ]]; then
    echo "$0: ERROR: limit.sh is not a readable file: $LIMIT_SH" 1>&2
    exit 6
fi
RPL_CMD=$(type -P rpl)
if [[ -z "$RPL_CMD" ]]; then
    echo "$0: ERROR: rpl command not found" 1>&2
    exit 5
fi
if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: V_FLAG: $V_FLAG" 1>&2
    echo "$0: debug[5]: test_dir: $JSON_TREE" 1>&2
    echo "$0: debug[5]: limit_ioccc.sh: $LIMIT_SH" 1>&2
    echo "$0: debug[5]: rpl: $RPL_CMD" 1>&2
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
# case: -l and -L: list all .json files
#
if [[ -n $LIST_CHANGE && -n $LIST_NOCHANGE ]]; then
    find "$JSON_TREE" -type f -name '*.json' -print 2>/dev/null | sort -u

# case: -l: list .json files that WILL change
#
elif [[ -n $LIST_CHANGE ]]; then

     grep -F -R --include '*.json' -l -- "$OLD_VER" "$JSON_TREE" 2>/dev/null | sort -u

# case: -L: list .json files that WILL NOT change
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
