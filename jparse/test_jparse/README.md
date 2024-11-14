# The jparse testing suite

Here we briefly describe for you our testing suite, for those who are
interested, though some of it is only relevant to repo maintainers (that is put
at the end of the file for that reason).

If you are looking for information on the `jparse` repo, see the [jparse repo
README.md](https://github.com/xexyl/jparse/blob/master/README.md).

For information on the `jparse` utilities see
[jparse_utils_README.md](https://github.com/xexyl/jparse/blob/master/jparse_utils_README.md).

For information on the `jparse` library, see
[jparse_library_README.md](https://github.com/xexyl/jparse/blob/master/jparse_library_README.md).

We also do recommend that you read the
[json_README.md](https://github.com/xexyl/jparse/blob/master/json_README.md)
document to better understand the JSON terms used in this repo.



# [test_jparse](https://github.com/xexyl/jparse/tree/master/test_jparse)

In this directory we have a [subdirectory with test JSON
files](https://github.com/xexyl/jparse/tree/master/test_jparse/test_JSON), both
[good](https://github.com/xexyl/jparse/tree/master/test_jparse/test_JSON/good)
and [bad](https://github.com/xexyl/jparse/tree/master/test_jparse/test_JSON/bad)
(plus a subdirectory to test error location reporting,
[bad_loc](https://github.com/xexyl/jparse/tree/master/test_jparse/test_JSON/bad_loc)),
and a number of tools and scripts that run a battery of tests on the parser
(internally and otherwise) and the tools, including decoding and encoding, via
code that is in
[json_parse.c](https://github.com/xexyl/jparse/blob/master/json_parse.c), for a
few examples.

This directory also has a number of tools and scripts, some of which work with
the test JSON files.


<div id="test-json-files"></div>

# Test JSON files

The files referred to above come from at least two sources: our own (with some
Easter eggs included due to a shared interest between Landon and Cody :-) ) as
well as from the [JSONTestSuite](https://github.com/nst/JSONTestSuite) (with
**MUCH GRATITUDE** to the maintainers: **THANK YOU!**), which also has good and
bad files (we simply copied the invalid JSON files to the bad subdirectory and
the valid JSON files to the good subdirectory), and all is good.

Now with the tests on these files in place, if for some reason the parser were to be
modified, in error or otherwise, and the test fails then we know there is a
problem. As the GitHub repo has workflows to make sure that this does not happen
(or rather it alerts us to the problem) it should never be added to the repo
(unless of course we happen to push a commit that does :-) but if that happens
we'll end up fixing it.


<div id="jparse-testing-suite-scripts"></div>

# jparse testing suite scripts and tools

The driver script is
[run_jparse_tests.sh](https://github.com/xexyl/jparse/blob/master/test_jparse/run_jparse_tests.sh).
This script, run by the Makefile rule test,  runs the other scripts.

If you must know what this script does, or what scripts and tools it uses,
please take a look at it, as it is a lot to write and probably most people are
not interested in it.

If, on the other hand, you are reporting a bug, it is best to run `make
bug_report` from the top level directory, and then open a bug report in the
[jparse bug report
page](https://github.com/xexyl/jparse/issues/new?assignees=&labels=bug&projects=&template=bug_report.yml&title=Bug%3A+%3Cbug+title%3E)
or otherwise, if you're reporting something else, perhaps a feature request or
enhancement, find the appropriate templates at the [jparse issues
page](https://github.com/xexyl/jparse/issues/new/choose). **PLEASE** attach the
bug report log file!


# Running the test-suite

If you wish to run this test-suite, try from the top level directory of the repo:

```sh
make clobber all test
```

<hr>

# Preparing the bad location tests

This task is only meant for the repo maintainers but for the curious if ever a
file is added to the `test_JSON/bad_loc` directory then one **MUST** take the
following steps.

First make a visual inspection to be sure that the output of `jparse` is correct
on the files. This is a **MUST!** If the output **IS CORRECT**, then run from
the top level directory the following command (if run from the
`jparse/test_jparse/` subdirectory it will first do a `cd ..`) these commands
exactly:

```sh
make rebuild_jparse_err_files
```

You should see something like:

```sh
$ make rebuild_jparse_err_files
jparse: make rebuild_jparse_err_files starting

rm -f test_jparse/test_JSON/bad_loc/*.err
make: [Makefile:693: rebuild_jparse_err_files] Error 1 (ignored)

Make sure to run make test from the top level directory before doing a
git add on all the *.json and *.json.err files in test_jparse/test_JSON/bad_loc!

jparse: make rebuild_jparse_err_files ending
```

Assuming you see the above you **MUST** then run `make test` from the top level
directory **AND** this directory and make sure that both do not fail.

Assuming that these both pass you can then do from this directory:

```sh
git add test_JSON/bad_loc/*.json
git add test_JSON/bad_loc/*.json.err
```

Then commit and push to the repo.

If the error files are not added, or something else is wrong, `make test` **WILL
FAIL!**
