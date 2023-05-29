#!/usr/bin/env bash

echo "Welcome to the IOCCC Soup Kitchen!" 1>&2
echo 1>&2

while :; do
    echo "(0) Cook soup" 1>&2
    echo "(1) Eat soup" 1>&2
    echo "(2) Quit" 1>&2
    echo 1>&2

    read -r -p "Make selection: " selection
    echo 1>&2

    case "$selection" in
	0) make soup;
	    ;;
	1) make eat;
	    ;;
	2) exit 0
	    ;;
	*) exit 1
    esac
    echo 1>&2
done
    

