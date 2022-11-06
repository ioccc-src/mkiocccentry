#!/usr/bin/env bash
#
# fmt_depend - format the output of cc .. -MM ..
#
# Different C compilers output the -MM dependency lines in
# slightly different formats.  The result is that make depend
# will produce different Makefiles on different systems.
#
# This tool will take in stdin, C compilers output the -MM
# dependency lines and produce a system independent make
# depend output.
#
# Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided that
# the above copyright, this permission notice and text this comment, and the
# disclaimer below appear in all of the following:
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
export FMT_DEPEND_VERSION="1.2 2022-11-05"

export USAGE="usage: $0 [-h] [-V] [-v level]

    -h		print help and exit
    -V		print version and exit
    -v		set verbosity level for this script (def level: 0)

Exit codes:
     0	 all is well
     2	 help message printed
     3	 invalid command line, invalid option or option missing an argument
 >= 10	 internal error

$0 version: $FMT_DEPEND_VERSION"

# parse args
#
export V_FLAG="0"
while getopts :hVv: flag; do
    case "$flag" in
    h) echo "$USAGE" 1>&2
       exit 2
       ;;
    V) echo "$FMT_DEPEND_VERSION" 1>&2
       exit 2
       ;;
    v) V_FLAG="$OPTARG";
       ;;
    \?) echo "$0: ERROR: invalid option: -$OPTARG" 1>&2
       exit 3
       ;;
    :) echo "$0: ERROR: option -$OPTARG requires an argument" 1>&2
       exit 3
       ;;
   *)
       ;;
    esac
done

# convert newlines tabs space and backslashes into a single space
#
tr -s '\n\t \\' ' ' |

# print a newline before make target (" foo:") in the line middle
# print a newline before make target ("^foo:") at the start of a line
#
sed -e 's/ \([^: ][^: ]*:\)/\n\1/g' \
    -e 's/^\([^: ][^: ]*:\)/\n\1/g' |

# print a blank line before and after non-empty lines
#
sed -e 's/^\(..*\)$/\1\n/' |

# By now, each make target and their dependencies are in a single long line
# that is surrounded by blank lines.  We use fmt to treat each
# make dependency line as a paragraph and format that paragraph
# (as whitespace delimited words) to a given width.
#
fmt -s -w 100 |

# By now, make targets are word formatted paragraph of limited line length.
# We indent the 2nd and subsequent lines of those paragraphs with a tab.
#
sed -e 's/^\([^:][^:]*\)$/\t\1/' |

# By now, make dependencies are blank line separated with 2nd and subsequent
# lines indented by a tab.
#
# print lines with a trailing \ unless they
# are preceded by a blank line, remove blank lines
#
awk '
    # initialize line to empty
    #
    BEGIN {
	line="";
    }

    # normal line processing
    #
    {
	# case: our current line is empty
	if (length($0) == 0) {

	    # case: line buffer is not empty
	    #
	    if (length(line) > 0) {

		# print line buffer followed by newline
		print line;
	    }

	    # clear the line buffer now that we printed it
	    line="";

       # case: our current line has text (not empty)
       #
       } else {

	   # case: line buffer is not empty
	   #
	   if (length(line) > 0) {

	       # print the line buffer followed by a space, blackslash and newline
	       print line " \\";
	   }

	   # store the current non-empty into the line buffer
	   line = $0;
       }
    }

    # on EOF / end of input
    #
    END {

	# case: line buffer is not empty
	#
	if (length(line) > 0) {

	    # print line buffer followed by newline
	    print line;
	}
    }'
