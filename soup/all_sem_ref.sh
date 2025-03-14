#!/usr/bin/env bash
#
# all_sem_ref.sh - form the semantic ref directory
#
# For the ref sub-directory using via ./jsemcgen.sh, from test .info.json
# and .auth.json files, form *.json.c and *.json.h semantic table files
# under a given directory.
#
# The JSON parser and this tool were co-developed in 2022 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
# and:
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# The concept of this file was developed by:
#
#	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
#
# Share and enjoy! :-)
#     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
#

# setup
#
export V_FLAG="0"
export JSEMTBLGEN="../jparse/jsemtblgen"
export JSEMCGEN_SH="../jparse/jsemcgen.sh"
export ALL_SEM_REF_VERSION="1.2.1 2023-03-14"

export USAGE="usage: $0 [-h] [-v level] [-V] [-j jsemtblgen] [-J jsemcgen.sh] info.head.c info.tail.c info.head.h
	info.tail.h auth.head.c auth.tail.c auth.head.h auth.tail.h info_dir auth_dir ref_dir

	-h		    print help message and exit
	-v level	    set verbosity level (def level: 0)
	-V		    print version string and exit

	-j jsemtblgen	    path to jsemtblgen (def: $JSEMTBLGEN)
	-J jsemcgen.sh	    path to jsemcgen.sh (def: $JSEMCGEN_SH)

	info.head.c	    .info.json style header for .c semantic table files
	info.tail.c	    .info.json style footer for .c semantic table files
	info.head.h	    .info.json style header for .h semantic table files
	info.tail.h	    .info.json style footer for .h semantic table files

	auth.head.c	    .auth.json style header for .c semantic table files
	auth.tail.c	    .auth.json style footer for .c semantic table files
	auth.head.h	    .auth.json style header for .h semantic table files
	auth.tail.h	    .auth.json style footer for .h semantic table files

	info_dir	    process *.json files as .info.json files
	auth_dir	    process *.json files as .auth.json files

	ref_dir		    sub-directory under which semantic table files are written

Exit codes:
     0	    all OK
     2	    -h and help string printed or -V and version string printed
     3	    command line error
     4	    jsemcgen.sh and/or jsemtblgen not found or not executable
     5	    missing or not readable header or footer file
     6	    missing, not readable, or not writable info_dir, auth_dir and/or ref_dir
 >= 10	    internal error

all_sem_ref.sh version: $ALL_SEM_REF_VERSION"


# IOCCC requires use of C locale
#
export LANG="C"
export LC_CTYPE="C"
export LC_NUMERIC="C"
export LC_TIME="C"
export LC_COLLATE="C"
export LC_MONETARY="C"
export LC_MESSAGES="C"
export LC_PAPER="C"
export LC_NAME="C"
export LC_ADDRESS="C"
export LC_TELEPHONE="C"
export LC_MEASUREMENT="C"
export LC_IDENTIFICATION="C"
export LC_ALL="C"


# parse args
#
while getopts :hv:Vj:J: flag; do
    case "$flag" in
    h)	echo "$USAGE" 1>&2
	exit 2
	;;
    v)	V_FLAG="$OPTARG";
	JSEMTBLGEN_ARGS="$JSEMTBLGEN_ARGS -v '$V_FLAG'";
	;;
    V)	echo "$ALL_SEM_REF_VERSION"
	exit 2
	;;
    j)	JSEMTBLGEN="$OPTARG";
	;;
    J)	JSEMCGEN_SH="$OPTARG";
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
if [[ $# -ne 11 ]]; then
    echo "$0: ERROR: expected 11 arguments, found $#" 1>&2
    echo 1>&2
    echo "$USAGE" 1>&2
    exit 3
fi
INFO_HEAD_C="$1"
INFO_TAIL_C="$2"
INFO_HEAD_H="$3"
INFO_TAIL_H="$4"
AUTH_HEAD_C="$5"
AUTH_TAIL_C="$6"
AUTH_HEAD_H="$7"
AUTH_TAIL_H="$8"
INFO_DIR="$9"
AUTH_DIR="${10}"
REF_DIR="${11}"
export INFO_HEAD_C INFO_TAIL_C INFO_HEAD_H INFO_TAIL_H
export AUTH_HEAD_C AUTH_TAIL_C AUTH_HEAD_H AUTH_TAIL_H
export AUTH_DIR AUTH_DIR REF_DIR
if [[ $V_FLAG -ge 3 ]]; then
    echo "$0: debug[3]: INFO_HEAD_C=$INFO_HEAD_C" 1>&2
    echo "$0: debug[3]: INFO_TAIL_C=$INFO_TAIL_C" 1>&2
    echo "$0: debug[3]: INFO_HEAD_H=$INFO_HEAD_H" 1>&2
    echo "$0: debug[3]: INFO_TAIL_H=$INFO_TAIL_H" 1>&2
    echo "$0: debug[3]: AUTH_HEAD_C=$AUTH_HEAD_C" 1>&2
    echo "$0: debug[3]: AUTH_TAIL_C=$AUTH_TAIL_C" 1>&2
    echo "$0: debug[3]: AUTH_HEAD_H=$AUTH_HEAD_H" 1>&2
    echo "$0: debug[3]: AUTH_TAIL_H=$AUTH_TAIL_H" 1>&2
    echo "$0: debug[3]: INFO_DIR=$INFO_DIR" 1>&2
    echo "$0: debug[3]: AUTH_DIR=$AUTH_DIR" 1>&2
    echo "$0: debug[3]: REF_DIR==$REF_DIR" 1>&2
fi

# firewall
#
if [[ ! -x $JSEMCGEN_SH ]]; then
    echo "$0: ERROR: jsemcgen.sh is not an executable: $JSEMCGEN_SH" 1>&2
    exit 4
fi
if [[ ! -x $JSEMTBLGEN ]]; then
    echo "$0: ERROR: jsemtblgen is not an executable: $JSEMTBLGEN" 1>&2
    exit 4
fi
#
if [[ ! -e $INFO_HEAD_C ]]; then
    echo "$0: ERROR: missing file info.head.c: $INFO_HEAD_C" 1>&2
    exit 5
fi
if [[ ! -f $INFO_HEAD_C ]]; then
    echo "$0: ERROR: info.head.c not a regular file: $INFO_HEAD_C" 1>&2
    exit 5
fi
if [[ ! -r $INFO_HEAD_C ]]; then
    echo "$0: ERROR: info.head.c not a readable file: $INFO_HEAD_C" 1>&2
    exit 5
fi
#
if [[ ! -e $INFO_HEAD_H ]]; then
    echo "$0: ERROR: missing file info.head.h: $INFO_HEAD_H" 1>&2
    exit 5
fi
if [[ ! -f $INFO_HEAD_H ]]; then
    echo "$0: ERROR: info.head.h not a regular file: $INFO_HEAD_H" 1>&2
    exit 5
fi
if [[ ! -r $INFO_HEAD_H ]]; then
    echo "$0: ERROR: info.head.h not a readable file: $INFO_HEAD_H" 1>&2
    exit 5
fi
#
if [[ ! -e $INFO_TAIL_C ]]; then
    echo "$0: ERROR: missing file info.tail.c: $INFO_TAIL_C" 1>&2
    exit 5
fi
if [[ ! -f $INFO_TAIL_C ]]; then
    echo "$0: ERROR: info.tail.c not a regular file: $INFO_TAIL_C" 1>&2
    exit 5
fi
if [[ ! -r $INFO_TAIL_C ]]; then
    echo "$0: ERROR: info.tail.c not a readable file: $INFO_TAIL_C" 1>&2
    exit 5
fi
#
if [[ ! -e $INFO_TAIL_H ]]; then
    echo "$0: ERROR: missing file info.tail.h: $INFO_TAIL_H" 1>&2
    exit 5
fi
if [[ ! -f $INFO_TAIL_H ]]; then
    echo "$0: ERROR: info.tail.h not a regular file: $INFO_TAIL_H" 1>&2
    exit 5
fi
if [[ ! -r $INFO_TAIL_H ]]; then
    echo "$0: ERROR: info.tail.h not a readable file: $INFO_TAIL_H" 1>&2
    exit 5
fi
#
if [[ ! -e $AUTH_HEAD_C ]]; then
    echo "$0: ERROR: missing file auth.head.c: $AUTH_HEAD_C" 1>&2
    exit 5
fi
if [[ ! -f $AUTH_HEAD_C ]]; then
    echo "$0: ERROR: auth.head.c not a regular file: $AUTH_HEAD_C" 1>&2
    exit 5
fi
if [[ ! -r $AUTH_HEAD_C ]]; then
    echo "$0: ERROR: auth.head.c not a readable file: $AUTH_HEAD_C" 1>&2
    exit 5
fi
#
if [[ ! -e $AUTH_HEAD_H ]]; then
    echo "$0: ERROR: missing file auth.head.h: $AUTH_HEAD_H" 1>&2
    exit 5
fi
if [[ ! -f $AUTH_HEAD_H ]]; then
    echo "$0: ERROR: auth.head.h not a regular file: $AUTH_HEAD_H" 1>&2
    exit 5
fi
if [[ ! -r $AUTH_HEAD_H ]]; then
    echo "$0: ERROR: auth.head.h not a readable file: $AUTH_HEAD_H" 1>&2
    exit 5
fi
#
if [[ ! -e $AUTH_TAIL_C ]]; then
    echo "$0: ERROR: missing file auth.tail.c: $AUTH_TAIL_C" 1>&2
    exit 5
fi
if [[ ! -f $AUTH_TAIL_C ]]; then
    echo "$0: ERROR: auth.tail.c not a regular file: $AUTH_TAIL_C" 1>&2
    exit 5
fi
if [[ ! -r $AUTH_TAIL_C ]]; then
    echo "$0: ERROR: auth.tail.c not a readable file: $AUTH_TAIL_C" 1>&2
    exit 5
fi
#
if [[ ! -e $AUTH_TAIL_H ]]; then
    echo "$0: ERROR: missing file auth.tail.h: $AUTH_TAIL_H" 1>&2
    exit 5
fi
if [[ ! -f $AUTH_TAIL_H ]]; then
    echo "$0: ERROR: auth.tail.h not a regular file: $AUTH_TAIL_H" 1>&2
    exit 5
fi
if [[ ! -r $AUTH_TAIL_H ]]; then
    echo "$0: ERROR: auth.tail.h not a readable file: $AUTH_TAIL_H" 1>&2
    exit 5
fi
#
if [[ ! -e $INFO_DIR ]]; then
    echo "$0: ERROR: missing directory info_dir: $INFO_DIR" 1>&2
    exit 6
fi
if [[ ! -d $INFO_DIR ]]; then
    echo "$0: ERROR: info_dir not a directory: $INFO_DIR" 1>&2
    exit 6
fi
if [[ ! -w $INFO_DIR ]]; then
    echo "$0: ERROR: info_dir not a writable directory: $INFO_DIR" 1>&2
    exit 6
fi
if [[ ! -r $INFO_DIR ]]; then
    echo "$0: ERROR: info_dir not a readable directory: $INFO_DIR" 1>&2
    exit 6
fi
#
if [[ ! -e $AUTH_DIR ]]; then
    echo "$0: ERROR: missing directory auth_dir: $AUTH_DIR" 1>&2
    exit 6
fi
if [[ ! -d $AUTH_DIR ]]; then
    echo "$0: ERROR: auth_dir not a directory: $AUTH_DIR" 1>&2
    exit 6
fi
if [[ ! -w $AUTH_DIR ]]; then
    echo "$0: ERROR: auth_dir not a writable directory: $AUTH_DIR" 1>&2
    exit 6
fi
if [[ ! -r $AUTH_DIR ]]; then
    echo "$0: ERROR: auth_dir not a readable directory: $INFO_DIR" 1>&2
    exit 6
fi
#
if [[ ! -e $REF_DIR ]]; then
    echo "$0: ERROR: missing directory ref_dir: $REF_DIR" 1>&2
    exit 6
fi
if [[ ! -d $REF_DIR ]]; then
    echo "$0: ERROR: ref_dir not a directory: $REF_DIR" 1>&2
    exit 6
fi
if [[ ! -w $REF_DIR ]]; then
    echo "$0: ERROR: ref_dir not a writable directory: $REF_DIR" 1>&2
    exit 6
fi
if [[ ! -r $REF_DIR ]]; then
    echo "$0: ERROR: ref_dir not a readable directory: $INFO_DIR" 1>&2
    exit 6
fi

TMP_FILE=$(mktemp .all_ref.XXXXXXXXXX.out)
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: ERROR: mktemp .all_ref.XXXXXXXXXX exit code: $status" 1>&2
    exit 10
fi
trap "rm -f \$TMP_FILE; exit" 0 1 2 3 15
export TMP_FILE

# form .info.json style semantic table .c files
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: form .info.json style semantic table .c files from $INFO_DIR" 1>&2
fi
find "$INFO_DIR" -type f -name '*.json' -print 2>/dev/null | while read -r path; do

    # determine filenames
    #
    file=$(basename "$path")
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: processing .info.json style file: $file" 1>&2
    fi
    out="$REF_DIR/$file.c"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: will try to form: $out" 1>&2
    fi

    # form output file
    #
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: about to execute: $JSEMCGEN_SH -j $JSEMTBLGEN -N sem_info -P chk -- $path . . . $TMP_FILE" 1>&2
    fi
    "$JSEMCGEN_SH" -j "$JSEMTBLGEN" -N sem_info -P chk -- "$path" . . . "$TMP_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: $JSEMCGEN_SH -j $JSEMTBLGEN -N sem_info -P chk -- $path . . . $TMP_FILE  exit code: $status" 1>&2
	exit 11
    fi
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: about to execute: cat $INFO_HEAD_C $TMP_FILE $INFO_TAIL_C > $out" 1>&2
    fi
    cat "$INFO_HEAD_C" "$TMP_FILE" "$INFO_TAIL_C" > "$out"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cat $INFO_HEAD_C $TMP_FILE $INFO_TAIL_C > $out exit code: $status" 1>&2
	exit 12
    fi
done

# form .info.json style semantic table .h files
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: form .info.json style semantic table .h files from $INFO_DIR" 1>&2
fi
find "$INFO_DIR" -type f -name '*.json' -print 2>/dev/null | while read -r path; do

    # determine filenames
    #
    file=$(basename "$path")
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: processing .info.json style file: $file" 1>&2
    fi
    out="$REF_DIR/$file.h"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: will try to form: $out" 1>&2
    fi

    # form output file
    #
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: about to execute: $JSEMCGEN_SH -j $JSEMTBLGEN -N sem_info -P chk -I -- $path . . . $TMP_FILE" 1>&2
    fi
    "$JSEMCGEN_SH" -j "$JSEMTBLGEN" -N sem_info -P chk -I -- "$path" . . . "$TMP_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: $JSEMCGEN_SH -j $JSEMTBLGEN -N sem_info -P chk -I -- $path . . . $TMP_FILE  exit code: $status" 1>&2
	exit 13
    fi
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: about to execute: cat $INFO_HEAD_H $TMP_FILE $INFO_TAIL_H > $out" 1>&2
    fi
    cat "$INFO_HEAD_H" "$TMP_FILE" "$INFO_TAIL_H" > "$out"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cat $INFO_HEAD_H $TMP_FILE $INFO_TAIL_H > $out exit code: $status" 1>&2
	exit 14
    fi
done

# form .auth.json style semantic table .c files
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: form .auth.json style semantic table .c files from $AUTH_DIR" 1>&2
fi
find "$AUTH_DIR" -type f -name '*.json' -print 2>/dev/null | while read -r path; do

    # determine filenames
    #
    file=$(basename "$path")
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: processing .auth.json style file: $file" 1>&2
    fi
    out="$REF_DIR/$file.c"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: will try to form: $out" 1>&2
    fi

    # form output file
    #
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: about to execute: $JSEMCGEN_SH -j $JSEMTBLGEN -N sem_auth -P chk -- $path . . . $TMP_FILE" 1>&2
    fi
    "$JSEMCGEN_SH" -j "$JSEMTBLGEN" -N sem_auth -P chk -- "$path" . . . "$TMP_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: $JSEMCGEN_SH -j $JSEMTBLGEN -N sem_auth -P chk -- $path . . . $TMP_FILE  exit code: $status" 1>&2
	exit 15
    fi
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: about to execute: cat $AUTH_HEAD_C $TMP_FILE $AUTH_TAIL_C > $out" 1>&2
    fi
    cat "$AUTH_HEAD_C" "$TMP_FILE" "$AUTH_TAIL_C" > "$out"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cat $AUTH_HEAD_C $TMP_FILE $AUTH_TAIL_C > $out exit code: $status" 1>&2
	exit 16
    fi
done

# form .auth.json style semantic table .h files
#
if [[ $V_FLAG -ge 1 ]]; then
    echo "$0: debug[1]: form .auth.json style semantic table .h files from $AUTH_DIR" 1>&2
fi
find "$AUTH_DIR" -type f -name '*.json' -print 2>/dev/null | while read -r path; do

    # determine filenames
    #
    file=$(basename "$path")
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: processing .auth.json style file: $file" 1>&2
    fi
    out="$REF_DIR/$file.h"
    if [[ $V_FLAG -ge 3 ]]; then
	echo "$0: debug[3]: will try to form: $out" 1>&2
    fi

    # form output file
    #
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: about to execute: $JSEMCGEN_SH -j $JSEMTBLGEN -N sem_auth -P chk -I -- $path . . . $TMP_FILE" 1>&2
    fi
    "$JSEMCGEN_SH" -j "$JSEMTBLGEN" -N sem_auth -P chk -I -- "$path" . . . "$TMP_FILE"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: $JSEMCGEN_SH -j $JSEMTBLGEN -N sem_auth -P chk -I -- $path . . . $TMP_FILE  exit code: $status" 1>&2
	exit 17
    fi
    if [[ $V_FLAG -ge 5 ]]; then
	echo "$0: debug[5]: about to execute: cat $AUTH_HEAD_H $TMP_FILE $AUTH_TAIL_H > $out" 1>&2
    fi
    cat "$AUTH_HEAD_H" "$TMP_FILE" "$AUTH_TAIL_H" > "$out"
    status="$?"
    if [[ $status -ne 0 ]]; then
	echo "$0: ERROR: cat $AUTH_HEAD_H $TMP_FILE $AUTH_TAIL_H > $out exit code: $status" 1>&2
	exit 18
    fi
done

# All Done!!! All Done!!! -- Jessica Noll, Age 2
#
rm -f "$TMP_FILE"
exit 0
