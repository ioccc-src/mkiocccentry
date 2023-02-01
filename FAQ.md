# Frequently Asked Questions about the `mkiocccentry` repo


## <a name="download">Where can I find the mkiocccentry tool?</a>

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


## <a name="compiling">How do I compile the mkiocccentry tool ?</a>

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
<a href="#reporting-bugs">how do I report bugs or other issues ?</a>



## How do I package my entry?

We recommend that you use the `mkiocccentry` tool to package your entry.
If you have not already done so, download the [mkiocccentry repo](https://github.com/ioccc-src/mkiocccentry)
(instructions <a href="#download">here</a>)
and compile it
(instructions <a href="#compiling">here</a>).

From the top level directory, run the `mkiocccentry` executable:

```sh
./mkiocccentry work_dir prog.c Makefile remarks.md [file ...]
```

The _work_dir_ is the path where your entry's files will be packaged from.
The _work_dir_ **must** exist as _mkiocccentry_ will create a subdirectory under it to
package your entry. The entry directory depends on the IOCCC contestant id and
the entry number.

Something like _/tmp/ioccc_ is a good choice:

```sh
mkdir -p /tmp/ioccc
```

The _prog.c_ is the path to the main C source code for your entry.

The _Makefile_ is the path to a Makefile used to build your entry.

The _remarks.md_ is a path to a
[markdown file](https://www.markdownguide.org/basic-syntax)
that describes your entry.

If you have other optional files that belong to your entry, give them
as additional paths at the end if your `mkiocccentry` tool command line.

The `mkiocccentry` tool, by default, will ask you a series of
questions about your entry and about the authors of your entry.
Once you have answered all of the questions, the tool will form a
XZ compressed tarball, in v7 format, under the _work_dir_ directory.


## Can't I just submit my obfuscated C program to the judges?

No. While we appreciate your enthusiasm for wanting to show us your obfuscated
code, the [IOCCC judges](https://www.ioccc.org/judges.html) request your help by
using the `mkiocccentry` tool to package your entry.

We need your entry in the form of an XZ compressed tarball in v7 format,
along with things like your Makefile, a pair of JSON files that
help describe you and your entry, some remarks you wrote about your
entry etc. The  `mkiocccentry` tool does a lot in packaging your
entry and we kindly request that you use it.

In short, you cannot simply upload your obfuscated C program as it needs to
be in a certain form and the `mkiocccentry` tool does that.


## Do I have to use mkiocccentry to package my entry?

Technically you do not have to use the `mkiocccentry` tool; however,
you run the risk of having your entry rejected if what you upload
to the submit server is malformed.  Moreover, the contents of the
XZ compressed tarball (in v7 format) that you upload to the submit
server must be in proper form or your entry could be rejected.
Therefore we highly recommend that you use the `mkiocccentry` tools.

If you simply must package your entry yourself then we strongly
recommend that you use the `txzchk` tool to verify that the tarball
you plan to upload to the submit server is OK.  We also strongly
recommend that use the `chkentry` tool to inspect the directory
that you used to form the tarball to verify that the contents under
that directory are also OK.


## <a name="reporting-bugs">How do I report bugs or other issues ?</a>

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


## How can I help test this repo ?

Thank you for any and all help!

Please see the
[README file](https://github.com/ioccc-src/mkiocccentry/blob/master/README.md)
for more details on what you can do to help us.


## What can I do if my system's tar(1) does not support the correct options ?


If your tar does not support the `-J` option you can either use a system
which does have such a tar or you can try downloading GNU Tar from
the [GNU website](https://www.gnu.org/software/tar/) and after extracting it,
compile it and then install it so that the tools may find it.


## Where can I find help with formatting markdown files for my entry?

Please see this simple
[markdown guide](https://www.markdownguide.org/basic-syntax) for more help.
