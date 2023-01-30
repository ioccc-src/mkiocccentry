# Frequently Asked Questions about this repo: troubleshooting etc.


## How do I report bugs or other issues ?

Please run the following from the main directory:


```sh
make bug_report
```


and open a new issue at the
[GitHub issues page](https://github.com/ioccc-src/mkiocccentry/issues/new/choose),
making sure to give us as much information on your problem as possible and
making sure to attach the bug report file.


The bug report filename is in the format:
`bug-report.$(/bin/date +%Y%m%d.%H%M%S).txt` where:


- `%Y` is the digit year (e.g. 2023).

- `%m` is the is the two digit month (01..12).

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
the [GNU website](https://www.gnu.org/software/tar/) and compile and install it
so that the tools may find it.


## I need help with formatting markdown files for my entry

Please see this simple
[markdown guide](https://www.markdownguide.org/basic-syntax) for more help.


