#!/usr/bin/env bash
#
# mkiocccentry_test.sh - a test suite for the mkiocccentry tool (not repo)
#
# This script was written in 2022 by:
#
#	@ilyakurdyukov		Ilya Kurdyukov
#
# with improvements by
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# Share and enjoy! :-)

# setup
#


# get tar path
TAR="$(type -P tar 2>/dev/null)"
# Make sure TAR is set:
#
# It's possible that the path could not be obtained so we set it to the default
# in this case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# could make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${TAR:=/usr/bin/tar} 2>/dev/null
#
# but due to the reasons cited above we must rely on the more complicated form:
[[ -z "$TAR" ]] && TAR="/usr/bin/tar"

# get ls path
LS="$(type -P ls 2>/dev/null)"
# Make sure LS is set:
#
# It's possible that the path could not be obtained so we set it to the default
# in this case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# could make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${LS:=/bin/ls} 2>/dev/null
#
# but due to the reasons cited above we must rely on the more complicated form:
[[ -z "$LS" ]] && LS="/bin/ls"

# get make path
MAKE="$(type -P make 2>/dev/null)"
# Make sure MAKE is set:
#
# It's possible that the path could not be obtained so we set it to the default
# in this case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# could make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${MAKE:=/bin/ls} 2>/dev/null
#
# but due to the reasons cited above we must rely on the more complicated form:
[[ -z "$MAKE" ]] && MAKE="/usr/bin/make"



export TXZCHK="./txzchk"
export FNAMCHK="./test_ioccc/fnamchk"

export MKIOCCCENTRY_TEST_VERSION="1.0.14 2025-02-25"
export USAGE="usage: $0 [-h] [-V] [-v level] [-J level] [-t tar] [-T txzchk] [-l ls] [-F fnamchk] [-m make] [-Z topdir]

    -h              print help and exit
    -V              print version and exit
    -v level        flag ignored
    -J level	    set JSON verbosity level
    -t tar	    path to tar that accepts -J option (def: $TAR)
    -T txzchk	    path to tar (def: $TXZCHK)
    -l ls	    path to ls executable (def: $LS)
    -F fnamchk	    path to fnamchk executable (def: $FNAMCHK)
    -m make         path to make (def: $MAKE)
    -Z topdir	    top level build directory (def: try . or ..)

Exit codes:
     0   all tests are OK
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
while getopts :hv:J:Vt:T:el:F:m:Z: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    V)	echo "$MKIOCCCENTRY_TEST_VERSION"
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	;;
    J)	J_FLAG="$OPTARG";
	;;
    Z)	TOPDIR="$OPTARG";
	;;
    t)  TAR="$OPTARG";
	;;
    T)	TXZCHK="$OPTARG";
	;;
    l)	LS="$OPTARG";
	;;
    F)	FNAMCHK="$OPTARG";
	;;
    m)  MAKE="$OPTARG";
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

# working locations - adjust as needed
#
Makefile="test_ioccc/Makefile.test"
workdir="test_ioccc/workdir"
topdir="test_ioccc/topdir"
topdir_topdir="test_ioccc/topdir/topdir"
topdir_topdir_topdir_topdir_topdir="test_ioccc/topdir/a/b/c/d/e"

# clean out test directories first
rm -rf -- "${workdir}" "${topdir}" "${topdir_topdir}" "${topdir_topdir_topdir_topdir_topdir}"
# be sure the working locations exist
#
mkdir -p -- "${workdir}" "${topdir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: error in creating working dirs: mkdir -p -- ${workdir} ${topdir}" 1>&2
    exit 9
fi

# firewall
#
# we need a working directory
if [[ ! -d ${workdir} ]]; then
    echo "$0: ERROR: workdir not found: ${workdir}" 1>&2
    exit 9
fi
# we also need the source directory
if [[ ! -d ${topdir} ]]; then
    echo "$0: ERROR: topdir not found: ${topdir}" 1>&2
    exit 9
fi

# if we are testing mkiocccentry we best have mkiocccentry! :-)
if [[ ! -x ./mkiocccentry ]]; then
    echo "$0: ERROR: executable not found: ./mkiocccentry" 1>&2
    exit 9
fi

# we need fnamchk
if [[ ! -x "${FNAMCHK}" ]]; then
    echo "$0: ERROR: fnamchk executable not found: ${FNAMCHK}" 1>&2
    exit 9
fi

# we need access to ls as well
if [[ ! -x "${LS}" ]]; then
    echo "$0: ERROR: executable ls not found: $LS" 1>&2
    exit 9
fi
# we need txzchk as well
if [[ ! -x "${TXZCHK}" ]]; then
    echo "$0: ERROR: executable not found: $TXZCHK" 1>&2
    exit 9
fi
# And we certainly need some tar to throw some feathers on!
#
# Well actually we DON'T want to throw feathers on it but we can't test txzchk
# without some tar now can we? :-)
if [[ ! -x "${TAR}" ]]; then
    echo "$0: ERROR: executable not found: $TAR" 1>&2
    exit 9
fi

# we need this for later and to make sure it's removed now
LONG_FILENAME="ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
# clean out the workdir area
workdir_esc="${workdir}"
test "${workdir:0:1}" = "-" && workdir_esc=./"${workdir}"
find "${workdir_esc}" -mindepth 1 -depth -delete
rm -f "${topdir}"/

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers()
{
cat <<"EOF"
test
0
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://a.host0.example.com/index.html
https://b.host0.example.com/index.html
@mastodon0@example.com
@github0
an affiliation for #0 author

author1 middle1a middle1b last1
UK






replaced_author1_handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://c.host2.example.com/index.html
http://d.host2.example.com/index.html
@mastodon2@example.com
@github2
an affiliation for #2 author

author3 middle3 last3
AU
user0@example.com
https://e.host0.example.com/index.html
https://f.host0.example.com/index.html
@mastodon3@example.com
@github3
an affiliation for #3 author
author3_last3
@#$%^
AU
user0@example.com
https://g.host0.example.com/index.html

@mastodon4@example.com
@github4
an affiliation for #4 author

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' soup/version.h | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous as if one is to move up
# in the file they'll see that answers is actually defined above as well.
#
# SC2218 (error): This function is only defined later. Move the definition up.
# https://www.shellcheck.net/wiki/SC2218
# shellcheck disable=SC2218
answers >>answers.txt

# fake a submission that only has the required files
#
test -f "${topdir}"/Makefile || cat "${Makefile}" >"${topdir}"/Makefile
test -f "${topdir}"/remarks.md || cat README.md >"${topdir}"/remarks.md
test -f "${topdir}"/prog.c || echo "int main(){}" >"${topdir}"/prog.c
# delete the work directory for next test
find "${workdir_esc}" -mindepth 1 -depth -delete
# test empty prog.c, ignoring the warning about it
echo "./mkiocccentry -y -q -W -i answers.txt -m $MAKE -F $FNAMCHK -t $TAR -T $TXZCHK -e -l $LS  -v $V_FLAG -J $J_FLAG -- ${workdir} ${topdir}"
./mkiocccentry -y -q -W -i answers.txt -m "$MAKE" -F "$FNAMCHK" -t "$TAR" -T "$TXZCHK" -e -l "$LS"  -v "$V_FLAG" -J "$J_FLAG" -- "${workdir}" "${topdir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi

echo "--"

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers()
{
cat <<"EOF"
test
1
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://a.host0.example.com/index.html
https://b.host0.example.com/index.html
@mastodon0@example.com
@github0
an affiliation for #0 author

author1 middle1a middle1b last1
UK






replaced_author1_handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://c.host2.example.com/index.html
http://d.host2.example.com/index.html
@mastodon2@example.com
@github2
an affiliation for #2 author

author3 middle3 last3
AU
user0@example.com
https://e.host0.example.com/index.html
https://f.host0.example.com/index.html
@mastodon3@example.com
@github3
an affiliation for #3 author
author3_last3
@#$%^
AU
user0@example.com
https://g.host0.example.com/index.html

@mastodon4@example.com
@github4
an affiliation for #4 author

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' soup/version.h | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous as if one is to move up
# in the file they'll see that answers is actually defined above as well.
#
# SC2218 (error): This function is only defined later. Move the definition up.
# https://www.shellcheck.net/wiki/SC2218
# shellcheck disable=SC2218
answers >>answers.txt

# fake some required, extra and optional files
#
mkdir -p "${topdir_topdir}"
test -f "${topdir}"/Makefile || cat "${Makefile}" >"${topdir}"/Makefile
test -f "${topdir}"/remarks.md || cat README.md >"${topdir}"/remarks.md
test -f "${topdir}"/extra1 || echo "123" >"${topdir}"/extra1
test -f "${topdir}"/extra2 || echo "456" >"${topdir}"/extra2
test -f "${topdir}"/try.sh || touch "${topdir}"/try.sh
test -f "${topdir}"/try.alt.sh || touch "${topdir}"/try.alt.sh
test -f "${topdir}"/prog.alt.c || touch "${topdir}"/prog.alt.c

rm -f "${topdir}"/prog.c
:> "${topdir}"/prog.c
# test empty prog.c, ignoring the warning about it
echo "./mkiocccentry -y -q -W -i answers.txt -m $MAKE -F $FNAMCHK -t $TAR -T $TXZCHK -e -l $LS  -v $V_FLAG -J $J_FLAG -- ${workdir} ${topdir}"
./mkiocccentry -y -q -W -i answers.txt -m "$MAKE" -F "$FNAMCHK" -t "$TAR" -T "$TXZCHK" -e -l "$LS"  -v "$V_FLAG" -J "$J_FLAG" -- "${workdir}" "${topdir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi

echo "--"

# Form 'submissions' that are unlikely to win the IOCCC :-)
#
test -f "${topdir}"/prog.c || {
cat >"${topdir}"/prog.c <<"EOF"
#include <stdio.h>
int main(void) { puts("Hello, World!"); }
EOF
}

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers()
{
cat <<"EOF"
test
2
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://h.host0.example.com/index.html
https://i.host0.example.com/index.html
@mastodon0@example.com
@github0
an affiliation for #0 author

author1 middle1a middle1b last1
UK






replaced_author1_handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://j.host2.example.com/index.html
http://k.host2.example.com/index.html
@mastodon2@example.com
@github2
an affiliation for #2 author

author3 middle3 last3
AU
user0@example.com
https://l.host0.example.com/index.html

@mastodon3@example.com
@github3
an affiliation for #3 author
author3_last3
@#$%^
AU
user0@example.com
https://m.host0.example.com/index.html

@mastodon4@example.com
@github4
an affiliation for #4 author

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' soup/version.h | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous.
#
# SC2218 (error): This function is only defined later. Move the definition up.
# https://www.shellcheck.net/wiki/SC2218
# shellcheck disable=SC2218
answers >>answers.txt

# run the test, looking for an exit
#
echo "./mkiocccentry -y -q -i answers.txt -m $MAKE -F $FNAMCHK -t $TAR -T $TXZCHK -e -l $LS -v $V_FLAG -J $J_FLAG -- ${workdir} ${topdir}"
./mkiocccentry -y -q -i answers.txt -m "$MAKE" -F "$FNAMCHK" -t "$TAR" -T "$TXZCHK" -e -l "$LS" -v "$V_FLAG" -J "$J_FLAG" -- "${workdir}" "${topdir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi

echo "--"

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers()
{
cat <<"EOF"
12345678-1234-4321-abcd-1234567890ab
3
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://n.host0.example.com/index.html
https://o.host0.example.com/index.html
@mastodon0@example.com
@github0
an affiliation for #0 author

author1 middle1a middle1b last1
UK






replaced_author1_handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com

http://p.host2.example.com/index.html
@mastodon2@example.com
@github2
an affiliation for #2 author

author3 middle3 last3
AU
user0@example.com
https://q.host0.example.com/index.html
https://r.host0.example.com/index.html
@mastodon3@example.com
@github3
an affiliation for #3 author
author3_last3
@#$%^
AU
user0@example.com
https://s.host0.example.com/index.html
https://t.host0.example.com/index.html
@mastodon4@example.com
@github4
an affiliation for #4 author

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' soup/version.h | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous.
#
# SC2218 (error): This function is only defined later. Move the definition up.
# https://www.shellcheck.net/wiki/SC2218
# shellcheck disable=SC2218
answers >>answers.txt

# fake a couple more files
#
test -f "${topdir}"/foo || cat LICENSE >"${topdir}"/foo
test -f "${topdir}"/bar || cat CODE_OF_CONDUCT.md >"${topdir}"/bar

# run the test, looking for an exit
#
echo "./mkiocccentry -y -q -i answers.txt -m $MAKE -F $FNAMCHK -t $TAR -T $TXZCHK -e -l $LS -v $V_FLAG -J $J_FLAG -- ${workdir} ${topdir}"
./mkiocccentry -y -q -i answers.txt -m "$MAKE" -F "$FNAMCHK" -t "$TAR" -T "$TXZCHK" -e -l "$LS" -v "$V_FLAG" -J "$J_FLAG" -- "${workdir}" "${topdir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi

echo "--"

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers()
{
cat <<"EOF"
test
4
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://a.host0.example.com/index.html
https://b.host0.example.com/index.html
@mastodon0@example.com
@github0
an affiliation for #0 author

author1 middle1a middle1b last1
UK






replaced_author1_handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://c.host2.example.com/index.html
http://d.host2.example.com/index.html
@mastodon2@example.com
@github2
an affiliation for #2 author

author3 middle3 last3
AU
user0@example.com
https://e.host0.example.com/index.html
https://f.host0.example.com/index.html
@mastodon3@example.com
@github3
an affiliation for #3 author
author3_last3
@#$%^
AU
user0@example.com
https://g.host0.example.com/index.html

@mastodon4@example.com
@github4
an affiliation for #4 author

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' soup/version.h | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous as if one is to move up
# in the file they'll see that answers is actually defined above as well.
#
# SC2218 (error): This function is only defined later. Move the definition up.
# https://www.shellcheck.net/wiki/SC2218
# shellcheck disable=SC2218
answers >>answers.txt

#
# this next test must FAIL as it has a filename that is too long
#

# fake a filename that's too long
#
test -f "${topdir}/$LONG_FILENAME" || touch "${topdir}/$LONG_FILENAME"

# run the test, looking for an exit
#
echo "./mkiocccentry -y -q -i answers.txt -m $MAKE -F $FNAMCHK -t $TAR -T $TXZCHK -e  -l $LS -v $V_FLAG -J $J_FLAG -- ${workdir} ${topdir}"
./mkiocccentry -y -q -i answers.txt -m "$MAKE" -F "$FNAMCHK" -t "$TAR" -T "$TXZCHK" -e  -l "$LS" -v "$V_FLAG" -J "$J_FLAG" -- "${workdir}" "${topdir}"
status=$?
if [[ ${status} -ne 4 ]]; then
    echo "$0: ERROR: mkiocccentry exit code not 4: $status" 1>&2
    exit 1
else
    echo "$0: NOTE: the above error is expected as we were testing that the filename" 1>&2
    echo "$0: NOTE: length limit works." 1>&2
fi

echo "--"

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers()
{
cat <<"EOF"
test
5
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://a.host0.example.com/index.html
https://b.host0.example.com/index.html
@mastodon0@example.com
@github0
an affiliation for #0 author

author1 middle1a middle1b last1
UK






replaced_author1_handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://c.host2.example.com/index.html
http://d.host2.example.com/index.html
@mastodon2@example.com
@github2
an affiliation for #2 author

author3 middle3 last3
AU
user0@example.com
https://e.host0.example.com/index.html
https://f.host0.example.com/index.html
@mastodon3@example.com
@github3
an affiliation for #3 author
author3_last3
@#$%^
AU
user0@example.com
https://g.host0.example.com/index.html

@mastodon4@example.com
@github4
an affiliation for #4 author

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' soup/version.h | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous as if one is to move up
# in the file they'll see that answers is actually defined above as well.
#
# SC2218 (error): This function is only defined later. Move the definition up.
# https://www.shellcheck.net/wiki/SC2218
# shellcheck disable=SC2218
answers >>answers.txt

# remove long filename
rm -f "${topdir}"/"${LONG_FILENAME}"
# make a file in the subdirectory
#
test -f "${topdir_topdir}/foo" || touch "${topdir_topdir}/foo"

# run the test, looking for an exit
#
echo "./mkiocccentry -y -q -i answers.txt -m $MAKE -F $FNAMCHK -t $TAR -T $TXZCHK -e -l $LS -v $V_FLAG -J $J_FLAG -- ${workdir} ${topdir}"
./mkiocccentry -y -q -i answers.txt -m "$MAKE" -F "$FNAMCHK" -t "$TAR" -T "$TXZCHK" -e -l "$LS" -v "$V_FLAG" -J "$J_FLAG" -- "${workdir}" "${topdir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi

echo "--"

# Answers as of mkiocccentry version: v0.40 2022-03-15
answers()
{
cat <<"EOF"
test
6
title-for-entry0
abstract for entry #0
5
author0 middle0 thisisaveryverylonglastname0
AU
user0@example.com
https://a.host0.example.com/index.html
https://b.host0.example.com/index.html
@mastodon0@example.com
@github0
an affiliation for #0 author

author1 middle1a middle1b last1
UK






replaced_author1_handle
EOF
# Avoid triggering an out of date shellcheck bug by using encoded hex characters
printf "Author2 \\xc3\\xa5\\xe2\\x88\\xab\\xc3\\xa7\\xe2\\x88\\x82\\xc2\\xb4\\xc6\\x92\\xc2\\xa9 LAST2\\n"
cat <<"EOF"
US
user2@example.com
http://c.host2.example.com/index.html
http://d.host2.example.com/index.html
@mastodon2@example.com
@github2
an affiliation for #2 author

author3 middle3 last3
AU
user0@example.com
https://e.host0.example.com/index.html
https://f.host0.example.com/index.html
@mastodon3@example.com
@github3
an affiliation for #3 author
author3_last3
@#$%^
AU
user0@example.com
https://g.host0.example.com/index.html

@mastodon4@example.com
@github4
an affiliation for #4 author

ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VERSION' soup/version.h | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
#
# We disable this shellcheck error because answers already is defined but we
# redefine it as well. The error is ironically erroneous as if one is to move up
# in the file they'll see that answers is actually defined above as well.
#
# SC2218 (error): This function is only defined later. Move the definition up.
# https://www.shellcheck.net/wiki/SC2218
# shellcheck disable=SC2218
answers >>answers.txt

# be sure the working location exist
#
mkdir -p -- "${topdir_topdir_topdir_topdir_topdir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: ERROR: error in creating working dirs: mkdir -p -- ${topdir_topdir_topdir_topdir_topdir}" 1>&2
    exit 9
fi

test -f "${topdir_topdir_topdir_topdir_topdir}/foo" || touch "${topdir_topdir_topdir_topdir_topdir}/foo"

# run the test, looking for an exit
#
echo "./mkiocccentry -y -q -i answers.txt -m $MAKE -F $FNAMCHK -t $TAR -T $TXZCHK -e -l $LS -v $V_FLAG -J $J_FLAG -- ${workdir} ${topdir}"
./mkiocccentry -y -q -i answers.txt -m "$MAKE" -F "$FNAMCHK" -t "$TAR" -T "$TXZCHK" -e -l "$LS" -v "$V_FLAG" -J "$J_FLAG" -- "${workdir}" "${topdir}"
status=$?
if [[ ${status} -ne 4 ]]; then
    echo "$0: ERROR: mkiocccentry zero exit code not 4: $status" 1>&2
    exit 1
else
    echo "$0: NOTE: the above error is expected as we were testing that the max depth" 1>&2
    echo "$0: NOTE: limit works." 1>&2
fi

echo "--"

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
exit 0
