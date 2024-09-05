# The jparse test suite

There are many files in this directory including a lot of JSON files, both good
and bad, in subdirectories, that are tested with the `-d` option to the
`jparse_test.sh` test-suite script, along with a text file of a number of valid
JSON strings and another file with invalid JSON strings. The good/valid JSON
must pass validation and the bad/invalid must fail validation.

To see the options of the script:

```sh
./jparse_test.sh -h
```

If you need more information on this tool, check the man page `jparse_test.8`.
To run the script it is better to use `make test` from the top level directory.

If, on the other hand, you are reporting a bug, it is best to run `make
bug_report` from the top level directory, and then open a bug report in the
[jparse bug report
page](https://github.com/xexyl/jparse/issues/new?assignees=&labels=bug&projects=&template=bug_report.yml&title=Bug%3A+%3Cbug+title%3E)
or otherwise, if you're reporting something else, perhaps a feature request or
enhancement, find the appropriate templates at the [jparse issues
page](https://github.com/xexyl/jparse/issues/new/choose). **PLEASE** attach the
bug report log file!


## Preparing the bad location tests

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
