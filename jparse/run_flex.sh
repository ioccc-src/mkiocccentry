#!/usr/bin/env bash
#
# run_flex.sh - try to run flex but use backup output file if needed
#
# "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
#
# This JSON scanner was co-developed in 2022 by:
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
export RUN_FLEX_VERSION="0.5 2023-01-05"
export FLEX_BASENAME="flex"
export PREFIX="jparse"
export SORRY_H="sorry.tm.ca.h"
export VERGE="./verge"
export USAGE="usage: $0 [-h] [-V] [-v level] [-o] [-f flex]
		        [-g verge] [-p prefix] [-s sorry.h] [-S] [-F dir] [-D dir] -- [flex_flags]

    -h              print help and exit
    -V              print version and exit
    -v level        set debug level (def: 0)
    -o		    do NOT use backup file, fail if flex cannot be used (def: use)
    -f flex	    flex tool basename (def: $FLEX_BASENAME)
    -g verge	    path to verge tool (def: ./verge)
    -p prefix	    the prefix of the files to be used (def: $PREFIX)
			NOTE: The flex input file will be prefix.l
			NOTE: If flex cannot be used, these backup
			NOTE: files are used:
			NOTE:
			NOTE:		prefix.ref.c
			NOTE		prefix.lex.ref.h
			NOTE:
    -s sorry.h	    file to prepend to C output (def: $SORRY_H)
    -S		    suppress prepending apology file
    -F dir          first look for flex in dir (def: look just along \$PATH)
		        NOTE: Multiple -F dir are allowed.
		        NOTE: Search is performed in dir order before the \$PATH path.
			NOTE: If dir is missing or not searchable, dir is ignored.
			NOTE: This is ignored if the final arg is NOT flex.
    -D dir	    look for prefix file in this directory
    --		    end of $0 flags
    flex_flags      optional flags to give to flex before the prefix.l argument

Exit codes:
     0   flex output file formed or backup file used instead
     1   flex not found or too old and -o used
     2   good flex found and ran but failed to form proper output files
     3   flex input file missing or not readable: backup files had to be used
     4   backup file is missing or are not readable
     5   failed to use backup file to form the flex C output files
     6   sorry.h file missing/not readable or verge missing/not executable
     7   -h and help string printed or -V and version string printed
     8   command line usage error
 >= 10   internal error

run_flex.sh version: $RUN_FLEX_VERSION"

export D_FLAG="."
export V_FLAG="0"
export MIN_FLEX_VERSION="2.6.4"		# minimum flex version needed to build the JSON parser
declare -a FLEX_DIRS=()
export O_FLAG=
export S_FLAG=

# parse args
#
while getopts :hv:Vof:g:p:s:SF:D: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 7
	;;
    v)	V_FLAG="$OPTARG";
	;;
    V)	echo "$RUN_FLEX_VERSION"
	exit 7
	;;
    o)	O_FLAG="-o"
	;;
    f)	FLEX_BASENAME="$OPTARG";
	;;
    g)	VERGE="$OPTARG";
	;;
    p)	PREFIX="$OPTARG";
	;;
    s)	SORRY_H="$OPTARG";
	;;
    S)	S_FLAG="true"
	;;
    F)	FLEX_DIRS[${#FLEX_DIRS[@]}]="$OPTARG";
	;;
    D)	D_FLAG="$OPTARG"
	;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 8
	;;
    :)	echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 8
	;;
    *)
	;;
    esac
done
if [[ -z $FLEX_BASENAME ]]; then
    echo "$0: ERROR: -f $FLEX_BASENAME name cannot be empty" 1>&2
    exit 8
fi
if [[ -z "$D_FLAG" ]]; then
    echo "$0: ERROR: -D dir cannot be empty" 1>&2
    exit 8
fi
if [[ -z $PREFIX ]]; then
    echo "$0: ERROR: -p prefix cannot be empty" 1>&2
    exit 8
fi
# set up the final prefix
PREFIX="$D_FLAG/$PREFIX"

# check args
#
shift $(( OPTIND - 1 ));
if [[ $V_FLAG -ge 1 ]]; then
    if [[ $# -gt 0 ]]; then
	echo "$0: debug[1]: args to pass to flex before $PREFIX.l: $*" 1>&2
    else
	echo "$0: debug[1]: pass no args to flex before $PREFIX.l" 1>&2
    fi
fi

# firewall
#
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
if [[ ! -e $SORRY_H && -z "$S_FLAG" ]]; then
    echo "$0: ERROR: sorry file not found: $SORRY_H" 1>&2
    exit 6
fi
if [[ ! -f $SORRY_H && -z "$S_FLAG" ]]; then
    echo "$0: ERROR: sorry file not a regular file: $SORRY_H" 1>&2
    exit 6
fi
if [[ ! -r $SORRY_H && -z "$S_FLAG" ]]; then
    echo "$0: ERROR: sorry file not a readable file: $SORRY_H" 1>&2
    exit 6
fi

# debug
#
if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: FLEX_BASENAME=$FLEX_BASENAME" 1>&2
    echo "$0: debug[5]: MIN_FLEX_VERSION=$MIN_FLEX_VERSION" 1>&2
    echo "$0: debug[5]: PREFIX=$PREFIX" 1>&2
    echo "$0: debug[5]: RUN_FLEX_VERSION=$RUN_FLEX_VERSION" 1>&2
    echo "$0: debug[5]: SORRY_H=$SORRY_H" 1>&2
    echo "$0: debug[5]: VERGE=$VERGE" 1>&2
    echo "$0: debug[5]: V_FLAG=$V_FLAG" 1>&2
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

# use_flex_backup - use backup flex C file in place of flex generated C file
#
# NOTE: If -o was given, then we will exit instead of using backup flex C file.
#
# This warning is specifically triggered because we check for the number of args
# passed to the function to make sure none are given so we have to disable it:
#
# warning: use_flex_backup references arguments, but none are ever passed. [SC2120]
# shellcheck disable=SC2120
#
use_flex_backup() {

    # parse args
    #
    if [[ $# -ne 0 ]]; then
	echo "$0: ERROR: use_flex_backup function expects 0 args, found $#" 1>&2
	exit 12
    fi

    # If -o, exit instead of using backup flex C file
    #
    if [[ -n $O_FLAG ]]; then
	echo "$0: ERROR: unable to find or use flex and -o given" 1>&2
	exit 1
    fi

    # look for backup flex C file
    #
    FLEX_BACKUP_C="$PREFIX.ref.c"
    FLEX_C="$PREFIX.c"
    FLEX_BACKUP_H="$PREFIX.lex.ref.h"
    FLEX_H="$PREFIX.lex.h"
    if [[ ! -e $FLEX_BACKUP_C ]]; then
	echo "$0: ERROR: file not found: $FLEX_BACKUP_C" 1>&2
	exit 4
    fi
    if [[ ! -f $FLEX_BACKUP_C ]]; then
	echo "$0: ERROR: not a regular file: $FLEX_BACKUP_C" 1>&2
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
    if [[ ! -f $FLEX_BACKUP_H ]]; then
	echo "$0: ERROR: not a regular file: $FLEX_BACKUP_H" 1>&2
	exit 4
    fi
    if [[ ! -r $FLEX_BACKUP_H ]]; then
	echo "$0: ERROR: not a readable file: $FLEX_BACKUP_H" 1>&2
	exit 4
    fi
    if [[ ! -s $FLEX_BACKUP_H ]]; then
	echo "$0: ERROR: empty file: $FLEX_BACKUP_H" 1>&2
	exit 4
    fi



    # copy backup flex C file in place
    #
    if [[ $V_FLAG -ge 1 ]]; then
	echo "# Warning: We are forced to use $FLEX_BASENAME backup files instead of $FLEX_BASENAME C output!" 1>&2
	echo "cp -f -v $FLEX_BACKUP_C $FLEX_C"
	cp -f -v "$FLEX_BACKUP_C" "$FLEX_C"
	status="$?"
    else
	cp -f "$FLEX_BACKUP_C" "$FLEX_C"
	status="$?"
    fi
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: failed to copy $FLEX_BACKUP_C to $FLEX_C exit code: $status" 1>&2
	exit 5
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "cp -f -v $FLEX_BACKUP_H $FLEX_H"
	cp -f -v "$FLEX_BACKUP_H" "$FLEX_H"
	status="$?"
    else
	cp -f "$FLEX_BACKUP_H" "$FLEX_H"
	status="$?"
    fi
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: failed to copy $FLEX_BACKUP_H to $FLEX_H exit code: $status" 1>&2
	exit 5
    fi


    # moved flex backup file in place
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
    MKTEMP_TEMPLATE=".sorry.$(basename "$0").XXXXXXXXXX.c"
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
    if [[ $V_FLAG -ge 1 ]]; then
	echo "# prepending comment and line number reset to $FILE"
	echo "cat $SORRY_H > $TMP_FILE"
    fi
    cat "$SORRY_H" > "$TMP_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cat $SORRY_H > $TMP_FILE exit code: $status" 1>&2
	exit 19
    fi
    echo "#line 1 \"$FILE\"" >> "$TMP_FILE"
    # warning: This $? refers to echo/printf, not a previous command. Assign to variable to avoid it being overwritten. [SC2320]
    # shellcheck disable=SC2320
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
    if [[ $V_FLAG -ge 1 ]]; then
	echo "mv -v -f $TMP_FILE $FILE"
	mv -v -f "$TMP_FILE" "$FILE"
	status="$?"
    else
	mv -f "$TMP_FILE" "$FILE"
	status="$?"
    fi
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: mv -v -f $TMP_FILE $FILE failed, exit code: $status" 1>&2
	exit 22
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "# completed update of $FILE"
    fi
    return 0
}


# run flex
#
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[3]: checking for flex as: $FLEX_BASENAME" 1>&2
fi

# check -F dirs first
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
    echo "$0: Warning: failed to discover the path for an up to date flex as: $FLEX_BASENAME" 1>&2
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
if [[ $V_FLAG -ge 1 ]]; then
    echo "$FLEX_PATH $* $PREFIX.l"
fi
"$FLEX_PATH" "$@" "$PREFIX.l"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: $FLEX_PATH failed, exit code: $status" 1>&2
    exit 2
fi

# verify the flex C file is a non-empty readable file
#
if [[ ! -f $PREFIX.c ]]; then
    echo "$0: Warning: $FLEX_PATH failed to form file: $PREFIX.c" 1>&2
    use_flex_backup
elif [[ ! -s $PREFIX.c ]]; then
    echo "$0: Warning: $FLEX_PATH failed to form non-empty file: $PREFIX.c" 1>&2
    use_flex_backup
elif [[ ! -f $PREFIX.lex.h ]]; then
    echo "$0: Warning: $FLEX_PATH failed to form file: $PREFIX.lex.h" 1>&2
    use_flex_backup
elif [[ ! -s $PREFIX.tab.h ]]; then
    echo "$0: Warning: $FLEX_PATH failed to form non-empty file: $PREFIX.lex.h" 1>&2
    use_flex_backup
else

    # prepend the apology file and line number reset if -S not used
    #
    if [[ -z "$S_FLAG" ]]; then
	add_sorry "$PREFIX.c"
	add_sorry "$PREFIX.lex.h"
    fi

    # print debug of result if -v
    #
    if [[ $V_FLAG -ge 1 ]]; then
	# note: Double quote to prevent globbing and word splitting. [SC2086]
	echo "$0: debug[1]: $(ls -l "$PREFIX.c")"
	# note: Double quote to prevent globbing and word splitting. [SC2086]
	echo "$0: debug[1]: $(ls -l "$PREFIX.lex.h")"
	echo "$0: debug[1]: $FLEX_BASENAME run OK: formed $PREFIX.c and $PREFIX.lex.h" 1>&2
    fi
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
