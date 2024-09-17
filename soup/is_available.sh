#!/usr/bin/env bash
#
# is_available.sh - check if a tool is an executable file, and in some cases, is sane
#
# This script was written in 2024 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#       chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# "Because even the hard working helpers sometime need a helping hand." :-)
#
# Share and enjoy! :-)

export VERSION="1.1 2024-09-15"
NAME=$(basename "$0")
export NAME
export PRINT_WHERE=""
export V_FLAG="0"

export USAGE="usage: $0 [-h] [-V] [-v level] [-w] tool

    -h		    print help and exit
    -V		    print version and exit
    -v level	    set verbosity level for this script: (def level: $V_FLAG)

    -w		    if tool is a sane executable file, print path to the tool (def: be silent)

    tool	    tool to check if it is a executable file

    NOTE: If tool is one of: shellcheck picky independ seqcexit
	  then a sanity check using known good data and args will also be performed

Exit codes:
     0   all OK
     1   tool is not an executable file or failed a sanity test
     2   -h and help string printed or -V and version string printed
     3   invalid command line
 >= 10   internal error or missing file or directory

$NAME version: $VERSION"

# parse args
#
while getopts :hVv:w flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    w)	PRINT_WHERE="true"
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
if [[ $# -ne 1 ]]; then
    echo "$0: ERROR: expected one arg, got $#" 1>&2
    echo 1>&2
    echo "$USAGE" 1>&2
    exit 3
elif [[ -z "$1" ]]; then
    echo "$0: ERROR: expected one non-empty arg" 1>&2
    echo 1>&2
    echo "$USAGE" 1>&2
    exit 3
fi
export ARG="$1"

# look for tool along the $PATH
#
TOOL="$(type -P "$ARG")"
if [[ -n "$TOOL" ]]; then
    if [[ "$V_FLAG" -ge 1 ]]; then
	echo "$0: debug[1]: found tool along \$PATH: $TOOL" 1>&2
    fi
else
    if [[ "$V_FLAG" -ge 1 ]]; then
	echo "$0: debug[1]: tool not found along \$PATH: $ARG" 1>&2
    fi
    exit 1
fi

# paranoia
#
# In case we are dealing with a BOGUS type -P.
#
if [[ ! -f $TOOL ]]; then
    if [[ "$V_FLAG" -ge 1 ]]; then
	echo "$0: debug[1]: tool does not exist: $TOOL" 1>&2
    fi
    exit 1
elif [[ "$V_FLAG" -ge 3 ]]; then
    echo "$0: debug[3]: tool exists: $TOOL" 1>&2
fi

# more paranoia - tool must be an executable file
#
if [[ ! -f $TOOL ]]; then
    if [[ "$V_FLAG" -ge 1 ]]; then
	echo "$0: debug[1]: tool is not a file: $TOOL" 1>&2
    fi
    exit 1
elif [[ "$V_FLAG" -ge 3 ]]; then
    echo "$0: debug[3]: tool is a file: $TOOL" 1>&2
fi
if [[ ! -x $TOOL ]]; then
    if [[ "$V_FLAG" -ge 1 ]]; then
	echo "$0: debug[1]: tool is not an executable file: $TOOL" 1>&2
    fi
    exit 1
elif [[ "$V_FLAG" -ge 3 ]]; then
    echo "$0: debug[3]: tool is an executable file: $TOOL" 1>&2
fi

# sanity check if the tool is one of the special: shellcheck, picky, independ, or seqcexit
#
TOOL_BASENAME="${TOOL##*/}"
export TOOL_BASENAME
case "$TOOL_BASENAME" in

# try shellcheck on a trivial bash script
#
shellcheck)

    # form a trivial bash script
    #
    export TMP_BASH_SCRIPT=".tmp.$NAME.BASH_SCRIPT.$$.sh"
    trap 'rm -f $TMP_BASH_SCRIPT; exit' 0 1 2 3 15
    rm -f "$TMP_BASH_SCRIPT"
    if [[ -e $TMP_BASH_SCRIPT ]]; then
	echo "$0: ERROR: cannot remove temporary bash script: $TMP_BASH_SCRIPT" 1>&2
	exit 10
    fi
    printf '%s\n%s\n' '#!/usr/bin/env bash' 'exit 0' > "$TMP_BASH_SCRIPT"
    if [[ ! -e $TMP_BASH_SCRIPT ]]; then
	echo "$0: ERROR: cannot create temporary bash script file: $TMP_BASH_SCRIPT" 1>&2
	exit 11
    fi
    chmod +x "$TMP_BASH_SCRIPT"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: chmod +x $TMP_BASH_SCRIPT" \
	     "failed, error code: $status" 1>&2
	exit 12
    fi

    # try shellcheck on the trivial bash script
    #
    if [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: about to run: $TOOL --shell=bash -- $TMP_BASH_SCRIPT" 1>&2
    fi
    TOOL_OUTPUT=$("$TOOL" --shell=bash -- "$TMP_BASH_SCRIPT" 2>&1)
    status="$?"
    export TOOL_OUTPUT
    if [[ $status -ne 0 ]]; then
	if [[ "$V_FLAG" -ge 1 ]]; then
	    echo "$0: debug[1]: tool failed the trivial test: $TOOL" 1>&2
	fi
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: $TOOL --shell=bash -- $TMP_BASH_SCRIPT failed," \
		 "error: $status" 1>&2
	fi
	exit 1
    elif [[ -n "$TOOL_OUTPUT" ]]; then
	if [[ "$V_FLAG" -ge 1 ]]; then
	    echo "$0: debug[1]: unexpected output from the trivial test: $TOOL" 1>&2
	    if [[ "$V_FLAG" -ge 3 ]]; then
		echo "$0: debug[3]: unexpected output starts below" 1>&2
		echo "$TOOL_OUTPUT" 1>&2
		echo "$0: debug[3]: unexpected output ends above" 1>&2
	    fi
	fi
	exit 1
    elif [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: tool passed the trivial test: $TOOL" 1>&2
    fi
    ;;

picky)

    # form a trivial C source file
    #
    export TMP_C_SRC=".tmp.$NAME.C_SRC.$$.c"
    trap 'rm -f $TMP_C_SRC; exit' 0 1 2 3 15
    rm -f "$TMP_C_SRC"
    if [[ -e $TMP_C_SRC ]]; then
	echo "$0: ERROR: cannot remove temporary C source: $TMP_C_SRC" 1>&2
	exit 13
    fi
    cat > "$TMP_C_SRC" << EOF
#include <stdio.h>

int
main(void)
{
	printf("hello, world\n");
	return 0;
}
EOF
    if [[ ! -e $TMP_C_SRC ]]; then
	echo "$0: ERROR: cannot create temporary C source file: $TMP_C_SRC" 1>&2
	exit 14
    fi

    # try picky on the trivial C source
    #
    if [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: about to run: $TOOL -w132 -u -s -t8 -v -e -- $TMP_C_SRC" 1>&2
    fi
    TOOL_OUTPUT=$("$TOOL" -w132 -u -s -t8 -v -e -- "$TMP_C_SRC" 2>&1)
    status="$?"
    export TOOL_OUTPUT
    if [[ $status -ne 0 ]]; then
	if [[ "$V_FLAG" -ge 1 ]]; then
	    echo "$0: debug[1]: tool failed the trivial test: $TOOL" 1>&2
	fi
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: $TOOL -w132 -u -s -t8 -v -e -- $TMP_C_SRC failed," \
		 "error: $status" 1>&2
	fi
	exit 1
    elif [[ -n "$TOOL_OUTPUT" ]]; then
	if [[ "$V_FLAG" -ge 1 ]]; then
	    echo "$0: debug[1]: unexpected output from the trivial test: $TOOL" 1>&2
	    if [[ "$V_FLAG" -ge 3 ]]; then
		echo "$0: debug[3]: unexpected output starts below" 1>&2
		echo "$TOOL_OUTPUT" 1>&2
		echo "$0: debug[3]: unexpected output ends above" 1>&2
	    fi
	fi
	exit 1
    elif [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: tool passed the trivial test: $TOOL" 1>&2
    fi
    ;;

independ)

    # find the C compiler
    #
    CC="$(type -P "cc")"
    if [[ -n "$CC" ]]; then
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: found C compiler along \$PATH: $CC" 1>&2
	fi
    else
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: C compiler not found along \$PATH: cc" 1>&2
	fi
	exit 15
    fi
    # paranoia - # In case we are dealing with a BOGUS type -P.
    if [[ ! -f $CC ]]; then
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: C compiler does not exist: $CC" 1>&2
	fi
	exit 16
    elif [[ "$V_FLAG" -ge 5 ]]; then
	echo "$0: debug[5]: C compiler exists: $CC" 1>&2
    fi
    # more paranoia - C compiler must be an executable file
    if [[ ! -f $CC ]]; then
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: C compiler is not a file: $CC" 1>&2
	fi
	exit 17
    elif [[ "$V_FLAG" -ge 5 ]]; then
	echo "$0: debug[5]: C compiler is a file: $CC" 1>&2
    fi
    if [[ ! -x $CC ]]; then
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: C compiler is not an executable file: $CC" 1>&2
	fi
	exit 18
    elif [[ "$V_FLAG" -ge 5 ]]; then
	echo "$0: debug[5]: C compiler is an executable file: $CC" 1>&2
    fi

    # form a trivial C source file
    #
    export TMP_C_SRC=".tmp.$NAME.C_SRC.$$.c"
    trap 'rm -f $TMP_C_SRC; exit' 0 1 2 3 15
    rm -f "$TMP_C_SRC"
    if [[ -e $TMP_C_SRC ]]; then
	echo "$0: ERROR: cannot remove temporary C source: $TMP_C_SRC" 1>&2
	exit 19
    fi
    cat > "$TMP_C_SRC" << EOF
#include <stdio.h>

int
main(void)
{
	printf("hello, world\n");
	return 0;
}
EOF
    if [[ ! -e $TMP_C_SRC ]]; then
	echo "$0: ERROR: cannot create temporary C source file: $TMP_C_SRC" 1>&2
	exit 20
    fi

    # try independ on the trivial C source
    #
    if [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: about to run: $CC -MM $TMP_C_SRC | $TOOL" 1>&2
    fi
    TOOL_OUTPUT=$("$CC" -MM "$TMP_C_SRC" | "$TOOL" 2>&1)
    status_codes=("${PIPESTATUS[@]}")
    export TOOL_OUTPUT
    if [[ ${status_codes[*]} =~ [1-9] ]]; then
	if [[ "$V_FLAG" -ge 1 ]]; then
	    echo "$0: debug[1]: tool failed the trivial test: $TOOL" 1>&2
	fi
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: $CC -MM $TMP_C_SRC | $TOOL failed," \
		 "error codes: ${status_codes[*]}" 1>&2
	fi
	exit 1
    elif [[ -z "$TOOL_OUTPUT" ]]; then
	if [[ "$V_FLAG" -ge 1 ]]; then
	    echo "$0: debug[1]: no output from the trivial test: $TOOL" 1>&2
	fi
	exit 1
    elif [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: tool passed the trivial test: $TOOL" 1>&2
    fi
    ;;

seqcexit)

    # form a trivial C source file
    #
    export TMP_C_SRC=".tmp.$NAME.C_SRC.$$.c"
    trap 'rm -f $TMP_C_SRC; exit' 0 1 2 3 15
    rm -f "$TMP_C_SRC"
    if [[ -e $TMP_C_SRC ]]; then
	echo "$0: ERROR: cannot remove temporary C source: $TMP_C_SRC" 1>&2
	exit 21
    fi
    cat > "$TMP_C_SRC" << EOF
#include <stdio.h>

int
main(void)
{
	printf("hello, world\n");
	return 0;
}
EOF
    if [[ ! -e $TMP_C_SRC ]]; then
	echo "$0: ERROR: cannot create temporary C source file: $TMP_C_SRC" 1>&2
	exit 22
    fi

    # try seqcexit on the trivial C source
    #
    if [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: about to run: $TOOL -- $TMP_C_SRC" 1>&2
    fi
    TOOL_OUTPUT=$("$TOOL" -- "$TMP_C_SRC" 2>&1)
    status="$?"
    export TOOL_OUTPUT
    if [[ $status -ne 0 ]]; then
	if [[ "$V_FLAG" -ge 1 ]]; then
	    echo "$0: debug[1]: tool failed the trivial test: $TOOL" 1>&2
	fi
	if [[ "$V_FLAG" -ge 3 ]]; then
	    echo "$0: debug[3]: $TOOL -- $TMP_C_SRC failed," \
		 "error: $status" 1>&2
	fi
	exit 1
    elif [[ -n "$TOOL_OUTPUT" ]]; then
	if [[ "$V_FLAG" -ge 1 ]]; then
	    echo "$0: debug[1]: unexpected output from the trivial test: $TOOL" 1>&2
	    if [[ "$V_FLAG" -ge 3 ]]; then
		echo "$0: debug[3]: unexpected output starts below" 1>&2
		echo "$TOOL_OUTPUT" 1>&2
		echo "$0: debug[3]: unexpected output ends above" 1>&2
	    fi
	fi
	exit 1
    elif [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: tool passed the trivial test: $TOOL" 1>&2
    fi
    ;;

# case: not a special tool, testing as an executable file is good enough (we hope)
#
*)
    if [[ "$V_FLAG" -ge 3 ]]; then
	echo "$0: debug[3]: tool is not special, skipping sanity check" 1>&2
    fi
    ;;
esac

# if -w, report where the OK tool is found to stdout
#
# If we got here, the tool must is OK.
#
if [[ -n $PRINT_WHERE ]]; then
    echo "$TOOL"
fi

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
exit 0
