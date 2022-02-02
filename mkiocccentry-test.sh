#!/usr/bin/env bash
#
# A simple mkioccccentry test,
# or an example of how to automate the process!

# working locations - adjust as needed
#
work_dir="test_work"
src_dir="test_src"

# be sure the working locations exist
#
mkdir -p -- "$work_dir" "$src_dir"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: FATAL: error in crearing working dirs: mkdir -p -- $work_dir $src_dir" 1>&2
    exit 250
fi

# cleanout the under work_dir area
#
work_dir_esc="$work_dir"
test "${work_dir:0:1}" = "-" && work_dir_esc=./"$work_dir"
find "$work_dir_esc" -mindepth 1 -depth -delete

# Form an entry that is unlikely to win the IOCCC :-)
#
test -f "$src_dir"/prog.c || {
cat > "$src_dir"/prog.c << "EOF"
#include <stdio.h>
int main() { puts("Hello, World!"); }
EOF
}

echo -n > "$src_dir"/empty_prog.c

# fake some required files
#
test -f "$src_dir"/Makefile || cat Makefile.example > "$src_dir"/Makefile
test -f "$src_dir"/remarks.md || cat README.md > "$src_dir"/remarks.md
test -f "$src_dir"/extra1 || echo "123" > "$src_dir"/extra1
test -f "$src_dir"/extra2 || echo "456" > "$src_dir"/extra2

# Answers as of mkiocccentry version: 0.29 2022-01-20
#
answers() {
cat << "EOF"
test
0
title
abstract
1
author name
cc
y
test@example.com
http://example.com/index.html
@twitter
@github
affiliation
y
y
EOF
}

rm -f answers.txt

# run the test, looking for an exit
#
answers | ./mkiocccentry -a answers.txt -- "$work_dir" "$src_dir"/{prog.c,Makefile,remarks.md,extra1,extra2}
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: FATAL: ./mkiocccentry non-zero exit code: $status" 1>&2
    exit "$status"
fi

# cleanout the under work_dir area, again
#
find "$work_dir_esc" -mindepth 1 -depth -delete

yes | ./mkiocccentry -i answers.txt -- "$work_dir" "$src_dir"/{empty_prog.c,Makefile,remarks.md,extra1,extra2}
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: FATAL: /mkiocccentry non-zero exit code: $status" 1>&2
    exit "$status"
fi

# cleanout the under work_dir area, again
#
find "$work_dir_esc" -mindepth 1 -depth -delete

# Note that as of commit c9b31d416c82932daa0e6ec4dd8d094d2d3edb25 redirecting
# stdin (or cat answers | ./mkiocccentry ...) no longer works due to the version
# tag and EOF marker so we have to use -i answers.txt here too.
yes | ./mkiocccentry -i answers.txt -- "$work_dir" "$src_dir"/{empty_prog.c,Makefile,remarks.md,extra1,extra2}
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: FATAL: /mkiocccentry non-zero exit code: $status" 1>&2
    exit "$status"
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
