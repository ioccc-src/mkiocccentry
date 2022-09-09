#!/usr/bin/env bash
#
# hostchk.sh - make various checks on a host to determine if it can use the
# mkiocccentry repo.
#
#
# NOTE: This is NOT yet finished. More needs to be discussed at
# https://github.com/ioccc-src/mkiocccentry/issues/250 (hostchk rule/script) and at
# https://github.com/ioccc-src/mkiocccentry/issues/248 (make bug-report
# rule/script).
#
# NOTE: This cannot be perfect and it cannot account for everything but along
# with bug-report.sh (to be written) we hope to account for most things. One can
# always report an issue at the GitHub issues page
# https://github.com/ioccc-src/mkiocccentry/issues (probably preferable as
# others can help as well) or email the Judges.
#

# set up
#

# attempt to fetch system specific paths to the tools we need
#
# get tar path
TAR="$(type -P tar 2>/dev/null)"
# get cc path
CC="$(type -P cc 2>/dev/null)"
# It's possible that the paths could not be obtained so we set to defaults in
# this case.
#
# We could do it via parameter substitution but since it tries to execute the
# command if for some reason the tool ever works without any args specified it
# would make the script block (if we did it via parameter substitution we would
# still have to redirect stderr to /dev/null). It would look like:
#
#   ${CC:=/usr/bin/cc} 2>/dev/null
#   ${TAR:=/usr/bin/tar} 2>/dev/null
#
# make sure CC is set
if [[ -z "$CC" ]]; then
    CC="/usr/bin/cc"
fi
# make sure TAR is set
if [[ -z "$TAR" ]]; then
    TAR="/usr/bin/tar"
fi

export USAGE="usage: $0 [-h] [-v level] [-D dbg_level] [-t tar] [-c cc]

    -h			    print help and exit 2
    -v level		    set verbosity level for this script: (def level: 0)
    -D dbg_level	    set verbosity level for tests (def: level: 0)
    -t tar		    path to tar that accepts -J option (def: $TAR)
    -c cc		    path to compiler (def: $CC)

exit codes:
    0 - all is well
    1 - at least one test failed
    2 - help mode exit
    3 - invalid command line
    >= 30 - internal error"

export EXIT_CODE=0

# parse args
#
export V_FLAG="0"
export DBG_LEVEL="0"
while getopts :hv:D:t:c: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    D) DBG_LEVEL="$OPTARG";
       ;;
    t) TAR="$OPTARG";
	;;
    c) CC="$OPTARG";
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

# firewall checks
#
# check for tar
#
if [[ ! -e $TAR ]]; then
    echo "$0: ERROR: tar not found: $TAR" 1>&2
    exit 4
fi
if [[ ! -f $TAR ]]; then
    echo "$0: ERROR: tar not a regular file: $TAR" 1>&2
    exit 4
fi
if [[ ! -x $TAR ]]; then
    echo "$0: ERROR: tar not executable: $TAR" 1>&2
    exit 4
fi

# check for cc
#
if [[ ! -e $CC ]]; then
    echo "$0: ERROR: cc not found: $CC" 1>&2
    exit 4
fi
if [[ ! -f $CC ]]; then
    echo "$0: ERROR: cc not a regular file: $CC" 1>&2
    exit 4
fi
if [[ ! -x $CC ]]; then
    echo "$0: ERROR: cc not executable: $CC" 1>&2
    exit 4
fi




# set up for tar test
#
RUN_TAR_TEST="true"
TEST_FILE=$(mktemp .hostchk.test_file.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp $TEST_FILE exit code: $status" 1>&2
    EXIT_CODE=30
    RUN_TAR_TEST=
fi
date > "$TEST_FILE"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: date > $TEST_FILE exit code: $status" 1>&2
    EXIT_CODE=31
    RUN_TAR_TEST=
fi
if [[ ! -r $TEST_FILE ]]; then
    echo "$0: ERROR: not a readable TEST_FILE: $TEST_FILE" 1>&2
    EXIT_CODE=32
    RUN_TAR_TEST=
fi
TAR_ERROR=$(mktemp -u .hostchk.tar_err.XXXXXXXXXX.out)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $TAR_ERROR exit code: $status" 1>&2
    EXIT_CODE=33
    RUN_TAR_TEST=
fi
TARBALL=$(mktemp -u .hostchk.tarball.XXXXXXXXXX.txz)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $TARBALL exit code: $status" 1>&2
    EXIT_CODE=34
    RUN_TAR_TEST=
fi

# run tar test
#
TAR_TEST_SUCCESS="true"
if [[ -n $RUN_TAR_TEST ]]; then
    "${TAR}" --format=v7 -cJf "$TARBALL" "$TEST_FILE" 2>"$TAR_ERROR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: $TAR --format=v7 -cJf $TARBALL $TEST_FILE 2>$TAR_ERROR exit code: $status" 1>&2
	EXIT_CODE=35
	TAR_TEST_SUCCESS=
    fi
    if [[ ! -s $TARBALL ]]; then
	echo "$0: ERROR: did not find a non-empty tarball: $TARBALL" 1>&2
	EXIT_CODE=36
	TAR_TEST_SUCCESS=
    fi
    if [[ -s $TAR_ERROR ]]; then
	echo "$0: notice: tar stderr follows:" 1>&2
	cat "$TAR_ERROR" 1>&2
	echo "$0: notice: end of tar stderr" 1>&2
	EXIT_CODE=37
	TAR_TEST_SUCCESS=
    fi
else
    echo "$0: notice: tar test disabled due to test set up error(s)" 1>&2
    TAR_TEST_SUCCESS=
fi

# tar test clean up
#
if [[ -n $TAR_TEST_SUCCESS ]]; then
    rm -f "$TEST_FILE" "$TAR_ERROR" "$TARBALL"
fi

# set up for compile test
#
RUN_INCLUDE_TEST="true"
PROG_FILE=$(mktemp -u .hostchk.prog.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $PROG_FILE exit code: $status" 1>&2
    EXIT_CODE=41
    RUN_INCLUDE_TEST=
fi

# run include tests
#
export INCLUDE_TEST_SUCCESS="true"
if [[ -n $RUN_INCLUDE_TEST ]]; then

    # test each required system include file
    #
    while read -r h; do

	# form C prog
	#
	rm -f "$PROG_FILE"
	printf "%s\n%s\n" "$h" "int main(void){ return 0; }" | "${CC}" -x c - -o "$PROG_FILE"

	# test compile
	#
	status="$?"
	if [[ $status -ne 0 ]]; then
	    echo "$0: FATAL: missing system include file <$h>" 1>&2
	    EXIT_CODE=42
	    INCLUDE_TEST_SUCCESS="false"
	fi
	if [[ -s "$PROG_FILE" && -x "$PROG_FILE" ]]; then
	    ./"$PROG_FILE"
	    status="$?"
	    if [[ $status -ne 0 ]]; then
		echo "$0: FATAL: unable to run executable compiled using: $h" 1>&2
		EXIT_CODE=43
		INCLUDE_TEST_SUCCESS="false"
	    else
		if [[ $V_FLAG -gt 1 ]]; then
		    echo "$0: Running executable compiled using: $h: success"
		fi
	    fi
	else
	    echo "$0: FATAL: unable to form an executable compiled using: $h" 1>&2
	    EXIT_CODE=44
	    INCLUDE_TEST_SUCCESS="false"
	fi

	# clean up after compile test
	#
	rm -f "$PROG_FILE"
    done < <(grep -h -o '#include.*<.*>' ./*.c ./*.h|sort -u)
else
    echo "$0: notice: include test disabled due to test set up error(s)" 1>&2
    EXIT_CODE=45
    INCLUDE_TEST_SUCCESS="false"
fi

exit "$EXIT_CODE"
