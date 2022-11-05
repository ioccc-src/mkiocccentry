# `test_txzchk`: text files for a test suite for `txzchk`

The subdirectories in this directory have text files for a test suite for the
txzchk tool.

The `good/` subdirectory has files that `txzchk` MUST report as valid.

The `bad/` subdirectory has files that `txzchk` MUST report as invalid.

The filenames MUST end with `.txt` and the script will do the right thing.  For
good files the filename MUST match the rules of the `fnamchk` tool; this is not
as clear for the bad tests though generally speaking it should also follow the
rules as it will make it harder to get valid test results.

The typical format is that which would be generated from `tar -tJvf` but
importantly these files are **NOT** tarballs: they are text files that
would have the output of the tar command. This is to make it much easier to add
test cases and it also prevents having the need to have tarballs in the repo (as
well as constructing said tarballs with invalid input that might be submitted
due to abuse). Over time more text files have been and will be added.

Here's an example good file:

```
drwxr-xr-x  0 501    20          0 Jun  3 05:50 test-0/
-rw-r--r--  0 501    20       1854 Jun  3 05:50 test-0/Makefile
-rw-r--r--  0 501    20          4 Jun  3 05:50 test-0/extra2
-rw-r--r--  0 501    20          0 Jun  3 05:50 test-0/prog.c
-rw-r--r--  0 501    20       2793 Jun  3 05:50 test-0/.auth.json
-rw-r--r--  0 501    20       3715 Jun  3 05:50 test-0/remarks.md
-rw-r--r--  0 501    20       1429 Jun  3 05:50 test-0/.info.json
-rw-r--r--  0 501    20          4 Jun  3 05:50 test-0/extra1
```

Let's say the file is called `entry.test-0.1924260612.txt`. We can run the
following command to test `txzchk` on it:

```sh
$ ./txzchk -T -E txt entry.test-0.1924260612.txt
```

This will report on whether or not the tarball is valid. If you want to not see
any output at all and only check the exit status one can do:

```sh
$ ./txzchk -T -E txt entry.test-0.1924260612.txt 2>&1 >/dev/null || echo invalid
```

This will not show anything as the tarball is actually valid.

However for the script you will see some output. Perhaps this should be directed
to a log file (as some of the output is also added to the log file) but for now
this will do as other things have to be done.

## Important note for adding files to the bad subdirectory

Whenever a new bad test file is added one must generate the proper err file. To
simplify it you can from the top level directory where the mkiocccentry.c source
file is located:

    for i in ./test/test_txzchk/bad/*.txt; do ./txzchk -q -v 0 -w -T -E txt -F ./fnamchk "$i" 2>"$i.err"; done

then do:

    git add ./test_txzchk/bad/*.txt ./test_txzchk/bad/*.err

(assuming that there aren't any other files with those extensions that should
not be there). We could have the `test_txzchk.sh` script do this but the problem
is we need to manually inspect that the errors are correct.
