# soup/ - the mkiocccentry kitchen :-)

If you wish for an overview of the tools in this repo, click
[here](#kitchen-appliances). Otherwise, if you wish to work on this repo, please
see the below list.

# Requirements for repo maintainers/developers

**IMPORTANT NOTE:** if you are submitting to the IOCCC, you do **NOT** need to
meet the requirements below!

For those who need to develop and maintain this repo, **in addition to the
list of requirements in
[test_ioccc/README.md](https://github.com/ioccc-src/mkiocccentry/blob/master/test_ioccc/README.md)**,
you will need:

* `bison(1)` version 3.8.2 or later[^0]
* `flex(1)` version 2.6.4 or later[^0]
* `independ`, version 1.00 2022-12-27 or later (see [independ
repo](https://github.com/lcn2/independ)).
* `shellcheck(1)` version 0.10.0 or later (see [shellcheck GitHub
repo](https://github.com/koalaman/shellcheck.net)).
* `seqcexit(1)` version 1.12 2022-11-09 or later (see [seqcexit
repo](https://github.com/lcn2/seqcexit)).
* `picky` version 2.6 or later (see [picky
repo](https://github.com/lcn2/picky)).
* a sense of humour :-)

You do **NOT** need to clone, compile and install the code from the `jparse`,
`dbg` or `dyn_array` repos (see further down in this file) in order to work on
this repo or use the code in this repo.

[^0]: With respect to `bison(1)` and `flex(1)`: strictly speaking, you do
**NOT** need the required versions of flex/bison for the JSON parser `jparse`,
or even `bison(1)` and `flex(1)` at all, as only the maintainers of the [jparse
repo](https://github.com/xexyl/jparse/), and only when modifying certain files,
need these tools. This is because this repo, which has a clone of the jparse
repo, has backup files of the generated code, for those who do not have a recent
enough version; and since only maintainers of the jparse repo will create the
backup files (when necessary), one does not need to have the tools to maintain
this repo.

## **IMPORTANT NOTES** for those who wish to make a pull request

If you wish to make a pull request, please run from the top level directory:

```sh
make prep
```

If any problems are encountered **PLEASE** fix them before committing and
opening a pull request.

Please do **NOT** open a pull request for the `iocccsize(1)` tool
or the `jparse/` subdirectory or the `dbg/` subdirectory or the `dyn_array/`
subdirectory as these come from elsewhere.

<div id="kitchen-appliances"></div>
## Reviewing the <del>appliances</del> applications in the <del>kitchen</del> repo :-)

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


```sh
man ./soup/man/man1/mkiocccentry.1
```


### `iocccsize`

The official **IOCCC** submission Rule 2b size tool.

This code is based on code by *@SirWumpus* (**Anthony Howe**):

[See @SirWumpus's iocccsize repo](https://github.com/SirWumpus/iocccsize)

For more information and examples, try:

```sh
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

```sh
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

This tool was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\


For more information and examples, try:

```sh
man ./soup/man/man1/chkentry.1
```

**NOTE**: After doing a `make all`, this tool may be found as: `./chkentry`.


###  `fnamchk`

The official **IOCCC** XZ compressed tarball filename sanity checker tool.

For more information and examples, try:

```sh
man ./test_ioccc/man/man1/fnamchk.1
```

**NOTE**: After doing a `make all`, this tool may be found as: `./test_ioccc/fnamchk`.

This tool was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

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

```sh
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

This tool was co-developed in 2023 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

For more information and examples, try:

```sh
man ./soup/man/man1/location.1
```

**NOTE**: After doing a `make all`, this tool may be found as: `./soup/location`.


### `jparse`: JSON parser

The mkiocccentry toolkit creates and validates JSON files. The JSON parser
`jparse`, which is cloned into this repo, was co-developed in 2022-2025 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

See the [jparse repo](https://github.com/xexyl/jparse/) for the original.

**NOTE**: you do **NOT** need a copy of this repo installed in order to use this
toolkit as we have a clone of it here. It is even possible that there are
divergences as once this repo is in a code freeze state, unless a critical bug
is fixed, any changes in the repo will not be synced to this repo. In fact, even
if you do install the code from that repo, compiling the code in this repo will
**NOT** use that copy!

You do **NOT** need to install this in order to compile this toolkit!

### `dbg`: the debug library

The mkiocccentry toolkit uses the `dbg(3)` library which comes from the [dbg
repo](https://github.com/lcn2/dbg).

You do **NOT** need to install this in order to compile this toolkit!

### `dyn_array`: the dynamic array library

The mkiocccentry toolkit uses the `dyn_array(3)` library which comes from the
[dyn_array repo](https://github.com/lcn2/dyn_array).

You do **NOT** need to install this in order to compile this toolkit!
