#!/usr/bin/env bash
#
# run_usage.sh - run -h on a tool to extract usage information in order to help
# with the creation and maintaining man pages
#
# This tool is in support of verifying that the man page SYNOPSIS is the same as
# the current options of the tool. It's not perfect and it is a hack. In
# particular there are some limitations including AT LEAST the following:
#
# (0) It does not use bold except on the command name itself. For example no
#     bold for option letters.
# (1) It does not set italics for option arguments where needed (this is
#     currently lacking in the man pages though so maybe that's not such a
#     problem).
# (2) It does not update the man page itself (nor should it!).
# (3) It might be buggy or other things as it was a quick hack to make it easier
#     (NOT NECESSARILY EASY!) to update the SYNOPSIS of the man pages. It might
#     be said it makes it easier to VERIFY that the SYNOPSIS is up to date.
# (4) It requires you to update the man page manually and fix any formatting (as
#     described above). See limitations 0, 1, 2 and 5.
# (5) Currently for shell scripts it expects the .sh in the filename of the man
#     page. This must be corrected. It is for this reason that I have not yet
#     added a man page for the tool to (obviously) verify it works. Instead I
#     have tested it with several man pages fixed in commit
#     d7f165c187de5e4847f75490e52505f062526e4c as well as some after the fact.
#     This will be corrected with a future commit.
# (6) There are bound to be places where the output is incorrect. Use with care!
#
# This quick, dirty and ugly hack was not written by Cody Boone Ferguson in
# support of issue https://github.com/ioccc-src/mkiocccentry/issues/275. :-)
# Okay it was written by Cody but it's ugly to say the least and it could be done
# better for sure. See limitations 0, 1, 2, 3, 4, 5 and 6! If it's not
# immediately obvious that's all the limitations including those that are not
# yet known.
#

# set up
#
export MAN_SECTION="1"
export M_FLAG=""

export USAGE="usage: $0 [-h] [-m section] tool

    -h		    print help and exit 1
    -m section	    man page section (def: $MAN_SECTION)
    --		    end of $0 flags
    tool	    one of the IOCCC tools to run with -h option

Exit codes:
    0	    all okay
    1	    help string printed
    2	    tool missing or command missing
    3	    man page missing
    4	    command line usage error
    5	    missing or inconsistent synopsis
    >=42    internal error"


# parse args
#
while getopts :hm: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 1
       ;;
    m) M_FLAG="-m"
       MAN_SECTION="$OPTARG";
       ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
       exit 4
       ;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
       exit 4
       ;;
   *)
       ;;
    esac
done

if [[ -z "$MAN_SECTION" ]]; then
    echo "$0: ERROR: -m MAN_SECTION cannot be empty" 1>&2
    exit 4
fi

# check args
#

# extract tool name
shift $(( OPTIND - 1 ));
if [[ $# -gt 1 ]]; then
    echo "$0: ERROR: expected 1 tool name, got $#"  1>&2
    exit 4
fi

# verify tool exists and is a regular file and can be executed
if [[ ! -e "$1" ]]; then
    echo "$0: missing tool: $1" 1>&2
    exit 2
fi
if [[ ! -f "$1" ]]; then
    echo "$0: tool not a regular file: $1" 1>&2
    exit 2
fi
if [[ ! -x "$1" ]]; then
    echo "$0: tool not executable: $1" 1>&2
    exit 2
fi

# we also have to verify that the commands we need exist

# check for executable grep
GREP=$(type -P grep)
if [[ -z "${GREP}" ]]; then
    echo "$0: unable to find grep" 1>&2
    exit 2
fi
if [[ ! -e "$GREP" ]]; then
    echo "$0: missing grep: $GREP" 1>&2
    exit 2
fi
if [[ ! -f "$GREP" ]]; then
    echo "$0: grep not a regular file: $GREP" 1>&2
    exit 2
fi
if [[ ! -x "$GREP" ]]; then
    echo "$0: grep not executable: $GREP" 1>&2
    exit 2
fi

# check for executable cut
CUT=$(type -P cut)
if [[ -z "${CUT}" ]]; then
    echo "$0: unable to find cut" 1>&2
    exit 2
fi
if [[ ! -e "$CUT" ]]; then
    echo "$0: missing cut: $CUT" 1>&2
    exit 2
fi
if [[ ! -f "$CUT" ]]; then
    echo "$0: cut not a regular file: $CUT" 1>&2
    exit 2
fi
if [[ ! -x "$CUT" ]]; then
    echo "$0: cut not executable: $CUT" 1>&2
    exit 2
fi

# check for executable sed
SED=$(type -P sed)
if [[ -z "${SED}" ]]; then
    echo "$0: unable to find sed" 1>&2
    exit 2
fi
if [[ ! -e "$SED" ]]; then
    echo "$0: missing sed: $SED" 1>&2
    exit 2
fi
if [[ ! -f "$SED" ]]; then
    echo "$0: sed not a regular file: $SED" 1>&2
    exit 2
fi
if [[ ! -x "$SED" ]]; then
    echo "$0: sed not executable: $SED" 1>&2
    exit 2
fi

# check for executable awk
AWK=$(type -P awk)
if [[ -z "${AWK}" ]]; then
    echo "$0: unable to find awk" 1>&2
    exit 2
fi
if [[ ! -e "$AWK" ]]; then
    echo "$0: missing awk: $AWK" 1>&2
    exit 2
fi
if [[ ! -f "$AWK" ]]; then
    echo "$0: awk not a regular file: $AWK" 1>&2
    exit 2
fi
if [[ ! -x "$AWK" ]]; then
    echo "$0: awk not executable: $AWK" 1>&2
    exit 2
fi

# check for man page
if [[ ! -e "$1.$MAN_SECTION" ]]; then
    echo "$0: missing man page for $1 in section $MAN_SECTION" 1>&2
    exit 3
fi
if [[ ! -f "$1.$MAN_SECTION" ]]; then
    echo "$0: man page $1.$MAN_SECTION not a regular file" 1>&2
    exit 3
fi
if [[ ! -r "$1.$MAN_SECTION" ]]; then
    echo "$0: man page $1.$MAN_SECTION not a readable file" 1>&2
    exit 3
fi

# WARNING: if your eyes or brain cannot deal with indecent hacks you should not
# look below this line as this is where it gets uglier and becomes a hack.

# extract the usage message but add the correct formatting (or as best as
# possible) for man pages. This is not perfect by any stretch of the
# imagination.
#
# We need to use single quotes for awk which shellcheck does not understand.
# shellcheck disable=SC2016
USAGE_FMT=$(./"$1" -h 2>&1 | $GREP usage: | $CUT -f2- -d: | $SED -e 's/^[[:space:]]*//g' -e 's,./,,g' | \
    $SED -e 's,^,\\fB,g' | $AWK '{$1=$1"\\fP"}1' | $SED -e 's,-,\\-,g');

# Attempt to determine the synopsis regexp that we have to check the man page
# for. This is not perfect either. It's a hack.
USAGE_RE="$(echo "$USAGE_FMT"|$SED -e 's/\\/\\\\/g' -e 's/\[/\\[/g' -e 's/\]/\\]/g')"

$GREP -q "$USAGE_RE" "$1"."${MAN_SECTION}"
STATUS=$?
if [[ "$STATUS" -ne 0 ]]; then
    echo "Missing or inconsistent usage message for $1." 1>&2
    echo "If you know what you're doing you can try adding the following string" 1>&2
    echo "to the SYNOPSIS section of the man page:" 1>&2
    echo "" 1>&2
    echo "$USAGE_FMT" 1>&2
    echo "" 1>&2
    echo "If you have any questions or doubts about this please don't do this!" 1>&2
    echo "If you do edit it please don't forget to update the date of the man page."
    exit 5
fi

# theoretically everything is okay if we get here
exit 0
