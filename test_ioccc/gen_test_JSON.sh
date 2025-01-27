#!/usr/bin/env bash
#
# gen_test_JSON.sh - generate test_JSON from the template tree
#
# The template_tree contains JSON files with %%TOKEN%% values that need to
# be substituted with current values.
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
export VERSION="1.0.0 2025-01-19"
NAME=$(basename "$0")
export NAME
export V_FLAG=0
if [[ -d template.test_JSON ]]; then
    TEMPLATE_TREE="./template.test_JSON"
else
    TEMPLATE_TREE="./test_ioccc/template.test_JSON"
fi
export TEMPLATE_TREE
if [[ -d test_JSON ]]; then
    JSON_TREE="./test_JSON"
else
    JSON_TREE="./test_ioccc/test_JSON"
fi
export JSON_TREE
export VERSION="1.0.0 2025-06-19"
if [[ -f ./soup/limit_ioccc.h ]]; then
    LIMIT_IOCCC_H="./soup/limit_ioccc.h"
else
    LIMIT_IOCCC_H="../soup/limit_ioccc.h"
fi
export LIMIT_IOCCC_H
if [[ -f ./soup/limit_ioccc.h ]]; then
    VERSION_H="./soup/version.h"
else
    VERSION_H="../soup/version.h"
fi
export VERSION_H

export USAGE="usage: $0 [-h] [-V] [-v level] [template_tree [test_JSON]]

    -h			print help and exit
    -V			print version and exit
    -v level		set verbosity level for this script: (def level: 0)

    template_tree	template tree with %%TOKEN%% strings (def: $TEMPLATE_TREE)
    test_JSON		the test_JSON tree to form (def: $JSON_TREE]

Exit codes:
     0   all OK
     1	 error in creating a new test_JSON tree
     2   -h and help string printed or -V and version string printed
     3   invalid command line
     4	 template_tree directory not found
     5	 important include file not found
     6	 value not found in an include file
 >= 10   internal error

gen_test_JSON.sh version: $VERSION"


# get_value - obtain value from include file
#
# usage:
#
#   get_value TOKEN file
#
#   TOKEN	The #define value to obtain
#   FILE	The include file to read
#
# returns:
#   TOKEN value from file
#
# NOTE: This function will exit 6 on error or if the TOKEN is not found in FILE
#
function get_value
{
    local TOKEN;    # name of TOKEN
    local FILE;	    # include file
    local VALUE;    # value of the TOKEN

    # parse args
    #
    if [[ $# -ne 2 ]]; then
        echo "$0: ERROR: in get_value expected 2 args, found $#" 1>&2
        return 6
    fi
    TOKEN="$1"
    FILE="$2"

    # fetch #define line from FILE
    #
    VALUE=$(grep -E "#define\s+$TOKEN\s" "$FILE")
    status="$?"
    if [[ $status -ne 0 ]]; then
        echo "$0: ERROR: fetch $TOKEN from $FILE failed, status: $status" 1>&2
        return 6
    fi
    if [[ -z $VALUE ]]; then
        echo "$0: ERROR: $TOKEN not found in: $FILE" 1>&2
        return 6
    fi

    # print a cleared value line
    #
    echo "$VALUE" | sed -e 's/(time_t)//' -e 's/^.[^"(]*["(]//' -e 's/[")].*$//'
    return 0
}


# parse args
#
while getopts :hVv: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$VERSION"
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
case "$#" in
    0) ;;
    1)  TEMPLATE_TREE="$1"
	;;
    2)  TEMPLATE_TREE="$1"
	JSON_TREE="$2"
	;;
    *)  echo "ERROR: expected 0, 1, or 2 arguments, found $#" 1>&2
        echo "$USAGE" 1>&2
        exit 3
	;;
esac


# verify we have our critical include files
#
if [[ ! -f $LIMIT_IOCCC_H ]]; then
    echo "$0: ERROR: include file not found: $LIMIT_IOCCC_H" 1>&2
    exit 5
fi
if [[ ! -f $VERSION_H ]]; then
    echo "$0: ERROR: include file not found: $LIMIT_IOCCC_H" 1>&2
    exit 5
fi


# The following tokens dynamically generated or constants:
#
#	%%FORMED_TIMESTAMP%%
#	%%FORMED_TIMESTAMP_USEC%%
#	%%CONTEST_ID%%
#
# We set FORMED_TIMESTAMP to the seconds to the epoch,
# and just set the FORMED_TIMESTAMP_USEC (microseconds)
# to a fixed value because that is good enough for testing.
#
FORMED_TIMESTAMP=$(date '+%s')
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: date value for FORMED_TIMESTAMP failed, status: $status" 1>&2
    exit 6
fi
export FORMED_TIMESTAMP
export FORMED_TIMESTAMP_USEC="123456"
export CONTEST_ID="00000000-0000-4000-8000-000000000000"


# The following tokens need to be substituted based on values from soup/limit_ioccc.h:
#
#	%%IOCCC_CONTEST%%
#	%%IOCCC_YEAR%%
#	%%MIN_TIMESTAMP%%
#	%%TIMESTAMP_EPOCH%%
#
IOCCC_CONTEST=$(get_value IOCCC_CONTEST "$LIMIT_IOCCC_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: IOCCC_CONTEST not found in: $LIMIT_IOCCC_H" 1>&2
    exit 6
fi
IOCCC_YEAR=$(get_value IOCCC_YEAR "$LIMIT_IOCCC_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: IOCCC_YEAR not found in: $LIMIT_IOCCC_H" 1>&2
    exit 6
fi
MIN_TIMESTAMP=$(get_value MIN_TIMESTAMP "$LIMIT_IOCCC_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: MIN_TIMESTAMP not found in: $LIMIT_IOCCC_H" 1>&2
    exit 6
fi
TIMESTAMP_EPOCH=$(get_value TIMESTAMP_EPOCH "$LIMIT_IOCCC_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: TIMESTAMP_EPOCH not found in: $LIMIT_IOCCC_H" 1>&2
    exit 6
fi
export IOCCC_CONTEST IOCCC_YEAR MIN_TIMESTAMP TIMESTAMP_EPOCH


# The following tokens need to be substituted based on values from version.h:
#
#	%%AUTH_VERSION%%
#	%%AUTHOR_VERSION%%
#	%%CHKENTRY_VERSION%%
#	%%ENTRY_VERSION%%
#	%%FNAMCHK_VERSION%%
#	%%INFO_VERSION%%
#	%%IOCCCSIZE_VERSION%%
#	%%MKIOCCCENTRY_VERSION%%
#	%%TXZCHK_VERSION%%
#
AUTH_VERSION=$(get_value AUTH_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: AUTH_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
AUTHOR_VERSION=$(get_value AUTHOR_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: AUTHOR_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
CHKENTRY_VERSION=$(get_value CHKENTRY_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: CHKENTRY_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
ENTRY_VERSION=$(get_value ENTRY_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: ENTRY_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
FNAMCHK_VERSION=$(get_value FNAMCHK_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: FNAMCHK_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
INFO_VERSION=$(get_value INFO_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: INFO_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
IOCCCSIZE_VERSION=$(get_value IOCCCSIZE_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: IOCCCSIZE_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
MKIOCCCENTRY_VERSION=$(get_value MKIOCCCENTRY_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: MKIOCCCENTRY_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
TXZCHK_VERSION=$(get_value TXZCHK_VERSION "$VERSION_H")
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: TXZCHK_VERSION not found in: $VERSION_H" 1>&2
    exit 6
fi
export AUTH_VERSION CHKENTRY_VERSION ENTRY_VERSION FNAMCHK_VERSION
export INFO_VERSION IOCCCSIZE_VERSION MKIOCCCENTRY_VERSION TXZCHK_VERSION


# print running info if verbose
#
# If -v 3 or higher, print exported variables in order that they were exported.
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: VERSION=$VERSION" 1>&2
    echo "$0: debug[3]: NAME=$NAME" 1>&2
    echo "$0: debug[3]: V_FLAG=$V_FLAG" 1>&2
    echo "$0: debug[1]: TEMPLATE_TREE: $TEMPLATE_TREE" 1>&2
    echo "$0: debug[1]: TEST_JSON: $JSON_TREE" 1>&2
    echo "$0: debug[1]: LIMIT_IOCCC_H: $LIMIT_IOCCC_H" 1>&2
    echo "$0: debug[1]: VERSION_H: $VERSION_H" 1>&2
    echo "$0: debug[1]: FORMED_TIMESTAMP: $FORMED_TIMESTAMP" 1>&2
    echo "$0: debug[1]: FORMED_TIMESTAMP_USEC: $FORMED_TIMESTAMP_USEC" 1>&2
    echo "$0: debug[1]: CONTEST_ID: $CONTEST_ID" 1>&2
    echo "$0: debug[1]: IOCCC_CONTEST: $IOCCC_CONTEST" 1>&2
    echo "$0: debug[1]: IOCCC_YEAR: $IOCCC_YEAR" 1>&2
    echo "$0: debug[1]: MIN_TIMESTAMP: $MIN_TIMESTAMP" 1>&2
    echo "$0: debug[1]: TIMESTAMP_EPOCH: $TIMESTAMP_EPOCH" 1>&2
    echo "$0: debug[1]: AUTH_VERSION: $AUTH_VERSION" 1>&2
    echo "$0: debug[1]: AUTHOR_VERSION: $AUTHOR_VERSION" 1>&2
    echo "$0: debug[1]: CHKENTRY_VERSION: $CHKENTRY_VERSION" 1>&2
    echo "$0: debug[1]: ENTRY_VERSION: $ENTRY_VERSION" 1>&2
    echo "$0: debug[1]: FNAMCHK_VERSION: $FNAMCHK_VERSION" 1>&2
    echo "$0: debug[1]: INFO_VERSION: $INFO_VERSION" 1>&2
    echo "$0: debug[1]: IOCCCSIZE_VERSION: $IOCCCSIZE_VERSION" 1>&2
    echo "$0: debug[1]: MKIOCCCENTRY_VERSION: $MKIOCCCENTRY_VERSION" 1>&2
    echo "$0: debug[1]: TXZCHK_VERSION: $TXZCHK_VERSION" 1>&2
fi
# verify we have a template tree
#
if [[ ! -d $TEMPLATE_TREE ]]; then
    echo "$0: ERROR: template_tree directory not found: $TEMPLATE_TREE" 1>&2
    exit 4
fi
# remove JSON tree if it exists
#
if [[ -d $JSON_TREE ]]; then
    if [[ $V_FLAG -ge 1 ]]; then
	 echo "$0: debug[1]: rm -rf $JSON_TREE" 1>&2
    fi
    rm -rf "$JSON_TREE"
fi
if [[ -d $JSON_TREE ]]; then
    echo "$0: ERROR: cannot pre-remove test_JSON: $JSON_TREE" 1>&2 1>&2
    exit 1
fi
# clone the test_JSON tree
#
if [[ $V_FLAG -ge 1 ]]; then
     echo "$0: debug[1]: cp -r $TEMPLATE_TREE $JSON_TREE" 1>&2
fi
cp -r "$TEMPLATE_TREE" "$JSON_TREE"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: cp -r $TEMPLATE_TREE $JSON_TREE failed, status: $status" 1>&2
    exit 1
fi
# substitute %%TOKEN%% for TOKEN values in test_JSON tree
#
find "$JSON_TREE" -type f -name '*.json' -print0 | xargs -0 perl -p -i -e \
  "s/%%FORMED_TIMESTAMP%%/$FORMED_TIMESTAMP/g;
   s/%%FORMED_TIMESTAMP_USEC%%/$FORMED_TIMESTAMP_USEC/g;
   s/%%CONTEST_ID%%/$CONTEST_ID/g;
   s/%%IOCCC_CONTEST%%/$IOCCC_CONTEST/g;
   s/%%IOCCC_YEAR%%/$IOCCC_YEAR/g;
   s/%%MIN_TIMESTAMP%%/$MIN_TIMESTAMP/g;
   s/%%TIMESTAMP_EPOCH%%/$TIMESTAMP_EPOCH/g;
   s/%%AUTH_VERSION%%/$AUTH_VERSION/g;
   s/%%AUTHOR_VERSION%%/$AUTHOR_VERSION/g;
   s/%%CHKENTRY_VERSION%%/$CHKENTRY_VERSION/g;
   s/%%ENTRY_VERSION%%/$ENTRY_VERSION/g;
   s/%%FNAMCHK_VERSION%%/$FNAMCHK_VERSION/g;
   s/%%INFO_VERSION%%/$INFO_VERSION/g;
   s/%%IOCCCSIZE_VERSION%%/$IOCCCSIZE_VERSION/g;
   s/%%MKIOCCCENTRY_VERSION%%/$MKIOCCCENTRY_VERSION/g;
   s/%%TXZCHK_VERSION%%/$TXZCHK_VERSION/g;"


# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
exit 0
