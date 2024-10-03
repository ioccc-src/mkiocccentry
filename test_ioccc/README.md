# Official IOCCC submission toolkit

Below are some details on the test suite of the repo. We do not give all details
but we at least show you a few things.
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

See additionally [the soup/README.md
file](https://github.com/ioccc-src/mkiocccentry/blob/master/soup/README.md).
