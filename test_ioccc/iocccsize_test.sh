#!/usr/bin/env bash
#
# test_iocccsize.sh - IOCCC Size Tool Tests
#
# "You are not expected to understand this" :-)
#
# Public Domain 1992, 2015, 2019 by Anthony Howe.  All rights released.
#
# IOCCC mods in 2019,2021,2022 by chongo (Landon Curt Noll) ^oo^
#
# ***
# *** NOTE when editing this file it contains UTF8 multibyte characters.
# ***

# setup
#
# Under CentOS shellcheck complains about the comment, reporting:
#
#	warning: Remove space after = if trying to assign a value (for empty string, use var='' ... ). [SC1007]
#
# which seems silly and pointless so we disable it for the next two
# lines.
#
export DIGRAPHS=	# assume #undef DIGRAPHS
export TRIGRAPHS=	# assume #undef TRIGRAPHS
export LIMIT_IOCCC="./soup/limit_ioccc.sh"
export WORK_DIR="./test_ioccc/test_iocccsize"
export IOCCCSIZE="./iocccsize"
export EXIT_CODE=0
export IOCCCSIZE_ARGS="-v 1 --"
export V_FLAG=0
export TOPDIR=
export TEST_IOCCCSIZE_VERSION="1.4 2023-02-04"

export USAGE="usage: $0 [-h] [-v lvl] [-V] [-i iocccsize] [-w work_dir] [-l limit] [-Z topdir] [-I iocccsize_args]

    -h		    print help message and exit
    -v lvl	    set debugging level to lvl (def: 0 ==> no debugging)
    -V		    print version and exit
    -i iocccsize    path to iocccsize tool (def: test with $IOCCCSIZE)
    -w work_dir	    working directory that is removed & rebuilt during the test (def: $WORK_DIR)
    -l limit	    path to limit_ioccc.sh executable shell script (def: $LIMIT_IOCCC)
		    A limit of . (dot) will disable use of an executable shell script.
    -Z topdir	    top level build directory (def: try . or ..)
    -I args	    the args to pass to iocccsize (def: $IOCCCSIZE_ARGS)
			NOTE: don't forget to add -- at the end of the list

Exit codes:
     0   all OK
     1   one or more tests failed
     2   -h and help string printed or -V and version string printed
     3	 command line error
     4	 missing or non executable iocccsize
     5	 missing or unreadable limit_ioccc.h
 >= 10	 internal error

test_iocccsize.sh version: $TEST_IOCCCSIZE_VERSION"

# parse args
#
while getopts 'hv:Vi:w:l:Z:I:' flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG"
	;;
    V)	echo "$TEST_IOCCCSIZE_VERSION"
	exit 2
	;;
    i)	IOCCCSIZE="$OPTARG"
	;;
    w)	WORK_DIR="$OPTARG"
	;;
    l)	LIMIT_IOCCC="$OPTARG"
	;;
    Z)  TOPDIR="$OPTARG";
        ;;
    I)	IOCCCSIZE_ARGS="$OPTARG";
	;;
    \?)	echo "$0: ERROR: invalid option: -$flag" 1>&2
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
shift $((OPTIND - 1))

# change to the top level directory as needed
#
if [[ -n $TOPDIR ]]; then
    if [[ ! -d $TOPDIR ]]; then
	echo "$0: ERROR: -Z $TOPDIR given: not a directory: $TOPDIR" 1>&2
	exit 3
    fi
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: -Z $TOPDIR given, about to cd $TOPDIR" 1>&2
    fi
    # SC2164 (warning): Use 'cd ... || exit' or 'cd ... || return' in case cd fails.
    # https://www.shellcheck.net/wiki/SC2164
    # shellcheck disable=SC2164
    cd "$TOPDIR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: -Z $TOPDIR given: cd $TOPDIR exit code: $status" 1>&2
	exit 3
    fi
elif [[ -f mkiocccentry.c ]]; then
    TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume TOPDIR is .: $TOPDIR" 1>&2
    fi
elif [[ -f ../mkiocccentry.c ]]; then
    cd ..
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cd .. exit code: $status" 1>&2
	exit 3
    fi
    TOPDIR="$PWD"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: assume TOPDIR is ..: $TOPDIR" 1>&2
    fi
else
    echo "$0: ERROR: cannot determine TOPDIR, use -Z topdir" 1>&2
    exit 3
fi
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: TOPDIR is the current directory: $TOPDIR" 1>&2
fi


# validate ./iocccsize
#
if [[ ! -e $IOCCCSIZE ]]; then
    echo "$0: ERROR: iocccsize does not exist: $IOCCCSIZE" 1>&2
    exit 4
fi
if [[ ! -f $IOCCCSIZE ]]; then
    echo "$0: ERROR: iocccsize is not a regular file: $IOCCCSIZE" 1>&2
    exit 4
fi
if [[ ! -x $IOCCCSIZE ]]; then
    echo "$0: ERROR: iocccsize is not an executable file: $IOCCCSIZE" 1>&2
    exit 4
fi

# validate readable limit_ioccc.h
#
if [[ $LIMIT_IOCCC != "." ]]; then
    if [[ ! -e $LIMIT_IOCCC ]]; then
	echo "$0: ERROR: limit_ioccc.h does not exist: $LIMIT_IOCCC" 1>&2
	exit 5
    fi
    if [[ ! -f $LIMIT_IOCCC ]]; then
	echo "$0: ERROR: limit_ioccc.h is not a regular file: $LIMIT_IOCCC" 1>&2
	exit 5
    fi
    if [[ ! -r $LIMIT_IOCCC ]]; then
	echo "$0: ERROR: limit_ioccc.h is not a readable file: $LIMIT_IOCCC" 1>&2
	exit 5
    fi
fi

# create an empty working directory
#
rm -rf  "$WORK_DIR"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: rm -rf $WORK_DIR failed, error code: $status" 1>&2
    exit 10
fi
mkdir -p "$WORK_DIR"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mkdir -p $WORK_DIR failed, error code: $status" 1>&2
    exit 11
fi

# Change DIGRAPHS and TRIGRAPHS according to limit_ioccc.h, unless .
#
if [[ $LIMIT_IOCCC != "." ]]; then
    # warning: ShellCheck can't follow non-constant source. Use a directive to specify location.
    #
    # We could try instead using shellcheck source="$LIMIT_IOCCC" but this
    # requires the -x option to shellcheck and not all versions support this
    # option so we have to disable the warning instead.
    #
    # SC1090 (warning): ShellCheck can't follow non-constant source. Use a directive to specify location.
    # https://www.shellcheck.net/wiki/SC1090
    # shellcheck disable=SC1090
    source "$LIMIT_IOCCC"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: source $LIMIT_IOCCC error code: $status" 1>&2
	exit 12
    fi
fi

# obtain a wc value of a file
#
get_wc()
{
    declare file="$1"
    declare field="$2"

    wc "$file" | sed -e's/^ *//; s/  */ /g' | cut -d' ' "-f$field"
}

# perform a specific iocccsize test
#
# usage:
#	test_size c_src_file "expected_Rule_2b expected_Rule_2a expected_keywords"
#
# NOTE: The expected_keywords (third expected arg) is ignored.
#
test_size()
{
    declare file="$WORK_DIR/$1"	# C program file to test
    declare expect="$2"		# expected args of test
    #
    declare expect_2a		# expected Rule 2b value
    declare expect_2b		# expected Rule 2b value
    #
    declare iocccsize_v0	# iocccsize -v 0 output
    declare iocccsize_v1	# iocccsize -v 1 output
    declare iocccsize_v0_2b	# iocccsize -v 0 Rule 2b (first arg)
    declare iocccsize_v1_2a	# iocccsize -v 1 Rule 2a (second arg)
    declare iocccsize_v1_2b	# iocccsize -v 1 Rule 2b (first arg)
    #
    declare wc_bytes		# wc -c bytes
    #
    declare pass		# test pass true or false

    # collect iocccsize -v 0 output
    #
    iocccsize_v0=$("$IOCCCSIZE" -v 0 -- "$file" 2>/dev/null)
    if [[ $V_FLAG -ge 3 ]]; then
	 echo "$0: debug[3]: $IOCCCSIZE -v 0 $file output: $iocccsize_v0" 1>&2
    fi

    # collect iocccsize -v 1 output
    #
    iocccsize_v1=$("$IOCCCSIZE" -v 1 -- "$file" 2>/dev/null)
    if [[ $V_FLAG -ge 3 ]]; then
	 echo "$0: debug[3]: $IOCCCSIZE -v 1 $file output: $iocccsize_v1" 1>&2
    fi

    # assume all test pass unless we discover otherwise
    #
    pass="true"

    # verify iocccsize -v 1 Rule 2a (second) value against wc -c
    #
    wc_bytes=$(get_wc "$file" 3)
    iocccsize_v1_2a=$(echo "$iocccsize_v1" | cut -d' ' -f2)
    if [[ $iocccsize_v1_2a -ne "$wc_bytes" ]]; then
	echo "$0: ERROR: $IOCCCSIZE -v 1 $file Rule 2a value: $iocccsize_v1_2a != wc -c value: $wc_bytes"
	EXIT_CODE=1
	pass="false"
    elif [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: $IOCCCSIZE -v 1 $file Rule 2a value: $iocccsize_v1_2a == wc -c value: $wc_bytes"
    fi

    # verify iocccsize -v 1 Rule 2a (second) value expected Rule 2a test value
    #
    expect_2a=$(echo "$expect" | cut -d' ' -f2)
    if [[ $iocccsize_v1_2a -ne "$expect_2a" ]]; then
	echo "$0: ERROR: $IOCCCSIZE -v 1 $file Rule 2a value: $iocccsize_v1_2a != expected Rule 2a value: $expect_2a" 1>&2
	EXIT_CODE=1
	pass="false"
    elif [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: $IOCCCSIZE -v 1 $file Rule 2a value: $iocccsize_v1_2a == expected Rule 2a value: $expect_2a" 1>&2
    fi

    # verify iocccsize -v 0 (first) Rule 2b value against expected Rule 2b test value
    #
    expect_2b=$(echo "$expect" | cut -d' ' -f1)
    iocccsize_v0_2b=$(echo "$iocccsize_v0" | cut -d' ' -f1)
    if [[ $expect_2b -ne "$iocccsize_v0_2b" ]]; then
	echo "$0: ERROR: $IOCCCSIZE -v 0 $file Rule 2b value: $iocccsize_v0_2b != expected Rule 2b value: $expect_2b" 1>&2
	EXIT_CODE=1
	pass="false"
    elif [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: $IOCCCSIZE -v 0 $file Rule 2b value: $iocccsize_v0_2b == expected Rule 2b value: $expect_2b" 1>&2
    fi

    # verify iocccsize -v 1 (first) Rule 2b value against expected Rule 2b test value
    #
    expect_2b=$(echo "$expect" | cut -d' ' -f1)
    iocccsize_v1_2b=$(echo "$iocccsize_v1" | cut -d' ' -f1)
    if [[ $expect_2b -ne "$iocccsize_v1_2b" ]]; then
	echo "$0: ERROR: $IOCCCSIZE -v 0 $file Rule 2b value: $iocccsize_v1_2b != expected Rule 2b value: $expect_2b" 1>&2
	EXIT_CODE=1
	pass="false"
    elif [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: $IOCCCSIZE -v 0 $file Rule 2b value: $iocccsize_v1_2b -= expected Rule 2b value: $expect_2b" 1>&2
    fi

    # report no test failures if debugging
    #
    if [[ $pass == "true" ]]; then
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: PASS: $file" 1>&2
	fi
    else
	echo "$0: ERROR: FAIL: $file" 1>&2
    fi
}

# perform the complete iocccsize test suite
#
# tests are of the form:
#
#	test_size c_src_file "expected_Rule_2b expected_Rule_2a expected_keywords"
#
# NOTE: The expected_keywords (third expected arg) is ignored.

#######################################################################

printf 'int x;\r\n' >"$WORK_DIR/crlf.c"
test_size crlf.c "2 8 1"

#######################################################################

# String literal with multibyte characters (mb2).
printf 'char str[] = "èéø";\r\n' >"${WORK_DIR}/utf8.c"
test_size utf8.c "15 24 1"

#######################################################################

cat <<EOF >"$WORK_DIR/splitline0.c"
#define FOO \\
    int a = 666;
FOO;
EOF
test_size splitline0.c "19 36 1"

#######################################################################

cat <<EOF >"$WORK_DIR/comment0.c"
// comment one line "with a comment string" inside
int x;
EOF
test_size comment0.c "44 58 1"

#######################################################################

cat <<EOF >"$WORK_DIR/comment1.c"
/* comment block same line 'with a comment string' */
int x;
EOF
test_size comment1.c "46 61 1"

#######################################################################

cat <<EOF >"$WORK_DIR/comment2.c"
/*
comment block
multiline
*/
int x;
EOF
test_size comment2.c "27 37 1"

#######################################################################

cat <<EOF >"$WORK_DIR/comment3.c"
a//foo
EOF
test_size comment3.c "6 7 0"

#######################################################################

cat <<EOF >"$WORK_DIR/comment4.c"
/*/ int if for /*/
EOF
test_size comment4.c "14 19 0"

#######################################################################

cat <<EOF >"$WORK_DIR/comment5.c"
'"' "/*" foobar "*/"
EOF
test_size comment5.c "17 21 0"

#######################################################################

cat <<EOF >"$WORK_DIR/comment6.c"
char str[] = "string /* with */ comment";
EOF
test_size comment6.c "30 42 1"

#######################################################################

cat <<EOF >"$WORK_DIR/comment7.c"
// comment with backslash newline \\
int a = 666;
EOF
test_size comment7.c "37 49 0"

#######################################################################

cat <<EOF >"$WORK_DIR/quote0.c"
char str[] = "and\"or";
EOF
test_size quote0.c "16 24 1"

#######################################################################

cat <<EOF >"$WORK_DIR/quote1.c"
char squote = '\\'';
EOF
test_size quote1.c "12 20 1"

#######################################################################

cat <<EOF >"$WORK_DIR/quote2.c"
char str[] = "'xor'";
EOF
test_size quote2.c "14 22 1"

#######################################################################

cat <<EOF >"$WORK_DIR/digraph.c"
char str<::> = "'xor'";
EOF
if [[ -z $DIGRAPHS ]]; then
test_size digraph.c "16 24 1"
else
test_size digraph.c "14 24 1"
fi

#######################################################################

cat <<EOF >"$WORK_DIR/trigraph0.c"
char str??(??) = "'xor'";
EOF
if [[ -z $TRIGRAPHS ]]; then
test_size trigraph0.c "18 26 1"
else
test_size trigraph0.c "14 26 1"
fi

#######################################################################

# Example from https://en.wikipedia.org/wiki/Digraphs_and_trigraphs#C
cat <<EOF >"$WORK_DIR/trigraph1.c"
// Will the next line be executed????????????????/
int a = 666;
EOF
if [[ -z $TRIGRAPHS ]]; then
test_size trigraph1.c "50 64 1"
else
test_size trigraph1.c "49 64 0"
fi

#######################################################################

# Example from https://en.wikipedia.org/wiki/Digraphs_and_trigraphs#C
cat <<EOF >"$WORK_DIR/trigraph2.c"
/??/
* A comment *??/
/
EOF
if [[ -z $TRIGRAPHS ]]; then
test_size trigraph2.c "18 24 0"
else
test_size trigraph2.c "12 24 0"
fi

#######################################################################

cat <<EOF >"$WORK_DIR/trigraph3.c"
#define FOO ??/
    int a = 666;
FOO;
EOF
if [[ -z $TRIGRAPHS ]]; then
test_size trigraph3.c "22 38 1"
else
test_size trigraph3.c "19 38 1"
fi

#######################################################################

cat <<EOF >"$WORK_DIR/main0.c"
int
main(int argc, char **argv)
{
	return 0;
}
EOF
test_size main0.c "22 47 4"

#######################################################################

cat <<EOF >"$WORK_DIR/hello.c"
#include <stdio.h>

int
main(int argc, char **argv)
{
	(void) printf("Hello world!\\n");
	return 0;
}
EOF
test_size hello.c "58 101 6"

#######################################################################

# Digraph for #include and curlys.  Digraphs are tokens and are not
# translated like trigraphs.
cat <<EOF >"$WORK_DIR/hello_digraph.c"
%:    include <stdio.h>

int
main(int argc, char **argv)
<%
	(void) printf("Hello world!\\n");
	return 0;
%>
EOF
if [[ -z $DIGRAPHS ]]; then
test_size hello_digraph.c "70 108 5"
else
test_size hello_digraph.c "60 108 6"
fi

#######################################################################

# Trigraph for #include and curlys.  Trigraphs are translated, unlike
# digraphs which are tokens.
cat <<EOF >"$WORK_DIR/hello_trigraph.c"
??=    include <stdio.h>

int
main(int argc, char **argv)
??<
	(void) printf("Hello world!\\n");
	return 0;
??>
EOF
if [[ -z $TRIGRAPHS ]]; then
test_size hello_trigraph.c "73 111 5"
else
test_size hello_trigraph.c "58 111 6"
fi

#######################################################################

cat <<EOF >"$WORK_DIR/include0.c"
#  include <stdio.h>
EOF
test_size include0.c "10 21 1"

#######################################################################

cat <<EOF >"$WORK_DIR/include1.c"
#  include <stdio.h>
#/*hi*/include <ctype.h>
EOF
test_size include1.c "26 46 2"

#######################################################################

cat <<EOF >"$WORK_DIR/curly0.c"
char str = "{ curly } ";
EOF
test_size curly0.c "12 25 1"

#######################################################################

# No spaces after curly braces in array initialiser.
cat <<EOF >"$WORK_DIR/curly1.c"
#include <stdlib.h>

#define STRLEN(s)		(sizeof (s)-1)

typedef struct {
	size_t length;
	const char *word;
} Word;

Word list[] = {
	{STRLEN("abutted curly"), "abutted curly"},
	{0, NULL}
};
EOF
test_size curly1.c "119 192 6"

#######################################################################

# Spaces after curly braces in array initialiser.
cat <<EOF >"$WORK_DIR/curly2.c"
#include <stdlib.h>

#define STRLEN(s)		(sizeof (s)-1)

typedef struct {
	size_t length;
	const char *word;
} Word;

Word list[] = {
	{ STRLEN("spaced  curly"), "spaced  curly"} ,
	{ 0, NULL}
} ;
EOF
test_size curly2.c "113 196 6"

#######################################################################

cat <<EOF >"$WORK_DIR/semicolon0.c"
char str = "; xor; ";
EOF
test_size semicolon0.c "10 22 1"

#######################################################################

# Spaces after semicolons in for( ; ; ).
cat <<EOF >"$WORK_DIR/semicolon1.c"
#include <stdio.h>

int
main(int argc, char **argv)
{
	int i;
	for (i = 0; i < 3; i++) {
		(void) printf("%d\\n", i);
	}
	return 0;
}
EOF
test_size semicolon1.c "65 133 8"

#######################################################################

# No spaces after semicolons in for(;;).
cat <<EOF >"$WORK_DIR/semicolon2.c"
#include <stdio.h>

int
main(int argc, char **argv)
{
	int i;
	for (i=0;i<3;i++) {
		(void) printf("%d\\n", i);
	}
	return 0;
}
EOF
test_size semicolon2.c "67 127 8"

#######################################################################

# String literal with multibyte charaters (mb3).
cat <<EOF >"${WORK_DIR}/hello-jp.c"
#include <stdio.h>

int
main(int argc, char **argv)
{
	(void) printf("こんにちは世界！\n");
	return 0;
}
EOF
test_size hello-jp.c "71 113 6"

#######################################################################

# Unicode identifier using hiragana and kanji.
cat <<EOF >"${WORK_DIR}/hello-jp2.c"
#include <stdio.h>

int
main(int argc, char **argv)
{
	char str_こんにちは世界！[] = "Kon'nichiwa sekai!\n";
	(void) printf(str_こんにちは世界！);
	return 0;
}
EOF
test_size hello-jp2.c "124 176 7"

#######################################################################

# Long Unicode identifier
cat <<EOF >"${WORK_DIR}/hello-jp3.c"
#include <stdio.h>

int
main(int argc, char **argv)
{
	/* Identifier with 4 ASCII + 20 hiragana, katakana, and kanji.
	 * There is nothing wrong with this source code.
	 */
	char str_このソースコードには何も問題はありません[] = "Kono sōsukōdo ni wa nani mo mondai wa arimasen.\n";
	(void) printf(str_このソースコードには何も問題はありません);
	return 0;
}
EOF
test_size hello-jp3.c "313 398 7"


# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
if [[ $EXIT_CODE -eq 0 ]]; then
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: all tests PASSED"
    fi
else
    echo "$0: ERROR: about to exit $EXIT_CODE" 1>&2
fi
exit "$EXIT_CODE"
