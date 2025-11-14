#!/usr/bin/env bash
#
# mkiocccentry_slots.sh - rebuild the slot test_ioccc/slot/good/workdir directories using mkiocccentry
#
# Copyright (c) 2025 by Landon Curt Noll.  All Rights Reserved.
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
# CODY BOONE FERGUSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
# SHALL CODY BOONE FERGUSON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
# DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
# OF THIS SOFTWARE.
#
# Share and enjoy! :-)
#     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
#

# setup


# IOCCC requires use of C locale
#
export LANG="C"
export LC_CTYPE="C"
export LC_NUMERIC="C"
export LC_TIME="C"
export LC_COLLATE="C"
export LC_MONETARY="C"
export LC_MESSAGES="C"
export LC_PAPER="C"
export LC_NAME="C"
export LC_ADDRESS="C"
export LC_TELEPHONE="C"
export LC_MEASUREMENT="C"
export LC_IDENTIFICATION="C"
export LC_ALL="C"


# setup
#
export MKIOCCCENTRY_SLOTS_VERSION="1.1.0 2025-11-13"
export MKIOCCCENTRY="./mkiocccentry"
export GOOD_TREE="test_ioccc/slot/good"
export BAD_TREE="test_ioccc/slot/bad"
#
export EXIT_CODE=0
export REPO_TOPDIR=

# usage message
#
export USAGE="usage: $0 [-h] [-V] [-v level] [-M mkiocccentry] [-g good_tree] [-b bad_tree] [-Z repo_topdir] [-k] [-B]

    -h			print help and exit
    -V			print version and exit
    -v level		set verbosity level for this script: (def level: 0)

    -M mkiocccentry	path to mkiocccentry executable (def: $MKIOCCCENTRY)
    -g good_tree	path to the good slot tree (def: $GOOD_TREE)
    -b bad_tree		path to the good slot tree (def: $BAD_TREE)
    -Z repo_topdir	top level repo directory for the mkiocccentry toolkit (def: try . or ..)

Exit codes:
     0   all OK
     1   at least one test failed
     2   -h and help string printed or -V and version string printed
     3   invalid command line
     4	 missing slot information
 >= 10   internal error or missing file or directory

mkiocccentry_slots.sh version: $MKIOCCCENTRY_SLOTS_VERSION"


# parse args
#
export V_FLAG="0"
while getopts :hVv:M:g:b:Z: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$MKIOCCCENTRY_SLOTS_VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    M)	MKIOCCCENTRY="$OPTARG";
	;;
    g)	GOOD_TREE="$OPTARG";
	;;
    b)	BAD_TREE="$OPTARG";
	;;
    Z)  REPO_TOPDIR="$OPTARG";
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
if [[ $# -ne 0 ]]; then
    echo "$0: ERROR: expected 0 arguments, found $#" 1>&2
    echo 1>&2
    echo "$USAGE" 1>&2
    exit 3
fi

# change to the top level directory as needed
#
if [[ -n $REPO_TOPDIR ]]; then
    if [[ ! -d $REPO_TOPDIR ]]; then
	echo "$0: ERROR: -Z $REPO_TOPDIR given: not a directory: $REPO_TOPDIR" 1>&2
	exit 3
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: -Z $REPO_TOPDIR given, about to cd $REPO_TOPDIR" 1>&2
    fi
    # SC2164 (warning): Use 'cd ... || exit' or 'cd ... || return' in case cd fails.
    # https://www.shellcheck.net/wiki/SC2164
    # shellcheck disable=SC2164
    cd "$REPO_TOPDIR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: -Z $REPO_TOPDIR given: cd $REPO_TOPDIR exit code: $status" 1>&2
	exit 3
    fi
elif [[ -f mkiocccentry.c ]]; then
    REPO_TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume REPO_TOPDIR is .: $REPO_TOPDIR" 1>&2
    fi
elif [[ -f ../mkiocccentry.c ]]; then
    cd ..
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cd .. exit code: $status" 1>&2
	exit 3
    fi
    REPO_TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume REPO_TOPDIR is ..: $REPO_TOPDIR" 1>&2
    fi
else
    echo "$0: ERROR: cannot determine REPO_TOPDIR, use -Z topdir" 1>&2
    exit 3
fi
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: REPO_TOPDIR is the current directory: $REPO_TOPDIR" 1>&2
fi

# check for mkiocccentry
#
if [[ ! -e $MKIOCCCENTRY ]]; then
    echo "$0: ERROR: mkiocccentry not found: $MKIOCCCENTRY" 1>&2
    exit 10
fi
if [[ ! -f $MKIOCCCENTRY ]]; then
    echo "$0: ERROR: mkiocccentry not a regular file: $MKIOCCCENTRY" 1>&2
    exit 11
fi
if [[ ! -x $MKIOCCCENTRY ]]; then
    echo "$0: ERROR: mkiocccentry not executable: $MKIOCCCENTRY" 1>&2
    exit 12
fi

# check that the good slot tree is a writable directory
#
if [[ ! -e $GOOD_TREE ]]; then
    echo "$0: ERROR: good slot tree not found: $GOOD_TREE" 1>&2
    exit 13
fi
if [[ ! -d $GOOD_TREE ]]; then
    echo "$0: ERROR: good slot tree not a directory: $GOOD_TREE" 1>&2
    exit 14
fi
if [[ ! -w $GOOD_TREE ]]; then
    echo "$0: ERROR: good slot tree not writable directory: $GOOD_TREE" 1>&2
    exit 15
fi

# verify good slot tree topdir sub-directory is writable
#
export GOOD_TOPDIR="$GOOD_TREE/topdir"
if [[ ! -e $GOOD_TOPDIR ]]; then
    echo "$0: ERROR: good slot tree topdir sub-directory directory not found: $GOOD_TOPDIR" 1>&2
    exit 16
fi
if [[ ! -d $GOOD_TOPDIR ]]; then
    echo "$0: ERROR: good slot tree topdir sub-directory not a directory: $GOOD_TOPDIR" 1>&2
    exit 17
fi
if [[ ! -w $GOOD_TOPDIR ]]; then
    echo "$0: ERROR: good slot tree topdir sub-directory not writable directory: $GOOD_TOPDIR" 1>&2
    exit 18
fi

# verify good slot tree answers sub-directory is writable
#
export GOOD_ANSWERS="$GOOD_TREE/answers"
if [[ ! -e $GOOD_ANSWERS ]]; then
    echo "$0: ERROR: good slot tree answers sub-directory directory not found: $GOOD_ANSWERS" 1>&2
    exit 19
fi
if [[ ! -d $GOOD_ANSWERS ]]; then
    echo "$0: ERROR: good slot tree answers sub-directory not a directory: $GOOD_ANSWERS" 1>&2
    exit 20
fi
if [[ ! -w $GOOD_ANSWERS ]]; then
    echo "$0: ERROR: good slot tree answers sub-directory not writable directory: $GOOD_ANSWERS" 1>&2
    exit 21
fi
export GOOD_BUILD_OUT="$GOOD_TREE/build.out"
export GOOD_WORKDIR="$GOOD_TREE/workdir"

# check that the bad slot tree is a writable directory
#
if [[ ! -e $BAD_TREE ]]; then
    echo "$0: ERROR: bad slot tree not found: $BAD_TREE" 1>&2
    exit 22
fi
if [[ ! -d $BAD_TREE ]]; then
    echo "$0: ERROR: bad slot tree not a directory: $BAD_TREE" 1>&2
    exit 23
fi
if [[ ! -w $BAD_TREE ]]; then
    echo "$0: ERROR: bad slot tree not writable directory: $BAD_TREE" 1>&2
    exit 24
fi

# verify bad slot tree topdir sub-directory is writable
#
export BAD_TOPDIR="$BAD_TREE/topdir"
if [[ ! -e $BAD_TOPDIR ]]; then
    echo "$0: ERROR: bad slot tree topdir sub-directory directory not found: $BAD_TOPDIR" 1>&2
    exit 25
fi
if [[ ! -d $BAD_TOPDIR ]]; then
    echo "$0: ERROR: bad slot tree topdir sub-directory not a directory: $BAD_TOPDIR" 1>&2
    exit 26
fi
if [[ ! -w $BAD_TOPDIR ]]; then
    echo "$0: ERROR: bad slot tree topdir sub-directory not writable directory: $BAD_TOPDIR" 1>&2
    exit 27
fi

# verify bad slot tree answers sub-directory is writable
#
export BAD_ANSWERS="$BAD_TREE/answers"
if [[ ! -e $BAD_ANSWERS ]]; then
    echo "$0: ERROR: bad slot tree answers sub-directory directory not found: $BAD_ANSWERS" 1>&2
    exit 28
fi
if [[ ! -d $BAD_ANSWERS ]]; then
    echo "$0: ERROR: bad slot tree answers sub-directory not a directory: $BAD_ANSWERS" 1>&2
    exit 29
fi
if [[ ! -w $BAD_ANSWERS ]]; then
    echo "$0: ERROR: bad slot tree answers sub-directory not writable directory: $BAD_ANSWERS" 1>&2
    exit 30
fi
export BAD_BUILD_OUT="$BAD_TREE/build.out"
export BAD_WORKDIR="$BAD_TREE/workdir"

# UUID regex
#
# A UUID has the 36 character format:
#
#     xxxxxxxx-xxxx-4xxx-Nxxx-xxxxxxxxxxxx
#
# where 'x' is a hex character, 4 is the UUID version, and N is one of 8, 9, a, or b.
#
export RE_HEX='[0-9A-Fa-f]'			    # 1 hex
export RE_HEX_3="$RE_HEX$RE_HEX$RE_HEX"		    # 3 hex
export RE_HEX_4="$RE_HEX_3$RE_HEX"		    # 4 hex
export RE_N='[89ABab]'				    # N is one of 8, 9, a, or b
export RE_UUID_1="$RE_HEX$RE_HEX$RE_HEX$RE_HEX"	    # 4 hex
export RE_UUID_0="$RE_UUID_1$RE_UUID_1"		    # 8 hex
export RE_UUID_2="4$RE_HEX$RE_HEX$RE_HEX"	    # UUID version 4
export RE_UUID_3="$RE_N$RE_HEX_3"		    # N + 3 hex
export RE_UUID_4="$RE_HEX_4$RE_UUID_0"		    # 12 hex
# UUID regex for IOCCC
export RE_UUID="$RE_UUID_0-$RE_UUID_1-$RE_UUID_2-$RE_UUID_3-$RE_UUID_4"

# slot number regex
#
export RE_SLOT_NUM='[0-9]'			    # IOCCC slot number is 1 digit

# UUID-SLOT_NUM regex
#
export RE_UUID_SLOT_NUM="$RE_UUID-$RE_SLOT_NUM"	    # IOCCC UUID-SLOT_NUM

# form temporary exit code file
#
TMP_EXIT_CODE=$(mktemp -u .exit_code.XXXXXXXXXX)
export TMP_EXIT_CODE
trap 'rm -f $TMP_EXIT_CODE; exit' 0 1 2 3 15
rm -f "$TMP_EXIT_CODE"
echo "0" > "$TMP_EXIT_CODE"
if [[ ! -s "$TMP_EXIT_CODE" ]]; then
    echo "$0: ERROR: could not create exit code file: $TMP_EXIT_CODE" 1>&2
    exit 31
fi
if [[ ! -w "$TMP_EXIT_CODE" ]]; then
    echo "$0: ERROR: exit code file not writable: $TMP_EXIT_CODE" 1>&2
    exit 32
fi

# debugging
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: MKIOCCCENTRY_SLOTS_VERSION=$MKIOCCCENTRY_SLOTS_VERSION" 1>&2
    echo "$0: debug[3]: MKIOCCCENTRY=$MKIOCCCENTRY" 1>&2
    echo "$0: debug[3]: GOOD_TREE=$GOOD_TREE" 1>&2
    echo "$0: debug[3]: BAD_TREE=$BAD_TREE" 1>&2
    echo "$0: debug[3]: EXIT_CODE=$EXIT_CODE" 1>&2
    echo "$0: debug[3]: REPO_TOPDIR=$REPO_TOPDIR" 1>&2
    echo "$0: debug[3]: GOOD_TOPDIR=$GOOD_TOPDIR" 1>&2
    echo "$0: debug[3]: GOOD_ANSWERS=$GOOD_ANSWERS" 1>&2
    echo "$0: debug[3]: GOOD_BUILD_OUT=$GOOD_BUILD_OUT" 1>&2
    echo "$0: debug[3]: GOOD_WORKDIR=$GOOD_WORKDIR" 1>&2
    echo "$0: debug[3]: BAD_TOPDIR=$BAD_TOPDIR" 1>&2
    echo "$0: debug[3]: BAD_ANSWERS=$BAD_ANSWERS" 1>&2
    echo "$0: debug[3]: BAD_BUILD_OUT=$BAD_BUILD_OUT" 1>&2
    echo "$0: debug[3]: BAD_WORKDIR=$BAD_WORKDIR" 1>&2
    echo "$0: debug[3]: RE_UUID=$RE_UUID" 1>&2
    echo "$0: debug[3]: RE_SLOT_NUM=$RE_SLOT_NUM" 1>&2
    echo "$0: debug[3]: RE_UUID_SLOT_NUM=$RE_UUID_SLOT_NUM" 1>&2
    echo "$0: debug[3]: TMP_EXIT_CODE=$TMP_EXIT_CODE" 1>&2
fi

# form an empty build.out sub-directory of good slot tree
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to form an empty build.out sub-directory of good slot tree: $GOOD_BUILD_OUT" 1>&2
fi
rm -rf "$GOOD_BUILD_OUT"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: rm -rf $GOOD_BUILD_OUT failed, exit code: $status" 1>&2
    exit 33
fi
mkdir -p -- "$GOOD_BUILD_OUT"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mkdir -p -- $GOOD_BUILD_OUT failed, exit code: $status" 1>&2
    exit 34
fi

# form an empty workdir sub-directory of good slot tree
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to form an empty workdir sub-directory of good slot tree: $GOOD_WORKDIR" 1>&2
fi
rm -rf "$GOOD_WORKDIR"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: rm -rf $GOOD_WORKDIR failed, exit code: $status" 1>&2
    exit 35
fi
mkdir -p -- "$GOOD_WORKDIR"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mkdir -p -- $GOOD_WORKDIR failed, exit code: $status" 1>&2
    exit 36
fi

# form an empty build.out sub-directory of bad slot tree
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to form an empty build.out sub-directory of good slot tree: $BAD_BUILD_OUT" 1>&2
fi
rm -rf "$BAD_BUILD_OUT"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: rm -rf $BAD_BUILD_OUT failed, exit code: $status" 1>&2
    exit 37
fi
mkdir -p -- "$BAD_BUILD_OUT"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mkdir -p -- $BAD_BUILD_OUT failed, exit code: $status" 1>&2
    exit 38
fi

# form an empty workdir sub-directory of bad slot tree
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: about to form an empty workdir sub-directory of good slot tree: $BAD_WORKDIR" 1>&2
fi
rm -rf "$BAD_WORKDIR"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: rm -rf $BAD_WORKDIR failed, exit code: $status" 1>&2
    exit 39
fi
mkdir -p -- "$BAD_WORKDIR"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mkdir -p -- $BAD_WORKDIR failed, exit code: $status" 1>&2
    exit 40
fi

# process all good slots
#
# We look for UUID-SLOT_NUM's that have both:
#
#   answers file:   $GOOD_ANSWERS/UUID-SLOT_NUM
#   code directory: $GOOD_TOPDIR/UUID-SLOT_NUM/
#
# Then we use mkiocccentry to generate a:
#
#   submission: $GOOD_WORKDIR/UUID-SLOT_NUM/12345678-1234-4321-abcd-1234567890ab-0/
#
# The submission is a directory that contains both:
#
#   submission tarball:	UUID-SLOT_NUM.txz
#   submission tree:	UUID-SLOT_NUM/
#
# The submission tree, if mkiocccentry is successful, will have:
#
#   non-empty auth JSON:	UUID-SLOT_NUM/.auth.json
#   non-empty info JSON:	UUID-SLOT_NUM/.info.json
#   prog.c:			UUID-SLOT_NUM/prog.c
#   non-empty Makefile:		UUID-SLOT_NUM/Makefile
#   non-empty remarks.md:	UUID-SLOT_NUM/remarks.md
#
# This code will attempt to process ALL UUID-SLOT_NUM's.
# All UUID-SLOT_NUM's are successful, the EXIT_CODE will be set to 0.
# If any UUID-SLOT_NUM fails, the EXIT_CODE will be set to non-zero.
#
find "$GOOD_ANSWERS" -type f -name '[0-9a-f]*-[0-9a-f]' -print |
    sed -e 's;^.*/;;' |
    grep -E "^$RE_UUID_SLOT_NUM$" |
    LANG=C sort |
    while read -r UUID_SLOT_NUM; do

	# verify that the answers file for UUID-SLOT_NUM is a readable non-empty file
	#
	if [[ ! -s $GOOD_ANSWERS/$UUID_SLOT_NUM || ! -r $GOOD_ANSWERS/$UUID_SLOT_NUM ]]; then
	    echo "$0: ERROR: not a readable non-empty answers file: $GOOD_ANSWERS/$UUID_SLOT_NUM" 1>&2
	    TMP_EXIT=1
	    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
	    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
	    continue
	fi

	# verify we have a topdir for this UUID-SLOT_NUM
	#
	if [[ -d $GOOD_TOPDIR/$UUID_SLOT_NUM ]]; then

	    # remove any existing workdir
	    #
	    if [[ -d $GOOD_WORKDIR/$UUID_SLOT_NUM ]]; then
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]:" rm -rf "${GOOD_WORKDIR/$UUID_SLOT_NUM:?}" 1>&2
		fi
		rm -rf "${GOOD_WORKDIR/$UUID_SLOT_NUM:?}"
		status="$?"
		if [[ $status -ne 0 ]]; then
		    echo "$0: ERROR: rm -rf $GOOD_WORKDIR/$UUID_SLOT_NUM failed, error code: $status" 1>&2
		    TMP_EXIT=1
		    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		    continue
		fi
		if [[ -d $GOOD_WORKDIR/$UUID_SLOT_NUM ]]; then
		    echo "$0: ERROR: failed to remove existing workdir: $GOOD_WORKDIR/$UUID_SLOT_NUM" 1>&2
		    TMP_EXIT=1
		    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		    continue
		fi
	    fi

	    # create a new empty workdir
	    #
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]:" mkdir -p -- "$GOOD_WORKDIR/$UUID_SLOT_NUM" 1>&2
	    fi
	    mkdir -p -- "$GOOD_WORKDIR/$UUID_SLOT_NUM"
	    status="$?"
	    if [[ $status -ne 0 ]]; then
		TMP_EXIT=1
		echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		continue
	    fi

	    # remove any build.out file
	    #
	    if [[ -e $GOOD_BUILD_OUT/$UUID_SLOT_NUM.out ]]; then
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]:" rm -f "$GOOD_BUILD_OUT/$UUID_SLOT_NUM.out" 1>&2
		fi
		rm -f "$GOOD_BUILD_OUT/$UUID_SLOT_NUM.out"
		status="$?"
		if [[ $status -ne 0 ]]; then
		    echo "$0: ERROR: rm -f $GOOD_BUILD_OUT/$UUID_SLOT_NUM.out failed, error code: $status" 1>&2
		    TMP_EXIT=1
		    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		continue
		    continue
		fi
		if [[ -e $GOOD_BUILD_OUT/$UUID_SLOT_NUM.out ]]; then
		    echo "$0: ERROR: failed to remove existing build.out: $GOOD_BUILD_OUT/$UUID_SLOT_NUM.out" 1>&2
		    TMP_EXIT=1
		    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		    continue
		fi
	    fi

	    # run mkiocccentry
	    #
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]:" "$MKIOCCCENTRY" -Y -i "$GOOD_ANSWERS/$UUID_SLOT_NUM" \
			"$GOOD_WORKDIR/$UUID_SLOT_NUM" "$GOOD_TOPDIR/$UUID_SLOT_NUM" \
			"> $GOOD_BUILD_OUT/$UUID_SLOT_NUM.out 2>&1" 1>&2
	    fi
	    "$MKIOCCCENTRY" -Y -i "$GOOD_ANSWERS/$UUID_SLOT_NUM" \
		    "$GOOD_WORKDIR/$UUID_SLOT_NUM" "$GOOD_TOPDIR/$UUID_SLOT_NUM" \
		    > "$GOOD_BUILD_OUT/$UUID_SLOT_NUM.out" 2>&1
	    status="$?"
	    if [[ $status -ne 0 ]]; then
		echo "$0: ERROR: mkiocccentry on $UUID_SLOT_NUM failed, error code: $status" 1>&2
		TMP_EXIT=1
		echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		continue
	    fi

	# case: topdir for this UUID-SLOT_NUM is missing
	#
	else
	    echo "$0: ERROR: missing good slot tree topdir sub-directory: $GOOD_TOPDIR/$UUID_SLOT_NUM" 1>&2
	    TMP_EXIT=1
	    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
	    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
	fi
    done

# process all bad slots
#
# We look for UUID-SLOT_NUM's that have both:
#
#   answers file:   $BAD_ANSWERS/UUID-SLOT_NUM
#   code directory: $BAD_TOPDIR/UUID-SLOT_NUM/
#
# Then we use mkiocccentry to generate a:
#
#   submission: $BAD_WORKDIR/UUID-SLOT_NUM/12345678-1234-4321-abcd-1234567890ab-0/
#
# The submission is a directory that contains both:
#
#   submission tarball:	UUID-SLOT_NUM.txz
#   submission tree:	UUID-SLOT_NUM/
#
# The submission tree, if mkiocccentry is successful, will have:
#
#   non-empty auth JSON:	UUID-SLOT_NUM/.auth.json
#   non-empty info JSON:	UUID-SLOT_NUM/.info.json
#   prog.c:			UUID-SLOT_NUM/prog.c
#   non-empty Makefile:		UUID-SLOT_NUM/Makefile
#   non-empty remarks.md:	UUID-SLOT_NUM/remarks.md
#
# This code will attempt to process ALL UUID-SLOT_NUM's.
# All UUID-SLOT_NUM's are successful, the EXIT_CODE will be set to 0.
# If any UUID-SLOT_NUM fails, the EXIT_CODE will be set to non-zero.
#
find "$BAD_ANSWERS" -type f -name '[0-9a-f]*-[0-9a-f]' -print |
    sed -e 's;^.*/;;' |
    grep -E "^$RE_UUID_SLOT_NUM$" |
    LANG=C sort |
    while read -r UUID_SLOT_NUM; do

	# verify that the answers file for UUID-SLOT_NUM is a readable non-empty file
	#
	if [[ ! -s $BAD_ANSWERS/$UUID_SLOT_NUM || ! -r $BAD_ANSWERS/$UUID_SLOT_NUM ]]; then
	    echo "$0: ERROR: not a readable non-empty answers file: $BAD_ANSWERS/$UUID_SLOT_NUM" 1>&2
	    TMP_EXIT=1
	    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
	    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
	    continue
	fi

	# verify we have a topdir for this UUID-SLOT_NUM
	#
	if [[ -d $BAD_TOPDIR/$UUID_SLOT_NUM ]]; then

	    # remove any existing workdir
	    #
	    if [[ -d $BAD_WORKDIR/$UUID_SLOT_NUM ]]; then
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]:" rm -rf "${BAD_WORKDIR/$UUID_SLOT_NUM:?}" 1>&2
		fi
		rm -rf "${BAD_WORKDIR/$UUID_SLOT_NUM:?}"
		status="$?"
		if [[ $status -ne 0 ]]; then
		    echo "$0: ERROR: rm -rf $BAD_WORKDIR/$UUID_SLOT_NUM failed, error code: $status" 1>&2
		    TMP_EXIT=1
		    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		    continue
		fi
		if [[ -d $BAD_WORKDIR/$UUID_SLOT_NUM ]]; then
		    echo "$0: ERROR: failed to remove existing workdir: $BAD_WORKDIR/$UUID_SLOT_NUM" 1>&2
		    TMP_EXIT=1
		    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		    continue
		fi
	    fi

	    # create a new empty workdir
	    #
	    if [[ $V_FLAG -ge 3 ]]; then
		echo "$0: debug[3]:" mkdir -p -- "$BAD_WORKDIR/$UUID_SLOT_NUM" 1>&2
	    fi
	    mkdir -p -- "$BAD_WORKDIR/$UUID_SLOT_NUM"
	    status="$?"
	    if [[ $status -ne 0 ]]; then
		TMP_EXIT=1
		echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		continue
	    fi

	    # remove any build.out file
	    #
	    if [[ -e $BAD_BUILD_OUT/$UUID_SLOT_NUM.out ]]; then
		if [[ $V_FLAG -ge 3 ]]; then
		    echo "$0: debug[3]:" rm -f "$BAD_BUILD_OUT/$UUID_SLOT_NUM.out" 1>&2
		fi
		rm -f "$BAD_BUILD_OUT/$UUID_SLOT_NUM.out"
		status="$?"
		if [[ $status -ne 0 ]]; then
		    echo "$0: ERROR: rm -f $BAD_BUILD_OUT/$UUID_SLOT_NUM.out failed, error code: $status" 1>&2
		    TMP_EXIT=1
		    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		continue
		    continue
		fi
		if [[ -e $BAD_BUILD_OUT/$UUID_SLOT_NUM.out ]]; then
		    echo "$0: ERROR: failed to remove existing build.out: $BAD_BUILD_OUT/$UUID_SLOT_NUM.out" 1>&2
		    TMP_EXIT=1
		    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		    continue
		fi
	    fi

	    # run mkiocccentry
	    #
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]:" "$MKIOCCCENTRY" -Y -i "$BAD_ANSWERS/$UUID_SLOT_NUM" \
			"$BAD_WORKDIR/$UUID_SLOT_NUM" "$BAD_TOPDIR/$UUID_SLOT_NUM" \
			"> $BAD_BUILD_OUT/$UUID_SLOT_NUM.out 2>&1" 1>&2
	    fi
	    "$MKIOCCCENTRY" -Y -i "$BAD_ANSWERS/$UUID_SLOT_NUM" \
		    "$BAD_WORKDIR/$UUID_SLOT_NUM" "$BAD_TOPDIR/$UUID_SLOT_NUM" \
		    > "$BAD_BUILD_OUT/$UUID_SLOT_NUM.out" 2>&1
	    status="$?"
	    if [[ $status -eq 0 ]]; then
		echo "$0: ERROR: mkiocccentry on $UUID_SLOT_NUM passed when it should have failed" 1>&2
		TMP_EXIT=1
		echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
		echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
		continue
	    fi

	# case: topdir for this UUID-SLOT_NUM is missing
	#
	else
	    echo "$0: ERROR: missing bad slot tree topdir sub-directory: $BAD_TOPDIR/$UUID_SLOT_NUM" 1>&2
	    TMP_EXIT=1
	    echo "$TMP_EXIT" > "$TMP_EXIT_CODE"
	    echo "$0: Warning: set exit code: $TMP_EXIT" 1>&2
	fi
    done

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
if [[ -s "$TMP_EXIT_CODE" ]]; then
    EXIT_CODE=$(< "$TMP_EXIT_CODE")
fi
if [[ $V_FLAG -ge 1 ]]; then
    if [[ $EXIT_CODE -eq 0 ]]; then
	echo "$0: debug[1]: all tests PASSED" 1>&2
    else
	echo "$0: debug[1]: some tests FAILED" 1>&2
    fi
fi
if [[ $V_FLAG -ge 1 && $EXIT_CODE -gt 0 ]]; then
    echo "$0: ERROR: about to exit: $EXIT_CODE" 1>&2
fi
exit "$EXIT_CODE"
