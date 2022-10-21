#!/usr/bin/env bash
#
# run_bison.sh - try to run bison but use backup output files if needed
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
export RUN_BISON_VERSION="0.3 2022-04-22"
export USAGE="usage: $0 [-h] [-V] [-v level] [-V] [-o] [-b bison] [-l limit_ioccc.sh]
		        [-g verge] [-p prefix] [-s sorry.h] [-B dir] -- [bison_flags]

    -h		    print help and exit
    -V		    print version and exit
    -v level	    set debug level (def: 0)
    -o		    do NOT use backup files, fail if bison cannot be used (def: use)
    -b bison	    bison tool basename (def: bison)
    -l limit.sh	    version info file (def: ./limit_ioccc.sh)
    -g verge	    path to verge tool (def: ./verge)
    -p prefix	    the prefix of files to be used (def: jparse)
			NOTE: The bison input file will be prefix.y
			NOTE: If bison cannot be used, these backup
			NOTE: files are used:
			NOTE:
			NOTE:	 prefix.tab.ref.c prefix.tab.ref.h
			NOTE:
    -s sorry.h	    file to prepend to C output (def: sorry.tm.ca.h)
    -B dir          first look for bison in dir (def: look just along \$PATH)
		        NOTE: Multiple -B dir are allowed.
		        NOTE: Search is performed in dir order before the \$PATH path.
			NOTE: If dir is missing or not searchable, dir is ignored.
			NOTE: This is ignored if the final arg is NOT bison.
    --		    end of $0 flags
    bison_flags    optional flags to give to bison before the prefix.y argument

Exit codes:
     0   bison output files formed or backup files used instead
     1   bison not found or too old and -o used
     2   good bison found and ran but failed to form proper output files
     3   bison input file missing or not readable: backup file(s) had to be used
     4   backup file(s) are missing, or are not readable
     5   failed to use backup file(s) to form the bison C output file(s)
     6   limit_ioccc.sh or sorry.h file missing/not readable or verge missing/not executable
     7   MIN_BISON_VERSION Missing or empty from limit_ioccc.sh
     8   -h and help string printed or -V and version string printed
     9   command line usage error
 >= 10   internal error

$0 version: $RUN_BISON_VERSION"
export PREFIX="jparse"
export V_FLAG="0"
export BISON_BASENAME="bison"
export LIMIT_IOCCC_SH="./limit_ioccc.sh"
export VERGE="./verge"
export MIN_BISON_VERSION=
declare -a BISON_DIRS=()
export SORRY_H="sorry.tm.ca.h"
export O_FLAG=

# parse args
#
while getopts :hv:Vob:l:g:p:s:B: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 8
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    V) echo "$RUN_BISON_VERSION"
       exit 8
       ;;
    o) O_FLAG="-o"
       ;;
    b) BISON_BASENAME="$OPTARG";
       ;;
    l) LIMIT_IOCCC_SH="$OPTARG";
       ;;
    g) VERGE="$OPTARG";
       ;;
    p) PREFIX="$OPTARG";
       ;;
    s) SORRY_H="$OPTARG";
       ;;
    B) BISON_DIRS[${#BISON_DIRS[@]}]="$OPTARG";
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
if [[ -z $BISON_BASENAME ]]; then
    echo "$0: ERROR: -b $BISON_BASENAME name cannot be empty" 1>&2
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
	echo "$0: debug[1]: args to pass to bison before $PREFIX.y: $*" 1>&2
    else
	echo "$0: debug[1]: pass no args to bison before $PREFIX.y" 1>&2
    fi
fi

# firewall
#
if [[ ! -e $LIMIT_IOCCC_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh file not found: $LIMIT_IOCCC_SH" 1>&2
    exit 6
fi
if [[ ! -f $LIMIT_IOCCC_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh not a regular file: $LIMIT_IOCCC_SH" 1>&2
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
    echo "$0: ERROR: verge not a regular file: $VERGE" 1>&2
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
    echo "$0: ERROR: sorry file not a regular file: $SORRY_H" 1>&2
    exit 6
fi
if [[ ! -r $SORRY_H ]]; then
    echo "$0: ERROR: sorry file not a readable file: $SORRY_H" 1>&2
    exit 6
fi

# source the limit_ioccc.sh file
#
# We have to disable this check because we can't use the shellcheck directive
# source="$LIMIT_IOCCC_SH" as this requires the -x option and not all shellcheck
# versions support -x:
#
# warning: ShellCheck can't follow non-constant source. Use a directive to specify location. [SC1090]
# shellcheck disable=SC1090
#
source "$LIMIT_IOCCC_SH" >/dev/null 2>&1
if [[ -z $MIN_BISON_VERSION ]]; then
    echo "$0: ERROR: MIN_BISON_VERSION missing or has an empty value from: $LIMIT_IOCCC_SH" 1>&2
    exit 7
fi

# debug
#
if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: RUN_BISON_VERSION=$RUN_BISON_VERSION" 1>&2
    echo "$0: debug[5]: PREFIX=$PREFIX" 1>&2
    echo "$0: debug[5]: V_FLAG=$V_FLAG" 1>&2
    echo "$0: debug[5]: BISON_BASENAME=$BISON_BASENAME" 1>&2
    echo "$0: debug[5]: LIMIT_IOCCC_SH=$LIMIT_IOCCC_SH" 1>&2
    echo "$0: debug[5]: VERGE=$VERGE" 1>&2
    echo "$0: debug[5]: MIN_BISON_VERSION=$MIN_BISON_VERSION" 1>&2
    echo "$0: debug[5]: SORRY_H=$SORRY_H" 1>&2
    for dir in "${BISON_DIRS[@]}"; do
	echo "$0: debug[5]: BISON_DIRS=$dir" 1>&2
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

    # if print mode, print this path on stdout
    #
    if [[ -n $PRINT_MODE ]]; then
	echo "$DIR/$TOOL"
    fi
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: tool: $PATH_FOUND version: $TOOL_VERSION is good" 1>&2
    fi
    # found a good tool in $PATH
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
    TOOL_PATH=$(type -P "$TOOL" 2>/dev/null)
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

# use_bison_backup - use backup bison C files in place of bison generated C files
#
# NOTE: If -o was given, then we will exit instead of using backup bison C files.
#
# This warning is specifically triggered because we check for the number of args
# passed to the function to make sure none are given so we have to disable it:
#
# warning: use_bison_backup references arguments, but none are ever passed. [SC2120]
# shellcheck disable=SC2120
use_bison_backup() {

    # parse args
    #
    if [[ $# -ne 0 ]]; then
	echo "$0: ERROR: use_bison_backup function expects 0 args, found $#" 1>&2
	exit 12
    fi

    # If -o, exit instead of using backup bison C files
    #
    if [[ -n $O_FLAG ]]; then
	echo "$0: ERROR: unable to find or use bison and -o given" 1>&2
	exit 1
    fi

    # look for bison backup bison C files
    #
    BISON_BACKUP_C="$PREFIX.tab.ref.c"
    BISON_C="$PREFIX.tab.c"
    BISON_BACKUP_H="$PREFIX.tab.ref.h"
    BISON_H="$PREFIX.tab.h"
    if [[ ! -e $BISON_BACKUP_C ]]; then
	echo "$0: ERROR: file not found: $BISON_BACKUP_C" 1>&2
	exit 4
    fi
    if [[ ! -f $BISON_BACKUP_C ]]; then
	echo "$0: ERROR: not a regular file: $BISON_BACKUP_C" 1>&2
	exit 4
    fi
    if [[ ! -r $BISON_BACKUP_C ]]; then
	echo "$0: ERROR: not a readable file: $BISON_BACKUP_C" 1>&2
	exit 4
    fi
    if [[ ! -s $BISON_BACKUP_C ]]; then
	echo "$0: ERROR: empty file: $BISON_BACKUP_C" 1>&2
	exit 4
    fi
    if [[ ! -f $BISON_BACKUP_H ]]; then
	echo "$0: ERROR: not a regular file: $BISON_BACKUP_H" 1>&2
	exit 4
    fi
    if [[ ! -r $BISON_BACKUP_H ]]; then
	echo "$0: ERROR: not a readable file: $BISON_BACKUP_H" 1>&2
	exit 4
    fi
    if [[ ! -s $BISON_BACKUP_H ]]; then
	echo "$0: ERROR: empty file: $BISON_BACKUP_H" 1>&2
	exit 4
    fi

    # copy bison backup C files in place
    #
    echo "Warning: We are forced to use $BISON_BASENAME backup files instead of $BISON_BASENAME C output!" 1>&2
    echo "cp -f -v $BISON_BACKUP_C $BISON_C"
    cp -f -v "$BISON_BACKUP_C" "$BISON_C"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: failed to copy $BISON_BACKUP_C to $BISON_C exit code: $status" 1>&2
	exit 5
    fi
    echo "cp -f -v $BISON_BACKUP_H $BISON_H"
    cp -f -v "$BISON_BACKUP_H" "$BISON_H"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: failed to copy $BISON_BACKUP_H to $BISON_H exit code: $status" 1>&2
	exit 5
    fi

    # moved bison backup files in place
    #
    return 0
}

# add_sorry - prepend the apology file and line number reset
#
add_sorry() {

    # parse args
    #
    if [[ $# -ne 1 ]]; then
	echo "$0: ERROR: add_sorry function expects 1 arg, found $#" 1>&2
	exit 13
    fi
    FILE="$1"

    # obtain a temporary filename
    #
    export MKTEMP_TEMPLATE TMP_FILE
    MKTEMP_TEMPLATE=".sorry.$(basename "$0").XXXXXXXXXX"
    TMP_FILE=$(mktemp "$MKTEMP_TEMPLATE")
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: mktemp $FILE exit code: $status" 1>&2
	exit 14
    fi
    if [[ ! -e $TMP_FILE ]]; then
	echo "$0: ERROR: tmp file not found: $TMP_FILE" 1>&2
	exit 15
    fi
    if [[ ! -f $TMP_FILE ]]; then
	echo "$0: ERROR: tmp not a regular file: $TMP_FILE" 1>&2
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


# run bison
#
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[3]: checking for bison as: $BISON_BASENAME" 1>&2
fi

# check -B dirs first
#
export BISON_PATH=
for dir in "${BISON_DIRS[@]}"; do

    # look for a good bison in dir
    #
    look_for "$BISON_BASENAME" "$MIN_BISON_VERSION" "$dir"
    status="$?"
    if [[ $status -eq 0 && -n $PATH_FOUND ]]; then
	BISON_PATH="$PATH_FOUND"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: found a good $BISON_BASENAME: $BISON_PATH" 1>&2
	fi
	break
    fi
done

# if no bison found so far, look on $PATH
#
if [[ -z $BISON_PATH ]]; then
    on_path "$BISON_BASENAME" "$MIN_BISON_VERSION"
    status="$?"
    if [[ $status -eq 0 && -n $PATH_FOUND ]]; then
	BISON_PATH="$PATH_FOUND"
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: $BISON_BASENAME on \$PATH is good: $BISON_PATH" 1>&2
	fi
    fi
fi

# case: no usable bison found
#
if [[ -z $BISON_PATH ]]; then
    echo "Warning: failed to discover the path for an up to date bison as: $BISON_BASENAME" 1>&2
    use_bison_backup
    exit 0
fi

# verify that bison input file is readable
#
if [[ ! -f "$PREFIX.y" ]]; then
    echo "$0: ERROR: $BISON_BASENAME input file not found: $PREFIX.y" 1>&2
    exit 3
fi
if [[ ! -r "$PREFIX.y" ]]; then
    echo "$0: ERROR: $BISON_BASENAME input file not readable: $PREFIX.y" 1>&2
    exit 3
fi

# execute bison
#
echo "$BISON_PATH $* $PREFIX.y"
"$BISON_PATH" "$@" "$PREFIX.y"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: $BISON_PATH failed, exit code: $status" 1>&2
    exit 2
fi

# verify the bison C files are non-empty readable files
#
if [[ ! -f $PREFIX.tab.c ]]; then
    echo "$0: Warning: $BISON_PATH failed to form file: $PREFIX.tab.c" 1>&2
    use_bison_backup
elif [[ ! -s $PREFIX.tab.c ]]; then
    echo "$0: Warning: $BISON_PATH failed to form non-empty file: $PREFIX.tab.c" 1>&2
    use_bison_backup
elif [[ ! -f $PREFIX.tab.h ]]; then
    echo "$0: Warning: $BISON_PATH failed to form file: $PREFIX.tab.h" 1>&2
    use_bison_backup
elif [[ ! -s $PREFIX.tab.h ]]; then
    echo "$0: Warning: $BISON_PATH failed to form non-empty file: $PREFIX.tab.h" 1>&2
    use_bison_backup
else

    # prepend the apology file and line number reset
    #
    add_sorry "$PREFIX.tab.c"
    add_sorry "$PREFIX.tab.h"

    # print debug of result if -v
    #
    if [[ $V_FLAG -ge 1 ]]; then
	# note: Double quote to prevent globbing and word splitting. [SC2086]
	echo "$0: debug[1]: $(ls -l "$PREFIX.tab.c")"
	# note: Double quote to prevent globbing and word splitting. [SC2086]
	echo "$0: debug[1]: $(ls -l "$PREFIX.tab.h")"
	echo "$0: debug[1]: $BISON_BASENAME run OK: formed $PREFIX.tab.c and $PREFIX.tab.h" 1>&2
    fi
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
