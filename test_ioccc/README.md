# Official IOCCC submission toolkit

Below are some details on the test suite of the repo. We do not give all details
but we at least show you a few things. If you wish for an overview of the tools
in this repo, click [here](#the-mkiocccentry-toolkit).

## Running the test suite

An exhaustive script is the `bug_report.sh` script. This is not strictly
necessary to use unless you are reporting a bug but it runs an exhaustive list
of checks on your system and it runs the entire test suite, writing to both
stdout and a lot file.

You are welcome to do this even if you do not have a problem. Should you find a
problem we welcome you opening an issue, or if you have a fix, opening a pull
request.

To run the script:

```sh
make bug_report
```

If there are any issues found we encourage you (and thank you!) to post the
entire log at the [GitHub issues
page](https://github.com/ioccc-src/mkiocccentry/issues). Of course if it does
not find any issues it does not necessarily mean there are no issues so you're
certainly welcome to report other issues and are encouraged to do so.

If the script does not report any issues you may delete the file safely (it will
tell you the log file name). Alternatively you can run:

``` <!---sh-->
./bug_report -x
```

which will delete the log file for you if no issues are found.

The [FAQ](https://github.com/ioccc-src/mkiocccentry/blob/master/FAQ.md) gives a
bit more information on this.


## Static analysis and dynamic analysis

If you wish to run static analysis you may wish to see the file
[static_analysis.md](static_analysis.md). For help on
running with valgrind you may see the document
[dynamic_analysis.md](dynamic_analysis.md).

Note that doing these are NOT important now, but if you wish to do so for some
reason, you are welcome to do so.

## Making pull requests

If you do wish to make a pull request, please make sure that running:

```sh
make prep
```

works fine. You should **NOT** need to run `make release` as that force rebuilds
the JSON parser, which is only necessary for those working on the [jparse
repo](https://github.com/xexyl/jparse/) and only if one modifies specific files.


## Reviewing the tools

For a list of tools that you may wish to look at in more detail, see the below
list.

If at any stage you feel like you have an improvement you may open a new issue
at the [GitHub issues page](https://github.com/ioccc-src/mkiocccentry/issues) or
you may alternatively offer a fix and open a pull request.

Finally we thank you once again in helping to make the IOCCC toolkit even better
in order to improve the IOCCC itself!


## The mkiocccentry toolkit


### `mkiocccentry`

Form an **IOCCC** submission as an XZ compressed tarball file.

For examples and more information, try:


``` <!---sh-->
man ./soup/man/man1/mkiocccentry.1
```


### `iocccsize`

The official **IOCCC** submission Rule 2b size tool.

This code is based on code by *@SirWumpus* (**Anthony Howe**):

[See @SirWumpus's iocccsize repo](https://github.com/SirWumpus/iocccsize)

For more information and examples, try:

``` <!---sh-->
man ./soup/man/man1/iocccsize.1
```

**NOTE**: After doing a `make all`, this tool may be found as: `./iocccsize`.


### `txzchk`

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

``` <!---sh-->
man ./soup/man/man1/txzchk.1
```

**NOTE**: After doing a `make all`, this tool may be found as: `./txzchk`.


### `chkentry`

The official **IOCCC** `.info.json` and `.auth.json` sanity checker tool.
Invoked by `mkiocccentry` on the `.info.json` file after it has been created and
invoked on the `.auth.json` file after it has been created, it will attempt to
validate the files. If there is any validation issue it is an error as there is
a mismatch between what is expected and what is actually there; in this case
`mkiocccentry` will fail.

As a stand-alone tool it will report whether the files are validly formed.

This tool was co-developed in 2022-2024 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\


For more information and examples, try:

``` <!---sh-->
man ./soup/man/man1/chkentry.1
```

**NOTE**: After doing a `make all`, this tool may be found as: `./chkentry`.


###  `fnamchk`

The official **IOCCC** XZ compressed tarball filename sanity checker tool.

For more information and examples, try:

``` <!---sh-->
man ./test_ioccc/man/man1/fnamchk.1
```

**NOTE**: After doing a `make all`, this tool may be found as: `./test_ioccc/fnamchk`.

This tool was written in 2022 by:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

with improvements for `txzchk(1)` and otherwise by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))


### `bug_report.sh`

Although we briefly mentioned this script earlier in this file, we provide
additional details here.

This script runs a series of tests, collecting system information in the
process, to help report bugs and issues.  Without any arguments, this tool
produces a bug report file of the form:

```
bug-report.YYYYMMDD.HHMMSS.txt
```

This bug report file is intended to be uploaded to a [mkiocccentry repo related
bug
report](https://github.com/ioccc-src/mkiocccentry/issues/new?assignees=&labels=bug&projects=&template=bug_report.yml&title=%5BBug%5D+%3Ctitle%3E).

This tool was written in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

with minor improvements by:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

For more information and examples, try:

``` <!---sh-->
man ./soup/man/man1/bug_report.1
```

**NOTE**: this tool may be found as: `./bug_report.sh`.

**NOTE**: this tool **MUST** be run from the mkiocccentry directory.


### `location`

The official **IOCCC** tool to look up ISO 3166 codes and location names to help
aid users in finding country codes. With the `-s` option one can search by
substring and with the `-a` option one can list all codes that match, though the `-a`
option is less useful without `-n` and is only checked for with at least one arg
specified.

This tool was developed in 2023 by:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

with improvements (`-a` and `-s` options via new re-entrant functions) by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))


For more information and examples, try:

``` <!---sh-->
man ./soup/man/man1/location.1
```

**NOTE**: After doing a `make all`, this tool may be found as: `./soup/location`.


### `jparse`: JSON parser

The mkiocccentry toolkit creates and validates JSON files. The JSON parser
`jparse`, which is cloned into this repo, was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

See the [jparse repo](https://github.com/xexyl/jparse/) for the original.

**NOTE**: you do **NOT** need a copy of this repo installed in order to use this
toolkit as we have a clone of it here. It is even possible that there are
divergences as once this repo is in a code freeze state, unless a critical bug
is fixed, any changes in the repo will not be synced to this repo.
