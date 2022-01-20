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
mkdir -p "$work_dir" "$src_dir"
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: FATAL: error in crearing working dirs: mkdir -p $work_dir $src_dir" 1>&2
    exit 250
fi

# cleanout the under work_dir area
#
find "$work_dir" -mindepth 1 -depth -delete

# Form an entry that is unlikely to win the IOCCC :-)
#
cat > "$src_dir"/prog.c << "EOF"
#include <stdio.h>
int main() { puts("Hello, World!"); }
EOF

# fake some requireed files
#
cat Makefile.example > "$src_dir"/Makefile
cat README.md > "$src_dir"/remarks.md
echo "123" > "$src_dir"/extra1
echo "456" > "$src_dir"/extra2

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
yes
test@example.com
http://example.com/index.html
@twitter
@github
affiliation
Y
y
EOF
}

# run the test, looking for an exit
#
answers | ./mkiocccentry "$work_dir" "$src_dir"/{prog.c,Makefile,remarks.md,extra1,extra2}
status="$?"
if [[ $status -ne 0 ]]; then
    echo "$0: FATAL: /mkiocccentry non-zero exit code: $status" 1>&2
    exit "$status"
fi

# All Done!!! -- Jessica Noll, Age 2
#
exit 0
