# Frequently Asked Questions about the `mkiocccentry` repo


<a name="toc">Table of Contents</a>

0. <a href="#download">Where can I download the mkiocccentry tool?</a>

1. <a href="#compiling">How do I compile the mkiocccentry tools?</a>

2. <a href="#package">How do I package my submission?</a>

3. <a href="#makefile">What do I do for the Makefile in my submission?</a>

4. <a href="#submitting">Can't I just submit my obfuscated C program to the judges?</a>

5. <a href="#mkiocccentry-requirements">Do I have to use mkiocccentry to package my submission?</a>

6. <a href="#installing">Do I need to install this code to use it?</a>

7. <a href="#help">How can I learn more about how to use the tools?</a>

8. <a href="#reporting-bugs">How do I report bugs or other issues?</a>

9. <a href="#how-to-help">How can I help test this repo?</a>

10. <a href="#tar">What can I do if my system's tar(1) does not support the correct options?</a>

11. <a href="#markdown">Where can I find help with formatting markdown files for my submission?</a>


## 0. <a name="download">Where can I find the mkiocccentry tool?</a>

The `mkiocccentry` tool source code is found in the
[mkiocccentry repo](https://github.com/ioccc-src/mkiocccentry).
If you have not already done so, you may download the source by using `git
clone`:


```sh
git clone git@github.com:ioccc-src/mkiocccentry.git
```

If you don't have `git` you may
[download the zip file](https://github.com/ioccc-src/mkiocccentry/archive/refs/heads/master.zip)
and then extract that file.


## 1. <a name="compiling">How do I compile the mkiocccentry tool?</a>

After downloading the repo (making sure that if you downloaded the zip file that
you unzip it first) move into the _mkiocccentry_ directory:

```sh
cd mkiocccentry
```

and compile everything from scratch:

```sh
make clobber all
```

If all is well, you will find the `mkiocccentry` executable in your
local directory.  If something went wrong, see
<a href="#reporting-bugs">how do I report bugs or other issues?</a>



## 2. <a name="package">How do I package my submission?</a>

We recommend that you use the `mkiocccentry` tool to package your submission.
If you have not already done so, download the [mkiocccentry repo](https://github.com/ioccc-src/mkiocccentry)
(instructions <a href="#download">here</a>)
and compile it
(instructions <a href="#compiling">here</a>).

From the top level directory, run the `mkiocccentry` executable:

```sh
./mkiocccentry work_dir prog.c Makefile remarks.md [file ...]
```

The _work_dir_ is the path where your submission's files will be packaged from.
The _work_dir_ **must** exist as _mkiocccentry_ will create a subdirectory under it to
package your submission. The submission directory depends on the IOCCC contestant id and
the submission number.

Something like _/tmp/ioccc_ is a good choice:

```sh
mkdir -p /tmp/ioccc
```

The _prog.c_ is the path to the main C source code for your submission.

The _Makefile_ is the path to a Makefile used to build your submission.

The _remarks.md_ is a path to a
[markdown file](https://www.markdownguide.org/basic-syntax)
that describes your submission.

If you have other optional files that belong to your submission, give them
as additional paths at the end if your `mkiocccentry` tool command line.

The `mkiocccentry` tool, by default, will ask you a series of
questions about your submission and about the authors of your submission.
Once you have answered all of the questions, the tool will form a
XZ compressed tarball, in v7 format, under the _work_dir_ directory.


## 3. <a href="makefile">What do I do for the Makefile in my submission?</a>

Although you are welcome to add additional rules, we recommend that you use the
example Makefile, [Makefile.example](Makefile.example), removing and changing
comments as appropriate, and making sure to add the correct specifics of each
rule.

XXX - make sure to link to the winner repo FAQ about the Makefile - XXX
XXX - this is not done now because of link changes and possibly an anchor - XXX
XXX - name being added to this FAQ item - XXX


## 4. <a name="submitting">Can't I just submit my obfuscated C program to the judges?</a>

No. While we appreciate your enthusiasm for wanting to show us your obfuscated
code, the [IOCCC judges](https://www.ioccc.org/judges.html) request your help by
using the `mkiocccentry` tool to package your submission.

We need your submission in the form of an XZ compressed tarball in v7 format,
along with things like your Makefile, a pair of JSON files that
help describe you and your submission, some remarks you wrote about your
submission etc. The  `mkiocccentry` tool does a lot in packaging your
submission and we kindly request that you use it.

In short, you cannot simply upload your obfuscated C program as it needs to
be in a certain form and the `mkiocccentry` tool does that.


## 5. <a name="mkiocccentry-requirements">Do I have to use mkiocccentry to package my submission?</a>

Technically you do not have to use the `mkiocccentry` tool; however, you run the
risk of having your submission rejected if what you upload to the submit server is
malformed.  Moreover, the contents of the XZ compressed tarball (in v7 format)
that you upload to the submit server must be in proper form or your submission could
be rejected. The mkiocccentry tool also creates two JSON files that are
required. Therefore we highly recommend that you use the `mkiocccentry` tools.

If you simply must package your submission yourself then we strongly recommend that
you use the `txzchk` tool to verify that the tarball you plan to upload to the
submit server is OK.  We also strongly recommend that use the `chkentry` tool to
inspect the directory that you used to form the tarball to verify that the
contents under that directory are also OK. In particular, that tool tests that
the JSON files are correct.

## 6. <a name="installing">Do I need to install this code to use it?</a>

No, installing the code in this repo is not necessary to use it. These tools
were designed to be used from the top level directory of the source, or after
installing, whichever you prefer.

You can simply execute the code from the top level directly of the source, after
compiling of course.

As `.` may not be (and probably shouldn't be) in your `$PATH`, you may need to
put _./_ before the name of a command.

For example:

```shell
./mkiocccentry -h
```

## 7. <a name="help">How can I learn more about how to use the tools?</a>

Assuming you have <a href="#download">downloaded</a> and <a
href="#compiling">compiled</a> the code you can get a quick reminder of command
options and arguments by use of the `-h` option of any tool:

For instance:

```shell
./mkiocccentry -h
./iocccsize -h
./chkentry -h
./txzchk -h
```

There are man pages that go into much more detail than the `-h` help modes. You
may use the `man(1)` command to read the man pages without having to install
them.

After compiling this code, various man pages are copied under the local `man/`
directory at the top of the source directory.

For example:

```shell
man man/man1/mkiocccentry.1
man man/man1/iocccsize.1
man man/man1/chkentry.1
man man/man1/txzchk.1
```

There are many other man pages under the `man/` directory for the curious to
read, using a similar technique.

If all that fails to satisfy your curiosity, we recommend you _"use the source,
Luke!"_ as you may find the code in this repo reasonably un-obfuscated and fairly
well commented.


## 8. <a name="reporting-bugs">How do I report bugs or other issues?</a>

Please run the following from the main directory:


```sh
make bug_report
```

and open a new issue at the
[GitHub issues page](https://github.com/ioccc-src/mkiocccentry/issues/new/choose),
making sure to give us as much information on your problem as possible and
making sure to attach the bug report file.

You may also run the `bug_report.sh` tool directly:

```sh
./bug_report.sh -v 1
```

The bug report filename is in the format:
`bug-report.$(/bin/date +%Y%m%d.%H%M%S).txt` where:


- `%Y` is the four digit year (e.g. 2023).

- `%m` is the two digit month (01..12).

- `%d` is the two digit day of the month (e.g. 30).

- `%H` is the two digit hour of the day (00..23).

- `%M` is the two digit minute of the hour of the day (00..59); and

- `%S` is the two digit second of the minute (in the range 0..60).


The script that the make rule runs, `bug_report.sh`, will tell you the name of
the file to upload.


## 9. <a name="how-to-help">How can I help test this repo?</a>

Thank you for any and all help!

Please see the
[README file](https://github.com/ioccc-src/mkiocccentry/blob/master/README.md)
for more details on what you can do to help us.


## 10. <a name="tar">What can I do if my system's tar(1) does not support the correct options?</a>


If your tar does not support the `-J` option you can either use a system
which does have such a tar or you can try downloading GNU Tar from
the [GNU website](https://www.gnu.org/software/tar/) and after extracting it,
compile it and then install it so that the tools may find it.

Some systems have a `GNU tar` that you can use. For instance FreeBSD has a
`gtar` command so if necessary you can use that. Note that you'll have to
specify in the tools the `-t tar` option to make this work.


## 11. <a name="markdown">Where can I find help with formatting markdown files for my submission?</a>

Please see this simple
[markdown guide](https://www.markdownguide.org/basic-syntax) for more help.
