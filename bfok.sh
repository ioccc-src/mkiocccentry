#!/usr/bin/env bash
#
# bfok.sh - check for bison and flex executables with OK versions
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
export USAGE="usage: $0 [-h] [-v level] [-V] [-b bison] [-f flex] [-l limit_ioccc.sh]
		        [-g verge] [-B dir] ... [-F dir] ... [-p bison| -p flex]

    -h              print help and exit 5
    -v level        set debug level (def: 0)
    -V              print version and exit 5
    -b bison        bison tool basename (def: bison)
    -f flex         flex tool basename (def: flex)
    -l limit_ioccc.sh   version info file (def: ./limit_ioccc.sh)
    -g verge	    path to verge tool (def: ./verge)
    -B dir          1st look for bison in dir (def: look just along \$PATH)
		        NOTE: Multiple -B dir are allowed
		        NOTE: Search is performed in dir order before \$PATH
			NOTE: Ignored if dir is missing or not not searchable
    -F dir          1st look for flex in dir (def: look just along \$PATH)
		        NOTE: Multiple -F dir are allowed
		        NOTE: Search is performed in dir order before \$PATH
			NOTE: Ignored if dir is missing or not not searchable
    -p bison	    Print just bison path if version is OK (def: print nothing)
    -p flex	    Print just flex path if version is OK (def: print nothing)
			NOTE: With -p, the existence and version of the
			      other tool is NOT checked

Exit codes:
    0    All is OK for both bison and flex
    1    bison and/or flex not found
    2    bison and/or flex found, or version unknown, or version is too old
    3    limit_ioccc.sh missing or not readable, verge missing or not executable
    4    BISON_VERSION and/or FLEX_VERSION missing from limit_ioccc.sh
    5    -h and help string printed or -V and version string printed
    6    Command line usage error
    >=7  internal error"
export BFOK_VERSION="0.1 2022-04-03"
export V_FLAG="0"
export BISON_BASENAME="bison"
export FLEX_BASENAME="flex"
export LIMIT_IOCCC_SH="./limit_ioccc.sh"
export VERGE="./verge"
export BISON_VERSION=
export FLEX_VERSION=
declare -a BISON_DIRS=()
declare -a FLEX_DIRS=()
export TEST_BISON="true"
export TEST_FLEX="true"
export PRINT_MODE=
export PATH_FOUND=
export BISON_FOUND=
export FLEX_FOUND=

# parse args
#
while getopts :hv:Vb:f:l:g:B:F:p: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 5
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    V) print "$BFOK_VERSION"
       exit 5
       ;;
    b) BISON_BASENAME="$OPTARG";
       ;;
    f) FLEX_BASENAME="$OPTARG";
       ;;
    l) LIMIT_IOCCC_SH="$OPTARG";
       ;;
    g) VERGE="$OPTARG";
       ;;
    B) BISON_DIRS[${#BISON_DIRS[@]}]="$OPTARG";
       ;;
    F) FLEX_DIRS[${#FLEX_DIRS[@]}]="$OPTARG";
       ;;
    p) case "$OPTARG" in
       bison) TEST_FLEX="" ;; # -p bison disables flex tests
       flex) TEST_BISON="" ;; # -p flex disables bison tests
       *) "$0: ERROR: -p option must be other bison or flex";
          exit 5
	  ;;
       esac
       PRINT_MODE="true"
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
if [[ -z $TEST_BISON && -z $TEST_FLEX ]]; then
    echo "$0: ERROR: cannot use both -p bison and -p flex" 1>&2
    exit 6
fi
if [[ -z $BISON_BASENAME ]]; then
    echo "$0: ERROR: -b bison name cannot be empty" 1>&2
    exit 6
fi
if [[ -z $FLEX_BASENAME ]]; then
    echo "$0: ERROR: -b flex name cannot be empty" 1>&2
    exit 6
fi

# check args
#
shift $(( OPTIND - 1 ));
if [[ $# -ne 0 ]]; then
    echo "$0: ERROR: expected 0 arguments, found $#" 1>&2
    exit 6
fi

# firewall
#
if [[ ! -e $LIMIT_IOCCC_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh file not found: $LIMIT_IOCCC_SH" 1>&2
    exit 3
fi
if [[ ! -f $LIMIT_IOCCC_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh not a file: $LIMIT_IOCCC_SH" 1>&2
    exit 3
fi
if [[ ! -r $LIMIT_IOCCC_SH ]]; then
    echo "$0: ERROR: limit_ioccc.sh not a readable file: $LIMIT_IOCCC_SH" 1>&2
    exit 3
fi
if [[ ! -e $VERGE ]]; then
    echo "$0: ERROR: verge file not found: $VERGE" 1>&2
    exit 3
fi
if [[ ! -f $VERGE ]]; then
    echo "$0: ERROR: verge not a file: $VERGE" 1>&2
    exit 3
fi
if [[ ! -x $VERGE ]]; then
    echo "$0: ERROR: verge not an executable: $VERGE" 1>&2
    exit 3
fi

# source the limit_ioccc.sh file
#
# shellcheck disable=SC1090
source "$LIMIT_IOCCC_SH" >/dev/null 2>&1
if [[ -z $BISON_VERSION ]]; then
    echo "$0: ERROR: BISON_VERSION missing or has an empty value from: $LIMIT_IOCCC_SH" 1>&2
    exit 4
fi
if [[ -z $FLEX_VERSION ]]; then
    echo "$0: ERROR: FLEX_VERSION missing or has an empty value from: $LIMIT_IOCCC_SH" 1>&2
    exit 4
fi

# debug
#
if [[ $V_FLAG -ge 5 ]]; then
    echo "$0: debug[5]: BFOK_VERSION=$BFOK_VERSION" 1>&2
    echo "$0: debug[5]: V_FLAG=$V_FLAG" 1>&2
    echo "$0: debug[5]: BISON_BASENAME=$BISON_BASENAME" 1>&2
    echo "$0: debug[5]: FLEX_BASENAME=$FLEX_BASENAME" 1>&2
    echo "$0: debug[5]: LIMIT_IOCCC_SH=$LIMIT_IOCCC_SH" 1>&2
    echo "$0: debug[5]: BISON_VERSION=$BISON_VERSION" 1>&2
    echo "$0: debug[5]: FLEX_VERSION=$FLEX_VERSION" 1>&2
    for dir in "${BISON_DIRS[@]}"; do
	echo "$0: debug[5]: BISON_DIRS=$dir" 1>&2
    done
    for dir in "${FLEX_DIRS[@]}"; do
	echo "$0: debug[5]: FLEX_DIRS=$dir" 1>&2
    done
    echo "$0: debug[5]: TEST_BISON=$TEST_BISON" 1>&2
    echo "$0: debug[5]: TEST_FLEX=$TEST_FLEX" 1>&2
    echo "$0: debug[5]: PRINT_MODE=$PRINT_MODE" 1>&2
fi

# look_for - look for executable in a given directory with a new enough version
#
# given:
#	tool		name of tool (bison or flex)
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
	exit 7
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
#	tool		name of tool (bison or flex)
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
	exit 7
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

# check bison
#
if [[ -n $TEST_BISON ]]; then

    # debug
    #
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[3]: checking bison: $BISON_BASENAME" 1>&2
    fi

    # check -B dirs first
    #
    for dir in "${BISON_DIRS[@]}"; do

	# look for a good bison in dir
	#
	look_for "$BISON_BASENAME" "$BISON_VERSION" "$dir"
	status="$?"
	if [[ $status -eq 0 && -n $PATH_FOUND ]]; then
	    BISON_PATH="$PATH_FOUND"
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: found a good bison: $BISON_PATH" 1>&2
	    fi
	    break
	fi
    done

    # if no bison found so far, look on $PATH
    #
    if [[ -z $BISON_PATH ]]; then
	on_path "$BISON_BASENAME" "$BISON_VERSION"
	status="$?"
	if [[ $status -eq 0 && -n $PATH_FOUND ]]; then
	    BISON_PATH="$PATH_FOUND"
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: bison on \$PATH is good: $BISON_PATH" 1>&2
	    fi
	fi
    fi
fi

# check flex
#
if [[ -n $TEST_FLEX ]]; then

    # debug
    #
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[3]: checking flex: $FLEX_BASENAME" 1>&2
    fi

    # check -B dirs first
    #
    for dir in "${FLEX_DIRS[@]}"; do

	# look for a good bison in dir
	#
	look_for "$FLEX_BASENAME" "$FLEX_VERSION" "$dir"
	status="$?"
	if [[ $status -eq 0 && -n $PATH_FOUND ]]; then
	    FLEX_PATH="$PATH_FOUND"
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: found a good flex: $FLEX_PATH" 1>&2
	    fi
	    break
	fi
    done

    # if no bison found so far, look on $PATH
    #
    if [[ -z $FLEX_PATH ]]; then
	on_path "$FLEX_BASENAME" "$FLEX_VERSION"
	status="$?"
	if [[ $status -eq 0 && -n $PATH_FOUND ]]; then
	    FLEX_PATH="$PATH_FOUND"
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: flex on \$PATH is OK: $FLEX_PATH" 1>&2
	    fi
	fi
    fi
fi

# determine how we exit
#
# If we only tested one tool, then check if that tool is OK.
# If we tested both tools, then check both tools are OK.
#
if [[ -n $TEST_BISON && -n $TEST_FLEX ]]; then

    # checking both tools so both tools must be good
    #
    if [[ -z $BISON_PATH && -z $FLEX_PATH ]]; then
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: bison not OK, flex not OK"
	fi
	exit 1
    elif [[ -z $BISON_PATH && -n $FLEX_PATH ]]; then
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: bison not OK, flex OK"
	fi
	exit 1
    elif [[ -n $BISON_PATH && -z $FLEX_PATH ]]; then
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: bison OK, flex not OK"
	fi
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: bison OK, flex OK"
    fi

# bison only test
#
elif [[ -n $TEST_BISON ]]; then
    if [[ -z $BISON_PATH ]]; then
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: bison not OK"
	fi
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: bison OK"
    fi

# flex only test
#
elif [[ -n $TEST_FLEX ]]; then
    if [[ -z $FLEX_PATH ]]; then
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: flex not OK"
	fi
	exit 1
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: flex OK"
    fi

# should not get here
#
else
    echo "$0: ERROR: should not get here" 1>&2
    exit 7
fi

# All Done!!! -- Jessica Noll, Age 2
#
# The tool(s) are OK
#
exit 0
