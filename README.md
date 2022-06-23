# Official IOCCC submission toolkit

*NOTE*: This code is currently under alpha-test.


## `mkiocccentry`

Form an **IOCCC** entry as an XZ compressed tarball file.

For examples and more information, try:


	    man ./mkiocccentry.1


## `iocccsize`

The official **IOCCC** entry Rule 2b size tool.

This code is based on code by *@SirWumpus* (**Anthony Howe**):

[See @SirWumpus's iocccsize repo](https://github.com/SirWumpus/iocccsize)

For more information and examples, try:


	    man ./iocccsize.1


## `txzchk`

The official **IOCCC** tarball validation checker.

It is invoked by `mkiocccentry`; `txzchk` in turn uses `fnamchk` to make sure
that the tarball was correctly named and formed (i.e. the `mkiocccentry` tool
was used).


This tool was written in 2022 by *@xexyl* (**Cody Boone Ferguson**). See
[https://xexyl.net](https://xexyl.net) and
[https://ioccc.xexyl.net](https://ioccc.xexyl.net).

For more information and examples, try:


	    man ./txzchk.1


##  `fnamchk`

The official **IOCCC** XZ compressed tarball filename sanity checker tool.

For more information and examples, try:


	    man ./fnamechk.1


##  `chkentry`

The official **IOCCC** `.info.json` and `.author.json` sanity checker tool.
Invoked by `mkiocccentry` after the `.info.json` and `.author.json` files have
been created, it will attempt to validate the files. If it does not successfully
validate the files there is a mismatch between what is expected and what is
actually there and `mkiocccentry` will fail.

As a stand-alone tool it will report whether the files are validly formed.

*NOTE*: This tool is **very much a work in progress** and as of 10 March 2022 it
was decided that the parsing should be done via `flex(1)` and `bison(1)` which
is being worked on in `jparse` (see below).

For more information and examples, try:


	    man ./chkentry.1



##  `jparse`

The official **IOCCC** `JSON` parser written in C via `flex(1)` and `bison(1)`.

It takes a block of memory from either a file (stdin or a text file) or a string
(via `-s` option) and parses it as JSON, reporting if it is validly formed JSON
or not.

As of 26 May 2022 it does not yet create a parse tree for other tools like the
`chkinfo` and `chkauth` tools. Some things still have to be worked out. Once
this is done the other two tools can be finished.

*NOTE*: At the risk of stating the obvious this tool is **very much a work in
progress**.  Good progress is being made but more thought and time is still
required.

For more information and examples, try:


	    man ./jparse.1


## `run_bison.sh`

This script attempts to find a recent enough version of `bison(1)` to generate
code from the `*.y` files in the repo. If it fails and `-o` is passed to the
script then it is an error; else if it fails and `-o` is not passed to the
script the backup files will be used instead.

This script is part of the creation of the JSON parser `jparse`.

## `run_flex.sh`

This script attempts to find a recent enough version of `flex(1)` to generate
code from the `*.l` files in the repo. If it fails and `-o` is passed to the
script then it is an error; else if it fails and `-o` is not passed to the
script the backup files will be used instead.

This script is part of the creation of the JSON parser `jparse`.

