#!/usr/bin/env bash

# This script was written in 2023 by:
#
#	@xexyl
#	https://xexyl.net		Cody Boone Ferguson
#	https://ioccc.xexyl.net
#
# "Because sometimes even the IOCCC Judges need some help." :-)
#
# The IOCCC soup is a joke between Landon and Cody after Cody misread, being
# very tired at the time, one word as 'soup', when discussing decreasing the
# number of files in the top level directory (thus the name of this directory).


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


echo "Welcome to the IOCCC Soup Kitchen!" 1>&2
echo 1>&2

while :; do
    echo "(0) Cook soup" 1>&2
    echo "(1) Eat soup" 1>&2
    echo "(Q) Quit" 1>&2
    echo 1>&2

    read -r -p "Make selection: " selection
    echo 1>&2

    case "$selection" in
	0) make soup
	    ;;
	1) make eat
	    ;;
	q|2|*) exit 0
    esac
    echo 1>&2
done
