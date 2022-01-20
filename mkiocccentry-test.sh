#!/usr/bin/env bash
#
# A simple mkioccccentry test,
# or an example of how to automate the process! 

work_dir="test_work"
src_dir="test_src"

mkdir -p "$work_dir" "$src_dir"
rm -rf "$work_dir"/test-* "$work_dir"/entry.*.txz

cat > "$src_dir"/prog.c << "EOF"
#include <stdio.h>
int main() { puts("Hello, World!"); }
EOF

cat Makefile.example > "$src_dir"/Makefile
cat README.md > "$src_dir"/remarks.md
echo "123" > "$src_dir"/extra1
echo "456" > "$src_dir"/extra2

answers() {
cat << "EOF"
test
0
title
abstract
1
author name
aa
yes
test@example.com
http://example.com/
@twitter
@github
affiliation
yes
yes
EOF
}

answers | ./mkiocccentry "$work_dir" "$src_dir"/{prog.c,Makefile,remarks.md,extra*}
