# Official IOCCC submission toolkit

*NOTE*: This code is currently under alpha-test.

## HOW TO HELP TEST THIS REPO

**IMPORTANT NOTE**: we currently are **NOT** _yet ready for this_. We will remove this
note once we are ready.

We first want to thank each and every person who helps us further test this repo
under different platforms.

In order to help you help us, we provide the following information.

### Running the test suite

Perhaps the most important thing you can do for us is run the `bug_report.sh`
script with all information like:

```sh
make bug_report
```

This tool will run an exhaustive list of checks on your system, including the
entire test suite, writing it to both stdout and a log file. If there are any
issues found we encourage you (and thank you!) to post the entire log at the
[GitHub issues page](https://github.com/ioccc-src/mkiocccentry/issues). Of
course if it does not find any issues it does not necessarily mean there are no
issues so you're certainly welcome to report other issues and are encouraged to
do so.

If the script does not report any issues you may delete the file safely (it will
tell you the log file name). Alternatively you can run:

```sh
./bug_report -x
```

which will delete the log file for you if no issues are found.

### Static analysis and dynamic analysis

If you wish to run static analysis you may wish to see the file
[test_ioccc/static_analysis.md](test_ioccc/static_analysis.md). For help on
running with valgrind you may see the document
[test_ioccc/dynamic_analysis.md](test_ioccc/dynamic_analysis.md).


### Reviewing the tools

For a list of tools that you may wish to look at in more detail, see the below
list. 

If at any stage you feel like you have an improvement you may open a new issue
at the [GitHub issues page](https://github.com/ioccc-src/mkiocccentry/issues) or
you may alternatively offer a fix and open a pull request.

Finally we thank you once again in helping to make the IOCCC toolkit even better
in order to improve the IOCCC itself!


## The mkiocccentry toolkit


### `mkiocccentry`

Form an **IOCCC** entry as an XZ compressed tarball file.

For examples and more information, try:


```sh
man ./soup/man/man1/mkiocccentry.1
```


### `iocccsize`

The official **IOCCC** entry Rule 2b size tool.

This code is based on code by *@SirWumpus* (**Anthony Howe**):

[See @SirWumpus's iocccsize repo](https://github.com/SirWumpus/iocccsize)

For more information and examples, try:

```sh
man ./soup/man/man1/iocccsize.1
```

NOTE: After doing a `make all`, this tool may be found as: `./iocccsize`.


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

NOTE: After doing a `make all`, this tool may be found as: `./txzchk`.

###  `chkentry`

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
man ./soup/man/man1/chkentry.1
```

NOTE: After doing a `make all`, this tool may be found as: `./chkentry`.


###  `fnamchk`

The official **IOCCC** XZ compressed tarball filename sanity checker tool.

For more information and examples, try:

```sh
man ./test_ioccc/man/man1/fnamchk.1
```

NOTE: After doing a `make all`, this tool may be found as: `./test_ioccc/fnamchk`.


### `bug_report.sh`

Run a series of tests, collecting system information in the process, to help report bugs and issues.
Without any arguments, this tool produces a bug report file of the form:

```
bug-report.YYYYMMDD.HHMMSS.txt
```

This bug report file is intended to be uploaded to a [mkiocccentry repo related bug report](https://github.com/ioccc-src/mkiocccentry/issues).

This tool was co-developed in 2022 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

For more information and examples, try:

```sh
man ./soup/man/man1/bug_report.1
```

NOTE: This tool may be found as: `./bug_report.sh`.



## HOW TO ENTER THE IOCCC

0) check to see of the IOCCC is open

Check the [status.json](https://www.ioccc.org/status.json) URL and
check the [IOCCC news](https://www.ioccc.org/index.html#news) to
see of the IOCCC is open.

1) register for the IOCCC

```
XXX - instructions TBD - XXX
```

2) obtain the latest mkiocccentry toolkit

If you do not have an mkiocccentry tool directory:

```sh
cd some_directory
git clone git@github.com:ioccc-src/mkiocccentry.git
cd mkiocccentry
```

If you already have an mkiocccentry tool directory:

```sh
cd mkiocccentry
git fetch
git rebase
```

3) Make the mkiocccentry toolkit

```sh
make clobber all
```

4) run the mkiocccentry tool to form your entry tarball

```sh
./mkiocccentry work_dir prog.c Makefile remarks.md [file ...]
```

where:

```
work_dir	directory where the entry directory and tarball are formed
prog.c		path to the C source for your entry


Makefile	Makefile to build (make all) and cleanup (make clean & make clobber)

remarks.md	Remarks about your entry in markdown format
		NOTE: See the [markdown syntax[(https://www.markdownguide.org/basic-syntax) guide.

[file ...]	extra data files to include with your entry
```

NOTE: It is *NOT* necessary to install to use these tools as you can run
these tools from the top of the _mkiocccentry repo_ directory just fine.

5) upload your entry to the IOCCC submit server

```
XXX - instructions TBD - XXX
```


