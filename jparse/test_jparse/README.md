# Preparing the bad location tests

If ever a file is added to the `test_JSON/bad_loc` directory then one **MUST**
take the following steps.

First make a visual inspection to be sure that the output of `jparse` is correct
on the files. If they are then run from `jparse/test_jparse/` the following
commands exactly:

```sh
make rebuild_jparse_err_files
```

You should see something like:

```sh
$ make rebuild_jparse_err_files

test_jparse: make rebuild_jparse_err_files starting

rm -f test_JSON/bad_loc/*.err
make: [Makefile:379: rebuild_jparse_err_files] Error 1 (ignored)

Make sure to run make test from the top level directory befor doing a
git add on all the *.json and *.json.err files in test_json/bad_loc!

test_jparse: make rebuild_jparse_err_files ending
```

Assuming you see the above you **MUST** then run `make test` from the top level
directory **AND** this directory and make sure that both do not fail.

Assuming that these both pass you can then do from this directory:

```sh
git add test_JSON/bad_loc/*.json
git add test_JSON/bad_loc/*.json.err
```

Then commit and make a pull request to have it added to the repo.

If the error files are not added, `make test` **WILL FAIL!**
