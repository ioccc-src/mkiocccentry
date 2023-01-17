#!/usr/bin/env bash
#
# run_usage.sh - run -h on a tool to extract usage information in order to help
# with the creation and maintaining man pages
#
# This tool is in support of verifying that the man page SYNOPSIS is the same as
# the current options of the tool. It's not perfect and at least part of it is a
# hack. There are limitations. For more details check the NOTES and BUGS
# sections of the man page.
#
# This quick, dirty and possibly ugly hack was not written by Cody Boone
# Ferguson in support of issue
# https://github.com/ioccc-src/mkiocccentry/issues/275. :-) Okay it was written
# by Cody but it's dirty and maybe ugly and there's room for improvement. As
# noted there are limitations. See the man page for details.
#

# set up
#
export MAN_SECTION="1"
export M_FLAG=""
export MAN_FLAG=""
export MAN_PAGE=""
export MAN_DIR="./man"
export RUN_USAGE_VERSION="0.2 2022-11-06"
export USAGE="usage: $0 [-h] [-V] [-m section] [-M man_file] [-D man_dir] tool

    -h		    print help and exit
    -V		    print version and exit
    -m section	    man page section (def: $MAN_SECTION)
    -M man file	    man page (including extension)
    -D man_dir	    directory that man pages are to be found in
    --		    end of $0 flags
    tool	    one of the IOCCC tools to run with -h option

Exit codes:
     0	 all okay
     1	 help or version string printed
     2	 tool missing or command missing
     3	 man page missing
     4	 command line usage error
     5	 missing or inconsistent synopsis
     6	 tool does not have usage string
     7	 man directory does not exist or is not a readable directory

$0 version: $RUN_USAGE_VERSION"

# parse args
#
while getopts :hVm:M:D: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 1
	;;
    V)	echo "$RUN_USAGE_VERSION"
	exit 1
	;;
    m)	M_FLAG="-m"
	MAN_SECTION="$OPTARG";
	;;
    M)	MAN_FLAG="-M"
	MAN_PAGE="$OPTARG";
	;;
    D)	MAN_DIR="$OPTARG";
	;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
	exit 4
	;;
    :)	echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
	exit 4
	;;
    *)
	;;
    esac
done

if [[ -z "$MAN_SECTION" && -z "$MAN_PAGE" ]]; then
    echo "$0: ERROR: -m MAN_SECTION and -M man page cannot both be empty" 1>&2
    exit 4
fi

# check args
#

# extract tool name
shift $(( OPTIND - 1 ));
if [[ $# -ne 1 ]]; then
    echo "$0: ERROR: expected 1 tool name, got $#"  1>&2
    echo "$USAGE"
    exit 4
fi

# verify tool exists and is a regular file and can be executed
if [[ ! -e "$1" ]]; then
    echo "$0: ERROR: missing tool: $1" 1>&2
    exit 2
fi
if [[ ! -f "$1" ]]; then
    echo "$0: ERROR: tool not a regular file: $1" 1>&2
    exit 2
fi
if [[ ! -x "$1" ]]; then
    echo "$0: ERROR: tool not executable: $1" 1>&2
    exit 2
fi

# verify man directory exists
if [[ ! -e "$MAN_DIR" ]]; then
    echo "$0: ERROR: missing man directory: $MAN_DIR" 1>&2
    exit 7
fi
if [[ ! -d "$MAN_DIR" ]]; then
    echo "$0: ERROR: man directory path not a directory: $MAN_DIR" 1>&2
    exit 7
fi
if [[ ! -r "$MAN_DIR" ]]; then
    echo "$0: ERROR: man directory not readable: $MAN_DIR" 1>&2
    exit 7
fi



# we also have to verify that the commands we need exist

# check for executable grep
GREP=$(type -P grep)
if [[ -z "${GREP}" ]]; then
    echo "$0: ERROR: unable to find grep" 1>&2
    exit 2
fi
if [[ ! -e "$GREP" ]]; then
    echo "$0: ERROR: missing grep: $GREP" 1>&2
    exit 2
fi
if [[ ! -f "$GREP" ]]; then
    echo "$0: ERROR: grep not a regular file: $GREP" 1>&2
    exit 2
fi
if [[ ! -x "$GREP" ]]; then
    echo "$0: ERROR: grep not executable: $GREP" 1>&2
    exit 2
fi

# check for executable cut
CUT=$(type -P cut)
if [[ -z "${CUT}" ]]; then
    echo "$0: ERROR: unable to find cut" 1>&2
    exit 2
fi
if [[ ! -e "$CUT" ]]; then
    echo "$0: ERROR: missing cut: $CUT" 1>&2
    exit 2
fi
if [[ ! -f "$CUT" ]]; then
    echo "$0: ERROR: cut not a regular file: $CUT" 1>&2
    exit 2
fi
if [[ ! -x "$CUT" ]]; then
    echo "$0: ERROR: cut not executable: $CUT" 1>&2
    exit 2
fi

# check for executable sed
SED=$(type -P sed)
if [[ -z "${SED}" ]]; then
    echo "$0: ERROR: unable to find sed" 1>&2
    exit 2
fi
if [[ ! -e "$SED" ]]; then
    echo "$0: ERROR: missing sed: $SED" 1>&2
    exit 2
fi
if [[ ! -f "$SED" ]]; then
    echo "$0: ERROR: sed not a regular file: $SED" 1>&2
    exit 2
fi
if [[ ! -x "$SED" ]]; then
    echo "$0: ERROR: sed not executable: $SED" 1>&2
    exit 2
fi

# check for executable awk
AWK=$(type -P awk)
if [[ -z "${AWK}" ]]; then
    echo "$0: ERROR: unable to find awk" 1>&2
    exit 2
fi
if [[ ! -e "$AWK" ]]; then
    echo "$0: ERROR: missing awk: $AWK" 1>&2
    exit 2
fi
if [[ ! -f "$AWK" ]]; then
    echo "$0: ERROR: awk not a regular file: $AWK" 1>&2
    exit 2
fi
if [[ ! -x "$AWK" ]]; then
    echo "$0: ERROR: awk not executable: $AWK" 1>&2
    exit 2
fi

# WARNING: if your eyes or brain cannot deal with indecent hacks you should not
# look below this line as this is where it starts to get uglier and later
# becomes a hack. Even so it could be worse and it's probably not _that_ ugly.

# extract the usage message but add the correct formatting (or as best as
# possible) for man pages. This is not perfect by any stretch of the
# imagination.
#
# We need to use single quotes for awk which shellcheck does not understand:
# note: Expressions don't expand in single quotes, use double quotes for that. [SC2016]
# shellcheck disable=SC2016
USAGE_FMT=$(./"$1" -h 2>&1 | $GREP usage: | $CUT -f2- -d: | $SED -e 's/^[[:space:]]*//g' -e 's,./,,g' | \
    $SED -e 's,^,\\fB,g' | $AWK '{$1=$1"\\fP"}1' | $SED -e 's,-,\\-,g');


if [[ -z "$USAGE_FMT" ]]; then
    echo "$0: ERROR: $1 has no usage string" 1>&2
    exit 6
fi

# Attempt to determine the synopsis regexp that we have to check the man page
# for. This is not perfect either. It might be a hack. See the man page.
USAGE_RE="$(echo "$USAGE_FMT"|$SED -e 's/\\/\\\\/g' -e 's/\[/\\[/g' -e 's/\]/\\]/g')"

# check for man page
if [[ -z "$MAN_FLAG" && -z "$MAN_PAGE" ]]; then
    MAN_PAGE="$MAN_DIR/$1.$MAN_SECTION"
fi

if [[ ! -e "$MAN_PAGE" ]]; then
    echo "$0: ERROR: missing man page for $1: $MAN_PAGE" 1>&2
    echo "Once a man page exists, you can try adding to the SYNOPSIS section:" 1>&2
    echo "" 1>&2
    echo "$USAGE_FMT" 1>&2
    echo "" 1>&2
    exit 3
fi

if [[ ! -f "$MAN_PAGE" ]]; then
    echo "$0: ERROR: man page $MAN_PAGE not a regular file" 1>&2
    echo "Once a man page exists, you can try adding to the SYNOPSIS section:" 1>&2
    echo "" 1>&2
    echo "$USAGE_FMT" 1>&2
    echo "" 1>&2
    exit 3
fi
if [[ ! -r "$MAN_PAGE" ]]; then
    echo "$0: ERROR: man page $MAN_PAGE not a readable file" 1>&2
    echo "Once a man page exists, you can try adding to the SYNOPSIS section:" 1>&2
    echo "" 1>&2
    echo "$USAGE_FMT" 1>&2
    echo "" 1>&2
    exit 3
fi

# This is is a hack but it's needed to try and determine if all the usage
# strings are present in the man page. It works by finding out how many usage
# messages are in the tool. Then if the grep -c on the pattern does not match
# the number of lines we can guess that it's not correct.
#
# Another way of getting the number of lines is echoing the USAGE_RE and piping
# it to wc -l but this would require also deleting spaces via say tr(1).
#
# NOTE: It still won't work right if all the usage strings are the same as
# it only works by the number of matches not what is matched. It works by first
# determining how many lines the regexp contains and then uses the -c option of
# grep, checking if the number returned by grep is the same as the number of
# lines.
LINES=$(./"$1" -h 2>&1 | grep -c usage)
if [[ $($GREP -c "$USAGE_RE" "$MAN_PAGE") -ne "$LINES" ]]; then
    echo "$0: ERROR: Possible missing or inconsistent usage message for $1." 1>&2
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
