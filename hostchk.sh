#!/usr/bin/env bash
#
# hostchk.sh - make various checks on a host to determine if it can use the
# mkiocccentry repo.
#
# NOTE: This is not perfect and it cannot account for everything.
#
# NOTE: This is a work in progress.

# set up
export EXIT_CODE=0

# set up for tar test
#
RUN_TAR_TEST="true"
TEST_FILE=$(mktemp .hostchk.test_file.XXXXXXXXXX)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp $TEST_FILE exit code: $status" 1>&2
    EXIT_CODE=14
    RUN_TAR_TEST=
fi
date > "$TEST_FILE"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: date > $TEST_FILE exit code: $status" 1>&2
    EXIT_CODE=15
    RUN_TAR_TEST=
fi
if [[ ! -r $TEST_FILE ]]; then
    echo "$0: ERROR: not a readable TEST_FILE: $TEST_FILE" 1>&2
    EXIT_CODE=16
    RUN_TAR_TEST=
fi
TAR_ERROR=$(mktemp -u .hostchk.tar_err.XXXXXXXXXX.out)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $TAR_ERROR exit code: $status" 1>&2
    EXIT_CODE=17
    RUN_TAR_TEST=
fi
TARBALL=$(mktemp -u .hostchk.tarball.XXXXXXXXXX.tgz)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp -u $TARBALL exit code: $status" 1>&2
    EXIT_CODE=18
    RUN_TAR_TEST=
fi

# run tar test
#
TAR_TEST_SUCCESS="true"
if [[ -n $RUN_TAR_TEST ]]; then
    tar --format=v7 -cJf "$TARBALL" "$TEST_FILE" 2>"$TAR_ERROR"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: tar --format=v7 -cJf $TARBALL $TEST_FILE 2>$TAR_ERROR exit code: $status" 1>&2
	EXIT_CODE=19
	TAR_TEST_SUCCESS=
    fi
    if [[ ! -s $TARBALL ]]; then
	echo "$0: ERROR: did not find a non-empty tarball: $TARBALL" 1>&2
	EXIT_CODE=20
	TAR_TEST_SUCCESS=
    fi
    if [[ -s $TAR_ERROR ]]; then
	echo "$0: notice: tar stderr follows:" 1>&2
	cat "$TAR_ERROR" 1>&2
	echo "$0: notice: end of tar stderr" 1>&2
	EXIT_CODE=21
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
	printf "%s\n%s\n" "$h" "int main(void){ return 0; }" | cc -x c - -o "$PROG_FILE"

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
		# XXX This should only be printed for certain debug levels but
		# right now there are no debug levels so we always print it to
		# be sure it works.
		echo "$0: Running executable compiled using: $h: success"
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
