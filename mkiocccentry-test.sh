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

# fake some required files
#
test -f "$src_dir"/Makefile || cat Makefile.example > "$src_dir"/Makefile
test -f "$src_dir"/remarks.md || cat README.md > "$src_dir"/remarks.md
test -f "$src_dir"/extra1 || echo "123" > "$src_dir"/extra1
test -f "$src_dir"/extra2 || echo "456" > "$src_dir"/extra2

# Answers as of mkiocccentry version: v0.33 2022-02-03
# The first line retrieves the answers version from mkiocccentry.c
answers() {
echo $(grep -E '^#define MKIOCCCENTRY_ANSWERS_VER' mkiocccentry.c|awk '{print $3  ;}'|sed 's/"//g')
cat << "EOF"
test
0
title
abstract
1
author name
CC
test@example.com
https://example.com/index.html
@twitter
@github
an affiliation
ANSWERS_EOF
EOF
}
rm -f answers.txt
answers > answers.txt

# run the test, looking for an exit
#
yes | ./mkiocccentry -i answers.txt -- "$work_dir" "$src_dir"/{prog.c,Makefile,remarks.md,extra1,extra2}
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: FATAL: /mkiocccentry non-zero exit code: $status" 1>&2
    exit "$status"
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
