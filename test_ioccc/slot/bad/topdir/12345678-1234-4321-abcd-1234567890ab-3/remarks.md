# Official IOCCC submission toolkit

*NOTE*: This code is currently under alpha-test.


## `mkiocccentry`

Form an **IOCCC** entry as an XZ compressed tarball file.

For examples and more information, try:


```sh
    man ./man/mkiocccentry.1
```


## `iocccsize`

The official **IOCCC** entry Rule 2b size tool.

This code is based on code by *@SirWumpus* (**Anthony Howe**):

[See @SirWumpus's iocccsize repo](https://github.com/SirWumpus/iocccsize)

For more information and examples, try:


```sh
    man ./man/iocccsize.1
```


## `txzchk`

The official **IOCCC** tarball validation tool.

It is invoked by `mkiocccentry`; `txzchk` in turn uses `fnamchk` to make sure
that the tarball was correctly named and formed (i.e. the `mkiocccentry` tool
was used).

`txzchk` verifies that the tarball does not have any feathers stuck in it (i.e.
the tarball conforms to the IOCCC tarball rules). Invoked by `mkiocccentry`;
`txzchk` in turn uses `fnamchk` to make sure that the tarball was correctly named
and formed. In other words `txzchk` makes sure that the `mkiocccentry` tool was
used and there was no screwing around with the resultant tarball.

`txzchk` was written in 2022 by *@xexyl* (**Cody Boone Ferguson**). See
[https://xexyl.net](https://xexyl.net) and
[https://ioccc.xexyl.net](https://ioccc.xexyl.net).

For more information and examples, try:


```sh
    man ./man/txzchk.1
```


##  `fnamchk`

The official **IOCCC** XZ compressed tarball filename sanity checker tool.

For more information and examples, try:


```sh
    man ./man/fnamchk.1
```


##  `chkentry`

The official **IOCCC** `.info.json` and `.auth.json` sanity checker tool.
Invoked by `mkiocccentry` after the `.info.json` and `.auth.json` files have
been created, it will attempt to validate the files. If it does not successfully
validate the files there is a mismatch between what is expected and what is
actually there and `mkiocccentry` will fail.

As a stand-alone tool it will report whether the files are validly formed.

This tool was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\


For more information and examples, try:


```sh
    man ./man/chkentry.1
```


##  `jparse`

The official **IOCCC** `JSON` parser written in C via `flex(1)` and `bison(1)`.

It takes a block of memory from either a file (stdin or a text file) or a string
(via `-s` option) and parses it as JSON, reporting if it is validly formed JSON
or not.

This tool was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

For more information and examples, try:

```sh
    man ./man/jparse.1
```


## `run_bison.sh`

This script attempts to find a recent enough version of `bison(1)` to generate
code from the `*.y` files in the repo. If it fails and `-o` is passed to the
script then it is an error; else if it fails and `-o` is not passed to the
script the backup files will be used instead.

This script is part of the creation of the JSON parser `jparse`.

For more information and examples, try:

```sh
    man ./man/run_bison.1
```


## `run_flex.sh`

This script attempts to find a recent enough version of `flex(1)` to generate
code from the `*.l` files in the repo. If it fails and `-o` is passed to the
script then it is an error; else if it fails and `-o` is not passed to the
script the backup files will be used instead.

This script is part of the creation of the JSON parser `jparse`.

For more information and examples, try:

```sh
    man ./man/run_flex.1
```
