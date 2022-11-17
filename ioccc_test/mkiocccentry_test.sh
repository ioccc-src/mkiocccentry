#!/usr/bin/env bash
#
# A simple mkiocccentry test, or an example of how to automate the process!

# setup
#
export MKIOCCCENTRY_TEST_VERSION="0.3 2022-11-05"
export USAGE="usage: $0 [-h] [-V] [-v level] [-J level] [-Z topdir]

    -h              print help and exit
    -V              print version and exit
    -v level        flag ignored
    -J level	    set JSON verbosity level
    -Z topdir	    top level build directory (def: try . or ..)

Exit codes:
     0   all is OK
     2   -h and help string printed or -V and version string printed
     3	 command line error
 >= 10   some make action exited non-zero

mkiocccentry_test.sh version: $MKIOCCCENTRY_TEST_VERSION"
export V_FLAG="0"
export J_FLAG="0"
export EXIT_CODE="0"
export TOPDIR=

# parse args
#
while getopts :hv:J:VZ: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 2
       ;;
    V) echo "$MKIOCCCENTRY_TEST_VERSION"
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    J) J_FLAG="$OPTARG";
       ;;
    Z) TOPDIR="$OPTARG";
       ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
       exit 3
       ;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
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
    exit 3
fi

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
    # warning: Use 'cd ... || exit' or 'cd ... || return' in case cd fails. [SC2164]
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

# working locations - adjust as needed
#
work_dir="ioccc_test/test_work"
src_dir="ioccc_test/test_src"

# be sure the working locations exist
#
mkdir -p -- "${work_dir}" "${src_dir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: error in creating working dirs: mkdir -p -- ${work_dir} ${src_dir}" 1>&2
    exit 250
fi

# firewall
#
if [[ ! -d ${work_dir} ]]; then
    echo "$0: ERROR: work_dir not found: ${work_dir}" 1>&2
    exit 250
fi
if [[ ! -d ${src_dir} ]]; then
    echo "$0: ERROR: src_dir not found: ${src_dir}" 1>&2
    exit 250
fi
if [[ ! -x ./mkiocccentry ]]; then
    echo "$0: ERROR: executable not found: ./mkiocccentry" 1>&2
    exit 250
fi

# clean out the work_dir area
work_dir_esc="${work_dir}"
test "${work_dir:0:1}" = "-" && work_dir_esc=./"${work_dir}"
find "${work_dir_esc}" -mindepth 1 -depth -delete

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers() {
cat <<"EOF"
test
0
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://host0.example.com/index.html
@twitter0
@github0
an affiliation for #0 author
n

author1 middle1a middle1b last1
UK





n
replaced-author1-handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://host2.example.com/index.html
@twitter2
@github2
an affiliation for #2 author
y

author3 middle3 last3
AU
user0@example.com
https://host0.example.com/index.html
@twitter3
@github3
an affiliation for #3 author
y
author3-last3
@#$%^
AU
user0@example.com
https://host0.example.com/index.html
@twitter4
@github4
an affiliation for #4 author
n

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' mkiocccentry.c | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous as if one is to move up
# in the file they'll see that answers is actually defined above as well.
#
# error: This function is only defined later. Move the definition up. [SC2218]
# shellcheck disable=SC2218
answers >>answers.txt

# fake some required files
#
test -f "${src_dir}"/Makefile || cat Makefile.example >"${src_dir}"/Makefile
test -f "${src_dir}"/remarks.md || cat README.md >"${src_dir}"/remarks.md
test -f "${src_dir}"/extra1 || echo "123" >"${src_dir}"/extra1
test -f "${src_dir}"/extra2 || echo "456" >"${src_dir}"/extra2

# delete the work directory for next test
find "${work_dir_esc}" -mindepth 1 -depth -delete
rm -f "${src_dir}"/empty.c
:> "${src_dir}"/empty.c
# test empty prog.c, ignoring the warning about it
yes | ./mkiocccentry -q -W -i answers.txt -v "$V_FLAG" -J "$J_FLAG" -- "${work_dir}" "${src_dir}"/{empty.c,Makefile,remarks.md,extra1,extra2}
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi
rm -f "${src_dir}"/empty.c

# Form entries that are unlikely to win the IOCCC :-)
#
test -f "${src_dir}"/prog.c || {
cat >"${src_dir}"/prog.c <<"EOF"
#include <stdio.h>
int main(void) { puts("Hello, World!"); }
EOF
}

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers() {
cat <<"EOF"
test
1
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://host0.example.com/index.html
@twitter0
@github0
an affiliation for #0 author
n

author1 middle1a middle1b last1
UK





n
replaced-author1-handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://host2.example.com/index.html
@twitter2
@github2
an affiliation for #2 author
y

author3 middle3 last3
AU
user0@example.com
https://host0.example.com/index.html
@twitter3
@github3
an affiliation for #3 author
y
author3-last3
@#$%^
AU
user0@example.com
https://host0.example.com/index.html
@twitter4
@github4
an affiliation for #4 author
n

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' mkiocccentry.c | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous as if one is to move up
# error: This function is only defined later. Move the definition up. [SC2218]
# shellcheck disable=SC2218
answers >>answers.txt

# run the test, looking for an exit
#
yes | ./mkiocccentry -q -i answers.txt -v "$V_FLAG" -J "$J_FLAG" -- "${work_dir}" "${src_dir}"/{prog.c,Makefile,remarks.md,extra1,extra2}
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers() {
cat <<"EOF"
12345678-1234-4321-abcd-1234567890ab
2
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://host0.example.com/index.html
@twitter0
@github0
an affiliation for #0 author
n

author1 middle1a middle1b last1
UK





n
replaced-author1-handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://host2.example.com/index.html
@twitter2
@github2
an affiliation for #2 author
y

author3 middle3 last3
AU
user0@example.com
https://host0.example.com/index.html
@twitter3
@github3
an affiliation for #3 author
y
author3-last3
@#$%^
AU
user0@example.com
https://host0.example.com/index.html
@twitter4
@github4
an affiliation for #4 author
n

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' mkiocccentry.c | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
answers >>answers.txt

# fake a few more files
#
test -f "${src_dir}"/foo || cat LICENSE >"${src_dir}"/foo
test -f "${src_dir}"/bar || cat CODE_OF_CONDUCT.md >"${src_dir}"/bar

# run the test, looking for an exit
#
yes | ./mkiocccentry -q -i answers.txt -v "$V_FLAG" -J "$J_FLAG" -- "${work_dir}" "${src_dir}"/{prog.c,Makefile,remarks.md,extra1,extra2,foo,bar}
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
