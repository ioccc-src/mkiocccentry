#!/usr/bin/env bash
#
# A simple mkiocccentry test, or an example of how
# to automate the process!

# working locations - adjust as needed
#
work_dir="test_work"
src_dir="test_src"

# be sure the working locations exist
#
mkdir -p -- "${work_dir}" "${src_dir}"
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: FATAL: error in creating working dirs: mkdir -p -- ${work_dir} ${src_dir}" 1>&2
    exit 250
fi

# clean out the work_dir area
work_dir_esc="${work_dir}"
test "${work_dir:0:1}" = "-" && work_dir_esc=./"${work_dir}"
find "${work_dir_esc}" -mindepth 1 -depth -delete

# Form entries that are unlikely to win the IOCCC :-)
#
test -f "${src_dir}"/prog.c || {
cat >"${src_dir}"/prog.c <<"EOF"
#include <stdio.h>
int main() { puts("Hello, World!"); }
EOF
}
rm -f "${src_dir}"/empty.c
# shellcheck disable=SC2188
> "${src_dir}"/empty.c

# fake some required files
#
test -f "${src_dir}"/Makefile || cat Makefile.example >"${src_dir}"/Makefile
test -f "${src_dir}"/remarks.md || cat README.md >"${src_dir}"/remarks.md
test -f "${src_dir}"/extra1 || echo "123" >"${src_dir}"/extra1
test -f "${src_dir}"/extra2 || echo "456" >"${src_dir}"/extra2

# Answers as of mkiocccentry version: v0.33 2022-02-03
answers() {
cat <<"EOF"
test
0
title-for-entry0
abstract for entry #0
5
author0 middle0 last0
AU
user0@example.com
https://host0.example.com/index.html
@twitter0
@github0
an affiliation for #0 author
author0-last0
author1 middle1a middle1b last1
UK






author2 middle2 last2
US
user2@example.com
http://host2.example.com/index.html
@twitter2
@github2
an affiliation for #2 author
author2-last2
author3 middle3 last3
AU
user0@example.com
https://host0.example.com/index.html
@twitter3
@github3
an affiliation for #3 author
author3-last3
author4 middle4 last4
AU
user0@example.com
https://host0.example.com/index.html
@twitter4
@github4
an affiliation for #4 author
author4-last4
ANSWERS_EOF
EOF
}
rm -f answers.txt
# Retrieve the answers version from mkiocccentry.c and write to answers file:
grep -E '^#define MKIOCCCENTRY_ANSWERS_VER' mkiocccentry.c | cut -d' ' -f3 | sed 's/"//g' >answers.txt
# Append answers + EOF marker
answers >>answers.txt

# run the test, looking for an exit
#
yes | ./mkiocccentry -i answers.txt -- "${work_dir}" "${src_dir}"/{prog.c,Makefile,remarks.md,extra1,extra2}
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: FATAL: /mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi

# delete the work directory for next test
find "${work_dir_esc}" -mindepth 1 -depth -delete
# test empty prog.c, ignoring the warning about it
yes | ./mkiocccentry -W -i answers.txt -- "${work_dir}" "${src_dir}"/{empty.c,Makefile,remarks.md,extra1,extra2}
status=$?
if [[ ${status} -ne 0 ]]; then
    echo "$0: FATAL: /mkiocccentry non-zero exit code: $status" 1>&2
    exit "${status}"
fi



# All Done!!! -- Jessica Noll, Age 2
#
exit 0
