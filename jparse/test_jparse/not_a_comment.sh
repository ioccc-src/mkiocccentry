#!/usr/bin/env bash
#
# not_a_comment.sh - Check of there are any "non-comment" lines in a file
#
# A "non-comment" line is a non-empty line that does NOT start
# with zero or more whitespace followed by a #.
#
# A use case for this tool is to detect of a file such as "Makefile.local"
# contains lines that contain make directives or other content that
# a Makefile might respond to.
#
# An empty file, or a file contains just whitespace, or a file containing
# just Makefile comments would not impact the execution of, say, make release.
# This tool, given such a file, would return true,
#
# Copyright (c) 2025 by Landon Curt Noll.  All Rights Reserved.
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
export VERSION="1.0 2025-01-03"
NAME=$(basename "$0")
export NAME
#
export V_FLAG=0
export EXIT_CODE=0


# usage
#
export USAGE="usage: $0 [-h] [-v level] [-V] [-n] [-N] [file ..]

	-h		print help message and exit
	-v level	set verbosity level (def level: $V_FLAG)
	-V		print version string and exit

	file		the file to check, - ==> read stdin

	NOTE: Empty, missing, and non-files are ignored.

	NOTE: By default (no args), this will exit 0.

Exit codes:
     0         all files are missing, empty, or contain only whitespace and/or # comments
     1	       some file contains non-comment / non-whitespace text
     2         -h and help string printed or -V and version string printed
     3         command line error
 >= 10         internal error

$NAME version: $VERSION"


# parse command line
#
while getopts :hv:V flag; do
  case "$flag" in
    h) echo "$USAGE"
	exit 2
	;;
    v) V_FLAG="$OPTARG"
	;;
    V) echo "$VERSION"
	exit 2
	;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
    *) echo "$0: ERROR: unexpected value from getopts: $flag" 1>&2
	echo 1>&2
	echo "$USAGE" 1>&2
	exit 3
	;;
  esac
done
#
# remove the options
#
shift $(( OPTIND - 1 ));


# print running info if verbose
#
# If -v 3 or higher, print exported variables in order that they were exported.
#
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: VERSION=$VERSION" 1>&2
    echo "$0: debug[3]: NAME=$NAME" 1>&2
    echo "$0: debug[3]: V_FLAG=$V_FLAG" 1>&2
fi


# process each arg
#
for file in "$@"; do

    # case: file is not -
    #
    if [[ $file != "-" ]]; then

	# ignore missing, empty and non-files
	#
	if [[ $V_FLAG -ge 3 ]]; then
	    echo "$0: debug[3]: processing: $file" 1>&2
	fi
	if [[ ! -e $file ]]; then
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: ignoring missing: $file" 1>&2
	    fi
	    continue
	fi
	if [[ ! -f $file ]]; then
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: ignoring non-file: $file" 1>&2
	    fi
	    continue
	fi
	if [[ ! -s $file ]]; then
	    if [[ $V_FLAG -ge 1 ]]; then
		echo "$0: debug[1]: ignoring empty file: $file" 1>&2
	    fi
	    continue
	fi
    fi

    # case: is - or is a not-empty file
    #
    if grep -E -v -q '^\s*$|^\s*#' "$file" >/dev/null 2>&1; then

	# case: file as non-comments found
	#
	if [[ $V_FLAG -ge 1 ]]; then
	    echo "$0: debug[1]: found non #-comments in: $file" 1>&2
	fi
	EXIT_CODE=1
	continue
    fi

    # case: file is empty or only contains whitespace and/or #-comments
    #
    if [[ $V_FLAG -ge 1 ]]; then
	echo "$0: debug[1]: file empty or only contains whitespace and/or #-comments: $file" 1>&2
    fi
    continue
done


# All Done!!! -- Jessica Noll, Age 2
#
exit "$EXIT_CODE"
