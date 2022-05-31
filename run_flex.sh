#!/usr/bin/env bash
#
# run_flex.sh - try to run flex but use backup output files if needed
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
export USAGE="usage: $0 [-h] [-v level] [-V] [-o] [-f flex] [-l limit_ioccc.sh]
		        [-g verge] [-p prefix] [-s sorry.h] [-F dir] -- [flex_flags ..]

    -h              print help and exit 8
    -v level        set debug level (def: 0)
    -V              print version and exit 8
    -o		    do NOT use backup files, fail if flex cannot be used (def: use)
    -f flex	    flex tool basename (def: flex)
    -l limit.sh	    version info file (def: ./limit_ioccc.sh)
    -g verge	    path to verge tool (def: ./verge)
    -p prefix	    The prefix of files to be used (def: jparse)
			NOTE: If the final arg is flex:
			NOTE: The flex input file will be prefix.y
			NOTE: If flex cannot be used, these backup
			NOTE: files are used:
			NOTE:
			NOTE:		prefix.c
			NOTE:
    -s sorry.h	    File to prepend to C output (def: sorry.tm.ca.h)
    -F dir          first look for flex in dir (def: look just along \$PATH)
		        NOTE: Multiple -B dir are allowed.
		        NOTE: Search is performed in dir order before the \$PATH path.
			NOTE: If dir is missing or not searchable, dir is ignored.
			NOTE: This is ignored if the final arg is NOT flex.
    --		    End of $0 flags
    flex_flags ..   optional flags to give to flex for the prefix.y argument

Exit codes:
    0    flex output files formed or backup files used instead
    1	 flex not found or too old and -o used
    2    good flex found and ran but failed to form proper output files
    3    flex input file missing or not readable:         backup file(s) had to be used
    4    backup file(s) are missing, or are not readable
    5    failed to use backup file(s) to form the flex C output file(s)
    6    limit_ioccc.sh or sorry file missing/not readable or verge missing/not executable
    7    MIN_FLEX_VERSION missing or empty from limit_ioccc.sh
    8    -h and help string printed or -V and version string printed
    9    Command line usage error
    >=10  internal error"
export RUN_FLEX_VERSION="0.3 2022-04-22"
export PREFIX="jparse"
export V_FLAG="0"
export FLEX_BASENAME="flex"
export LIMIT_IOCCC_SH="./limit_ioccc.sh"
export VERGE="./verge"
export MIN_FLEX_VERSION=
declare -a FLEX_DIRS=()
export SORRY_H="sorry.tm.ca.h"

# parse args
#
while getopts :hv:Vof:l:g:p:s:F: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 8
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    V) echo "$RUN_FLEX_VERSION"
       exit 8
       ;;
    o) O_FLAG="-o"
       ;;
    f) FLEX_BASENAME="$OPTARG";
       ;;
    l) LIMIT_IOCCC_SH="$OPTARG";
       ;;
    g) VERGE="$OPTARG";
       ;;
    p) PREFIX="$OPTARG";
       ;;
    s) SORRY_H="$OPTARG";
       ;;
    F) FLEX_DIRS[${#FLEX_DIRS[@]}]="$OPTARG";
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
if [[ -z $FLEX_BASENAME ]]; then
    echo "$0: ERROR: -b $FLEX_BASENAME name cannot be empty" 1>&2
    exit 9
fi
if [[ -z $PREFIX ]]; then
    echo "$0: ERROR: -p prefix cannot be empty" 1>&2
    exit 9
fi

# check args
#
shift $(( OPTIND - 1 ));
if [[ $V_FLAG -ge 1 ]]; then
    if [[ $# -gt 0 ]]; then
	echo "$0: debug[1]: args to pass to flex before $PREFIX.y: $*" 1>&2
    else
	echo "$0: debug[1]: pass no args to flex before $PREFIX.y" 1>&2
    fi
fi

# firewall
#
if [[ ! -e $LIMIT_IOCCC_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh file not found: $LIMIT_IOCCC_SH" 1>&2
    exit 6
fi
if [[ ! -f $LIMIT_IOCCC_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh not a file: $LIMIT_IOCCC_SH" 1>&2
    exit 6
fi
if [[ ! -r $LIMIT_IOCCC_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh not a readable file: $LIMIT_IOCCC_SH" 1>&2
    exit 6
fi
if [[ ! -e $VERGE ]]; then
    echo "$0: ERROR: verge file not found: $VERGE" 1>&2
    exit 6
fi
if [[ ! -f $VERGE ]]; then
    echo "$0: ERROR: verge not a file: $VERGE" 1>&2
    exit 6
fi
if [[ ! -x $VERGE ]]; then
    echo "$0: ERROR: verge not an executable: $VERGE" 1>&2
    exit 6
fi
if [[ ! -e $SORRY_H ]]; then
    echo "$0: ERROR: sorry file not found: $SORRY_H" 1>&2
    exit 6
fi
if [[ ! -f $SORRY_H ]]; then
    echo "$0: ERROR: sorry file not a file: $SORRY_H" 1>&2
    exit 6
fi
if [[ ! -r $SORRY_H ]]; then
    echo "$0: ERROR: sorry file not a readable file: $SORRY_H" 1>&2
    exit 6
fi

# source the limit_ioccc.sh file
#
# warning: ShellCheck can't follow non-constant source. Use a directive to specify location. [SC1090]
# shellcheck disable=SC1090
source "$LIMIT_IOCCC_SH" >/dev/null 2>&1
if [[ -z $MIN_FLEX_VERSION ]]; then
    echo "$0: ERROR: MIN_FLEX_VERSION missing or has an empty value from: $LIMIT_IOCCC_SH" 1>&2
    exit 7
fi

# debug
#
if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: RUN_FLEX_VERSION=$RUN_FLEX_VERSION" 1>&2
    echo "$0: debug[5]: PREFIX=$PREFIX" 1>&2
    echo "$0: debug[5]: V_FLAG=$V_FLAG" 1>&2
    echo "$0: debug[5]: FLEX_BASENAME=$FLEX_BASENAME" 1>&2
    echo "$0: debug[5]: LIMIT_IOCCC_SH=$LIMIT_IOCCC_SH" 1>&2
    echo "$0: debug[5]: VERGE=$VERGE" 1>&2
    echo "$0: debug[5]: MIN_FLEX_VERSION=$MIN_FLEX_VERSION" 1>&2
    echo "$0: debug[5]: SORRY_H=$SORRY_H" 1>&2
    for dir in "${FLEX_DIRS[@]}"; do
	echo "$0: debug[5]: FLEX_DIRS=$dir" 1>&2
    done
fi


# look_for - look for executable in a given directory with a new enough version
#
# given:
#	tool		name of tool
#	min_version	minimum version allowed
#	dir		directory to look into (empty ==> look on $PATH)
#
# returns:
#	0	dir/tool is a valid tool with a version >= min_version
#	1	tool not found, not executable, or no version info,
#		    or version too old, or usage error, or dir missing
#
look_for() {

    # parse args
    #
    if [[ $# -ne 3 ]]; then
	echo "$0: ERROR: look_for function expects 3 args, found $#" 1>&2
	exit 10
    fi
    local TOOL="$1"
    local MIN_VERSION="$2"
    local DIR="$3"
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: look_for \"$TOOL\" \"$MIN_VERSION\" \"$DIR\"" 1>&2
    fi

    # clear path found
    #
    PATH_FOUND=

    # ignore if dir is not a searchable directory
    #
    if [[ ! -d $DIR ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: ignoring missing directory: $DIR" 1>&2
	fi
	return 1
    fi
    if [[ ! -x $DIR ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: ignoring non-searchable directory: $DIR" 1>&2
	fi
	return 1
    fi

    # ignore if tool is not an executable in dir
    #
    if [[ ! -e $DIR/$TOOL ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: ignoring missing tool: $DIR/$TOOL" 1>&2
	fi
	return 1
    fi
    if [[ ! -f $DIR/$TOOL ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: ignoring non-file tool: $DIR/$TOOL" 1>&2
	fi
	return 1
    fi
    if [[ ! -x $DIR/$TOOL ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: ignoring non-executable tool: $DIR/$TOOL" 1>&2
	fi
	return 1
    fi

    # dir/tool exists, try to get the version
    #
    TOOL_VERSION=$("$DIR/$TOOL" -V 2>/dev/null | head -1 | awk '{print $NF;}')
    if [[ -z $TOOL_VERSION ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: unable to obtain version of tool: $DIR/$TOOL" 1>&2
	fi
	return 1
    else
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: tool: $DIR/$TOOL has version: $TOOL_VERSION" 1>&2
	fi
    fi

    # determine if the version is >= minimum version
    #
    "$VERGE" -- "$TOOL_VERSION" "$MIN_VERSION" >/dev/null 2>&1
    status="$?"
    if [[ $status -ge 3 ]]; then
	echo "$0: Warning: verge: $VERGE returned code >= 3: $status" 1>&2
	return 1
    elif [[ $status -eq 1 ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: tool: $DIR/$TOOL version: $TOOL_VERSION < min version: $MIN_VERSION" 1>&2
	fi
	return 1
    else
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: tool: $DIR/$TOOL version: $TOOL_VERSION >= min version: $MIN_VERSION" 1>&2
	fi
    fi

    # set path found
    #
    PATH_FOUND="$DIR/$TOOL"
    return 0;
}


# on_path - look for executable on $PATH with a new enough version
#
# given:
#	tool		name of tool
#	min_version	minimum version allowed
#
# returns:
#	0	tool found on $PATH and is a valid tool with a version >= min_version
#	1	tool not found, not executable, or no version info,
#		    or version too old, or usage error, or $PATH is empty
#
on_path() {

    # parse args
    #
    if [[ $# -ne 2 ]]; then
	echo "$0: ERROR: on_path function expects 2 args, found $#" 1>&2
	exit 11
    fi
    local TOOL="$1"
    local MIN_VERSION="$2"
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: on_path \"$TOOL\" \"$MIN_VERSION\"" 1>&2
    fi
    local TOOL_PATH=

    # clear path found
    #
    PATH_FOUND=

    # ignore if $PATH is empty
    #
    if [[ -z $PATH ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: PATH is empty" 1>&2
	fi
	return 1
    fi

    # ignore if tool is not on path
    #
    TOOL_PATH=$(command -v "$TOOL" 2>/dev/null)
    status="$?"
    if [[ $status -ne 0 || -z $TOOL_PATH ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: tool: $TOOL not found on PATH: $PATH" 1>&2
	fi
	return 1
    fi

    # dir/tool exists, try to get the version
    #
    TOOL_VERSION=$("$TOOL_PATH" -V 2>/dev/null | head -1 | awk '{print $NF;}')
    if [[ -z $TOOL_VERSION ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: unable to obtain version of tool: $TOOL_PATH" 1>&2
	fi
	return 1
    else
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: tool: $TOOL_PATH has version: $TOOL_VERSION" 1>&2
	fi
    fi

    # determine if the version is >= minimum version
    #
    "$VERGE" -- "$TOOL_VERSION" "$MIN_VERSION" >/dev/null 2>&1
    status="$?"
    if [[ $status -gt 1 ]]; then
	echo "$0: Warning: verge: $VERGE returned code > 1: $status" 1>&2
	return 1
    elif [[ $status -eq 1 ]]; then
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: tool: $TOOL_PATH version: $TOOL_VERSION < min version: $MIN_VERSION" 1>&2
	fi
	return 1
    else
	if [[ $V_FLAG -ge 7 ]]; then
	    echo "$0: debug[7]: tool: $TOOL_PATH version: $TOOL_VERSION >= min version: $MIN_VERSION" 1>&2
	fi
    fi

    # set path found
    #
    PATH_FOUND="$TOOL_PATH"

    # if print mode, print this path on stdout
    #
    if [[ -n $PRINT_MODE ]]; then
	echo "$TOOL_PATH"
    fi
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: tool on \$PATH: $TOOL_PATH version: $TOOL_VERSION is good" 1>&2
    fi
    # found a good tool on $PATH
    return 0;
}

# use_flex_backup - use backup flex C files in place of flex generated C files
#
# NOTE: If -o was given, then we will exit instead of using backup flex C files.
#
# warning: use_flex_backup references arguments, but none are ever passed. [SC2120]
# shellcheck disable=SC2120
use_flex_backup() {

    # parse args
    #
    if [[ $# -ne 0 ]]; then
	echo "$0: ERROR: use_flex_backup function expects 0 args, found $#" 1>&2
	exit 12
    fi

    # If -o, exit instead of using backup flex C files
    #
    if [[ -n $O_FLAG ]]; then
	echo "$0: ERROR: unable to find or use flex and -o given" 1>&2
	exit 1
    fi

    # look for backup flex C files
    #
    FLEX_BACKUP_C="$PREFIX.ref.c"
    FLEX_C="$PREFIX.c"
    if [[ ! -f $FLEX_BACKUP_C ]]; then
	echo "$0: ERROR: not a file: $FLEX_BACKUP_C" 1>&2
	exit 4
    fi
    if [[ ! -r $FLEX_BACKUP_C ]]; then
	echo "$0: ERROR: not a readable file: $FLEX_BACKUP_C" 1>&2
	exit 4
    fi
    if [[ ! -s $FLEX_BACKUP_C ]]; then
	echo "$0: ERROR: empty file: $FLEX_BACKUP_C" 1>&2
	exit 4
    fi

    # copy backup flex C files in place
    #
    echo "# Warning: We are forced to use $FLEX_BASENAME backup files instead of $FLEX_BASENAME C output!" 1>&2
    echo "cp -f -v $FLEX_BACKUP_C $FLEX_C"
    cp -f -v "$FLEX_BACKUP_C" "$FLEX_C"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: failed to copy $FLEX_BACKUP_C to $FLEX_C exit code: $status" 1>&2
	exit 5
    fi

    # moved flex backup files in place
    #
    return 0
}

# add_sorry - prepend the apology file and line number reset
#
add_sorry() {

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: add_sorry function expects 1 args, found $#" 1>&2
	exit 13
    fi
    FILE="$1"

    # obtain a temporary filename
    #
    TMP_FILE=$(mktemp -t "$FILE.XXXXXXXX")
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: mktemp -t $FILE exit code: $status" 1>&2
	exit 14
    fi
    if [[ ! -e $TMP_FILE ]]; then
	echo "$0: ERROR: tmp file not found: $TMP_FILE" 1>&2
	exit 15
    fi
    if [[ ! -f $TMP_FILE ]]; then
	echo "$0: ERROR: tmp not a file: $TMP_FILE" 1>&2
	exit 16
    fi
    if [[ ! -r $TMP_FILE ]]; then
	echo "$0: ERROR: tmp not a readable file: $TMP_FILE" 1>&2
	exit 17
    fi
    if [[ ! -w $TMP_FILE ]]; then
	echo "$0: ERROR: tmp not a writable file: $TMP_FILE" 1>&2
	exit 18
    fi

    # form the file with the apology
    #
    echo "# prepending comment and line number reset to $FILE"
    echo "cat $SORRY_H > $TMP_FILE"
    cat "$SORRY_H" > "$TMP_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cat $SORRY_H > $TMP_FILE exit code: $status" 1>&2
	exit 19
    fi
    echo "#line 1 \"$FILE\"" >> "$TMP_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: appending line number reset failed, exit code: $status" 1>&2
	exit 20
    fi
    echo "cat $FILE >> $TMP_FILE"
    cat "$FILE" >> "$TMP_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cat $FILE >> $TMP_FILE failed, exit code: $status" 1>&2
	exit 21
    fi

    # move the modified file into place
    #
    echo "mv -v -f $TMP_FILE $FILE"
    mv -v -f "$TMP_FILE" "$FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: mv -v -f $TMP_FILE $FILE failed, exit code: $status" 1>&2
	exit 22
    fi
    echo "# completed update of $FILE"
    return 0
}


# run flex
#
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[3]: checking for flex as: $FLEX_BASENAME" 1>&2
fi

# check -B dirs first
#
export FLEX_PATH=
for dir in "${FLEX_DIRS[@]}"; do

    # look for a good flex in dir
    #
    look_for "$FLEX_BASENAME" "$MIN_FLEX_VERSION" "$dir"
    status="$?"
    if [[ $status -eq 0 && -n $PATH_FOUND ]]; then
	FLEX_PATH="$PATH_FOUND"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: found a good $FLEX_BASENAME: $FLEX_PATH" 1>&2
	fi
	break
    fi
done

# if no flex found so far, look on $PATH
#
if [[ -z $FLEX_PATH ]]; then
    on_path "$FLEX_BASENAME" "$MIN_FLEX_VERSION"
    status="$?"
    if [[ $status -eq 0 && -n $PATH_FOUND ]]; then
	FLEX_PATH="$PATH_FOUND"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: $FLEX_BASENAME on \$PATH is good: $FLEX_PATH" 1>&2
	fi
    fi
fi

# case: no usable flex found
#
if [[ -z $FLEX_PATH ]]; then
    echo "# Warning: failed to discover the path for an up to date flex as: $FLEX_BASENAME" 1>&2
    use_flex_backup
    exit 0
fi

# verify that flex input file is readable
#
if [[ ! -f "$PREFIX.l" ]]; then
    echo "$0: ERROR: $FLEX_BASENAME input file not found: $PREFIX.l" 1>&2
    exit 3
fi
if [[ ! -r "$PREFIX.l" ]]; then
    echo "$0: ERROR: $FLEX_BASENAME input file not readable: $PREFIX.l" 1>&2
    exit 3
fi

# execute flex
#
echo "$FLEX_PATH $* $PREFIX.l"
"$FLEX_PATH" "$@" "$PREFIX.l"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: $FLEX_PATH failed, exit code: $status" 1>&2
    exit 2
fi

# verify the flex C files are non-empty readable files
#
if [[ ! -f $PREFIX.c ]]; then
    echo "$0: Warning: $FLEX_PATH failed to form file: $PREFIX.c" 1>&2
    use_flex_backup
elif [[ ! -s $PREFIX.c ]]; then
    echo "$0: Warning: $FLEX_PATH failed to form non-empty file: $PREFIX.c" 1>&2
    use_flex_backup
else

    # prepend the apology file and line number reset
    #
    add_sorry "$PREFIX.c"

    # print debug of result if -v
    #
    if [[ $V_FLAG -ge 1 ]]; then
	# note: Double quote to prevent globbing and word splitting. [SC2086]
	# shellcheck disable=SC2086
	echo "$0: debug[1]: $(ls -l $PREFIX.c)"
	echo "$0: debug[1]: $FLEX_BASENAME run OK: formed $PREFIX.c" 1>&2
    fi
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
