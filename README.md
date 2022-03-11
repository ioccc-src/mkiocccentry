# Official IOCCC submission toolkit

*NOTE*: This code is currently under alpha-test.


## mkiocccentry

Form an **IOCCC** entry as a compressed tarball file.

For examples and more information, try:


	    man ./mkiocccentry.1


## iocccsize

The official **IOCCC** entry Rule 2b size tool.

This code is based on code by *@SirWumpus* (**Anthony Howe**):

[See @SirWumpus's iocccsize repo](https://github.com/SirWumpus/iocccsize)

For more information and examples, try:


	    man ./iocccsize.1


## txzchk

The official **IOCCC** tarball validation checker.

It is invoked by `mkiocccentry`; txzchk in turn uses fnamchk to make sure that the
tarball was correctly named and formed (i.e. the `mkiocccentry` tool was used).


This tool was written in 2022 by *@xexyl* (**Cody Boone Ferguson**). See
[https://xexyl.net](https://xexyl.net) and
[https://ioccc.xexyl.net](https://ioccc.xexyl.net).


For more information and examples, try:


	    man ./txzchk.1


##  fnamchk

The official **IOCCC** compressed tarball filename sanity checker tool.

For more information and examples, try:


	    man ./fnamechk.1


##  jinfochk

The official **IOCCC** `.info.json` sanity checker tool. Invoked by `mkiocccentry`
after the `.info.json` file has been created but prior to forming the
`.author.json` file, validating it with `jauthchk` and then forming the tarball.


*NOTE*: This tool and `jauthchk` is (and are) **very much a work(s) in
progress** and as of *10 March 2022* it was decided that the parsing should be
done via `flex(1)` and `bison(1)` which will require some time and thought. In
time the two tools will be merged into one which can parse one or both of
`.info.json` and/or `.author.json`. This is because some fields MUST be the same
value in both files.

Additionally there will likely be a `jparse` tool that will take a block of
memory from either stdin or a file and attempt to parse it as json.


For more information and examples, try:


	    man ./jinfochk.1


##  jauthchk

The official **IOCCC** `.author.json` sanity checker tool. Invoked by
`mkiocccentry` after the `.author.json` file has been created but prior to
forming the tarball. The `.author.json` file is not formed if the `jinfochk`
tool fails to validate the `.info.json` file.


*NOTE*: This tool and `jinfochk` is (and are) **very much a work(s) in
progress** and as of *10 March 2022* it was decided that the parsing should be
done via `flex(1)` and `bison(1)` which will require some time and thought. In
time the two tools will be merged into one which can parse one or both of
`.author.json` and/or `.info.json.` This is because some fields MUST be the same
value in both files.

Additionally there will likely be a `jparse` tool that will take a block of
memory from either stdin or a file and attempt to parse it as json.


For more information and examples, try:


	    man ./jauthchk.1


