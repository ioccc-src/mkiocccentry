# Significant changes in the JSON parser repo


## Release 2.4.0 2025-09-20

Now using stdio help function from the [pr repo](https://github.com/lcn2/pr).
Removed `libpr` duplicate from `util.c`.
Removed `libpr` duplicate from `util.h`.

Replaced calls to the `check_invalid_option()` function with the more general
`fchk_inval_opt()` interface from `libpr.a`.


## Release 2.3.3 2025-09-16

In keeping with the mkiocccentry toolkit, the new header
files from dbg are included.

Where needed the include files from dyn_array
are consistently included.

Added `${PICKY_OPTIONS}` to specify the picky options
for the `${C_SRC}` and ${H_SRC} files.

Added `${LESS_PICKY_OPTIONS}` to specify the picky options
for the `${LESS_PICKY_CSRC}` and ${LESS_PICKY_HSRC} files.

Added `${SH_PICKY_OPTIONS}` to specify the picky options
for the `${SH_FILES}` files.

Where applicable, added ${BISON_FLEX_PICKY_OPTIONS` to specify the picky options
for the `${BISONFILES}` and ${FLEXFILES} files.

The `make picky` rule tests a single set, if that set is non-empty,
one at a time.

Removed unused macros from util.h, some of which are leftovers from mkiocccentry
(at the time of moving code out of here that never belonged here but was here
due to the history of the repo the macros were missed).

Updated `JPARSE_UTILS_VERSION` to `"2.1.3 2025-09-20"` (yes this jumped up but
that's because the last two code updates to util.[ch] did not have a version
update).


## Release 2.3.2 2025-09-09

Improved `jparse(1)` to allow for more than one arg of the specific type (string
if `-s` is used, file otherwise). If any of the args is invalid JSON it will
exit 1, else 0.

Updated error location files as the `jparse(1)` error now specifies the filename
of each invalid JSON file. This is important for the test script.

Updated `JPARSE_TOOL_VERSION` to `"2.0.2 2025-09-09"`.


## Release 2.3.1 2025-09-08

Updated `jparse/jsemtblgen.c` to use the new `dyn_array_qsort(3)` function
from [dyn_array repo](https://github.com/lcn2/dyn_array) "2.4.0 2024-09-08".

Updated `JPARSE_REPO_VERSION` to "2.3.1 2025-09-08".
Updated `JSEMTBLGEN_VERSION` to "2.0.3 2025-09-08".


## Release 2.3.0 2025-09-01

Moved nearly all of the tree and file utility functions
that were in `util.c` and `util.h` over to the
[mkiocccentry toolkit repo](https://github.com/ioccc-src/mkiocccentry)
files `soup/file_util.c` and `pr.h`.
The code that was moved was not needed for the JSON
parser nor the parser's related utilities.
Moving that code will both simplify the JSON
parser code base and make it easier for the
[mkiocccentry toolkit repo](https://github.com/ioccc-src/mkiocccentry)
to improve and enhance tree and file utility functions.
Only a few small utility functions such as `exists()`,
`is_file()`, and `is_read()` remain behind as private
functions in `jparse.l` and `test_jparse/jnum_gen.c`.

Moved the `test_jparse/util_test` code over to the
[mkiocccentry toolkit repo](https://github.com/ioccc-src/mkiocccentry)
as `test_ioccc/test_file_util.c` and ran via that repo's
`test_ioccc/ioccc_test.sh` test script.  That test script
is in its own file and thus no longer has to be copied
from some other source file in order to be compiled.

Restored some of the double blank lines separating functions
in `util.c`.

The `test_jparse/run_jparse_tests.sh` no longer runs `util_test`.
Changed the `test_jparse/jnum_chk` test exit code to `27` to be in sequence.

Made `.gitignore` much more robust, ignoring intermediate files
that may be created while performing `make release`.

Removed `UTIL_TEST`.

Removed `UTIL_TEST_VERSION`.  That symbol has become, over in the
[mkiocccentry toolkit repo](https://github.com/ioccc-src/mkiocccentry)
`soup/version.h` the symbol `FILE_UTIL_TEST_VERSION`
and now has the value "2.0.0 2025-09-01".

Updated, improved and sorted `.gitignore`.

Updated `RUN_JPARSE_TESTS_VERSION` to "2.0.1 2025-09-01".

Updated `JPARSE_UTILS_VERSION` to "2.1.0 2025-09-01".

Updated `JPARSE_REPO_VERSION` to "2.3.0 2025-09-01".

**NO** JSON parser code was changed as a result of
these moves.  Nevertheless the movement of code
serves as a "_checkpoint_" tag and thus warrants
a 2nd-level version number change.


## Release 2.2.49 2025-09-01

Increased the debug level of most debug messages for functions in
`jparse/verge.c`.  In some cases a debug message was turned into a warning when
an non-fatal error condition was detected.

Increased the debug level of non-`UTIL_TEST` code debug messages for functions
in `jparse/util.c`.

The `test_jparse/Makefile` removes `test_jparse/util_test.o` for
`make clean`, and removes `test_jparse/util_test.c` for `make clobber`.

Updated `JPARSE_UTILS_VERSION` to `"2.0.15 2025-09-01"`.
Updated `VERGE_VERSION` to `"2.0.4 2025-09-01"`.


## Release 2.2.48 2025-08-30

Changed `util.h` to include `<limits.h>` for the `CHAR_BIT` define,
add code to define `CHAR_BIT` as 8 if undefined, and changed
the `BITS_IN_BYTE` macro to use `CHAR_BIT` instead.

## Release 2.2.47 2025-07-23

Update issues URL in `jparse_bug_report.sh`.

Added issues URL to `jparse_bug_report.1`.

Updated `BUG_REPORT_VERSION` to `"2.1.2 2025-07-23"`.


## Release 2.2.46 2025-07-04

Remove debug output in `surrogate_pair_to_codepoint()` as having it allows for
debug output when it's simply that there is no surrogate pair which is not
invalid.

Changed debug level in `surrogate_pair_to_codepoint()` to `DBG_VVHIGH` (up from
`DBG_MED`).

Added function `is_surrogate_pair()`:

```c
extern bool is_surrogate_pair(const int32_t xa, const int32_t xb);
```

to `json_utf8.c`. This is not used in the json decode string functions as it
would needlessly complicate the code but now one can use it if they need to
check and not parse it (i.e. just check).

Typo fix in `jstr_test.sh`.

Updated `JPARSE_UTF8_VERSION` to `"2.1.3 2025-07-04"`.
Updated `JSTR_TEST_VERSION` to `"2.0.1 2025-07-04"`.


## Release 2.2.45 2025-06-26

Don't warn on invalid range in `surrogate_pair_to_codepoint()` but rather make
it debug output (at level high). This is because it can give a warning where it
is not desired. Warning might cause too much chatter anyway.

Updated `JPARSE_UTF8_VERSION` to `"2.1.2 2025-06-28"`.


## Release 2.2.44 2025-06-26

Changed remaining calls of `malloc()` to `calloc()` outside of the generated
code. Also, don't do `foo = calloc(size, sizeof(char))` etc. but instead do `foo
= calloc(size, sizeof(*foo))`. The boolean that existed, `malloced`, was also
changed to `calloced`. Comments for code that had already been changed to use
`calloc(3)` but still said `malloced` were changed as well.

Updated `JSEMTBLGEN_VERSION` to `"2.0.2 2025-06-26"`.
Updated `JSTRDECODE_VERSION` to `"2.2.3 2025-06-26"`.
Updated `JSTRENCODE_VERSION` to `"2.2.3 2025-06-26"`.
Updated `VERGE_VERSION` to `"2.0.3 2025-06-26"`.
Updated `JPARSE_LIBRARY_VERSION` to `"2.3.1 2025-06-26"`.
Updated `JPARSE_UTILS_VERSION` to `"2.0.13 2025-06-26"`.


## Release 2.2.43 2025-06-25

Fix `jparse_bug_report.sh` to check if one is inside a repo (hopefully jparse
:-) ) prior to running git commands.

Rewrite the UTF-8 library, with some research, although the code might seem not
changed much, due to the fact that a lot of it consists of magic numbers. This
does not mean that the issue #31 is resolved.  It is not possible for me to test
that but in any event it'll likely take much more work to integrate into the
`json_parse.c` and `json_utf8.c` code and is for further down the road. It might
involve the code from the original reporter, @SirWumpus, but this will all be
done another time (the problem also has to be looked at in more detail before
anything can be done with it).

Updated `BUG_REPORT_VERSION` to `"2.1.1 2025-06-25"`.
Updated `JPARSE_UTF8_VERSION` to `"2.1.1 2025-06-25"`.


## Release 2.2.42 2025-06-20

Fix bug in checking arrays for paths to honour `fnmatch(3)`. The following
function prototypes have changed with a new `bool fn`:

```c
extern bool array_has_path(struct dyn_array *array, char *path, bool match_case, bool fn, intmax_t *idx);
extern char *find_path_in_array(char *path, struct dyn_array *paths, bool match_case, bool fn, intmax_t *idx);
extern bool append_path(struct dyn_array **paths, char *str, bool unique, bool duped, bool match_case, bool fn);
```

It is entirely unclear if every case of 'true'/'false' is correct in every case
due to the fact that this update is mostly for the fts(3) routines and there is
an option to ignore and also an option to match, both of which can accept globs.
If a problem arises the call to these functions can be revised but this does
resolve a problem observed in the need for the fnmatch(3) change.

Updated `JPARSE_UTILS_VERSION` to `"2.0.12 2025-06-21"`.
Updated `UTIL_TEST_VERSION` to `"2.0.7 2025-06-21"`.


## Release 2.2.41 2025-06-20

Changed `fnmatch_flags` in `struct fts` to `fn_ignore_flags` and added
`fn_match_flags`. Added `struct dyn_array *match` for a list of paths (or if
`fn_match_flags` >= 0 then globs for `fnmatch(3)`) to find. If a file matches
then it will be 'returned' otherwise it'll be skipped. This does mean that a
large list of files to find will make it slower. This is also by request for
mkiocccentry.

The function `reset_fts()` now takes a new boolean (sorry but it's necessary for
this enhancement). If true it clears out the match list.

Fix memory leak in `reset_fts()`.

Updated `JPARSE_UTILS_VERSION` to `"2.0.11 2025-06-20"`.
Updated `UTIL_TEST_VERSION` to `"2.0.6 2025-06-20"`.


## Release 2.2.40 2025-06-19

Added `fnmatch_flags` to `struct fts` for ignored list of files. This is by
request for mkiocccentry. It is possibly feasible to have it for non-ignored
paths but that is not for now if it will be done (or even desired).

Removed a test in `util_test` because it is faulty in logic due to depth. A test
for `fnmatch(3)` was added.

Updated `JPARSE_UTILS_VERSION` to  `"2.0.10 2025-06-19"`.
Updated `UTIL_TEST_VERSION` to `"2.0.5 2025-06-19"`.


## Release 2.2.39 2025-06-05

Added `size_if_file()` which will return the `st_size` (from `stat(2)`) if the
path exists and `stat(2)` does not fail on it and it is a regular file. It is an
error if the path is NULL. For all other cases 0 is returned.

Check ignored file list `read_fts()` before checking file type as there is no
point in checking the file type if the file is to be ignored.

Updated `util_test` to test this new function.

Updated `JPARSE_UTILS_VERSION` to `"2.0.9 2025-06-05"`.
Updated `UTIL_TEST_VERSION` to `"2.0.4 2025-06-05"`.


## Release 2.2.38 2025-03-15

Add `setlocale(LC_ALL, "");` to all `main()` functions.

After a discussion and review now early on in every `main()` we do a
`setlocale(LC_ALL, "");` so that the default locale (in the system) based on
`LANG` will be used.

NOTE: this is not done in the library for very good reasons.

Updated `JSEMTBLGEN_VERSION` to `"2.0.1 2025-03-15"`.
Updated `JSTRDECODE_VERSION` to `"2.2.2 2025-03-15"`.
Updated `JSTRENCODE_VERSION` to `"2.2.2 2025-03-15"`.
Updated `JNUM_CHK_VERSION` to `"2.0.1 2025-03-15"`.
Updated `JNUM_GEN_VERSION` to `"2.0.1 2025-03-15"`.
Updated `PR_JPARSE_TEST_VERSION` to `"2.0.1 2025-03-15"`.
Updated `UTIL_TEST_VERSION` to `"2.0.3 2025-03-15"`.
Updated `VERGE_VERSION` to `"2.0.2 2025-03-15"`.
Updated `JPARSE_TOOL_VERSION` to `"2.0.1 2025-03-15"`.

Undo the variable swap (`< 0` check) in `sum_and_count()` and add a comment
about it so that when we don't stupidly change it again.

Updated `JPARSE_UTILS_VERSION` to `"2.0.8 2025-03-15"`.


## Release 2.2.37 2025-03-14

Add man pages for `json_tree_walk()` and `vjson_tree_walk()` (symlinked to
`jparse.3` which is under `man/man3`). The `jparse.3` has been updated to have
these two functions.

Changed the macro `JPARSE_VERSION` to `JPARSE_TOOL_VERSION` and the string
`jparse_version` to `jparse_tool_version` to better reflect that it's the tool
`jparse(1)` version, not something else.

Don't use `setlocale(3)` in `jstrencode(1)` or `jstrdecode(3)`.

Updated `JSTRENCODE_VERSION` to `"2.2.1 2025-03-15"` (yes that was a typo).
Updated `JSTRDECODE_VERSION` to `"2.2.1 2025-03-14"`.

Typo fix in at least one place.

In places (in `json_util.c`) where the debug output can be confusing a link to
the jparse library README.md file is now in a comment, although for
`json_string` it is not so much about the debug output as the struct itself.

Fix bad bug in `sum_and_count()` - wrong value checked for < 0.

Updated `JPARSE_UTILS_VERSION` to `"2.0.7 2025-03-14"`.

## Release 2.2.36 2025-03-11

Fix bugs in `resolve_path()`. If one had the path of `./foo` or even `/foo` then
it should check that the path is a regular executable file. And if not it should
return NULL. Otherwise it should return the path. Then, if the path is not a
regular executable file and `$PATH` is empty it should return NULL, not a
strdup()d copy of the path. This way the caller can know for sure that the path
returned is in fact a regular executable file.

Updated `JPARSE_UTILS_VERSION` to `"2.0.6 2025-03-11"`.


## Release 2.2.35 2025-03-10

Improved `shell_cmd()` and `pipe_open()` to resolve path (using `resolve_path()`
which searches `$PATH`) if there is no `/` in the command.

Updated `JPARSE_UTILS_VERSION` to `"2.0.5 2025-03-10"`.


## Release 2.2.34 2025-03-09

Updated `base_name()`, `dir_name()` and added new function `resolve_path()`.

The function `base_name()` now will, on a NULL or empty string, return a copy of
`"."` just like `basename(3)` does.

The function `dir_name()` now will, on a NULL or empty string or a string that
has no `/` (unless `level < 0`) , return a copy of `"."` just like `dirname(3)`
(though that function does not have the `level` functionality).

The function `resolve_path()` will go through the process's `$PATH` and search
for an executable file with the name passed to the function, returning either a
copy of the path or NULL. If the command starts with `/` or `./` then the
`$PATH` is not searched and instead that exact string is duplicated and
returned.

Updated `JPARSE_UTILS_VERSION` to `"2.0.4 2025-03-09"`.
Updated `UTIL_TEST_VERSION` to `"2.0.2 2025-03-09"`.


## Release 2.2.33 2025-03-07

Additional sanity checks added to FTS code.

The `read_fts()` function now does not just skip on NULL or empty path name; it
will abort just like `check_fts_info()` does (though that would never have been
reached as we skipped that item and moved to the next). If there is a NULL
pointer or an empty string something is wrong and it's better to abort early.

Also in `check_fts_info()` when the `fts_info == FTS_NS` the `ent->fts_errno` is
the errno so we set `errno = ent->fts_errno` prior to calling `errp()` (instead
of `err()`).

Updated `JPARSE_UTILS_VERSION` to `"2.0.3 2025-03-07"`.

Moved `main()` in `verge.c` to `verge_main.c` and added function `vercmp()`
which does the work of what `verge` does. The `verge.o` is now linked into the
library and `verge.h` is installed with `make install`. This was necessary for
mkiocccentry.

Updated `VERGE_VERSION` to `"2.0.1 2025-03-07"`.


## Release 2.2.32 2025-03-02

Updated `filemode()` with new boolean (sorry!) to mask `S_IFMT` if true. In
other words if you want to print out the value it returns the `st_mode &
~S_IFMT` (otherwise it still determines the file type and masks the correct
bits).

Since enums have different namespaces than function names the function
`type_of_file()` was renamed `file_type()`.

Also, `is_mode()` now uses the function (that did not exist at the time of
original writing) `file_type()` rather than repeatedly having to use `lstat(2)`
and `stat(2)` on the path.

Fun fact: the release version, `2.2.32`, with the exception of the year in the
date and the zeros (i.e. the `5` and zeroes in `2025-03-02`), has only the same
digits as the date, `2025-03-02` (i.e. `2` and `3`).

Fix bug in `read_fts()` with checking basename versus full path. In particular
if the number of directories is just one then it has to be a basename check.
Thus it's `(fts->base || count_dirs(name) == 1) && ...`. If it's not base or the
`count_dirs(name) != 1` then it tries the other way (with `fts->base == false`).
This solves a problem where files to be ignored were not ignored in the case of
base being false (when we needed that to be the case).

Save and restore errno prior to returning from `pipe_open()` and `shell_cmd()`.
In some systems `popen(3)` are not reliable with errno (like macOS) but in linux
it CAN in some cases set errno. For `system(3)` it does no seem to set errno at
all but we still do this process especially as it also uses a function that can
have errno (and we return from the function in that case with an error).

Updated `JPARSE_UTILS_VERSION` to `"2.0.2 2025-03-02"`.
Updated `UTIL_TEST_VERSION` to `"2.0.1 2025-03-02"`.


## Release 2.2.31 2025-03-01

Fix critical bug in `copyfile()`: obtaining the FD of the source file should not
have the flag `O_WRONLY` but rather `O_RDONLY`.

Updated `JPARSE_UTILS_VERSION` to `"2.0.1 2025-03-01"`.


## Release 2.2.30 2025-02-28

Add util function that uses `file_size()` called `is_empty()`.

Updated all tools by changing `x.y.z` to `x.y+1.0` except that for those under <
`2.0.0` they have been changed to `2.0.0`.

Add copyright message to all source (code, header, scripts) and Makefiles.

Fix enum `FTS_TYPE_ANY` - add missing `FTS_TYPE_FIFO`.


## Release 2.2.29 2025-02-27

The script `jparse_bug_report.sh` actually can have the script in the `TOOLS`
variable as it will not actually cause an infinite loop as it only gets the
version of the tool - which is quite useful to know that the user is using the
most recent version of the script.


## Release 2.2.28 2025-02-26

Finish 'depth' check concept in `struct fts`.

Add to `struct fts` a `min_depth` and `max_depth` to complete the concept of the
depth checks. This will only be used if depth <= 0 and only was added because in
some cases it can be useful, kind of like `find -mindepth` and `find -maxdepth`
can be (although there are slightly different semantics as we only want to have
the checks if > 0 and if depth == 0 too). If someone supplies a bogus range, for
instance a `min_depth > max_depth` then they will not get anything because what
they asked cannot be fulfilled. The util test code tests this.

Updated `JPARSE_UTILS_VERSION` to `"1.0.25 2025-02-26"`.
Updated `UTIL_TEST_VERSION` to `"1.0.23 2025-02-26"`.


## Release 2.2.27 2025-02-24

Enhance and make more sane the FTS functions even more.

The `struct fts` now holds a `struct dyn_array *ignore` list of paths to ignore.
The function `read_fts()` checks for ignored paths and if one is encountered it
is skipped. This uses the same logic as that of `find_path()` and `find_paths()`
when looking for matches of filenames (not to be ignored).

The `read_fts()` function handles the ignored files based on the booleans (in
the `struct fts`) `base` and `match_case` (in the `struct fts`).

The `read_fts()` function now checks the depth (instead of the `find_path()` and
`find_paths()` functions that use it).

In order to make the `reset_fts()` function saner it now requires another arg
and one also should `memset()` it to 0 prior to calling `reset_fts()`. Doing
this (with the new `bool` in `struct fts` called `initialised`) allows one to
close the stream if it's not NULL (say because they used `read_fts()` without
finishing the loop, leaving the stream in a bad state). It also allows to check
the ignored list - deciding to free it or not based on the new bool
(`free_ignored`). Having these updates is a trade-off (having to now use
`memset(3)` prior to calling `reset_fts()` for the first time) but one that is
worth it and good practice anyway.

Fixed some comments in the functions (or above them) and removed some duplicate
comments as well.

Updated `JPARSE_UTILS_VERSION` to `"1.0.24 2025-02-24"`.
Updated `UTIL_TEST_VERSION` to `"1.0.21 2025-02-24"`.

## Release 2.2.26 2025-02-23

Updated `jsemtblgen.c` for recent change to `json_sem.[ch]`.

Internal improvement to `find_path()` and `find_paths()` functions: don't
repeatedly call `lstat(2)`/`stat(2)` per file for the `FTS_DEFAULT`
case: instead use the function `type_of_file()` once which will at most
call `lstat(2)` once and if that is not used it will call `stat(2)` once. We
can then check the file type as a simple comparison.

Updated `JSEMTBLGEN_VERSION` to `"1.2.2 2025-02-23"`.
Updated `JPARSE_UTILS_VERSION` to `"1.0.23 2025-02-23"`.

## Release 2.2.25 2025-02-22

Significantly simplify use of FTS functions.

Refactored the functions `read_fts()`, `find_path()`, `find_paths()` and added
helper function `reset_fts()` (which takes a `struct fts *`). A **significant**
amount of thought was put into this to make these much more useful and easier to
use. The real gain of this, besides more modularity, is that it will not force
users to update function calls if something has to change (with more use cases a
problem could be discovered, for example). The number of arguments is
**significantly** reduced.

With the function `reset_fts()` one can reuse the same structure if they need to
(it also allows one to simply initialise things to a sane value prior to using
the functions). This does set the tree (`fts->tree`) to NULL which means that if
one is using `read_fts()` and does not finish the `do..while` loop (thus the
stream is not closed) then they must use `fts_close()` on the stream before
`reset_fts()` as otherwise the stream will be lost. It was done this way quite
deliberately: the function cannot safely check for not NULL and it is a burden
to force the user to set the stream to NULL (the struct does not need to be
allocated on the stack although one could if they wanted to). The `find_path()`
and `find_paths()` functions do take care of this, whether or not they finish
the loop.

Made it possible to not get the absolute path of a found path in `find_paths()`
even if `dir == NULL && dirfd < 0` (new boolean) (in fact the `dir` and `dirfd`
now play no part in this). The absolute path code was also fixed by now
obtaining it AFTER the first call to `read_fts()`. Previously it did it before
which means that if `dir` was not NULL or `dirfd` was >=0 and either
`chdir(dir)` or `fchdir(dirfd)` succeeded the path could be very wrong. As we
only need the absolute path temporarily the variable is freed prior to
returning.

The `read_fts()` function is now simply:

```c
FTSENT *read_fts(char *dir, int dirfd, int *cwd, struct fts *fts);
```

The args `dir`, `dirfd` and `cwd` are the same. The new arg `fts` is a pointer
to a new struct `fts` (see util.h) which has everything that the older versions
had but as args to the functions.

The function `find_path()` is now simply:

```c
char *find_path(char const *path, char *dir, int dirfd, int *cwd, bool abspath,
    struct fts *fts);
```

(It now returns a `char *` not a `char const *` as it should always have done
due to the fact one should free the returned value when they're done with it.)

The args are the same as before with the addition of the `bool abspath`. The
function still uses the `read_fts()` function; some of the `struct fts` is used
there and the rest in the `find_path()` function itself.

The function `find_paths()` is now simply:

```c
struct dyn_array *find_paths(struct dyn_array *paths, char *dir, int dirfd, int *cwd,
    bool abspath, struct fts *fts);
```

The args `dir`, `dirfd` and `cwd` are what they used to be. The new struct has
all the parameters that used to be args (just like the other two functions). The
boolean `abspath` functions like in `find_path()` (see note below on when the
absolute path is obtained, below). The same rule about which function uses which
members in the `struct fts` applies to this function too, just like with
`find_path()`.

Updated `JPARSE_UTILS_VERSION` to `"1.0.22 2025-02-22"`.
Updated `UTIL_TEST_VERSION` to `"1.0.20 2025-02-22"`.


## Release 2.2.24 2025-02-21

Update and add utils.

Yes I am afraid that more things were thought of, particularly when using some
of the functions. Unfortunately making the assumption of using `strcmp()` or
`strcasecmp()` when looking for files is wrong as some file systems (like the
default here - macOS) are case insensitive and sometimes one might or might not
want to have better control (like the use case). Thus the functions ought to
have an option to specify if you want to make a case-sensitive or
case-insensitive search (if the boolean is false it means case insensitive).
This applies to `find_path()`, `find_path_in_array()`, `find_paths()`,
`append_path()` and the new function `array_has_path()` (which takes a `intmax_t
*` which if not NULL will be set to either `-1` or the index in the array). Yes
the `find_path*()` functions have **way too many** args and it would be good if
they can be refactored but that will have to happen another time.

As I needed an easier way to test the above searching I added the functions
`touch()` and `touchat()` which are analogous to the `touch(1)` command (except
it does not do anything if the file does already exists) and `openat(2)`. That
means that `touchat(2)` uses `touch()` which uses `open(2)` with the flag
`O_CREAT`. Importantly it is not like `creat(2)` which will truncate the file if
it already exists. It takes a `mode_t mode` to set the mode.

Updated .gitignore with some paths that the test code makes or copies (and now
`make clobber` also removes those).

New function `paths_in_array()` which counts the number of paths in the array
(if not NULL) - i.e. it uses `dyn_array_tell()` but only if not NULL; if the
array is NULL it simply returns 0.

Added `data` (a `void *`) to struct `json_sem` and `json_sem_check()`. This may
be NULL so it is the responsibility of the user to check this before using it.

Updated `JPARSE_UTILS_VERSION` to `"1.0.21 2025-02-21"`.
Updated `UTIL_TEST_VERSION` to `"1.0.19 2025-02-21"`.


## Release 2.2.23 2025-02-20

New util function to find a path in an array.

This new function is `find_path_in_array()` which will attempt to find a path in
a `struct dyn_array *` (presumably of paths). If `idx` (an `intmax_t *`) is not
NULL we first set `*idx` to `-1` and then if the path is found `*idx` will be
set to the index. The boolean `empty` allows for empty paths to match (if the
path and the path in the paths array are both empty and was added due to the
fact that `append_path()` also uses it and that is a special feature of the
`find_path*()` functions). The new function will be used elsewhere. (Yes we do
realise that a lot of the functions in util.c are not related to JSON parsing
but there are historical reasons for this.)

Updated `JPARSE_UTILS_VERSION` to `"1.0.20 2025-02-20"`.
Updated `UTIL_TEST_VERSION` to `"1.0.18 2025-02-20"`.


## Release 2.2.22 2025-02-19

Fix bug in `dir_name()` with `level < 0` with many test cases added to the test
code.

Updated `JPARSE_UTILS_VERSION` to `"1.0.19 2025-02-19"`.
Updated `UTIL_TEST_VERSION` to `"1.0.17 2025-02-19"`.


## Release 2.2.21 2025-02-18

Enhance (w/o changing args) `find_path*()` functions.

The enum `fts_type` now has the any type `FTS_TYPE_ANY` as not as 0 but the
bitwise OR of all the others so that (for example) if you want to get all types
except directories you could do `FTS_TYPE_ANY &
~FTS_TYPE_DIR` or so (this is done in the test code).

Added helper macros for setting/testing bits. In the case of the above example
that won't be helpful but since multiple places in the code now require removing
and setting bits it is useful.

Added new function `type_of_file()` which will return an enum (`file_type`)
based on the type of file (if `errno == ENOENT` it will be `FILE_TYPE_ENOENT`
and if errno is something else it will just be `FILE_TYPE_ERR`, otherwise it'll
be the type of file according to `lstat(2)` - and if not a symlink, `stat(2)`).

As a helpful feature of the `find_path*()` functions: if `dir == NULL` and
`dirfd < 0` it will return the absolute paths instead of the relative path. It
is possible that this should be done in all cases but that is TBD another time.

A bug was uncovered in a function and a `FIXME` was put in there. It will be
looked at soonish.

Updated `JPARSE_UTILS_VERSION` to `"1.0.18 2025-02-18"`.
Updated `UTIL_TEST_VERSION` to `"1.0.16 2025-02-18"`.


## Release 2.2.20 2025-02-17

A couple fixes and further enhancements with `find_path*()` functions. More use
cases have come to mind and the function arg ordering now matches `read_fts()`
to make it slightly less confusing (though there are still too many args they're
necessary).

It is now possible to specify file type in the `find_path*()` functions. There
is a new enum `fts_type`. This enum is composed of bits so one can OR them
together if they want more than one kind of file. The test code looks for some
of these and it is an error if there ARE any of them.

The enum is as follows:

```c
/*
 * enum for the find_path() functions (bits to be ORed)
 */
enum fts_type
{
    FTS_TYPE_ANY        = 0,        /* all types of files allowed */
    FTS_TYPE_FILE       = 1,        /* regular files type allowed */
    FTS_TYPE_DIR        = 2,        /* directories allowed */
    FTS_TYPE_SYMLINK    = 4,        /* symlinks allowed */
    FTS_TYPE_SOCK       = 8,        /* sockets allowed */
    FTS_TYPE_CHAR       = 16,       /* character devices allowed */
    FTS_TYPE_BLOCK      = 32,       /* block devices allowed */
    FTS_TYPE_FIFO       = 64        /* FIFO allowed */
};
```

The new function prototypes are:

```c
FTSENT *read_fts(char *dir, int dirfd, int *cwd, int options, bool logical, FTS **fts,
        int (*cmp)(const FTSENT **, const FTSENT **), bool(*check)(FTS *, FTSENT *));

char const *find_path(char const *path, char *dir, int dirfd, int *cwd,
        int options, bool logical, enum fts_type type, int count, int depth,
        bool base, bool seedot, int (*cmp)(const FTSENT **, const FTSENT **),
        bool(*check)(FTS *, FTSENT *));

struct dyn_array *find_paths(struct dyn_array *paths, char *dir, int dirfd, int *cwd,
        int options, bool logical, enum fts_type type, int count, int depth,
        bool base, bool seedot, int (*cmp)(const FTSENT **, const FTSENT **),
        bool(*check)(FTS *, FTSENT *));
```

The `dir` was changed to a `char *` from a `char const *`. This is not needed
now but there was an enhancement in mind that there is no time to do right now
(and it does not hurt to have it non-const so it's okay to keep it this way for
now). The reason for the reordering (in addition to the new parameter) is to
make it match the order in `read_fts()` a bit better.

If one wanted to only find directories of a given name, they would just use
`FTS_TYPE_DIR`. But if files and directories are okay they would do
`FTS_TYPE_DIR|FTS_TYPE_FILE` (for example). If any type is okay they can do
`FTS_TYPE_ANY`.

The functions (when checking the type) now also refer to `FTS_DP` for the case
that `FTS_LOGICAL` is used (though it should be seen with `FTS_D` anyway).

A new feature of the `find_path*()` functions is that if the string to find is
an empty string (i.e. `*str == '\0'`) it will find any file. This is useful if
you just need to get an entire listing (and if you need to restrict it to
specific types you can use a type other than `FTS_TYPE_ANY`).

Soon these functions should be in better use so it will be known if anything
else has to be changed but that (at this time) seems unlikely (despite the fact
there have been multiple enhancements already).

There is a new function which simplifies the `find_paths()` function (and use of
it) as well:

```c
void free_paths_array(struct dyn_array **paths, bool only_empty);
```

The `only_empty` boolean is useful because it is now possible to exit the loop
early (making sure of course to close the FTS stream and setting it to NULL) and
still return a valid array. This was actually used in more than one location in
the function but it now is only used at the end (if one needed to add a way to
exit early it would now allow not duplicating code). It is also used to make the
test code simpler. It takes a `struct dyn_array **` so that we can set it to
NULL in the caller (if needed). This makes it a lot easier. Functionally it can
be used with any `dyn_array` of `char *` but it is named `free_paths_array()` as
it is far more likely that there is a `free_array()` function than
`free_paths_array()`.

Relax the error to a warning if an invalid FD is passed to `read_fts()` (when
trying to close it, that is).


Updated `JPARSE_UTILS_VERSION` to `"1.0.12 2025-02-17"`.
Updated `UTIL_TEST_VERSION` to `"1.0.15 2025-02-17"`.


## Release 2.2.19 2025-02-16

Improve and bug fix various util functions.

Set `FTS_SKIP` on `FTS_DC` instead of using `err()`.
Set `FTS_SKIP` on `FTS_DNR` instead of using `err()`.

The above two (as well as the check for `FTS_ERR` and `FTS_NS`, both of which
are errors) are done in the new function (a callback that can be passed to the
three functions `read_fts()`, `find_path()` and `find_paths()` - if NULL it
defaults to the `check_fts_info()`). This was made a callback as some users
might have a different requirement (for instance the IOCCC requires that only
files and directories are allowed so `FTS_DEFAULT` is forbidden). If (in
`read_fts()`) this function returns false the next entry is read (if there is
one, else it'll return NULL like before).

Besides the new function pointer, there is another arg (sorry, tm Canada 🇨 :-) )
to the functions `read_fts()`, `find_path()` and `find_paths()` which fixes a
problem. The `fts_open()` needs either `FTS_PHYSICAL` or `FTS_LOGICAL`.  This
new boolean is `logical`: if logical (i.e. dereference/follow a symlink, meaning
act on the file it points to) is true then `FTS_LOGICAL` is set; otherwise
`FTS_PHYSICAL` is set and the `FTSENT *` will refer to the symlink itself. An
important point is that if `FTS_LOGICAL` is not set `fts_read()` will NOT detect
broken symlinks (this is how I discovered the problem); instead it'll just call
it a symlink. If you pass in `FTS_PHYSICAL` and/or `FTS_LOGICAL` to the options
in the functions it (or they) will be unset prior to setting the correct one.
The requirement that `FTS_NOCHDIR` still be in place is also there.

A word about yesterday's update on why we UNSET `FTS_NOSTAT`. IF this flag IS
set it makes the `fts_info` always be `FTS_NSOK` which means that we cannot
determine the file type! This would make the `read_path()` functions always
return NULL and it would also be useless for `read_fts()` too (in almost all
cases).

The new function `check_fts_info()` is:

```c
bool check_fts_info(FTS *fts, FTSENT *ent);
```

and you can use it (in a call to `read_fts()`) like:

```c

ent = read_fts("test_jparse", -1, &cwd, -1, &fts, fts_cmp, fn, true);
```

where `fn` is your function. Alternatively if you do:

```c
ent = read_fts("test_jparse", -1, &cwd, -1, &fts, fts_cmp, NULL, true);
```

The function `check_fts_info()` (as noted above) does certain checks on the
`fts_info` but even so the `read_fts()` function does the checks afterwards, in
case you override it and do not provide the right checks.

The util test code now checks paths that are non-existent (or should not exist
anyway).

The util test code now checks that `FTS_NOSTAT` is indeed unset.

Another new function, `fts_path()`, takes an `FTSENT *` and determines what should be
used for `fts_path` (it depends on the length of the string). This removes the
need of always calculating an offset; instead of that just use `fts_path(ent);`
(obviously you can assign it to a `char *`).

It is hoped that these functions should not have to change again but as the
functions are being used more is discovered in what is necessary. Thus there
might be another update or two. In the end these should be very useful
functions (though some might question why this is in jparse - and the answer is
related to the IOCCC and out of the scope of this changelog).

There was a bug fix in `is_symlink()`: by an oversight it used `stat(2)` instead
of `lstat(2)`.

Updated `JPARSE_UTILS_VERSION` to `"1.0.11 2025-02-16"`.
Updated `UTIL_TEST_VERSION` to `"1.0.14 2025-02-16"`.

## Release 2.2.18 2025-02-15

Renamed and improved the `find_file*()` functions to `find_path*()` (i.e.
`find_path()` and `find_paths()`).

The functions now allow you to collect path name(s) of regular files,
directories, symlinks and others that match `FTS_DEFAULT` rather than just
files.

The functions also have a new boolean which is an analogue to the `FTS_SEEDOT`
flag to `fts_open()`: if it's true and the files are `.` or `..` then it will
not be skipped; otherwise if it's false they will be (we search with a
`path_argv` of `"."` so they are seen even without the `FTS_SEEDOT` option).

There was a fix with the references to `fts_path` (in particular `fts_path + 2`)
as not all path names are long enough. The function `append_path()` was renamed
from `append_filename()` (this is where the problem was originally discovered).

Additionally the functions (the two and also `read_fts()`) now remove the option
`FTS_NOSTAT` as this is problematic to not have `stat(2)`.

The `util_test` now adds to the list of paths a couple directories.

Updated `JPARSE_UTILS_VERSION` to `"1.0.10 2025-02-15"`.
Updated `UTIL_TEST_VERSION` to `"1.0.13 2025-02-15"`.


## Release 2.2.17 2025-02-14

Make a new function like `find_file()` but which is more useful: it is now
possible to find a list of files (names in a `struct dyn_array` of `char *`s).
The function is `find_files()` and it is almost exactly the same as
`find_file()` but the `filename` arg is a `struct dyn_array *filename` and it
returns a newly allocated `struct dyn_array *`.

The other function is `append_filename()` which is used by `find_files()` and
can also be used to set up the function (i.e. to create the filenames array). It
takes a `struct dyn_array **` and if `*array` is NULL it allocates a new array
(if `array` is NULL it is an error); otherwise it will append to the array
that's already (hopefully) created. If it makes a new array it will use the
chunk size of 64. It also takes two bools: `unique` which means don't add the
filename if it's already in the array and `duped` which means that the string
was dynamically allocated (if it's false it will be `strdup()`d first).

Fix memory error in `find_file()`: return a `strdup()`d copy of the FTS entry
rather than the entry itself as once the function closes the stream prior to
returning.

Updated `JPARSE_UTILS_VERSION` to `"1.0.9 2025-02-14"`.
Updated `UTIL_TEST_VERSION` to `"1.0.12 2025-02-14"`.


## Release 2.2.16 2025-02-13

New util function `read_fts()` which allows one to use `fts_open()` and
`fts_read()` in a 're-entrant' way (not strictly true: one always has to pass to
`fts_read()` the `FTS *`) an more importantly allowing one to not have to rely
on using `fts_open()`, checking for NULL, then in a loop using `fts_read()` etc.
The args are as follows:

```c
 /*
 *  dir     -   char * which is the path to chdir(2) to before opening path but
 *              only if != NULL && *fts == NULL
 *  dirfd   -   if dir == NULL and dirfd > 0, fchdir(2) to it, else don't change
 *              at all
 *  cwd     -   if != NULL set *cwd PRIOR to chdir(dir)
 *  options -   options to pass to fts_open()
 *  fts     -   pointer to pointer to FTS to set to return value of fts_open()
 *  compar  -   if != NULL use it for the compar() function in fts_open(), else
 *              use fts_cmp() (see also fts_rcmp())
 */
```

One can use the function like:

```c
    FTS *fts = NULL;                    /* FTS stream for fts_open() */
    FTSENT *ent = NULL;                 /* FTSENT for each item from read_fts() */

    ent = read_fts(NULL, -1, NULL, FTS_NOCHDIR | FTS_PHYSICAL, &fts, fts_cmp);
    if (ent == NULL) {
        /* handle error */
    }  else {
        do {
            /* do stuff per entry */
        } while ((ent = read_fts(NULL, -1, NULL, FTS_NOCHDIR | FTS_PHYSICAL, &fts, fts_cmp) != NULL);
    }
```

or so, san typos.

As a useful feature if you call `fts_read()` like:

```c
read_fts(NULL, -1, &cwd, -1, NULL, NULL);
```

and `*cwd` (an `int`) is `>= 0` it'll try doing `fchdir(*cwd)` to restore the
directory to where it started, in case a change happened (as is quite likely).

The function `fts_cmp()` is like `strcmp()` with the full path of a file (from
the directory `.` - see below) and the function `fts_rcmp()` is the opposite of
`strcmp()` on the full path.

The function (`read_fts()`) does check for specific error conditions. For more
details read the comments or the source (until eventually the functions in
util.c are documented better).


Added function `find_file()` which uses `read_fts()` to find file by name (base
name or full path) from a directory (or current working directory if not
requested) at a specified depth (if > 0). It is:

```c
extern char const *find_file(char const *filename, char const *dir, int dirfd, int *cwd, bool base,
        int (*compar)(const FTSENT **, const FTSENT **), int options, int count, short int depth);
```

and the `filename` arg is the name to look for, the `dir` is the directory
(name) to switch to (if not NULL), `dirfd` is the directory FD to switch to (if
dir is NULL or `chdir(dir)` fails), `base` indicates whether to look by basename
or full path (relative to directory, which if dir == NULL && dirfd < 0 we do not
change directory at all), `compar()` is the same as in `read_fts()` (not
required, one may simply use NULL just  like in `read_fts()`), options are the
options to pass to `read_fts()` (if <= 0 it's set to `FTS_NOCHDIR`, otherwise we
OR options with `FTS_NOCHDIR`), `count` allows one to control which number to
match (that is if there are two files with the same match and you want the
second one if you use count == 2 it'll not find the second one; if count <= 0
this check is skipped) and `depth` allows one to find files at a specific depth
(`<= 0` means skip this check). Before returning the FTS stream is closed and
the original directory is restored. The `cwd` if not NULL will be set (in
`read_fts()`) to the current working directory in case one needs to restore it.
If it's not NULL this does mean the FD is left open until it is closed.

Added function `has_mode()` which is similar to `is_mode()` but it checks that
any of the bits are set (i.e. `stat.st_mode & mode`).

`util_test` tests all of these functions.


Updated `JPARSE_REPO_VERSION` to `"2.2.16 2025-02-13"`.
Updated `UTIL_TEST_VERSION` to `"1.0.11 2025-02-13"`.


## Release 2.2.15 2025-02-12

New util functions to detect other file types (besides directories and regular
files) and to check if the path's mode (as in `stat.st_mode`) is an exact mode
(based on the file type) as well as one to return the mode of a path. The
following functions have been added:

```c
extern bool is_socket(char const *path);
extern bool is_symlink(char const *path);
extern bool is_chardev(char const *path);
extern bool is_blockdev(char const *path);
extern bool is_fifo(char const *path);
extern bool is_mode(char const *path, mode_t mode);
extern mode_t filemode(char const *path);
```

The util test code now uses `filemode()` and also `is_mode()` as well as
`is_chardev()` (informative only). This completes the '`is_*()` file type
functions and these functions were added primarily to help with `is_mode()` but
there are other reasons besides.

Update `JPARSE_UTILS_VERSION` to `"1.0.7 2025-02-12"`.
Updated `UTIL_TEST_VERSION` to `"1.0.10 2025-02-12"`.

## Release 2.2.14 2025-02-05

New util function `mkdirs()` (using `mkdir(2)`) which acts as `mkdir -p` with
specific modes (uses `chmod(2)` as it's not affected by the umask and also
because anything but permissions set with `mkdir(2)` is undefined). If the first
arg (an int) is `-1` (actually < 0) it uses the current working directory to
start out with; but one can pass a file descriptor of the directory to start out
with. The mode is only set on directories that are created (i.e. no error)
because otherwise an already existing directory could have its mode changed.
Just like with `mkdir(2)` one must be careful with the mode. Of course if one
sets a mode like 0 then trying to work under it would be a problem but that's on
the user. If there is an error in creating a directory then it only aborts if
errno is not `EEXIST` (already exists) so that it can continue (just like `mkdir
-p`).

`make clobber` in `test_jparse/Makefile` now removes the test directories
created by `util_test` (which creates a directory tree of `test_jparse/a/b/c`
and `test_jparse/a`).

Updated `JPARSE_UTILS_VERSION` to `"1.0.6 2025-02-05"`.
Updated `UTIL_TEST_VERSION` to `"1.0.9 2025-02-05`.


## Release 2.2.13 2025-02-03

Improve `copyfile()` function so that it can now, depending on a boolean, copy
the stat `st_mode` of the source file to the destination file (like a true copy)
or otherwise, if the boolean is false, set the mode specified. In the case that
the mode is copied we do an extra sanity check to make sure that source
`st_mode` is the same as dest `st_mode` but this is not possible when setting
the mode to a specific value so that extra sanity test (which probably is not
even necessary) cannot be done; the caller would have to do this (as the util
test code actually does for all of these).

Other fixes were applied like making it an error if the source file does not
exist or is not a regular readable file rather than warning.

Use (in `copyfile()`) `errp()` in some cases where it was `err()` (when we had
`errno`). Also in case of `errp()` use `strerror(errno)`.

Updated `JPARSE_UTILS_VERSION` to `"1.0.5 2025-02-03"`.
Updated `UTIL_TEST_VERSION` to `"1.0.8 2025-02-03"`.


## Release 2.2.12 2025-02-02

Added new util function `copyfile()` which takes a source (`char const *`) and
dest (`char const *`) file (paths) (and a `mode_t`) and copies the source into
the dest, assuming that src file is a regular readable file and the dest file
does not exist. If the number of bytes read is not the same as the number of
bytes written, or if the contents of the dest file is not the same as the
contents of the source file (after copying) it is an error. If `mode` is not 0
it uses `fchmod(2)` to set the file mode.  This function does NOT create
directories but it can take directories as args, as long as they exist.

Updated `JPARSE_UTILS_VERSION` to `"1.0.4 2025-02-02"`.
Updated `UTIL_TEST_VERSION` to `"1.0.7 2025-02-02"`.

## Release 2.2.11 2025-01-31

Add new util function `path_has_component()` which takes two `char *`s: a full
path and a name to check against. This function will allow one to check a full
path to see if it has a specific component. This can be used along with
`sane_relative_path()` should one need to skip specific components such as
`.git`.

Updated `JPARSE_UTILS_VERSION` to `"1.0.3 2025-01-31"`.
Updated `UTIL_TEST_VERSION` to `"1.0.6 2025-01-31"`.


## Release 2.2.10 2025-01-26

Fix memory error in `count_comps()` (in `util.c`).

Updated `sane_relative_path()` to allow for the path to start with `./`. This is
necessary in some cases (whether it should check for `././` is another matter
entirely but in that case it is an error). A string starting with `.//` is not a
relative path as the first two characters being skipped will make it `/`. This
update allows for one to use `fts_open()` on `"."` which would prepend a `./` to
every filename. This process is done if the new boolean, the last parameter to the
function, `dot_slash_okay`, is true.

Updated util test code to test these new features.

Updated `JPARSE_UTILS_VERSION` to `"1.0.2 2025-01-26"`.
Updated `UTIL_TEST_VERSION` to `"1.0.5 2025-01-26"`.


## Release 2.2.9 2025-01-24

Bug fixes in `sane_relative_path()` to do with return value checks. Also the
`PATH_ERR_UNKNOWN` value (enum `path_sanity`) changed to `0` and `PATH_OK` to 1.

Updated `JPARSE_UTILS_VERSION` to `"1.0.1 2025-01-24"`.


## Release 2.2.8 2025-01-18

Fix warnings about args to `%x` specifier in `sscanf(3)` being `unsigned int
*`s, not `int *`s and explicitly added `-Wformat` to Makefiles.

Updated `JPARSE_LIBRARY_VERSION` to `"2.2.7 2025-01-18"` from `"2.2.6
2025-01-17"`.

Add missing `JPARSE_UTILS_VERSION` to various tools.

Updated man pages and code to show `version strings` (when referring to `-V` and
exit codes) as each tool has more than one version string. Minor detail but
worth noting. For `verge` a bit more care was taken to help distinguish version
args versus version of the tool, the utils version, the UTF-8 library version
and the jparse library itself. This is not very important but done to be more
correct.

Split usage message for `jsemtblgen` into two strings as it was quite long.
Updated `JSEMTBLGEN_VERSION` to `"1.2.1 2025-01-18"` from `"1.2.0 2024-10-09"`.

Added `verge` to `jparse_utils_README.md`. Other tools should also be documented
but more pressing matters delay will delay this further for some time.

Fixed `jparse_bug_report.sh` variable `TOOLS`. Each one was missing a `./` which
meant the tools had to be installed which makes that part of the script almost
useless.

Sequenced exit codes in util.c.


## Release 2.2.7 2025-01-17

Update `count_comps()` to have a boolean `remove_all` which will, if true,
remove all the trailing delimiter characters. The `count_dirs()` sets that
boolean to `false` now. To explain the fixes, we assume that the path is
`foo///` and the delimiter character is `/`: we now remove all but the last
trailing `/`. With this there is one component. In other words, if there are 0
delimiting characters it is 0. If however the string is `foo//foo` the middle
`//` is changed to "/" and it ends up being one component. But if it was
instead `foo//foo/` it would be two because of the trailing `/`. This allows
for properly counting directories at the same time as accounting for files in
the directories. For instance if there is a filename `bar` in the directory
`foo` then it is incorrect to say that there are two directories. On the other
hand, if one needs to count components in a different way, say because it's not
directories and they want all trailing delimiter chars removed, they can use
`true`.

Updated util test code quite a bit: when no error is encountered in the
functions added in the previous releases (`dir_name()`, `count_dirs()` and
`sane_relative_path()`) we print out the result as well. Also more test cases
were added to test the above change with how components are counted. An
additional test case was added for `count_comps()` with `remove_all` set to
true.

Fixed format warnings in various files so that we no longer need `-Wno-format`.
Fixing this allows for errors and suspect code (both types of issues were
corrected, both in `json_parse.c` and `json_utf8.c`).

Added new macro `JPARSE_UTILS_VERSION` set at `"1.0.0 2025-01-17"`. This is
strictly so that the JSON parser library version is not changed when something
not strictly related the jparse JSON parse(r) related routines are changed. In
other words, `json_util.c` is a jparse library version (like `jparse.l` and
`jparse.y`) change but a change in `util.c` is a change in the
`JPARSE_UTILS_VERSION`. The tools now refer to this in both `-h` and `-V`.

Updated `JPARSE_LIBRARY_VERSION` to `"2.2.6 2025-01-17"` (this was done prior to
adding the `JPARSE_UTILS_VERSION`).
Updated `JPARSE_UTF8_VERSION` to `"2.0.6 2025-01-17"`.


## Release 2.2.6 2025-01-13

Add new utility functions that act on directory paths:

- `dir_name()`: takes a path and strips off `level` components (i.e. `/`). In
the case of successive `/`s it removes those as well. The comments at the top of
the function explains in more detail the way the `level` works. This is modelled
after the `base_name()` function which functions as the `basename(3)` function;
`dir_name()` functions as `dirname(3)`.
- `count_comps()`: counts in a string the number of components delimited by the
component character (a `char`). Successive component characters are counted as
one. The comments at the top of the function details specifics.
- `count_dirs()`: using `count_comps()` (with component `/`), count the number
of directory components in a path.

Updated the utility test code to test the new functions.

Updated `JPARSE_LIBRARY_VERSION` to `""2.2.5 2025-01-13""` from  "2.2.4
2025-01-07".

Updated `UTIL_TEST_VERSION` to `"1.0.2 2025-01-13"` from `"1.0.1 2025-01-08"`.


## Release 2.2.5 2025-01-10

Improve comment in `sane_relative_path()` about the regexp, keeping in mind that
the locale does affect `[[:alnum:]]`.

Fixed typo in jparse.l (this means that jparse.ref.c was regenerated).


## Release 2.2.4 2025-01-08

Add comment about the regexp that `sane_relative_path()` should enforce, namely:

```regexp
^([[:alnum:]_+-]+/)*([[:alnum:]_+-]+(\.[[:alnum:]_+-]+))?$
```

and update the util test code to test additional checks.


## Release 2.2.3 2025-01-07

Updated comments in jparse.l and jparse.y and rebuilt backup files.

Updated sorry.tm.ca.h to be more specific on what files bison and flex generate.

`make install` now installs `jparse.y` and `jparse.l` into
`${PREFIX}/include/jparse` like other projects do with their bison and flex
source files. These files are deleted by `make uninstall` because the
subdirectory is deleted.

New utility functions:

```c
extern enum path_sanity sane_relative_path(char const *str, uintmax_t max_path_len, uintmax_t max_filename_len,
        uintmax_t max_depth);
extern char const *path_sanity_name(enum path_sanity sanity);
extern char const *path_sanity_error(enum path_sanity sanity);
```

which determine if a path is both relative and POSIX plus + safe, based on the
maximum depth, maximum path length and the maximum length of each component. An
enum was added:


```c
/*
 * for the path sanity functions
 */
enum path_sanity {
    PATH_ERR_UNKNOWN = -1,              /* unknown error code (default in switch) */
    PATH_OK = 0,                        /* path (str) is a sane relative path */
    PATH_ERR_PATH_IS_NULL,              /* path string (str) is NULL */
    PATH_ERR_PATH_EMPTY,                /* path string (str) is 0 length (empty) */
    PATH_ERR_PATH_TOO_LONG,             /* path (str) > max_path_len */
    PATH_ERR_MAX_PATH_LEN_0,            /* max_path_len <= 0 */
    PATH_ERR_MAX_DEPTH_0,               /* max_depth is <= 0 */
    PATH_ERR_NOT_RELATIVE,              /* path (str) not relative (i.e. it starts with a '/') */
    PATH_ERR_NAME_TOO_LONG,             /* path component > max_filename_len */
    PATH_ERR_MAX_NAME_LEN_0,            /* max filename length <= 0 */
    PATH_ERR_PATH_TOO_DEEP,             /* current depth > max_depth */
    PATH_ERR_NOT_POSIX_SAFE             /* invalid/not sane path component */
};
```

The new function `sane_relative_path()` returns one of those values depending on
the condition (except that it won't return `PATH_ERR_UNKNOWN` as that is for the
other two functions in the case one passes an invalid value). The function
`path_sanity_name()` returns a read-only string of the enum value that matches
the name (i.e. `"PATH_OK"` for `PATH_OK`). `path_sanity_error()` returns a
simple message based on the value passed in although there is some room for
improvement.

The markdown files have a `Last updated:` at the top now, to more easily keep
track of when the file was last modified.

The man page `jparse.3` was updated with clarifications of a function.

The utility test code has many new cases that tests every condition of
`sane_relative_path()`.

Updated version of `JPARSE_LIBRARY_VERSION` to `"2.2.4 2025-01-07"` from `"2.2.3
2024-12-31"`.


## Release 2.2.2 2025-01-04

Add `test_jparse/not_a_comment.sh` and update `test_jparse/prep.sh` to use it.
This will test if any Makefile.local files exist and if any do it'll warn at the
end so that the user of `make prep` or `make release` can be aware of it and be
sure it does not skew the results.

The script `not_a_comment.sh` is from the mkiocccentry repo and was
written by Landon Curt Noll (thanks!).

Fix `shellcheck` for `test_jparse/prep.sh` and add script to missing `SH_FILES`
in `test_jparse/Makefile` (the reason it was not caught by `make shellcheck` is
because the script was missing from the Makefile).


## Release 2.2.1 2025-01-02

Disable 2 invalid JSON encode/decode string tests in `jstr_test.sh`.


## Release 2.2.0 2025-01-01

Bug fixes to do with exit codes in `test_jparse/jparse_test.sh`. Some functions
being passed invalid data did not exit but rather change the exit code which
could then be changed by a function that runs later. Also in the case that a
test passed, in one location, it would change the exit code back to 0, thus
changing the result of a failed test back to not failing, giving a false result.
As the exit code starts at 0 now if any test fails it'll never be a 0 exit code
(though if an internal error occurs later the exit code won't indicate a test
failed, if there was one).


## Release 2.1.10 2024-12-31

Improve invalid JSON token error message (`yyerror()`)

In particular, instead of surrounding the bad token with `<>`s, which could be
one of the invalid chars and which might make someone think that that's the
problematic character, do not surround it any more. Since it is by itself after
the ':' (and the next char is the newline) it should be clearer this way.

Updated `JPARSE_VERSION` and `JPARSE_LIBRARY_VERSION` to `"1.2.8 2024-12-31"`
and `"2.2.3 2024-12-31"` respectively.

Fix typo in `test_jparse/Makefile` rule `rebuild_jparse_err_files` that caused a
display problem.

The `rebuild_jparse_err_files` rule **MUST NOT** use `-v ${VERBOSITY}` because
the error message files are specifically for -v 0 (the `jparse_test.sh` script
does not use `-v` in the error location tests at all)!

Add new JSON file and error location files
`test_jparse/test_JSON/bad_loc/party.json` and
`test_jparse/test_JSON/bad_loc/party.json.err`. This is necessary as it allows
another repo, the [mkiocccentry
repo](https://github.com/ioccc-src/mkiocccentry), to not have any problems and
still be able to test the party.json error file. This problem has to do with the
error message has to be exact and rebuilding the error location file was
problematic with the above updates.

To be more portable, don't use `-v` by default in `install` rule. If one needs
it, they can do `make INSTALL_V=-v install`.


## Release 2.1.9 2024-12-26

Invert the meaning of `jstrdecode -j`.

Now by default the tool will not validate the input as JSON. If that is needed
one should use the `-j` option (previously it disabled the check). This is
because it is not always needed, it can cause problems and it also is annoying
to have to always double quote the input (forcing something like `jstrdecode
'"foo"'`).

`jstrdecode(1)` version is now `"2.1.5 2024-12-26"`.


## Release 2.1.8 2024-12-22

Bug fixes in both `jstrdecode(1)` and `jparse(1)`.

There was a missing arg in `usage()` in `jstrdecode(1)` which caused a memory
error causing invalid output when `-h` was used.

In `jparse(1)` the `usage()` function referred to `json_verbosity_level` rather
than the default value of `JSON_DBG_DEFAULT`. What that means is that if one
were to have done `-J 5 -h` it the default would have been shown as 5 when in
fact it's 0.

Updated version of `jparse(1)` to `"1.2.7 2024-12-22"`.
Updated version of `jstrdecode(1)` to `"2.1.4 2024-12-22"`.

A note about why the repo release version only had the date changed: it's
because when updating documentation on 2 December 2024 I updated it but did not
document it here. But since it's not really necessary to note this (though it's
been done sometimes) it did not seem useful to add another entry in this file.


## Release 2.1.7 2024-12-02

Add more documentation about each `json_` struct and added some details to
`json_README.md` for JSON numbers.

Fixed some comments in `json_parse.h`.


## Release 2.1.6 2024-12-01

Modularise the `vjson_fprint()` function (for `-J level` of the parser) by
breaking each `JTYPE_` into its own function. The `JTYPE_NUMBER` already had
this and now the others do too, though the most important one is for
`JTYPE_STRING`.

Moved debug documentation from `jparse_utils_README.md` to
`jparse_library_README.md` as it is not limited to tool found here.

Also, the library documentation has been greatly expanded, in
`jparse_library_README.md`, although there certainly could be more done.


## Release 2.1.5 2024-11-30

Bug fix in JSON debug output for numbers. Due to a flow control problem, the
floating notation flag was always set to true if no errors were found and a `.`
was found in the string. This meant that the F flag was always present even for
non-floating point numbers.

Fix make clobber - add missing `jstr_util.o`.

Removed the `jstr_util.o` from compilation of `jstrdecode` and `jstrencode` as
it's not properly in the library.

Add back inclusion (in Makefiles) local Makefiles except it's now called
`Makefile.local`. This must be used with care and another change is pending
(though down the road) where the `make test` will first move any local
modification Makefiles to a temporary file to make sure tests can go okay
without any modified Makefiles interfering. The `jparse_bug_report.sh` now
checks for these files and cats them to the log/stdout in the case they exist.

Add details about JSON debug output flags (in other words if `-J level` used and
high enough level) for `JTYPE_NUMBER` and `JTYPE_STRING`, the types that have a
lot of flags that might mean very little to anyone who has not looked at the
code.


## Release 2.1.5 2024-11-25

Add another internal include possibility where one could (though this is
not recommended as it is recommended to install the `dbg` and
`dyn_array` libraries) include both `jparse` in their project which also
have `dbg/` and `dyn_array/` subdirectories (under `jparse/`).

More importantly, and this is why the change above is being committed
(it was done on a whim at the console when I had nothing better to do,
at the console, where I was for a few minutes, but it really has no
practical use), some of the checks for the internal include macros were
in source files rather than header files, and these have been moved to
the header files.

No functional changes were made and it is quite possible that no real changes
will be made for some time, though as they say 'time will tell'. There are some
changes in the pipeline, however, and that includes fixing some (possible) bugs
that were uncovered; it would also be ideal if `jstrencode` were to encode code
points to Unicode symbols.


## Release 2.1.4 2024-11-19

Remove `makefile.local` functionality in the Makefiles as this can cause
compilation problems and the purpose of the file is no longer needed. Should one
really want to override things they can pass it at the command line manually.
This change means that the `jparse_bug_report.sh` script has been updated as
well. This problem actually caused the mess of commits
bcb793e7d996da45fa9d47ae70084c9294eb1f2c and the commit that rolled it back,
bcb793e7d996da45fa9d47ae70084c9294eb1f2c, as when trying to fix a mess in
another repo that used it, by accident it was pushed to this repo, undoing fixes
from yesterday.

Added to `jparse.3` man page details about freeing a JSON parse tree (the
`json_tree_free()` function) and a new file (symlink) for it. Made some minor
improvements to the man page as well.

Removed unused macro `JSON_DBG_LEVEL` from `json_util.h`.

## Release 2.1.3 2024-11-18

Improve function `parse_json_str()` and `parse_json()` by allowing for empty or
NULL strings in the following way: `parse_json_str()` will pass a NULL filename
to `parse_json()` which will keep the filename as NULL, which will, due to
updates in the error function, not show a file (the difference is that it now
checks that it's not NULL **AND** not a NUL string).
Thus to parse a string rather than a file, one can use either
`parse_json_str()` (a simplified version) or else `parse_json()` with a NULL
filename. Using an empty filename in the latter function will set it to `"-"` for
stdin, though it's important to realise that the function `parse_json()` acts on
a `char *`. If one needs to read in a file, they should instead use
`parse_json_file()` (if they have only a filename) or else `parse_json_stream()`
if they have a `FILE *`.

Renamed some internal functions that were too ambiguous with the `parse_json()`
family of functions. In particular the functions that parse specific JSON types
like `json_parse_string()` which used to be `parse_json_string()` (too similar
to `parse_json_str()` which has a very different purpose). All of the
`parse_json_()` functions were renamed to `json_parse_()` which also match the
conversion and allocation functions in name format.

Fixed `jparse.3` man page and added a new file (also a symlink like the others)
for `parse_json_str()`.

Fixed at least one issue with `jstrdecode(1)` by first using the jparse parser
on the string prior to decoding. This will solve the problem of input like
`'"foo\"'` not reporting an error (previously it would throw a warning but not
an error but it is NOT valid JSON). Furthermore something like `'"\"\"\""\'`
will now report an error:

```
syntax error node type JTYPE_STRING at line 1 column 9: <\>
ERROR[15]: main: invalid JSON
```

In the case that one needs to or wants to not validate the JSON first, they can
use the `-j` option. The `-J level` sets the JSON debug level.

Fix compilation error in linux for `util_test` - add missing `-lm` to
`test_jparse/Makefile`.

Updated the version of `jstrdecode` to: "2.1.3 2024-11-18".
Updated version of the jparse library to: `"2.2.0 2024-11-18"`.
Updated version of `jparse(1)` to: `"1.2.4 2024-11-18"`.


## Release 2.1.2 2024-11-17

The `-e` for `jstrdecode(1)`, only encloses each decoded arg in escaped
double quotes when there are multiple args.  If there is only one arg,
no escaped double quotes are added.

The usage message for `-Q` and `-e` for `jstrdecode(1)` has been clarified
as well as being in sync with the above mentioned `-e` change.

The new version of `jstrdecode(1)` is `"2.1.2 2024-11-17"`
and the library is `"2.1.1 2024-11-17"`.

Improved `test_jparse/jstr_test.sh` to show, when some tests fail,
showing what was expected vs the result found when what was expected
was a simple string (and not a file).

Added new tests to `test_jparse/jstr_test.sh` as per the above
changes to `jstrdecode(1)`.

Updated `jstrdecode(1)` man page according to the above.

The new version of `test_jparse/jstr_test.sh` is now "1.2.3 2024-11-17".

Added new JSON util function `open_json_dir_file()` in `json_util.c` that uses
`open_dir_file()`, which if that function returns non-NULL, it will attempt to
parse as a JSON file. If the file is valid JSON, it'll return a `struct json *`
tree, otherwise NULL. Any error condition in `open_dir_file()` will result in an
error in this new function and if filename is NULL it is also an error (even
though the other function checks this). Note that this new function does NOT
close the `FILE *` as the parse function does this, as long as it is not NULL.
The way the function checks if it is valid JSON, is by using the
`parse_json_stream()` function (see `jparse.l`). That parse function will close
the stream or `clearerr()` as long as no error occurs.

Added `UTIL_TEST_VERSION` to the `util_test.c` file (comes from util.c). Added new
test to test the new function `open_json_dir_file()` which now tests
`jparse.json`. If it fails it will be an error and workflows/make test will
fail. Can be run from both `test_jparse/` and the top level directory.


## Release 2.1.1 2024-11-16

Use `date` rather than `/bin/date` in `jparse_bug_report.sh`.


## Release 2.1.0 2024-11-15

Release version `"2.1.0 2024-11-15"` as a significant change to the jparse
library and some important fixes were made in `jstrdecode(1)` and
`jstrencode(1)` as well.

`jstrencode(1)` should and now does enclose the output with double quotes.
Another change had been made, due to examples, that although technically valid,
are misleading and this detail was forgotten/overlooked. This has been rolled
back but it's why the version of this tool was updated twice today.

`jstrdecode(1)` now requires, unless `-d` is given, double quotes to enclose
each string arg.

The new version of each of the tools `jstrencode(1)` and `jstrdecode(1)` is
`"2.1.1 2024-11-15"` and the library is `"2.1.0 2024-11-15"`.

The functions `json_decode()` and `json_decode_str()` take a new boolean,
`quote`, which if true, means that the string must start with a `"` and end with
a `"`. If the length is < 2 then NULL is returned. If the first character is not
a `"` or the last character is not a `"` it is an error.

The man pages have been updated, with a fix in one, and some fun injected into
them (that correspond to some fun injected into certain tools, though this fun
does not affect any functionality; it's merely in wording in a string), plus the
new option to `jstrdecode(1)` along with the removal of the misleading examples
in `jstrdecode(1)` and `jstrencode(1)`.

Add simplified interface/version for `parse_json()` called `parse_json_str()`
which does not take a filename arg. It uses `parse_json()` with the filename set
to `"-"`. An unfortunate problem is that the error messages in the parser do
refer to a filename and so the best option is to say stdin, even if it's not
always from stdin strictly speaking.


## Release 2.0.8 2024-11-14

Renamed `utf8encode()` to `utf8_to_unicode()` to be less confusing as although
converting a code point to unicode is called encoding (from all sources we have
seen) in JSON, according to encoders **AND** decoders out there, a code point in a
string should be converted to unicode.

A number of bugs have been identified in `jstrencode(1)` during discussion in
'the other repo' (or one of the 'other repos'). This is in `jstrencode(1)` now
(as of yesterday); prior to yesterday it was in `jstrdecode(1)` due to the
unfortunate swap in names. This swap happened because when focusing on issue #13
(the decoding - which turned out to be encoding - bug of `\uxxxx`) focus of the
fact that the `jstrencode(1)` tool is not strictly UTF-8 but rather JSON was
lost. The man page has had these bugs added so it is important to remove them
when the bugs are fixed. A new issue #28 has been opened for these problems.


## Release 2.0.7 2024-11-13

Swap encode/decode terminology again. This is because it refers to **JSON**
encoding/decoding. This issue arose when working on correcting converting a
codepoint to a Unicode symbol. It has been discovered that encoding **AND**
decoding `\uxxxx` should, according to the JavaScript `JSON` library(?), convert
it to the unicode symbol.

The above is the next step but it means that for now the
encoding/decoding of codepoints to unicode does NOT work for now. New
version for jstrencode(1), jstrdecode(1), jparse UTF-8 library, jparse
library and jstr_test.sh.


## Release 2.0.6 2024-11-09

After some discussion, part of the change to `fprint_line_buf()` from  yesterday
was rolled back. In particular, the `fprint_line_buf()` function should print,
as part of debug output, the raw data and not encoded/decoded strings. Thus if
`'\a'` is encountered it should print just `"\a"`, not `"\\a"`, and if `Ã` is
encountered then it should print `"\xc3\x83"`, not `"\\xc3\\x83"`, and so on.

The return value error fix was kept in place and as an extra sanity check before
checking `isprint()` check `isascii(c)`, though this might not matter in some or
maybe most cases.

Improve man page for `jstrencode` and `jstrdecode` with more examples, more
details and also reword what the programs do. The `jparse_utils_README.md` has
had the same sort of improvements. The source code and header files for these
two tools have also had the update (the summary of what the tool does).


## Release 2.0.5 2024-11-08

Important bug fixes in `fprint_line_buf()` in `util.c`, as follows. First, for
example, the text `"\xf0"` is NOT valid JSON but `"\\xf0"` is. However, that
function printed the former, not the latter, and thus if one were to copy paste
the debug output to a file it would be invalid JSON. Second, there was a
miscalculation in the increment of count in one case, possibly due to the
earlier function.

What prompted this fix is that it was thought that it might be nice to have
debug output print unicode characters (emojis, non-ASCII characters etc.). Now
this might be nice with a new flag for the tools but this means modifying a lot
of functions and might or might not be worth it. What is definitely worth it is
these fixes, however, so that is done for now. If a new option is desired to
print unicode symbols then that can be considered later.

The `JPARSE_VERSION` was updated to `"1.2.1 2024-11-08"`.

The `JPARSE_LIBRARY_VERSION` was updated to `"2.0.1 2024-11-08"`.


## Release 2.0.4 2024-11-07

Removed `utf8decode()` from `json_utf8.c` as it appears we will not need it
after all.

Added extra sanity checks in `surrogates_to_unicode()` that warn if `hi` or `lo`
are `< 0`. This should theoretically never happen but the point is that if it
does happen it is warned about before returning `UNICODE_NOT_SURROGATE_PAIR`
(these would be triggered in the other check too but we do not warn about those
because the function tests whether it is a surrogate pair or not, so the only
function difference is that if either is `< 0` it is warned about as it should
never happen).

Updated `JPARSE_UTF8_VERSION` to `"2.0.3 2024-11-07"`.

Further improvements to `jparse_bug_report.sh`.


## Release 2.0.3 2024-11-05

Fixed `jparse_bug_report.sh` to not refer to the IOCCC except in the top
comment, and remove tool checks that are not needed for this repo. Because the
exit code `42` was by mere chance the `make all` the exit code sequence has been
kept the same by not updating the exit code sequence after removing tools not
needed here.

Reorganise documentation, moving utilities to a new file (renaming
`json_util_README.md`
[jparse_util_README.md](https://github.com/xexyl/jparse/blob/master/jparse_util_README.md)
to better reflect that they are related to jparse). Moved the details about
`jstrencode(1)` and `jstrdecode(1)` to this new file and moved the library
details from the `README.md` to the new file
[jparse_library_README.md](https://github.com/xexyl/jparse/blob/master/jparse_library_README.md).
This is to make the top level README.md file easier to go through.

Commented out all details about unwritten (for now) tools in the util README.md
file.

Reordered the utilities a bit.


## Release 2.0.2 2024-11-03

Remove unused macro from `json_utf8.h`. Updated `JPARSE_UTF8_VERSION`.

Fixed and improved some comments in `json_parse.c` and `jparse.l` and
`jparse.y`.


## Release 2.0.1 2024-11-01

Remove blockage of certain UTF-8 codepoints like non-characters as it appears
that these are allowed (in more recent versions of Unicode, if not before), even
if only used internally (in some cases). This means that the files previously
moved from test_jparse/test_JSON/good to test_jparse/test_JSON/bad have been
moved back to the `good/` subdirectory and thus every file that comes from the
[JSONTestSuite](https://github.com/nst/JSONTestSuite) is correct and matches.
These files are in particular:

	test_jparse/test_JSON/good/y_string_escaped_noncharacter.json
	test_jparse/test_JSON/good/y_string_last_surrogates_1_and_2.json
	test_jparse/test_JSON/good/y_string_unicode_U+10FFFE_nonchar.json
	test_jparse/test_JSON/good/y_string_unicode_U+1FFFE_nonchar.json
	test_jparse/test_JSON/good/y_string_unicode_U+FDD0_nonchar.json
	test_jparse/test_JSON/good/y_string_unicode_U+FFFE_nonchar.json

The new `JPARSE_UTF8_VERSION` is `"2.0.1 2024-11-01"`.

Expand `jparse.json` to include source files for our tools and to include
subdirectories and the tools/directories under that (in other words under
`test_jparse/`).

Fixed comments in some tools.

Fix format/display issue in `jstrdecode -h`.

Add missing `const char *const jparse_utf8_version` (assigned to
`JPARSE_UTF8_VERSION`) and rename `json_parser_version` to
`jparse_library_version` to match the `#define`d macro name, which was changed a
while back.

Improve comments in `jparse.l`, at least for those who are forced to read the
generated code, perhaps in a torture chamber or something like that, or for
those who want hallucinations or nightmares :-) or simply those who are really
curious what flex does.

Add extra sanity checks to `jstrencode(1)` and `jstrdecode(1)` when freeing the
lists. The function `free_jstring_list()` now takes a `struct jstring
**jstring_list` and if not NULL it will set `*jstring_list` to NULL, after
freeing the list, in case the caller does something silly. Even so, the two
tools now set the list to NULL after calling the free function.


## Release 2.0.0 2024-10-31

Major release. The tools `jstrencode(1)` and `jstrdecode(1)` have been swapped
as converting a code point to (say) an emoji is actually encoding, not decoding,
according to other sources. Most places that referred to encode were changed to
decode and those that were decode are now encode. This major change is quite
unfortunate but was deemed necessary; as we have said: sorry (tm Canada :-) ).

The version of the repo, the UTF-8 library, the jparse library, `jstrencode(1)`,
`jstrdecode(1)` have all been updated to `"2.0.0 2024-10-31"`.

Some duplicate man pages in `man/man3` are now symlinks to the source file,
`jparse.3`. This happened when the initial import to this repo occurred. Also,
the man pages for `jstrdecode` and `jstrencode` were swapped as well as fixed
(the terms were incorrect in those man pages wrt the tool).

Improve `jparse_bug_report.sh` for easier parsing. The script now quotes what is
about to be executed / what was executed. A few other changes were made and the
TOOLS variable was sorted as it should have been.

Fix segfault in `jparse -J level` when acting on strings.


## Release 1.2.9 2024-10-30

Add function `utf8decode()` (from the same location as the `utf8encode()` but
also modified to check for a NULL pointer.

Make the updates of pointers `p` and `utf8` and the `int32_t` bytes in
`decode_json_string()` the same order in the surrogate pairs as the single
`\uxxxx` when processing `\u` in strings.

Updated `JPARSE_UTF8_VERSION` to `"1.2.3 2024-10-30"`.

Fix `make install` to install `json_utf8.h`.

Improve `jparse_bug_report.sh` to test compile with `jparse/jparse.h` to try and
determine if the `dbg`, `dyn_array` and `jparse` libraries are installed.

Typo fix in `test_jparse/prep.sh`. A missing `G` in `LOGFILE`. This was a
carry-over from the mkiocccentry repo script.


## Release 1.2.8 2024-10-27

Remove duplicate code from jstrencode.c and jstrdecode.c as follows:

- The `parse_entertainment()` function is now in `jstr_util.c` as the purpose of
the file is for both jstrencode and jstrdecode.
- Added function `free_jstring_list()` in `jstr_util.c` and use that instead of
the unique functions (that do the same thing) in jstrencode.c and jstrdecode.c
(`free_json_encoded_strings` and `free_json_decoded_strings`).

Updated version of both tools to `"1.2.5 2024-10-27"`.


## Release 1.2.7 2024-10-26

Update `free_jstring()` to take a `struct jstring **jstr` rather than a `struct
jstring *`. The function now, if `jstr != NULL && *jstr != NULL`, frees the
struct from the calling function. This function still sets `jstr` to NULL in the
case that code were to for some reason be added to the function. The struct in
the calling code also sets it to NULL, even if redundant, as an extra sanity
check.

Updated the man page for jstrencode to point out a subtlety with the `-Q`
option.

Updated the `hexval` table to be of size `JSON_BYTE_VALUES`, even though that
table is not used (it might be deleted as obsolete but that has not been
determined yet).


## Release 1.2.6 2024-10-22

Add extra paranoia to `calloc_path()`: make sure calloc()d `char *` is zeroed
out.

Add code to test `util.c` code. At this time it only tests `calloc_path()` but
more tests can be devised. This code is called from
`test_jparse/run_jparse_tests.sh`. Updated the Makefile in `test_jparse` to
depend on this. This test code will only be compiled if one runs `make test`.

Fixed exit codes in `test_jparse/run_jparse_tests.sh`.

Add extra test to `jdecencchk()` in `json_parse.c`.

Add inclusion of `dbg.h` in `util.h`. This allows `make test` to work in the
rare case where jparse/ is a subdirectory, at least with one level deep.


## Release 1.2.5 2024-10-20

Fix comments in `json_utf8.c` that were an artefact of the previous way the
function `utf8len()` worked and check `bytes <= 0` in `json_parse.c` from
function `utf8len()` and `utf8encode()`.


## Release 1.2.4 2024-10-13

The codepoints `0xFF` and `0xFE` are valid where `0xFF` is [Latin small
letter y with
diaeresis](https://www.fileformat.info/info/unicode/char/00ff/index.htm) and
`0xFE` is [Latin small letter
thorn](https://www.fileformat.info/info/unicode/char/fe/index.htm).

Set `JPARSE_UTF8_VERSION` to `"1.2.2 2024-10-13"`.

Bug fix `test_jparse/jparse_test.sh` to show the file that failed in some cases
(an `$` was left off by accident).


## Release 1.2.3 2024-10-12

Add extra sanity check in `sum_and_count()` (see `util.c`).

Greatly improve the test suite with new script
`test_jparse/run_jparse_tests.sh` and a number of bug fixes and enhancements
in other places, as described below.

The script does what the `test_jparse/Makefile` `test` rule did but allows to
specify the path to certain tools (`jparse`, `pr_jparse_test`, `jnum_chk`) and
it allows for verbosity to be used (as in `jparse_test.sh -D`, although when
used via `make` it uses the Makefile `VERBOSITY` level, which is also used for
the `-v` flag), and so on.

A simple man page exists for this tool though there is not much to say as it
really is just a driver for the other test tools.

The `test_jparse/prep.sh` now sets the `VERBOSITY` variable to the `-v level`
specified so one can do `make VERBOSITY=2 test` or `make VERBOSITY=3 prep`, for
example.

The top level Makefile no longer says to check `build.log` with the rules
`slow_prep` and `slow_release` as in those rules the log file does not exist,
instead writing to `stdout` and `stderr`.

Sequenced exit codes.

Added man page for `pr_jparse_test`.

Fix stray 'OK' lines in build.log after `make prep`.

Improve README.md for `jstrencode(1)` and `jstrdecode(1)` and fix usage synopsis
of both `jstrencode(1)` and `jstrdecode(1)`.


## Release 1.2.2 2024-10-11

Add a bit of fun to `jstrencode(1)` and `jstrdecode(1)`.

Fix some man pages (`jstrencode.1`, `jstrdecode.1` and `jparse.1`).

Fix some output problems in `test_jparse/prep.sh` wrt `jparse_bug_report.sh`.


## Release 1.2.1 2024-10-10

Add option `-F` to `test_jparse/jparse_test.sh` which means that files passed on
the command line should be read as actual full JSON documents, rather than JSON
blobs per line. In this case `stdin` is **NOT** read! The file `jparse.json` is
now controlled this way.

Added new function `jdecencchk()` to `json_parse.c` which `jstrencode` and
`jstrdecode` use with the `-t` option. Beware of the dragon!

Changed `JSON_PARSER_VERSION` to `JPARSE_LIBRARY_VERSION`.

The `-V` and `-h` option of all tools (compiled tools, not scripts) now show the
jparse UTF-8 version as well as the tool's version and the library version. The
string format of the library version has been changed too (it now shows:
`"jparse library version: %s"`).

Improve the way `utf8len()` works. It now returns a `size_t` and the `size_t
*bytes` was removed from it. Returns `-1` if an error occurs. This better fits
the name and purpose of the function.

Add new option `-N` to `jstrdecode(1)`. This option ignores a final newline in
the input for easier typing of commands. It does not change the validity
checking of JSON.

Add new option `-N` to `jstrencode(1)`. This option ignores all newlines in
decoding data. It does not change the validity checking of JSON.


## Release 1.2.0 2024-10-09

Remove `has_nul` in `struct json_string` as UTF-8 should, it is my
understanding, not have a NUL byte.

Fix path in `jsemcgen.sh`.

Update all tools and the release to be the same version after issue #13 was
resolved: `1.2.0 2024-10-09`. `1.2.0` was chosen because it was the first one >
some of the versions and the others could be bumped up to it without any harm.

Do a final clean up of `json_utf8.[ch]`: removed all unnecessary code and macros
as well as clean up comments.

Added a `JPARSE_UTF8_VERSION` in order to keep track of the current UTF-8 code.
Set this version to the same as the other versions at this time: `1.2.0
2024-10-09`.

Rename `count_utf8_bytes` to `utf8len()` to better account for what it does.


## Release 1.0.23 2024-10-08

Fix surrogate pair decoding in `json_decode()` / `decode_json_string()`. Now one
can do for example:

```sh
jstrdecode "\ud834\udd1e"
```

(which is UTF-16 as `U+1D11E`) and expect to get:

```
𝄞
```

or:

```sh
jstrdecode "\ud834\udd1ef"
```

and get:

```
𝄞f
```

At this time the only issue with JSON decoding that I am aware of is with
characters that are `!isprint()`. Once that is resolved I believe that #13 will
be resolved.

Updated versions of `jstrencode`, `jstrdecode` and the JSON parser library for
this fix.

Formatted `json_utf8.[hc]` a bit. This includes removing duplicate `#define`s
and formatting comments. This does not include the changing of `type*` to `type
*` as much as we want that. A line has to be drawn somewhere, and we don't even
know every function we will have to keep.

```
-----------------
```

`^` The above is the line.

Reject certain UTF-8 codepoints. It is believed that issue #13 might be resolved
fully. Codepoints that were thought to be valid but not displayed might be
because they are not printable at the console. In a web page the following:

```html
&#x1e;
```

shows something but pasting it into this document shows nothing at all.

It is possible that #13 is not completely resolved but it seems in good shape at
this time, anyway, and unless something comes up, it might be declared good,
except that the files `json_utf8.[hc]` could be cleaned up to not have what we
do not need.

Add more encode/decode tests to `test_jparse/jstr_test.sh`.

Greatly simplify `json_utf8.[hc]` by removing code/macros not needed. It might
be possible to further reduce it but that will depend on at least one question
that still has to be thought out.

Removed `unicode` boolean in `struct json_string`.

Renamed `jencchk() to `chkbyte2asciistr()`.


## Release 1.0.22 2024-10-07

Improve `test_jparse/jparse_test.sh`. Don't show blank line at top of failure
messages (when more than one failure) and don't prefix each file/string with
`FILE:`/`STRING:` as this is redundant due to how it is displayed. Updated
script to version `1.0.7 2024-10-07`.

Add `dbg()` calls to `utf8encode()`.

## Release 1.0.21 2024-10-06

Fix test cases in `jstr_test.sh`. Extra bytes were being written due to the way
the size of the decoded string was being calculated. Instead a new function is
in `json_utf8.c` called `count_utf8_bytes()`. This does the same sanity checks
(though slightly different) in `json_decode()`. As well the for loop had to be
fixed (the increment stage and the `\uxxxx` parsing) in `decode_json_string()`.
With this change the `jstr_test.sh` script now passes all tests.

The above fixes required a modification to `jstr_test.txt` and its change is the
answer to the above fixes: it had extra bytes in it as all decoding with
`\uxxxx` did. After that problem was fixed (the `count_utf8_bytes()`) the loop
had to be fixed so that `char *utf8` (renamed from `offset`) is always
incremented by 1 (this is why in `decode_json_string()` we now have `utf8 +=
bytes - 1;` and `p += bytes;`).

The `byte2asciistr` table had to be updated as obviously some size differences
occurred.

The JSON parser version has been bumped to `"1.1.7 2024-10-06"`.

The issue #13 is NOT completely resolved as the recent commits do not address
sequences in `\uxxxx\uxxxx` and it also does not work for every `\uxxxx`
sequence.


## Release 1.0.20 2024-10-05

Add to `struct byte2asciistr` a `size_t` for the decoded length in order to
verify the decoded length is correct, since not every byte has the same decoded
length. This allows us to test the decoded length in the table.


## Release 1.0.19 2024-10-04

Fix the JSON decode bug #13. Using the function `utf8encode()` it appears that
the bug is now fixed. More testing does need to be done but numerous test cases
now seem to work.

The calculation of the length to allocate has been changed somewhat to get this
to work. The retlen had to be updated too, to refer to len, not mlen.

The check for the `unicode` boolean should be improved and it is not
even clear if it can remain the way it is (this is the part where it checks for
invalid codes).

Added test file `test_jparse/jstr_test.txt` to compare for a new decoding test
in the `jstr_test.sh` script.

A few tests in `jstr_test.sh` had to be temporarily disabled as in fixing the
decoding seems to have broken them. Whether another fix with decoding bug is
needed or a fix with the script itself is unclear at this time.

The `jstr_test.sh` exit codes have been changed. Man page updated.

Once the test cases have been enabled and the `unicode` boolean is implemented
properly, and assuming this bug is resolved, the file `json_utf8.c` and
`json_utf8.h` can be cleaned up.


## Release 1.0.18 2024-10-01

Improve `test_jparse/is_available.sh`: it now will do a trivial test on
`checknr`. This check is unlike the others in that if the tool returns 0 then it
is an error. This is because it checks that an erroneous man page is declared
invalid. The original version always returned 0 which meant that it was not as
easy to determine if it was in error (in the test suites - one had to run it
manually and look at the output). Thus to verify that it's reliable we check for
a non-zero status on a man page with an error in it.

The `prep.sh` script now skips the `check_man` rule if `checknr` cannot be found
or is unreliable.


## Release 1.0.17 2024-09-30

Make use of `test_jparse/is_available.sh` in the Makefiles and in
`test_jparse/prep.sh` script.


## Release 1.0.16 2024-09-29

Add a couple functions to `json_utf8.c` for UTF-8 encoding/decoding. These
functions are not currently used but might be when issue #13 is looked at more.

Improve and replace tabs with four spaces in `jparse.json`.


## Release 1.0.15 2024-09-25

Use `FOO_BASENAME` when `prog == NULL` in `usage()` functions of the tools
written in C.

Fix broken link in `jparse_bug_report.sh` and fix some formatting issues there
as well.

Add `jparse.json` file with information about this repo and what it consists of.


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
