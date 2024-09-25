# Significant changes in the JSON parser repo

## Release 1.0.15 2024-09-25

Use `FOO_BASENAME` when `prog == NULL` in `usage()` functions of the tools
written in C.


## Release 1.0.14 2024-09-24

Implement boolean `unicode` of `struct json_string` in the decoding functions.
If `json_conv_string()` finds that calling `json_decode()` which calls
`decode_json_string()` causes the bool `unicode` to be false it sets the
`converted` boolean to `false`. This will then flag an error in parsing if there
is an invalid Unicode symbol. This does not mean that decoding all symbols work:
it simply means that we detect if there are invalid Unicode symbols. The check
is done on the original string but if it turns out it has to be done on the
decoded string that can be done easily.

The `json_decode()` had a bug fix: it allocated memory for the return string
when `decode_json_string()` does that and this caused a memory leak.

If `decode_json_string()` (which is now a static function in `json_parse.c`)
detects an error, the allocated memory is freed before returning `NULL`.

Updated `JSON_PARSER_VERSION` to `"1.1.6 2024-09-24"`.

Updated `JPARSE_REPO_VERSION` to `"1.0.13 2024-09-24"`.

Added `MIN` and `MAX` macros to `util.h`.

Fix build of libjparse.a - add `json_utf8.o`.

Remove `#line ..` from `json_utf8.h`.

Added `version.h` which has the versions for the repo release, the jparse JSON
parser and the jparse tool. The other tools have their respective version in
their source code file. This file was added primarily so that `verge` could
refer to the JSON parser version. In order to get this to work, the `jparse.y`
file now has `#include "version.h"`. This means the backup parser source code
has been rebuilt with `make parser-o`.

The tools now have a `FOO_BASENAME` in their header file which is used in both
the usage string and the version option.

The file `test_jparse/pr_jparse_test.h` has been added for that tool.

The Makefiles have been updated including new dependencies.



## Release 1.0.13 2024-09-23

Rename `jenc` to `byte2asciistr` in `json_parse.c` to avoid confusion about its
purpose.

Expand the output of `jstrencode -t` and `jstrdecode -t` to express that the
encode/decode tests have not yet been written. This depends on bug #13 being
resolved first.

Changed optimisation flags in the Makefiles to not specify `-g3` as debug
symbols are almost useless when optimising and we have `-O3` in use. During
debugging one can always use:

```sh
make C_OPT="-g3" clobber all
```

to compile in debug symbols or have a file in the respective directories (those
needed) called `makefile.local` with the line:

```makefile
C_OPT= -g3
```

which is used for development purposes but should not normally be done.

Fix potential use without initialisation of `inputlen` in `jstrencode.c`.

Add function `decode_json_string()` to help simplify the `json_decode()`
function as it's quite long. This new function takes the length and calculated
decoded size as well as the pointers (the block of memory, the return length and
the `has_nul` as well) and then allocates the `char *` and does what was the
second half of the `json_decode()` function. As `json_encode()` is much simpler
it seems like at this time that something like this is not needed. This new
function is not static but it is entirely unclear if that is necessary.

Add to `struct json_string` the `bool unicode`. Currently unused (just
initialised to false) the purpose will be to indicate whether or not the string
has any invalid unicode symbols found during decoding.


## Release 1.0.11 2024-09-20

Add (as `json_utf8.h` and `json_utf8.c`) the files `unicode.h` and `unicode.c`
from the C unicode library [unicode-c
repo](https://github.com/benkasminbullock/unicode-c), slightly modified to fit
our needs. More modification can be done once the bug in `json_parse.c`'s
`json_decode()` function is modified to use what appears to be the function
`ucs2_to_utf8()`. It is not clear at this point but it might be possible to
greatly reduce these new files in code to just the bare minimum of what we
require but right now it is all included, even the repeat macros in the C file
(when `HEADER` is defined which it is not). The test code was removed from these
files as that was part of its test suite that we do not need. A link back to the
repo has been added, along with the author and the same header comments in the
files. If it turns out we can just use the UTF-8 decoding algorithm by itself we
might reduce the code to just that, making sure to credit (and link back) the
author.  But in the meantime we still have to resolve the UTF-8 decoding bugs.

The Makefiles now compile and link in `json_utf8.c`.

Run `make seqcexit`.


## Release 1.0.10 2024-09-19

Add call to `setlocale()` in `jstrencode.c` and `jstrdecode.c`.

Removed helper function `is_utf8()` as it appears to be not useful and might
actually be incorrect. A copy of this function has been made in the case it
actually does prove useful, unlikely as that seems.


## Release 1.0.9 2024-09-15

Add helper function `is_utf8()` to determine if a `char *` is a UTF-8 encoded
string. This code, modified somewhat (including a typo fix and allowing all
ASCII chars, including control chars, which are allowed), comes from
<https://stackoverflow.com/a/1031773/9205647>, and we thank the answerer very
much! This function, we believe, will be of use, in our fixing of a JSON
decoding bug that is known and being resolved (or is being discussed to be
resolved), hopefully sooner than later and better soon. Nonetheless this
function is not yet used so we may discover it is not of use or there is a
problem that has to be resolved.

Added a warning comment in `json_util.c` to functions (and their respective
structs, enums and macros in `json_util.h`) that are subject to change: they
were being used for tools that were in the works (before this was its own repo)
but were not completed due to illness and then, when well again, other
priorities taking over (and they themselves have been removed until the 'right
time').

Fixed `check_man` make rule so that it would fail if `checknr` exits non-zero.
This helped uncover a problem (though not in display) in` jparse.1`.

Added helper script `test_jparse/is_available.sh` which exits 0 if the arg can
be found via `type -P`, else 1. This will be used in prep.sh, in a future
update, to help prevent superfluous bug reports.


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

The tool `pr_jparse_test` now shows the JSON parser version: with `-V` and `-h`
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
