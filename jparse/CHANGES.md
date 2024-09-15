# Significant changes in the JSON parser repo

## Release 1.0.8 2024-09-14

Rename option `-e` in `jstrdecode(1)` to `-m` with the alias `-e`. Add missing
`dbg()` call in `jstrdecode(1)` for `-m`/`-e` option. Updated version to `"1.0.3
2024-09-14"`.


## Release 1.0.7 2024-09-13

Extend `jstr_test.sh` to test that the `jstrdecode(1)` options `-Q` and `-e`
work, each by themselves and together.

Add an extra sanity check to `jencchk()`: the macro `JSON_BYTE_VALUES` must
equal 256. Previously we did check that the table length of `jenc` is
`JSON_BYTE_VALUES` with the assumption that this was 256 but now we make sure
that it is 256, before we check the table length.


## Release 1.0.6 2024-09-12

Enhancements and bug fixes in `jstrdecode(1)` and `jstrencode(1)`. In
`jstrdecode(1)` the `-Q` option did not work right in a number of ways.

The tool `jstrdecode(1)` has a new option `-e` which says to enclose each
decoded arg with escaped quotes. This option can be used with `-Q`.

In order to resolve these problems a new set of files were added, `jstr_util.h`
and `jstr_util.c`, which are strictly for the `jstr*` tools. A new `struct
jstring` was added which holds a buffer (that is allocated already so no
`strdup()` - due to the way the encoding/decoding functions work and how they
have to work) and a size of the buffer along with a pointer to the next in the
list. The new function `alloc_jstr()` allocates a new `struct jstring *` adding
an ALREADY allocated `char *` with its size to it. This size is important due to
the fact that these strings MAY contain a NUL byte. The function
`free_jstring()` is what it sounds like: a function to free the memory of a
single `struct jstring *`. With this function too it is assumed that the `char
*` is allocated but since it MAY contain NUL bytes it could leave a memory leak
in some cases.

Making use of these functions and the struct, `jstrencode(1)` and
`jstrdecode(1)` now each have their own list (encoded strings and decoded
strings respectively) and a function each to add to the list and free the list
(this is done this way to simplify things). The function that reads from a
stream calls the function to add to the respective list, as long as no errors
occur (NULL pointers). On the other hand, one can (and the tools do when not
reading from stdin) call the `alloc_jstr()` function directly and then the add
to list function (if needed). The add function adds to the end of the list so
that the order is correct, and a list is used because the `char *`s can hold NUL
bytes which causes problems for the dynamic array facility, and because for some
options like `jstrdecode -Q` we have to wait until after everything is
processed, particularly due to reading from stdin.

The tool `pr_jparse_test` now shows the JSON parser version with `-V` and `-h`
and `-h` now works (the function checks for `devnull` which is now opened before
parsing args).



## Release 1.0.5 2024-09-11

Fixes in `#include` paths in `test_jparse`, run `make depend`, add JSON parser
version string to `jnum_gen` and `jnum_chk`.

Rename Makefile variable `Q_V_OPTION` to `VERBOSITY` and add it to many rules so
one can more easily do something like:

```sh
make VERBOSITY=3 test
```

or so. Not all rules have the `-v ${VERBOSITY}` as not all rules should have it.
In many cases where this is true there is a comment in the Makefiles.

Other fixes in the Makefiles have been applied as well, in particular the tags
related rules that referred to `../dbg` and (incorrectly) `../dyn_alloc` (the
repo is actually `dyn_array`).

The GitHub workflow has been updated to use `VERBOSITY=3` for more details in
the case that there is a failure.

Add to `-V` and `-h` options of `jparse` (the tool) and `jsemtblgen` the JSON
parser version (this was already done in the tools in `test_jparse/` and will be
done with `jstrencode` and `jstrdecode` along with some bug fixes when those
have been properly addressed).



## Release 1.0.4 2024-09-09

Remove the IOCCC tools' paths from util.h. These have been moved to another file
in that repo.


## Release 1.0.3 2024-09-08

Fix `make clobber` to remove `jparse_test.log` and `Makefile.orig`.

Fix `make legacy_clobber` to remove `jparse.a`.

Fix `${RM}` in Makefiles to use `${Q}` variable (not in `make depend` as it is
used in an earlier command in the multiple line commands), in some cases changed
from the wrong variable, and `${RM_V}` (where this was not done).

Do not use `-r` in rm in Makefiles unless removing a directory, for safety.

Make `rm` in Makefiles silent by default (as in do not use `-v`).


## Release 1.0.2 2024-09-07

Improve error messages if invalid JSON in the following ways:

- If verbosity level is > 0, then it will show the invalid token and hex value
of that token (along with the line and column).
- If verbosity is not specified, then it will just show the syntax error (the
bad token with the line and column) and then the warning that the JSON tree is
NULL (just like if verbosity specified) and then the error message (from
`jparse(1)` itself).

The error files in the `bad_loc` have been updated as now the error output has
changed.

Updated `jparse(1)` version to 1.1.6 2024-09-07.


## Release 1.0.1 2024-09-06

Add option `-L` to `jparse_test.sh` to skip error location tests. This is useful
in the case one has a different directory structure or the need of options that
change the paths causes incorrect error files.

`jparse_test.sh` now will try the default JSON strings files if no file is given
on the command line.

Updated man page `jparse_test.8`.

The new version of `jparse_test.sh` is `1.0.6 2024-09-06`.

Removed where necessary some references to the [IOCCC](https://www.ioccc.org)
and the [mkiocccentry repo](https://github.com/ioccc-src/mkiocccentry), changing
some to [this repo](https://github.com/xexyl/jparse).

Don't report 'valid JSON' in `jparse` unless verbosity level > 0. It is still an
error if the JSON is invalid.


## Release 1.0.0 2024-09-06

Cloned from
[mkiocccentry/jparse](https://github.com/ioccc-src/mkiocccentry/tree/master/jparse)
version Release 1.5.1 2024-08-28 as tagged by
[v1.5.1](https://github.com/ioccc-src/mkiocccentry/releases/tag/v1.5.1)
with the exception of this `CHANGES.md` file on 2024-Aug-28.

Updated versions of the tool and the library to `1.1.5 2024-09-04`.

Added `prep.sh`, `make release`, `make slow_release`, `make prep`, `make
slow_prep` rules.

Fixed various scripts that were not working in this repo due to the original
structure of the repo (a subdirectory in another repo).

Enhanced the `jparse_test.sh` script.

Added missing scripts to `make test`.
