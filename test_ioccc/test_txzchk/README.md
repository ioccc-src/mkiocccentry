# `test_txzchk/` - data files for the `txzchk_test(8)` test-suite for `txzchk(1)`

The subdirectories in this directory have text files which contain output that
match the format of `tar(1)` listings, although some of them are bogus to test
that `txzchk(1)` does the right thing even when there is invalid output from
`tar -tJvf`.


<div id="good-dir"></div>

## `good/` subdirectory

The `good/` subdirectory has files that `txzchk` **MUST** report as **VALID**.
They **MUST** be _text files_ and they **MUST** end with `.txt`.

The files not ending with `.txt` will be ignored by the script.

<div id="bad-dir"></div>

## `bad/` subdirectory

The `bad/` subdirectory has files that `txzchk` **MUST** report as **INVALID**.
They **MUST** be _text files_ and they **MUST** end with `.txt` and there
**MUST** be a matching file ending with the extension `.err` (i.e. the file
`bad/submit.12345678-1234-4321-abcd-1234567890ab-2-19944411114.txt` must also
have a corresponding
`bad/submit.12345678-1234-4321-abcd-1234567890ab-2-19944411114.txt.err` file).

Any file not ending with `.txt` will be ignored and need not have an `.err`
file.

<div id="fnamchk"></div>

## `fnamchk(1)`

The `fnamchk(1)` tool is part of the algorithm of `txzchk(1)`, in particular to
give it the right top  level directory name. With one exception (see below), the
tests **MUST** pass or it will cause an error. This exception is only valid in
testing and will **NOT** be valid during a contest!


<div id="fnamchk-exception"></div>

### Exception to the `fnamchk(1)` rules

There is one thing that does **NOT** have to (but may and sometimes does) pass:
in particular, the `txzchk(1)` tool (when run from `txzchk_test.sh(8)` as it
gives `txzchk(1)` the `-x` option) will give the `-T` option to `fnamchk(1)`
which means that the timestamp check test is ignored; this is important because
each contest the minimum timestamp will be changed, thus breaking the tests: in
order to check that the top level (submission) directory name (in the tarball)
is correct, `fnamchk(1)` **MUST** return the directory name and **NOT** an
error.  Without the `-T` option, any time the minimum timestamp changed it would
break the test-suite and would require that the filenames were renamed.

<div id="file-format"></div>

## Format of the test files

The typical format is, as noted above, that which _would_ be generated from `tar
-tJvf` on a tarball **but importantly** these files are **NOT** tarballs: _they
are **text** files_ that _would have the output_ of the tar command (although
see below on the [bad files](#bad)). This is to make it much easier to add test
cases and it also prevents having the need to have tarballs in the repo (as well
as constructing said tarballs with invalid input that might be submitted due to
abuse and can be hard to create as well). Over time more text files have been
and more might still be added.

<div id="good"></div>

## Good test files (i.e. those that `txzchk(1)` must report as VALID)

Here's an example good file:

```
drwxr-xr-x  0 501    20          0 Jun  3 05:50 test-0/
-r--r--r--  0 501    20       1854 Jun  3 05:50 test-0/Makefile
-r--r--r--  0 501    20          4 Jun  3 05:50 test-0/extra2
-r--r--r--  0 501    20          0 Jun  3 05:50 test-0/prog.c
-r--r--r--  0 501    20       2793 Jun  3 05:50 test-0/.auth.json
-r--r--r--  0 501    20       3715 Jun  3 05:50 test-0/remarks.md
-r--r--r--  0 501    20       1429 Jun  3 05:50 test-0/.info.json
-r--r--r--  0 501    20          4 Jun  3 05:50 test-0/extra1
```

Let's say the file is called `submit.test-0.1924260612.txt`. We can run the
following command to test `txzchk` on it:

```sh
txzchk -x -T -E txt submit.test-0.1924260612.txt
```

This will report on whether or not the tarball is valid. If you want to not see
any output at all and only check the exit status one can do:

```sh
txzchk -x -T -E txt submit.test-0.1924260612.txt 2>&1 >/dev/null || echo invalid
```

This will not show anything as the tarball is actually valid.

<div id="bad"></div>

## Bad test files (i.e. those that `txzchk(1)` must report as INVALID)

An example of a bad file is `bad/`:

```
drwxr-xr-x  0 501    20          0 Feb  6 02:28 test-9/
-rw-r--r--  0 501    20          0 Feb  6 02:28 test-9/Makefile
-rw-r--r--  0 501    20          4 Feb  6 02:28 test-9/extra2
-rw-r--r--  0 501    20         29 Feb  6 02:30 test-9/prog.c
-rw-r--r--  0 501    20          0 Feb  6 02:28 test-9/.auth.json
-rw-r--r--  0 501    20          0 Feb  6 02:28 test-9/remarks.md
-rw-r--r--  0 501    20          0 Feb  6 02:28 test-9/.info.json
-rw-r--r--  0 501    20          4 Feb  6 02:28 test-9/extra1
```

The reason this file is bad is because the regular files have the wrong
permissions (not `0444` i.e. `-r--r--r--`). However, there are **MANY** other
checks that `txzchk(1)` runs and if you need to know you will have to check the
source code; the function `show_tarball_info()` is a nice place to see an
overview of what it checks though it is possible at times it does not show
everything checked.

And yes, the tool detects invalid formats and even characters that are not POSIX
plus + safe only chars as well as login / group names.


<div id="adding-bad-files"></div>

### Important note for adding files to the bad subdirectory

Whenever a new bad test file is added or one is modified or if `txzchk(1)` is
updated in such a way as to make the output differ, one must generate the proper
`.err` file (or in the latter case, files). To simplify it you can from the top
level directory where the mkiocccentry.c source file is located:

```sh
make rebuild_txzchk_test_errors
git add test_ioccc/test_txzchk/bad/*.txt test_ioccc/test_txzchk/bad/*.err
```

BUT MAKE SURE THAT ALL FILES ARE CORRECT! The make rule will run the `-B` option
on the test script which will first prompt you to make sure if you wish to do
it. Again one must make sure that the files are correct!

This, of course, is assuming that there aren't any other files with those
extensions that should not be there.

**PLEASE NOTE**: this is strictly for **maintainers of the repo** and **almost
always** me (@xexyl / Cody Boone Ferguson).
