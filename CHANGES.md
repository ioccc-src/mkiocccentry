# Major changes to the IOCCC entry toolkit


## Release 2.3.13 2025-01-19

Bug fix `noprompt_yes_or_no()` in `mkiocccentry.c` (it had missing
`not_reached()` after `err()` calls).

Bug fix `inspect_Makefile()` in `mkiocccentry.c` to skip built-in Makefile rules
(those starting with a '.') because if someone has, for instance, what we have
here and in jparse (`.NOTPARALLEL:`) it will confuse the checker and cause the
'all' rule to not be detected as first which puts the submitter at a big risk of
violating rule 17.

Change `MKIOCCCENTRY_VERSION` to `"1.2.3 2025-01-19"`.


## Release 2.3.12 2025-01-18

Increase the `OPTIONAL_SUBMISSION_FILES` to 3 due to the additional optional
file `try.alt.sh`.

Sync [jparse repo](https://github.com/xexyl/jparse/) to `jparse/` to fix some
bugs and add missing strings (to `-h` and `-V` in some tools). Also bug fixed
the `jparse_bug_report.sh` script (yes this is ironic).

## Release 2.3.11 2025-01-17

Bug fix `txzchk` to check for required files in top level directory only (with
the exception that dot files are not allowed in any subdirectory and the only
dot files allowed in the top level directory are `.info.json` and `.auth.json`).

Added new test files to `txzchk` test suite, both good and bad (with associated
`.err` files).

This fix involved a bug fix and enhancement to the jparse library (not the JSON
parser but the `util.c` file) which involved syncing the jparse repo again.

Updated `TXZCHK_VERSION` to `"1.1.4 2025-01-17"`.

Added new macro (in jparse) `JPARSE_UTILS_VERSION` set at `"1.0.0 2025-01-17"`.
This is strictly so that the JSON parser library version is not changed when
something not strictly related the jparse JSON parse(r) related routines are
changed. In other words, `json_util.c` is a jparse library version (like
`jparse.l` and `jparse.y`) change but a change in `util.c` is a change in the
`JPARSE_UTILS_VERSION`. The tools (in jparse and here) now refer to this in both
`-h` and `-V`.

Added even more bad `txzchk` test files for those with special bits (as anything
not marked as `-` or `d` in `ls -l`).

Reversed the argument order for `chkentry(1)`.  Now the `.auth.json` arg
comes before `.info.json`.  Now, one may do the following, assuming that
the `topdir` has the `.auth.json` and `.info.json` files:

```sh
chkentry topdir/.*.json
```

Major updates to `txzchk(1)`. It no longer tests for total files against the
maximum but rather extra files against the extra file maximum. As subdirectories
are now allowed the warning for more than one file with the same name depends on
the directory. It might be worth noting that when an identical filename is found
more than once it increments the count but it can cause confusing output exactly
because it is confusing (and should never happen unless someone is messing with
their tarball or has some system error). It also now checks for optional files
which do not count against extra files and neither do required files. In a
function a variable was no longer used so it has been removed and in some other
places code was no longer necessary (where it was) so said code was also
removed. Added a new test case file in both good and bad directories. Rebuilt
error files.

Possibly some bug fixes were made during the above.

Updated version of `txzchk` to `"1.1.5 2025-01-18"` and updated JSON files to
account for this.

New array in `soup/entry_util.c` which is of optional filenames (these only count
if they're in the top level directory).

Bug fix `bug_report.sh` to check for `Makefile.local`, not `makefile.local`.
This same change was done in other files that still referred to it with the
exception of this file and .gitignore. It was done in dbg/ and dyn_array/ but
the change needs to be done in the repos too.


## Release 2.3.10 2025-01-16

Added `-E` to `mkiocccentr(1)` to exit non-zero (1 in fact) on
the first warning.

Added `-s seed` to `mkiocccentr(1)` seed a pseudo-random generator,
then generate a `random_answers.seed` file with pseudo-random answers,
and then to use that as if `-i random_answers.seed` had been called with
pseudo-random answers.  This is useful as a **dry run** in that one does
not have go thru the interactive process of answering questions.

Added `-d` as an alias for `-s 21701`.

While an effective seed is any value from 0 thru 2147483647, we tested
`mkiocccentr -s seed` for seeds 0 thru 32767.

Fixed the message about use of the 'test" user to not refer
to sending Email.

Removed the need for `IOCCC_WINNER_HANDLE_READY` as it is now ready.
The hint now refers to the FAQ URL:

    https://www.ioccc.org/faq.html#find_author_handle

Fixed typos in `soup/location_tbl.c` where there were improper trailing
commas in strings.

Moved `MKIOCCCENTRY_ANSWERS_EOF` from `mkiocccentry.h` to
`soup/version.h`, even though it is not strictly a version related
string, because `MKIOCCCENTRY_ANSWERS_EOF` is used in conjunction with
`MKIOCCCENTRY_ANSWERS_VERSION` which is in `soup/version.h`.

Fixed `mkiocccentry -y` so that it prevents a yes/no prompt from being
used.  The use of `mkiocccentry -i answers` will ask the user to verify
the list of files for the submission unless `-y` is also given.

The highbit warning is not permanently set to `false` as we now allow
all UTF-8 in IOCCC source code.

Changed `MKIOCCCENTRY_VERSION` from "1.1.6 2025-01-13"
to "1.2 2025-01-16".

Changed `MKIOCCCENTRY_REPO_VERSION` from "2.3.9 2025-01-14"
to "2.3.10 2025-01-16".

Fixed `make all test; make test` failure.

Added to list of forbidden files `GNUmakefile`. This involved an update to the
`forbidden_filenames` array in `soup/entry_util.c` and making both `txzchk` and
`mkiocccentry` use that array. This means that the `MKIOCCCENTRY_VERSION` was
bumped again, making it `"1.2.1 2025-01-16"`. The `TXZCHK_VERSION` was updated
to `"1.1.3 2025-01-16"`. The test JSON files were updated for both of these
version updates. A new bad test file for txzchk was added along with its
appropriate err file.

Make `mkiocccentry_test.sh` test `-d` option (which uses `-s seed). Updated
version of script to `"1.0.4 2025-01-16"`.

Removed blank lines from `mkiocccentry.1` (they're not needed and some say it's
incorrect).

Make `mkiocccentry` random answers file prepend `http://example.com/` (and
`https://example.com/` to the URLs. The emails probably should be `@example.com`
too but that can be worried about another time.

Changed `MANDATORY_FILE_COUNT` to `MANDATORY_SUBMISSION_FILES` to clarify
the meaning of this value.  This value remains at 5.

Added `OPTIONAL_SUBMISSION_FILES` to list the 2 files that are
may be added without being counted an extra file.

Added `MAX_EXTRA_FILE_COUNT` to give the maximum number of extra files
for a submission.  This does **NOT** include mandatory files, nor does
it include optional files a submission.  This value as been set to 31.

Now `MAX_FILE_COUNT` refers to the maximum total file count, including
mandatory files, optional files, and extra files for a submission.

Added important comments to `soup/limit_ioccc.h` about
both submission and winning entry mandatory and optional files.

**NOTE**: The above file limits refer to submissions, not a winning entries
as IOCCC judges are free to add additional files to a winning entry as needed.

**NOTE**: As a result of the above, `MAX_FILE_COUNT` is 5+2+31 = 38,
an increase from the effective file count limit of 42-5 = 37 when
the mandatory file count was subtracted from MAX_FILE_COUNT in the code.


## Release 2.3.9 2025-01-14

Renamed `test_extra_file()` to better account for what it is:
`test_extra_filename()`. Improved the function to use two `char *[]`s so that we
don't have to have checks on each file manually in each tool that uses it,
though at present only `chkentry(1)` uses it (it is not clear if any other tool
can use it in the future but this update allows for more easily maintaining the
list of filenames that must be present and that must not be present).



## Release 2.3.8 2025-01-13

Work done on new `mkiocccentry` options `-d` and `-s seed` (both will be
documented when completed).

Sync [jparse repo](https://github.com/xexyl/jparse/) to `jparse/` with new
utility functions that act on directory paths. These functions are:

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

Updated the jparse utility test code to test the new functions.

Remove blacklisting of `inventory.html` file. This involved a new version of
`mkiocccentry`, `txzchk` and `chkentry` as all of these tools prevented such
files from existing.


## Release 2.3.7 2025-01-12

Removed `Makefile.example` as this file duplicates the contents of
[Makefile.example from "the other repo"](https://github.com/ioccc-src/winner/blob/master/next/Makefile.example).


## Release 2.3.6 2025-01-10

Update regexp error message in mkiocccentry. (This was done in jparse too and
that was synced from the jparse repo).

Fix check in mkiocccentry (<= 0, not < 0).

Minor bug fix in txzchk where errno was set to 0 after the function call.

Updated txzchk and mkiocccentry version which required updating JSON files too.

Slight update to GitHub issue template files to match the other one (and should
look nicer in the subject line).



## Release 2.3.5 2025-01-08

Update `mkiocccentry_test.sh` to test the new subdirectory option of
`mkiocccentry`, with the depth being valid and the depth being too high.

Fix regexp in error message for basename of files according to how
`sane_relative_path()` functions.

Sync `jparse/` from [jparse repo](https://github.com/xexyl/jparse/) with a
comment to `sane_relative_path()` (about regexp it enforces) and additional test
cases to the util test code.

Updated FAQ about how to use `mkiocccentry as well as the man page (with some
details on how the mkiocccentry will copy directories now). The FAQ, guidelines
and rules have to be updated in the winner repo and that'll come next unless
something comes up here.

## Release 2.3.4 2025-01-07

Submission tarballs can now have subdirectories! This is a major release that
will require updates to the guidelines, the rules and FAQ in the website (and
this will be done soon). To get this to work a new jparse library release was
also made, synced from the [jparse repo](https://github.com/xexyl/jparse/).
There are other changes as well but the relevant ones are:

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

The utility test code has many new cases that tests every condition of
`sane_relative_path()`.

Tools in this repo that were changed for this enhancement are `txzchk`,
`mkiocccentry` and `chkentry`. To include a subdirectory in a submission tarball
you may just specify the path and `mkiocccentry(1)` will do the right thing by
using `-r` to `cp`.

Maximum depth of 4 directories. Reduced size of max filename length (to 38, the
length of UUIDs) and max path length of a file is 99.

New test tarball files. Rebuilt error files.

Changed `SOUP_VERSION` to `"1.1.4 2024-12-31"`.
Changed `MKIOCCCENTRY_VERSION` to `"1.1.3 2025-01-07"`.
Changed `TXZCHK_VERSION` to `"1.1.0 2025-01-07"`.
Changed `CHKENTRY_VERSION` to `"1.0.4 2025-01-07"`.

## Release 2.3.3 2025-01-04

Added `soup/not_a_comment.sh` to test if file that exists
contains a non-#-comment.

Both `make prep` and `make release` will issue a notice (via
`soup/not_a_comment.sh`) if a non-empty `Makefile.local` is found
containing more than just comments.  This will allow for one to
keep a `Makefile.local` with only comments in it, without raising
a notice as such a file will not impact the make procedure.
Then if one needs to temporary add comments (perhaps by uncommenting
lines in a `Makefile.local` file), one can.  However if one then
forgets, then the notice will alert you to the potential problem.

Changed `test_ioccc/prep.sh` PREP_VERSION from "1.0.4 2024-11-16"
to "1.0.5 2025-01-03".

Changed `MKIOCCCENTRY_REPO_VERSION` from "2.3.1 2025-01-01"
to "2.3.3 2025-01-04".

Synced `jparse/` from [jparse repo](https://github.com/xexyl/jparse/) with the
new script `not_a_comment.sh` which is now used in `jparse/test_jparse/prep.sh`
as well. Thanks Landon! Doing this also uncovered a bug in
`jparse/test_jparse/prep.sh`: namely that shellcheck failed due to the fact the
script was missing from `jparse/test_jparse/Makefile` (this goes all the way
back to the initial import of `jparse` as there was no `prep.sh` at the time!).


## Release 2.3.2 2025-01-01

Fix exit code errors in `jparse_test.sh`.

Remove rule `rebuild_jparse_err_files` from top level Makefile. This rule should
only be in jparse/. The reason it would cause a failure is because of `argv[0]`
being in error messages and since in this repo `jparse` binary is in a
subdirectory it changes the name of the program so the error files are
incorrect. It might be possible to do a `cd jparse && ...` but this is not
necessary as there are no error location files in this repo itself due to
precisely this reason.

Happy New Year!

Disabled 2 invalid JSON string encode/decode tests.


## Release 2.3.1 2024-12-31

Fix a minor issue relating to the invalid UUID error message.

Changed `MKIOCCCENTRY_REPO_VERSION` from "2.3 2024-12-31"
to "2.3.1 2024-12-31".

Changed `SOUP_VERSION` from "1.1.3 2024-12-31"
to "1.1.4 2024-12-31".


## Release 2.3 2024-12-31

Improve `jparse` error messages in `yyerror()`.

In particular, instead of surrounding the bad token with `<>`s, which could be
one of the invalid chars and which might make someone think that that's the
problematic character, do not surround it any more. Since it is by itself after
the ':' (and the next char is the newline) it should be clearer this way.

Updated `JPARSE_VERSION` and `JPARSE_LIBRARY_VERSION` to `"1.2.8 2024-12-31"`
and `"2.2.3 2024-12-31"` respectively.

**NOTE**: there is **ABSOLUTELY NO** functional change in the above, just a
display improvement, and so it's a useful change to include here too.

Fix typo in `jparse`'s `rebuild_jparse_err_files` rule that was causing a
display problem. Also here the -v flag cannot depend on the `VERBOSITY` level as
the script was designed specifically for -v 0! This has been fixed.

Moved `party.json` and `party.json.err` files from
`test_ioccc/test_JSON/general.json/bad_loc` to the jparse repo so that it's now
under `jparse/test_jparse/test_JSON/bad_loc`. This had to be done due to the
improvement with `yyerror()` and how the error messages have to be exact. It
might be possible to fix the issue but it is not worth it as long as jparse here
tests location errors and it now does. Plus `party.json` belongs in jparse
anyway.

Updated `test_ioccc/ioccc_test.sh` to use option `-L` in `jparse_test.sh`
as we no longer want to test error location messages. This is the only
functional difference in this commit.

Updated `ioccc_test.sh` version to: `"1.0.3 2024-12-31"`.

`mkiocccentry` updated to show that the registration URL and submit server URL
are available and to show what file and what slot to upload, how to register
etc. The macros that checked for the readiness of these servers are no more as
they are always ready now even if one can only register if in pending and open
and only submit if status is open.

Updated `mkiocccentry version to `"1.1.2 2024-12-31"` and soup version to
`"1.1.3 2024-12-31"`. The reason the soup version was updated is because of
macro changes, including new ones for the URLs.


## Release 2.2 2024-12-30

Resolve issues #1044, #1045 and #1046.

Fix tools for filename length rule (part of rule 17)

Fix `mkiocccentry`, `chkentry` and `txzchk` for the requirement of filenames to
be of length > 0 && <= 99 as per rule 17.

This is somewhat involved and required all the JSON test files to be updated
(for versions) as well as some new JSON files.

A new macro `MAX_FILENAME_LEN` had to be added. Then in `soup/entry_util.c` a new
function called `test_filename_len()` was added. This function is then used in
`txzchk.c` (function `check_all_txz_files()`), `mkiocccentry.c` (in the `check_*()`
functions) and then, for `chkentry`, in `chk_validate.c` the function
`chk_extra_file()`; it is not necessary in the code that checks for the required
files as the functions that test those files check for exact filenames. The
function `test_extra_file()` calls the new function as `test_extra_file()` does
a variety of kinds of tests on the file.

A new bad txzchk test file was also added (with its corresponding .err file).

The `mkiocccentry_test.sh` script along with `txzchk_test.sh` (see above) and
`chkentry_test.sh` now test for too long filenames and too short filenames. In
the case of `mkiocccentry_test.sh` a change in the script itself had to be made
so the version was updated to `"1.0.2 2024-12-30"` from `"1.0.1 2023-02-05"`.

Changed `SOUP_VERSION` to `"1.1.2 2024-12-30"` from `"1.1.1 2024-12-26"`.

Changed `MKIOCCCENTRY_VERSION` to `"1.1.1 2024-12-30"` from `"1.1.0
2024-12-28"`.

Changed `TXZCHK_VERSION` to `"1.0.9 2024-12-30"` from `"1.0.8 2024-07-11"`.

Changed `CHKENTRY_VERSION` to `"1.0.3 2024-12-30"` from `"1.0.2 2024-08-26"`.


## Release 2.1 2024-12-28

This is a formal release for public use.

Changed `MKIOCCCENTRY_REPO_VERSION` from "2.0 2024-11-17"
to "2.1 2024-12-28".

Updated `Makefile.example` from
[Makefile.example from the other repo](https://github.com/ioccc-src/temp-test-ioccc/blob/master/next/Makefile.example)


## Release 2.0.3 2024-12-27

Resolve issue #1037

The submit server details are now ready!

Also change the temp-test-ioccc URLs to the main IOCCC website, for once the
Great Fork Merge is performed and the IOCCC website is updated.

Updated `mkiocccentry` version to: `"1.1.0 2024-12-28"`.

## Release 2.0.2 2024-12-27

Resolve issue #1039

This syncs jparse from jparse repo with a variety of bug fixes and improvements,
including a lot of documentation updates.


## Release 2.0.1 2024-12-26

Fix issues #1036, #1035 and #1033

The issues in respective order:

- Typo fix in mkiocccentry.c ('You title must ...' -> 'Your title must
...').
- mkiocccentry UUID type 4 was too strict
- Renamed makefile.local -> Makefile.local.

Updated version of mkiocccentry due to the above changes. For the second
one new macros are in soup/limit_ioccc.h so the soup version has been
updated as well.


## Release 2.0 2024-11-17

This is a formal release for public use.

Changed `MKIOCCCENTRY_REPO_VERSION` from "1.6.13 2024-11-16"
to "2.0 2024-11-17".

This repo is now under a **CODE FREEZE**.

Only super-critical bugs that have a significant impact the running
of IOCCC28 or the **Great Fork Merge** process and the temp-test-ioccc
repo will be considered until sometime after the winners of IOCCC28 have
been released.


## Release 1.6.14 2024-11-17

Update from dbg and jparse repos.

Prep for code freeze and formal release for IOCCC28.


## Release 1.6.13 2024-11-16

Added minor changes to the `jparse/jstrdecode(1)` tool.


## Release 1.6.12 2024-11-16

Add missing `-q` option to `txzchk`.


## Release 1.6.11 2024-11-15

Fixes and improvements in `jparse` synced from the [jparse
repo](https://github.com/xexyl/jparse/). This includes an update to the jparse
library and some bug fixes and additions to the jstrdecode and jstrencode tools.

Typo fix in mkiocccentry.c.

Also roll back a commit in chkentry.c - a work in progress for **after** IOCCC28
that was, in a rush to leave the house but still get important fixes in, was
accidentally committed yesterday.


## Release 1.6.10 2024-11-14

Synced `jparse/` from the [jparse repo](https://github.com/xexyl/jparse/). The
term `encode` and `decode` were swapped back to the original meaning. This
problem occurred due to the fact that when focusing for some time on the
encoding/decoding bug of `\uxxxx` code points in the jparse repo the fact that
it is a **JSON** encoder was lost in focus. This change also means that terms
were swapped here.

In the process of an issue in 'the other repo', some bugs were uncovered in
`jstrencode(1)` but this should not, I believe, affect the timeline of the next
IOCCC, fortunately. The website tools have to be updated which I will work on
next a bit later on today.



## Release 1.6.9 2024-11-09

Synced `jparse/` from the [jparse repo](https://github.com/xexyl/jparse/). This
came from discussion on JSON debug output as well as discussion about wording. A
fix in one of the man pages was also made (no longer true statement) and the man
pages and utils README.md were all expanded with more details and examples.


## Release 1.6.8 2024-11-08

Synced `jparse/` from the [jparse repo](https://github.com/xexyl/jparse/). This
includes some important bug fixes in a utility function that resulted, in debug
output, invalid JSON, plus an incorrect calculation in one case.

## Release 1.6.7 2024-11-07

Synced `jparse/` from the [jparse repo](https://github.com/xexyl/jparse/) to
remove unused code and extra sanity checks added to a function and to match a
filename change.

Improve (to help parse output a bit better) the scripts `bug_report.sh` and
`hostchk.sh`.


## Release 1.6.6 2024-11-05

Synced `jparse/` from the [jparse repo](https://github.com/xexyl/jparse/) with
some important documentation changes along with some typo fixes.


## Release 1.6.5 2024-11-03

Sync the `jparse/` directory from [jparse
repo](https://github.com/xexyl/jparse/). This adds some extra sanity checks in
`jstrencode(1)`/`jstrdecode(1)` and it also makes some fixes to the change in
decode/encode terminology, namely to do with comments. An unused macro (after a
fix) was removed. Comments were updated and improved in `jparse.l` and
`jparse.y`. Regenerated parser backup files.


## Release 1.6.4 2024-11-01

Sync the `jparse/` directory from [jparse
repo](https://github.com/xexyl/jparse/). This includes some important bug fixes,
namely that some codepoints that were rejected are no longer rejected as later
Unicode standards allow them. This means that the
[JSONTestSuite](https://github.com/nst/JSONTestSuite) files now completely pass
jparse.

The `CODE_OF_CONDUCT.md` was removed from `jparse/` as it referred to the IOCCC
judges which is not correct. Until a better way of doing this is derived, this
file no longer exists. To be clear the mkiocccentry `CODE_OF_CONDUCT.md` still
exists!


## Release 1.6.3 2024-10-31

Sync the `jparse/` directory from [jparse
repo](https://github.com/xexyl/jparse/). This includes some important bug fixes,
including a segfault when JSON debug level was specified and a string was
encountered (the problem was that when `has_nul` was removed it the format
specifier, a `%s`, was not removed).

A more significant change (that only requires minor changes in mkiocccentry
code) is that the tools `jstrencode(1)` and `jstrdecode(1)` were swapped as
according to other sources, converting a code point to another character
(accented character, emoji etc.) is encoding, not decoding. This means that
functions and comments were also changed. Only minor changes in this repo had to
be made, as noted, however and specifically function name changes.

With this merged the temp-test-ioccc repo will have to have the scripts updated
to use the right tool (not jstrdecode but jstrencode) and the correct version
(2.0.0).


## Release 1.6.2 2024-10-30

Sync the `jparse/` directory from [jparse
repo](https://github.com/xexyl/jparse/). This includes some important bug fixes,
especially in the `make install` rule that did not install a header file. Not
having this would cause a compilation error if one were to try and include
`jparse/jparse.h` outside this repo.

The `jparse_bug_report.sh` script has an improvement that will test compile a
jparse program to see if the system can link in `libjparse.a`, `libdbg.a` and
`libdyn_array.a`. This script is not needed for this repo though.


## Release 1.6.2 2024-10-23

Fix annoying bug where the `MKIOCCCENTRY_ANSWERS_VERSION` could not be in
`soup/version.h` and the `MKIOCCCENTRY_ANSWERS_EOF` could not be in
`mkiocccentry.h`.

Fix `write_echo()` in `test_ioccc/prep.sh` to not leave lines in the log file
that simply say `OK`.


## Release 1.6.1 2024-10-22

Sync the `jparse/` directory with state, as of 2024-10-22, of the
[jparse repo](https://github.com/xexyl/jparse/) .  Those updates
include improvements to the jparse test suite including new test cases,
test suite improvements, and related man pages.  The `jstrencode(1)`,
`jstrdecode(1)` and related Unicode / UTF-8 functions have been improved.

Changed `MKIOCCCENTRY_ANSWERS_VERSION` from "MKIOCCCENTRY_ANSWERS-IOCCCMOCK-1.0"
to "MKIOCCCENTRY_ANSWERS_IOCCC28-1.0".

Changed `MKIOCCCENTRY_REPO_VERSION` from "1.6 2024-10-10"
to 1.6.1 2024-10-22.


## Release 1.6 2024-10-09

Prep for "**Release 1.6 2024-10-10**".

This is a pre-**Great Fork Merge** code freeze for this repo.

We expect to make one minor change before the full **Great Fork Merge**
when we replace all references for "_ioccc-src/temp-test-ioccc_"
with "_ioccc-src/winner_" and replace all links to the
`https://ioccc-src.github.io/temp-test-ioccc/` web site with the official
`https://www.ioccc.org/index.html` web site.

Changed `MKIOCCCENTRY_REPO_VERSION` from "1.5.25 2024-10-10"
to "1.6 2024-10-10".



## Release 1.5.25 2024-10-09

Synced `jparse` from [jparse repo](https://github.com/xexyl/jparse/). This
includes better (additional) testing of JSON encoding/decoding ('Beware of the
<del>dragon</del> [fire
drake](https://www.glyphweb.com/arda/f/firedrakes.html)!' :-) ), a new version
string (UTF-8 version, which is now also displayed in the tools in this repo, as
it also helps identify if there is a mismatch in versions in bug reporting or
something else), amongst various other things.  Should one wish to further
progress their dementia! :-) they can look at that repo's log or the
jparse/CHANGES.md file.

New option `-N` to `jstrdecode(1)` and `jstrencode(1)` to ignore (in decoding
and encoding internal to the tool itself, not JSON) all newlines found in data.


## Release 1.5.24 2024-10-09

Synced `jparse` from [jparse repo](https://github.com/xexyl/jparse/). This
cleans up some code, removes code that's unnecessary and syncs the versions of
all tools to the same: `1.2.0 2024-10-09`. A new version string was added as
well, `JPARSE_UTF8_VERSION`. A bug in a script was also fixed.

Removed `soup/entry_time.c` and `soup/entry_time.h`.  We call `time(3)`
directly from `test_formed_timestam()` in `soup/entry_util.c`.


## Release 1.5.23 2024-10-08

Noted dependency of `MAX_SUBMIT_SLOT` and `MAX_TARBALL_LEN` defines
in `soup/limit_ioccc.h` with the IOCCC submit server.

The files `utf8_posix_map.c` and `utf8_posix_map.h` have been renamed
`default_handle.c` and `default_handle.h` respectively to better account
for their true purpose.  Additionally the function `check_utf8_posix_map()`
was renamed to `check_default_handle_map()` for the same clarifying reason.

Synced `jparse` from [jparse repo](https://github.com/xexyl/jparse/). This
should fix JSON decoding bugs. Although a couple things have to be done still to
close [that issue](https://github.com/xexyl/jparse/issues/13) out it appears
everything is good now. Some 'good' JSON files were changed to be 'bad' as they
have invalid UTF-8 bytes, it seems, but if this turns out to be false (or if the
so-called JSON spec allows for accepting it), this can be changed back,
especially as some test suites suggest that those files are legal, even though
multiple sources say otherwise.


## Release 1.5.22 2024-10-01

Improve `soup/is_available.sh` to now verify that `checknr(1)` is reliable and
if it is unreliable or cannot be found, the step is skipped in `prep.sh`. The
original version of `checknr(1)` always returned 0 even with an error so this
check in `is_available.sh` expects a non-zero value, not 0, in order to verify
it works okay, as in order to make sure that it works right we construct a
temporary erroneous man page.

The `prep.sh` here now skips `check_man` rule if `checknr` cannot be found or is
unreliable.

The Makefiles (except for `dbg` and `dyn_array` as these repos do not yet have
the script or the changes) have been updated to better explain the problem if
`is_available.sh` reports non-zero.

Sync `jparse` from [jparse repo](https://github.com/xexyl/jparse/) with the
above changes (`jparse` is where it originated).


## Release 1.5.21 2024-09-30

Bug fix `prep.sh` wrt skipped messages, and indent URL of each tool.

Make use of `is_available.sh` in the Makefiles.

Synced `jparse` from [jparse repo](https://github.com/xexyl/jparse/) with the
above changes to do with the jparse repo.


## Release 1.5.20 2024-09-29

Sync `jparse` from [jparse repo](https://github.com/xexyl/jparse/). This
includes a new JSON file with some details about it as well as some fixes in the
README.md file (newer synopsis of some tools) as well as a couple functions
added.

## Release 1.5.19 2024-09-27

Add `FOO_BASENAME` to all tools in this repo. This is used in both `-V` option
and `-h` option.

The `-V` and `-h` option of all tools now show the JSON parser version. This is
because it is helpful to know what parser version is linked in, especially for
any bug reports. This should not normally be a problem as the copies in this
repo are linked in but in case there is a problem it is now done (for some the
parser is actually used so it's even more important).

Sync `jparse/` from [jparse repo](https://github.com/xexyl/jparse/). The changes
consist of the `FOO_BASENAME` changes described above for the tools in this
repo, plus a broken link in `jparse_bug_report.sh`. This needs to be done for
the dbg repo and the dyn_array repo.

Fix comment about `CUNKNOWN` in Makefile.example.

The `ENTRY_VERSION` has been changed from "1.1 2024-02-11" to "1.2 2024-09-25".
Both "_title_" and "_abstract_" we added to the `.entry.json` file format.

The `entry_JSON_format_version` was changed from "1.1 2024-02-11" to "1.2 2024-09-25".


## Release 1.5.18 2024-09-24

Sync `jparse/` from [jparse repo](https://github.com/xexyl/jparse/). The
following was added to the jparse CHANGES.md file:

- Implement boolean `unicode` of `struct json_string` in the decoding functions.
If `json_conv_string()` finds that calling `json_decode()` which calls
`decode_json_string()` causes the bool `unicode` to be false it sets the
`converted` boolean to `false`. This will then flag an error in parsing if there
is an invalid Unicode symbol. This does not mean that decoding all symbols work:
it simply means that we detect if there are invalid Unicode symbols. The check
is done on the original string but if it turns out it has to be done on the
decoded string that can be done easily.

- The `json_decode()` had a bug fix: it allocated memory for the return string
when `decode_json_string()` does that and this caused a memory leak.

- If `decode_json_string()` (which is now a static function in `json_parse.c`)
detects an error, the allocated memory is freed before returning `NULL`.

- Updated `JSON_PARSER_VERSION` to `"1.1.6 2024-09-24"`.

- Updated `JPARSE_REPO_VERSION` to `"1.0.13 2024-09-24"`.

- Added `MIN` and `MAX` macros to `util.h`.

- Rename `jenc` to `byte2asciistr` in `json_parse.c` to avoid confusion about its
purpose.

- Expand the output of `jstrencode -t` and `jstrdecode -t` to express that the
encode/decode tests have not yet been written. This depends on bug #13 being
resolved first.

- Changed optimisation flags in the Makefiles to not specify `-g3` as debug
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

- Fix potential use without initialisation of `inputlen` in `jstrencode.c`.

- Add function `decode_json_string()` to help simplify the `json_decode()`
function as it's quite long. This new function takes the length and calculated
decoded size as well as the pointers (the block of memory, the return length and
the `has_nul` as well) and then allocates the `char *` and does what was the
second half of the `json_decode()` function. As `json_encode()` is much simpler
it seems like at this time that something like this is not needed. This new
function is not static but it is entirely unclear if that is necessary.

- Add to `struct json_string` the `bool unicode`. Currently unused (just
initialised to false) the purpose will be to indicate whether or not the string
has any invalid unicode symbols found during decoding.

- Add (as `json_utf8.h` and `json_utf8.c`) the files `unicode.h` and `unicode.c`
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

- The Makefiles now compile and link in `json_utf8.c`.

- Run `make seqcexit`.

- Add call to `setlocale()` in `jstrencode.c` and `jstrdecode.c`.

- Fix build of libjparse.a - add `json_utf8.o`.

- Remove `#line ..` from `json_utf8.h`.

- Removed helper function `is_utf8()` as it appears to be not useful and might
actually be incorrect. A copy of this function has been made in the case it
actually does prove useful, unlikely as that seems.

- Added `version.h` which has the versions for the repo release, the jparse JSON
parser and the jparse tool. The other tools have their respective version in
their source code file. This file was added primarily so that `verge` could
refer to the JSON parser version. In order to get this to work, the `jparse.y`
file now has `#include "version.h"`. This means the backup parser source code
has been rebuilt with `make parser-o`.

- The tools now have a `FOO_BASENAME` in their header file which is used in both
the usage string and the version option.

- The file `test_jparse/pr_jparse_test.h` has been added for that tool.

- The Makefiles have been updated including new dependencies.

The `FOO_BASENAME` will be done for the tools in this repo too. It was done this
way to make sure that the version strings match the tool without having to type
the string literally more than once.

## Release 1.5.17 2024-09-15

Sync `jparse/` from [jparse repo](https://github.com/xexyl/jparse/). Added
function to check a string for UTF-8. Fixed make `check_man`.

Added script `soup/is_available.sh`: it takes a single arg and if it can find a
program by that name (as in by `type -P`) it will return 0; otherwise it returns
1.

Fix `check_man` Makefile rule in all Makefiles. Previously it ignored the result
which meant that `prep.sh` never reported a problem in a problematic man page.


## Release 1.5.16 2024-09-13

Sync `jparse/` from [jparse repo](https://github.com/xexyl/jparse/).

This makes some important fixes to the tools `jstrdecode(1)` and `jstrencode(1)`
where in `jstrdecode(1)` the `-Q` option did not work and for both the printing
of everything should happen after everything is parsed (especially for the `-Q`
option but not strictly for that reason). The `pr_jparse_test` tool had a bug
fix as well where the `-h` option did not work.

Add to `jstr_test.sh` tests for `jstrdecode(1)` options `-Q` and `-e` (both
separately and together).

Clarified comment in `jparse.l` and rebuilt backup `jparse.c` (`jparse.ref.c`).

Add an extra sanity check to `jencchk()`: the macro `JSON_BYTE_VALUES` must
equal 256. Previously we did check that the table length of `jenc` is
`JSON_BYTE_VALUES` with the assumption that this was 256 but now we make sure
that it is 256, before we check the table length.


## Release 1.5.14 2024-09-11

Sync `jparse/` from [jparse repo](https://github.com/xexyl/jparse/). This
includes a number of fixes and improvements to various tools and the Makefiles
(including some that will likely be implemented in the Makefiles here).


## Release 1.5.13 2024-09-09

Fix Eszett (`ß`) in `soup/utf8_posix_map.c` to map to `ss`, not just one `s`. In
parentheses the word (though all caps) `ESZETT` was added to the `SHARP S`
(another term for it). This was done to make it easier to find for those of us
used to the German term.

Moved paths used by IOCCC tools out of jparse/util.h and into soup/soup.h, the
obvious location.

Updated the repo release version to account for this change.

## Release 1.5.12 2024-09-08

In `test_ioccc/`, `soup` and the top level Makefiles the `${RM}` variable now
uses the `${Q}` control variable.

Other than `dbg` and `dyn_array` Makefiles (as those changes have to be
committed over there and then synced) the `RM_V` variable is now empty by
default as it used to be that the `${RM}` did not use `-v`.

Remove from `${RM}` the `-r` option where it is not needed i.e. when a directory
is not being removed.

Sync `dbg` and `dyn_array` subdirectories from the [dbg
repo](https://github.com/lcn2/dbg) and
[dyn_array](https://github.com/lcn2/dyn_array), with fixes to the Makefiles.

Sync `jparse` subdirectory from [jparse repo](https://github.com/xexyl/jparse/)
with some fixes. The changes in particular include:

- Fix `make clobber` to remove `jparse_test.log` and `Makefile.orig`.

- Fix `make legacy_clobber` to remove `jparse.a`.

- Fix `${RM}` in Makefiles to use `${Q}` variable (not in `make depend` as it is
used in an earlier command in the multiple line commands), in some cases changed
from the wrong variable, and `${RM_V}` (where this was not done).

- Do not use `-r` in rm in Makefiles unless removing a directory, for safety.

- Do not by default use `-v` for `rm` in Makefiles, to match what was previously
done here.

## Release 1.5.11 2024-09-07

Synced `jparse` subdirectory from the [jparse
repo](https://github.com/xexyl/jparse/).

It was decided that error messages in `jparse(1)` could be improved. This was
done in the following ways:

- If verbosity level is > 0, then it will show the invalid token and hex value
of that token (along with the line and column).
- If verbosity is not specified, then it will just show the syntax error (the
bad token with the line and column) and then the warning that the JSON tree is
NULL (just like if verbosity specified) and then the error message (from
`jparse(1)` itself).

The error files in the `bad_loc` have been updated as now the error output has
changed.

Updated `jparse(1)` version to 1.1.6 2024-09-07.

Removed timestamps from `beginning`/`ending` messages in Makefiles.

Fix clone rules in Makefile to exclude the `.github` subdirectory of outside
repos.

Updated `.exclude` to include (not that that makes sense :-) ) the glob
`._.DS_Store` which sometimes shows up in macOS (it is different from
`.DS_Store`).


## Release 1.5.10 2024-09-06

Synced `jparse` subdirectory from the [jparse
repo](https://github.com/xexyl/jparse/).

Add `-L` option to `jparse_test.sh` to disable the error location tests. This is
not usually needed but it can be useful in some cases.

`jparse_test.sh` now will, in the case that no file is specified on the command
line, try and read from the default good JSON strings file and the default bad
JSON strings file, in order to verify that things are working well there, too.

Unless some problem occurs this should be the final change for `jparse` until
after the IOCCC28.


## Release 1.5.10 2024-09-05

Synced `jparse` subdirectory from the [jparse
repo](https://github.com/xexyl/jparse/) with some useful updates and fixes. See
the git log of that repo for more detailed information. Changes that these
updates required here (and some that are in the jparse subdirectory):

- The `test_ioccc/ioccc_test.sh` script no longer runs the `jstr_test.sh`,
`jnum_chk` tests and it does not run `jparse_test.sh` except
on the `test_ioccc/test_JSON` directory as the `jparse/Makefile` runs the
appropriate tests in `test_jparse/` (in this repo under `jparse/test_jparse`) -
obviously the jparse repo knows nothing about `test_ioccc/test_JSON` so it has
to be done this way. It's likely that with the `-Z topdir` option (that was
recently added to `jparse_test.sh`) or some other workaround the script could
run from `test_ioccc/ioccc_test.sh` but it is redundant and not useful so it no
longer does.
- Due to `jparse_test.sh` path updates (one of the changes is to fix it to work
on its own but this required some changes in test error files) the error files
under `jparse/test_jparse/test_JSON/bad_loc` have been updated here (this is
another reason that we cannot as easily run `jparse_test.sh` from `ioccc_test/`
without the `-Z topdir` hack or some other workaround).
- The `bad_loc` error files are now always tested as if there is not a matching
error in the file it indicates a problem with the parser.

A useful update (besides the addition of the `-Z topdir` hack) to
`jparse_test.sh` that was synced here is the new `-f` option for the files that
hold JSON blobs, one per line: it inverts the check, saying that the JSON blobs
must be invalid. This required a new file here,
`jparse/test_jparse/json_teststr_fail.txt`. As `jparse_test.sh` always runs on
at least `json_teststr.txt` (if no files specified) it might be good to not have
the option and always run a fail test on the new file but this can be worried
about another time. As the `jparse/test_jparse/Makefile` runs it with this
option it doesn't matter much anyway.

The `jparse/test_jparse/jparse_test.sh` version is now `1.0.5 2024-09-04` (fixed
from the <del>more natural</del> :-) international way `1.0.5 04-09-2024` that
was added by habit, to match the format of the other versions); the old version
was `1.0.3 2023-08-01`.

`make release` should be fine now, after the updates to `ioccc_test.sh`. The
version of that script is now `"1.0.2 2024-09-05"`.

In the Makefile rules that have echo lines that end with `starting"` and those
that end with `ending"` it now shows the timestamp (except or the `dyn_array`
and `dbg` Makefiles as those are from other repos also).

Repo release version is now `1.5.10 2024-09-05` (a recent update did not update
the version string so it jumped more than one). Except for some last minute
fixes that might be required it might be the last release until after IOCCC28
(let's all hope it is!).


## Release 1.5.9 2024-09-04

Add to `-V` option of compiled tools that use the jparse library in some form or
another (i.e. links it in) to print out the current json parser version. The
json parser and jparse versions were also updated (from the jparse repo).


## Release 1.5.8 2024-09-01

Add to Makefiles the `PREFIX` variable to allow for installing to a different
location than the default `/usr/local`.

Added some uninstall rules to the Makefile in a number of subdirectories. The
external repos that are synced to this repo have not had any direct changes as
those repos first need those rules (which will be done soon). This is also why
the top level Makefile does not run `make uninstall` in those subdirectories.
Once these are all done the top level Makefile can have a simpler uninstall
rule.

Along the lines of silencing (non-verbose) `install` in the `install` rule one
may now do the same with the uninstall (instead of `INSTALL_V` it is `RM_V`).
This is only done to be congruent with the `install` rule.

Updated the `MKIOCCCENTRY_REPO_VERSION` to `"1.5.7 2024-09-01"`.

## Release 1.5.7 2024-08-31

Synced `jparse` subdirectory from the [jparse
repo](https://github.com/xexyl/jparse/). There was no code change and the only
functionality changes are that the install rule installs more header files (now
in a subdirectory - `/usr/local/include/jparse`) and an uninstall rule is added
(for those who wish to deobfuscate their system :-) ).

Updated the `MKIOCCCENTRY_REPO_VERSION` to `"1.5.6 2024-08-31"`.


## Release 1.5.6 2024-08-31

Add to `dyn_array/Makefile` or `dyn_test` the flag `-L../dbg` in hopes to solve
the workflow failure. If this works then the `dyn_array` repo will also have to
have this added to its Makefile.

Add to `jparse/Makefile` `-L../dbg -L../dyn_array` for the same reasons as
above. If this works then the `jparse` repo Makefile will also have to be
updated for this (along with some other pending changes).

Add to `jparse/test_jparse/Makefile` `-L../dbg -L../dyn_array` for the above
reasons as well.



## Release 1.5.5 2024-08-30

We updated `dbg/` from the dbg repo.

We updated `dyn_array/` from the dyn_array repo.

We updated `jparse/` from the jparse repo.

Renamed `MKIOCCCENTRY_SRC` to `INTERNAL_INCLUDE`.

Updated the `MKIOCCCENTRY_REPO_VERSION` to `"1.5.5 2024-08-30"`.


## Release 1.5.4 2024-08-29

Rename libraries for easier linking in other applications. In particular the
[jparse repo](https://github.com/xexyl/jparse/) (which currently requires some
changes in order to work on its own but this release is part of that) needs the
`dbg` and `dyn_array` libraries but in order to link these in properly one must
name them `libdbg.a` and `libdyn_array.a` (i.e. add the `lib`) so that one can
then do `-ldbg` and `-ldyn_array`. Along the same lines `jparse.a` has been
renamed to `libjparse.a` for linking in (this is something that has not yet been
done in the jparse repo but in order to link it in it will have to be done).

Updated the `MKIOCCCENTRY_REPO_VERSION` to `"1.5.4 2024-08-29"`.


## Release 1.5.3 2024-08-28

Fix `hostchk.sh` that was broken with the recent change of `MKIOCCCENTRY_SRC`.
New version for hostchk.sh: `"1.0.2 2024-08-28"`.


## Release 1.5.2 2024-08-28

Remove dependence of `jparse/jsemtblgen.h` on `../iocccsize.h`.

Update `dyn_array/`, `dbg/`, `jparse/` trees as per their GitHub repos.

Require `MKIOCCCENTRY_SRC` to be defined in order to compile.
We add `C_SPECIAL=${C_SPECIAL}` to all calls to `${MAKE}` in
the `Makefile`.

Added `${C_SPECIAL}` make variable so we can define special things
on the compile line.  Bu default we use `C_SPECIAL= -DMKIOCCCENTRY_SRC`.

Improved the `make dbg.help`, and `make dyn_array.help` and `make jparse.help`.
Removed `make dgb.replace_from_clone`, `make dyn_array.replace_from_clone`,
and `make parse.replace_from_clone`.

When `make dbg.update_from_clone` is used, we also perform
a `make depend` where we force `C_SPECIAL=-DMKIOCCCENTRY_SRC`.

When `make dyn_array.update_from_clone` is used, we also perform
a `make depend` where we force `C_SPECIAL=-DMKIOCCCENTRY_SRC`.

When `make jparse.update_from_clone` is used, we also perform
a `make depend` where we force `C_SPECIAL=-DMKIOCCCENTRY_SRC`.

The above 3 items allows us to clone from the repo and then to force
`make depend` under `-DMKIOCCCENTRY_SRC` so that include files for
`dbg` and `dyn_array` will be found under this directory tree.

When `make dbg.update_into_clone` is used, we also perform
a `make depend` where we force `C_SPECIAL=-UMKIOCCCENTRY_SRC`.

When `make dyn_array.update_into_clone` is used, we also perform
a `make depend` where we force `C_SPECIAL=-UMKIOCCCENTRY_SRC`.

When `make jparse.update_into_clone` is used, we also perform
a `make depend` where we force `C_SPECIAL=-UMKIOCCCENTRY_SRC`.

The above 3 items allows us to clone from the repo and then to force
`make depend` under `-UMKIOCCCENTRY_SRC` so that include files for
`dbg` and `dyn_array` will be found under `/usr/local/include`.


## Release 1.5.1 2024-08-28

Move `open_json_dir_file()` from `chkentry.c` to `open_dir_file()` in
`jparse/util.c`.

Updated `MKIOCCCENTRY_REPO_VERSION` version to "1.5.1 2024-08-28".


## Release 1.5 2024-08-27

Prep for "**Release 1.5 2024-08-27**".

Changed `MKIOCCCENTRY_REPO_VERSION` from "1.4 2024-08-11"
to "1.5 2024-08-27".


## Release 1.4.4 2024-08-27

Running `chkentry(1)` with a single argument is now reserved for future use
(related to a winning IOCCC entry instead of a submission).

Changed `CHKENTRY_VERSION` from "1.0.1 2024-03-02"
to "1.0.2 2024-08-26".


## Release 1.4.3 2024-08-23

Added a number of test JSON files to demonstrate that the JSON parser is okay
with mixed spacing (spaces and tabs or different number of spaces with or
without tabs etc.), no spacing and so on, even though the `mkiocccentry(1)` tool
will form json files with multiples of 4 spaces and no tabs.

Changed `MKIOCCCENTRY_VERSION` from "1.4 2024-08-11"
to "1.0.8 2024-08-23".


## Release 1.4.2 2024-08-21

Changed `mkiocccentry(1)` to form `.info.json` and `.auth.json` with
multiples of 4 spaces and no TAB characters.

Further simplified the `Makefile.example` file.


## Release 1.4.1 2024-08-17

Add macros `ENTRY_VERSION` and `AUTHOR_VERSION` that correspond to the
`.entry.json` file of each winning entry and the `author_handle.json` file that
corresponds to each winning author.


## Release 1.4 2024-08-11

Prep for "**Release 1.4 2024-08-11**".

Changed `MKIOCCCENTRY_REPO_VERSION` from "1.3 2024-08-02"
to "1.4 2024-08-11".


## Release 1.3.2 2024-08-10

Removed code files and documentation related to `jfmt(1)`, `jval(1)` and `jnamval(1)`
as these tools are not yet ready for release and we do not want people to become
confused about their functionality in the interim.


## Release 1.3.1 2024-08-08

Fixed a bug where `jparse/jfmt` was printing debug messages even at debug level 0.


## Release 1.3 2024-08-02

Changed `MKIOCCCENTRY_REPO_VERSION` from "1.2 2024-07-04".
to "1.3 2024-08-02".


## Release 1.2.5 2024-07-28

We add a lot of good and bad JSON files to the test JSON directories
(`test_ioccc/test_JSON/good` and `test_ioccc/test_JSON/bad` respectively) that
come from the [JSONTestSuite](https://github.com/nst/JSONTestSuite) repo with
all tests succeeding. We give our gratitude to the maintainers of the repo! For
a quick note on why these results are significant, besides the fact that it is
more validation that our parser is in good shape, is from the maintainers of the
repo. They write about the files:

    These files contain weird structures and characters that parsers may understand
    differently, e.g.:

        huge numbers
        dictionaries with similar keys
        NULL characters
        escaped invalid strings

Again we give a **HUGE THANK YOU** to the
[JSONTestSuite](https://github.com/nst/JSONTestSuite) repo maintainers!

The jparse/README.md file has been updated to move the history section to the
very bottom and to discuss (briefly) the testing procedure that Landon and I
(Cody) have implemented.

The `make test`, `make prep` and `make slow_prep` rules now show the time of
starting and time of ending.

Also, since this was not mentioned, we recently added (with thanks to
@SirWumpus) a workflow that runs `make slow_prep` on push and pull request so we
can more easily find problems (in case someone forgets to run the rule prior to
commit or if there is a case where the local environment of the committer makes
the test pass even though it should not, which has happened before).


## Release 1.2.4 2024-07-14

Add constants for `test_formed_timestamp_usec()` in `entry_util.c`.

Fix error messages in a number of `sem_member_value_*` functions in
`jparse/json_sem.c`, where the `*` is some of the number checks. In particular
a number of non-boolean checks reported incorrectly that the type was not a bool
when it actually expects something else.


## Release 1.2.3 2024-07-12

Add `-e` option to `mkiocccentry(1)` which if specified will be passed to
`txzchk(1)`. The test suite always uses this option. The man page for
`mkiocccentry(1)` was updated to show this new option. The guidelines might want
to be updated for this but they might not want to be. This is TBD later.

Updated `MKIOCCCENTRY_VERSION` to `"1.0.7 2024-07-12"` for the `-e` option.



## Release 1.2.2 2024-07-11

Removed `-q` from `txzchk(1)` and make it always show warnings. If one wants to
not see warnings they can do `2> /dev/null`. The point of this change is so that
the submit server does not have to use `-q` but also warnings should be showed
in any case if there are any. The option to show warnings, `-w`, still exists
but using this should not normally be required. This commit will require an
update to the guidelines which I will do when this has been merged.

The punny messages in `txzchk(1)` are now in their own option (or actually
options): `-e` and `-f`; `-f` requires an argument that can be converted to an
unsigned integer but you will have to either try it out, read the source or
check the man page if you wish to know more as I (@xexyl) do not want to spoil
the fun of finding it (including the additional ones)! `mkiocccentry(1)` will
use the first option but not the second one. This was suggested on GitHub just
like the removal of `-q` was.

Updated `TXZCHK_VERSION` to `"1.0.8 2024-07-11"`.

Ran `make rebuild_txzchk_test_errors` due to slight message changes and then
`make release` to test everything.


## Release 1.2.1 2024-07-10

Fixed `txzchk(1)` to NOT show warnings if `-q` is used unless `-w` is used
which now disables `-q` as it's necessary for the test suite (that always held
but with this fix it caused problems to not disable quiet mode and since `-q`
does not make sense with `-w` anyway it is not a problem).

`txzchk_test.sh` no longer uses `-q` but this is only because there is no point
in using it due to the change in `txzchk(1)`.

I (@xexyl) (re)discovered this problem when working on adding the details of
using `txzchk(1)` to the guidelines, as I know how it works best and I felt
there were some details that could be clarified.

Changed `TXZCHK_VERSION` to `"1.0.7 2024-07-10"`.
Changed `TXZCHK_TEST_VERSION` to `"1.0.3 2024-07-10"`.


## Release 1.2 2024-07-04

Fixed `txzchk(1)` for BSD (in particular NetBSD) by `#include <sys/wait.h>`.
Thanks to @SirWumpus for the report!

Changed `txzchk` TXZCHK_VERSION` "1.0.3 2024-03-02"
to `"1.0.6 2024-06-25".

Updated `bug_report.sh` script with new option `-m make` to specify path to
`make(1)`. The script now also tries to find `gmake`, first and if that fails
it will look for `make`. In the case that it is not a regular executable file it
is considered a command line error along the lines of 'command not found'. This
helps simplify the handling of the error codes as without `make(1)` the script
is essentially useless anyway (or mostly useless).

Changed `bug_report.sh` BUG_REPORT_VERSION from "1.0.3 2024-06-26""
to "1.0.4 2024-06-26"`. Man page updated for these changes.

Fixed bugs that prevented NetBSD from compiling this code.  Fixed
a number of compiler warnings under NetBSD.  Thanks to @SirWumpus
for the detailed bug report and his assistance in testing.

Fixed a bug that caused `iocccsize(1)` to crash on some systems, such
as MacOS under Apple Silicon, when given a binary blob such as the
`iocccsize` binary as input.

Changed `iocccsize` IOCCCSIZE_VERSION from "28.14 2024-01-04"
to "28.15 2024-06-27".

Updated `Makefile.example` to be more like IOCCC winning entry Makefiles.

Expanded `remarks.example.md` to include more information about
IOCCC markdown guidelines as well as more consistent with the new
IOCCC submission process.

Improve the Makefile to allow for NetBSD to run `make prep`,
`make slow_prep` and `make release`.

Changed `test_ioccc/prep.sh`  PREP_VERSION from "1.0.1 2024-03-02"
to "1.0.2 2024-07-03".

Disabled parallel make for most of the repo.

Updated `.github/workflows/codeql.yml` to use up to date actions for
checkout, node setup, Initialize CodeQL, Autobuild, and Perform CodeQL Analysis.

Added `.github/workflows/test.yml` to perform make actions such as
`make slow_prep` and `make test` as part of the GitHub workflow.

Fix `.github/dependabot.yml` to set an update schedule for GitHub Actions.

Changed MKIOCCCENTRY_REPO_VERSION from "1.1 2024-05-19"
to "1.2 2024-07-04".


## Release 1.1.1 2024-06-23

Changed `LOCATION_VERSION` to `"1.0.4 2024-05-22"`.

Fixed usage string in `location_main.c`: it did not suggest that one can specify
more than one location but it can.

Added to the README.md brief documentation of the `location(1)` tool (it was not
there at all) and fixed some typos and added some clarity in the `location.1`
man page.

Updated the Rule 2a size from 4096 to 4993: a change that keeps the
"Rule 2b:Rule 2a" ratio (that was recently changed from 2053 to 2503)
similar to the 2001-2020 IOCCC era.


## Release 1.1 2024-05-19

Release version 1.1 of the mkiocccentry tool set.


## Release 1.0.64 2024-05-18

Changed all _submission_num_ to _submit_slot_ (including json tags) and changed
all _MAX_SUBMISSION_NUM_ to _MAX_SUBMIT_SLOT_NUM_ and
all _MAX_SUBMISSION_CHARS_ to _MAX_SUBMIT_SLOT_CHARS_ and
changed relater comment and prompt text to be more consistent with the
pending submit server AND reduce the chance that someone revising their
submission incorrectly thinks they need to increment that value each time.

Remove _formed_UTC_ JSON member from `.auth.json` and `.info.json`.
The  _formed_UTC_ JSON member was redundant in that it was simply
a different coding of the _formed_timestamp_ in ASCII date string form.
It was a source of errors in editing _formed_timestamp_ values.

Changed INFO_VERSION from "1.15 2024-03-02"
to "1.16 2024-05-18" as a result of the JSON member name above changes.

Changed AUTH_VERSION from "1.21 2024-03-04"
to "1.22 2024-05-18" s a result of the JSON member name above changes.

Fix `soup/reset_tstamp.sh` and `soup/Makefile` to look for the
correct location of the `limit_ioccc.h` file and fixed the prompts
result messages.

Changed `soup/reset_tstamp.sh` RESET_TSTAMP_VERSION from "1.0.1 2024-03-02"
to "1.0.2 2024-05-15" as a result of the above changes.

Changed MKIOCCCENTRY_VERSION from "1.0.5 2024-05-15"
to "1.0.6 2024-05-19" as a result of the above changes.

Changed TXZCHK_VERSION from "1.0.4 2024-05-16"
to "1.0.6 2024-06-25" as a result of the above changes.

Performed `make reset_min_timestamp` to change MIN_TIMESTAMP from 1675547786
to 1716103155 due to the changes in both `.auth.json` and `.info.json`.


## Release 1.0.63 2024-05-16

Changed more references of _entry_ to _submission_ and _entries_ to
_submissions_ and updated the man pages for `mkiocccentry(1)` and `chkentry(1)`
about this as there are places where it is NOT appropriate to update the terms.
It might be that there are no more places where _entry_ or _entries_ is used
incorrectly (except perhaps for _iocccsize.c_) but the other terms have not been
checked.

Make `txzchk(1)` not show output if `-q` specified. Updated version to `"1.0.4
2024-05-16"`.

The `mkiocccentry(1)` tool prints both the location name and the
common location name when prompting the user to validate author
location data.


## Release 1.0.62 2024-05-15

Changed MKIOCCCENTRY_REPO_VERSION from "1.0.1 2023-02-14"
to "1.0.62 2024-05-15" to match the above release version.

Changed IOCCC_CONTEST from "IOCCCMOCK"
to "IOCCC28" for testing purposes.  :-)

Changed IOCCC_YEAR from "2023"
to "2024" for testing purposes.  :-)

Changed MKIOCCCENTRY_VERSION from "1.0.4 2024-03-04"
to "1.0.5 2024-05-15" because of the above constant changes.

The filename formed by _mkiocccentry(1)_ was changed
to start with _submit._ instead of _entry._.

Changed FNAMCHK_VERSION from "1.0.1 2024-03-02"
to "1.0.2 2024-05-15" because of the above change.

Updated `mkiocccentry(1)` and `chkentry(1)` man pages accordingly.

Updated the test files according to the above changes.


## Release 1.0.61 2024-03-07

Remove from comments the `winner_handle`. Fixed the regexp of an allowed handle
(in comments and in strings). It is regrettable but due to the use of
`posix_plus_safe()` additional characters are allowed that might be better left
out (these have been allowed for a long time but the comments and strings said
otherwise). The regexp that is allowed for `author_handle`s is:

```re
^[0-9A-Za-z][0-9A-Za-z._+-]*$
```

The difference is that it allows upper case characters too. In particular the
old regexp was `^[0-9a-z][0-9a-z._+-]*$`.

Typo fix in txzchk.h: referred to `posix_safe_plus()` instead of
`posix_plus_safe()`.

Change words `entry` and `entries` to `submission` and `submissions` (exception:
in `iocccsize`).

Added FAQ 12 "Why do these tools sometimes use the incorrect IOCCC terms?".


## Release 1.0.59 2024-03-04

Added a distinction between a "common name" and "name" for locations.

Added `-c` to the location command line to change using "name"
(the default) to ""common name".

Removed _location_name_ JSON member from `.auth.json`.

While `mkiocccentry` will ask the user to verify their _location_code_
by also showing them the _location_name_, it will no longer add
the JSON member with the JSON member name of "location_name" to `.auth.json`.

Changed LOCATION_VERSION from "1.0.2 2023-08-04" to "1.0.3 2024-03-03".
Changed MKIOCCCENTRY_VERSION from "1.0.3 2024-03-02 to "1.0.4 2024-03-04".
Changed IOCCC_auth_version from "1.20 2024-03-02" to "1.21 2024-03-04".
Changed MKIOCCCENTRY_REPO_VERSION from "1.0.1 2023-02-14" to "1.1 2023-03-04".

Added a simple `make unstall` rule to uninstall all files installed by this repo.


## Release 1.0.58 2024-03-02

Address part (possibly all) of the issue of new IOCCC terms (only part because
some things need to be decided still especially with file names and related
specifics but possibly all if it's decided these should stay the same).

Changed version of `txzchk` to `"1.0.3 2024-03-02"`.
Changed version of `fnamchk` to `"1.0.1 2024-03-02"`.
Changed version of `mkiocccentry` to `"1.0.3 2024-03-04"`.
Changed `AUTH_VERSION` to `"1.20 2024-03-02"`.
Changed `INFO_VERSION` to `"1.15 2024-03-02`".

In many cases the word `entry` was changed to `submission`. For `chkentry` and
`mkiocccentry` it has been decided to keep the names. However in text, variable
and comments the word is now `submission`. It is not clear, however, if a
submission directory needs to have a different prefix (currently it's `entry.`).
If this is necessary then more changes will have to be made. If it needs to be
renamed then changes in `fnamchk` and `txzchk` plus data files (at the very
least - there might be other places that'd have to be updated) will have to be
made.

`winner`/`winners` changed to `winning author`/`winning authors` (a more
thorough check has to be performed to make sure all are caught, see below). A
winning submission is an entry and at least in some cases this is now clear. In
the case of `winner_handle` (or even just `winner handle`) this was not changed.
There already exists an `author_handle` and that's what is in the files but in
comments `winner_handle` also exists. In other words this ambiguity should be
addressed.

The json `past_winner` was changed to `past_winning_author`. This
involved updating some patch files and all the auth json files as well.

Typo fix in `bug_report.sh`. No version update.

In many cases (not all) the year 2023 was changed to 2024. The `IOCCC_YEAR` was
NOT updated to 2024 as this causes other problems that require a bit more time.

A fun problem to resolve is the text along the lines of 'submit your entry'
being changed to 'submit your submission'. In some cases the surrounding text
was reworded but not in all. If you like this can be submissions all the way
down!


## Release 1.0.57 2024-01-21

Rename "print_test" to "pr_jparse_test".  The name "print_test" was
too generic and might conflict with some test print application
if it was installed.


## Release 1.0.56 2024-01-20

Changed `"iocccsize_version"` in JSON files (those that were `"28.13
2023-02-04"`) to `""28.14 2024-01-04"`. This fixed a problem with `make prep`
failing due to the script `chkentry_test.sh` failing due to many errors
reported, some of which didn't seem to make any sense whatever.

Change `fd_is_ready()` to return true if `isatty(fd)`.


## Release 1.0.55 2023-11-07

Add another forbidden file name in submissions: `prog.alt` (prog.alt.c is
allowed).

Modularise the checks for invalid filenames in entries. For instance in
`check_extra_file()` there's no need to check each extra filename and then give
the same error message changing the macro of the filename that's disallowed when
we can just print the string being tested against. The only difference is that
there's one if (with multiple checks) and instead of duplicating the same error
message we just print it once with the string being tested against. Note that
there are two sets of checks: one for extra files being required filenames and
another for disallowed filenames.

Make sure to use the macros for the filenames, not the literal strings (e.g. use
`PROG_FILENAME` not `"prog"`).

Check filenames in alphabetical order (I think :-) .. very tired so maybe missed
one or two).

Note that the function `check_extra_file()` CANNOT be used in every case so it's
not used except in chkentry!


## Release 1.0.54 2023-11-06

The following filenames are no longer allowed in an entry's extra files list:

- README.md
- prog
- prog.orig
- prog.orig.c
- index.html
- inventory.html

The mkiocccentry tool checks for this, giving an error early on (rather than
making the user retry after inputting more details). The function
`test_extra_file()` now checks for these files too, giving an error if any are
found.

New mkiocccentry version: 1.0.2 2023-11-06.

`txzchk` also checks for these files in the tarball. New version: 1.0.2
2023-11-06.


## Release 1.0.53 2023-09-13

Minor fix in `jval` and `jnamval` - new version `"0.0.15 2023-09-13"`. The `ne`
operator for `-S` and `-n` already was parsed (by foresight that it would
be needed, maybe) but it was not in the help. Also I decided to change the order
of the enum so that `JSON_CMP_OP_NE` comes right after `JSON_CMP_OP_EQ` rather
than at the end.

Man pages updated - added `ne` to `-S` and `-n` options.


## Release 1.0.52 2023-08-09

New version of `jval` and `jnamval`, `"0.0.14 2023-08-09"`.

Fixed the lists of `-S` and `-n` option parsing. For now it should just be a
string that's strdup()d. The operators are in the list in the order specified,
one list for strings and one list for numbers. This can be changed to a single
list later on if necessary (as I suspect it might need to be but the way it is
now is set up as two which is what I'm operating under).

The function that frees the `-S` and `-n` lists in `jval` and `jnamval` is now
in `json_util.c` as they are actually in a struct common to both `jval` and
`jnamval`. The functions that free them in `jval_util.c` and `jnamval_util.c`
simply check that `jval` or `jnamval` is not NULL and then calls the new
`free_json_util_cmp_list()` function.

Made the json util operator macros an enum.

Rename the enum `output_format` to `JSON_UTIL_OUTPUT_FMT`.


## Release 1.0.51 2023-08-07

Fix link in make rule `jparse.clone` to use
`https://github.com/xexyl/jparse.git` as the link used,
`https://github.com/lcn2/jparse.git`, is actually a clone of the repo I set up
(empty for now) which will be populated with `jparse` later on when the parser
is tested more fully.

Add `-F` to common options of `json_util_README.md`.

Change `jnamval -P` to `jnamval -r` (restrict printing types). There might be a
reference to something non-programming in this change. New version `"0.0.13
2023-08-07"`.

Slightly improve `dbg_example.c` by adding a call to `not_reached()` after the
`errp()` call.

## Release 1.0.50 2023-08-06

New version of `jfmt`, `jval` and `jnamval`: `"0.0.12 2023-08-06"`. `jfmt` and
`jval` jumped from 0.0.10 to make them match. Later on after version 1.0.0 this
will no longer happen.

Updated the `-F` option list. It now is:

```
-f fmt		change the json format style (def: use default)

		tty		when output is to a tty, use colour, otherwise use simple
		simple		each line has one json level determined by '[]'s and '{}'s
		colour		simple plus ansi colour syntax highlighting
		color		alias for colour (colour excluding you :-) )
		1line		one line output
		nows		One line output, no extra whitespace
```

Updated man pages for the three tools for this change as well as a typo fix in
`jnamval.1` for the `-p` option (wrongly listed as `-P` which is something
else).

Updated `tmp/TODO.md`: the options for the three tools should be in the same
order, particularly the common options, both in usage string and man pages. But
since the specs are not finalised this needn't be done yet.


## Release 1.0.49 2023-08-05

New version of `jnamval`: `"0.0.11 2023-08-05"`. Add `-I` option with same
semantics as `jfmt`. Updated `jnamval.1` to add this option and fix typos.

Change available options for `jnamval -t` to be that of `jval -t`. Updated man
page.

Typo fixes in `jfmt.1` and `jval.1`. Removed `-I` from `jval.1` as it's not a
valid option.

Fix typo in usage string in `jnamval` which referred to non-existent option (it
is actually an option arg, not an option, that is correct and that is now what
it says).

Add option `-P` to `jnamval`. This is just like what `-t` for `jnamval` used to
be but it will indicate what to print rather than what to match. Man page
updated for this option. As the code is common to both `jval` and `jnamval` the
`jval` utility could be updated easily to support this option is desired but
this is TBD later.

Add some bad test JSON files in the `jparse/test_jparse/test_JSON/bad_loc`
directory, updated Makefile rule Added appropriate `err` files.
Updated Makefile rule `rebuild_jparse_err_files` to not show the command being
run for each file to make the output clean (and to show what it looks like in
the README.md file - which has been updated due to slight changes).

Fix warning message of `mkiocccentry -y` and move it to happen before the option
to ignore all warnings as that one says no more warnings will be displayed so it
should come last.

Removed the "program" arg from `parse_verbosity()`.  The `parse_verbosity()`
no longer exits on errors or bad arguments.  Instead `parse_verbosity()`
returns `DBG_INVALID`, a value < 0.

The return from `parse_verbosity()` is now checked for a < 0 return
value and an appropriate error is raised when that happens.


## Release 1.0.48 2023-08-04

Bug fixes in `location` tool wrt the `-a` and `-s` options. New version `"1.0.2
2023-08-04"`.

Change `-N` to `-n`.

Always use re-entrant version of the lookup functions as this allows for proper
showing of both name and code whether or not one is using `-s` or `-n`. If `-a`
we still show all (as before) but without `-n` we previously showed both name
and code like `code ==> name` when it should only show both if verbosity level
is > 0. Note that the chkentry tool _SHOULD NOT_ and _DOES NOT_ use the
re-entrant versions.

Use `parse_verbosity()` for parsing the `-v` option.

Updated man page with an example added and updating `-N` to `-n`.


## Release 1.0.47 2023-08-03

New version of `location` tool: `"1.0.1 2023-08-03"`.

Improved tool to allow for `-a` (show all) and `-s` (substring search). Uses new
re-entrant versions of the functions `lookup_location_name()` and
`lookup_location_code()`. These new functions take a `size_t *idx` which is set
to the _next_ element in the table (if != NULL) so that the next call can skip
continue _after_ the previous element. They also take a `struct location **`
which stores the _found_ location. This is useful for `-v 1` as one can then see
something like:


``` <!---sh-->
./location -a -s -N -v 1 'united'
United Arab Emirates ==> AE
United Kingdom of Great Britain and Northern Ireland (the) ==> GB
United States Miscellaneous Pacific Islands ==> PU
Tanzania, United Republic of ==> TZ
United Kingdom ==> UK
United States Minor Outlying Islands ==> UM
United Nations ==> UN
United States of America ==> US
```

Without the `-v 1` it will only show:

``` <!---sh-->
$ ./location -a -s -n 'united'
AE
GB
PU
TZ
UK
UM
UN
US
```

If one does not use the `-n` option the `-a` is less useful as that function
checks explicitly that the length is two characters so it has to be an exact
match. Nevertheless there is a re-entrant version of the function that works
much the same way as the other and the `-a` is processed without `-n`. Use of
`-s` requires an arg much like `-n` but it does NOT require `-n` itself.

The rationale behind these changes is they will make it easier for people to
find their country code, if they do not know what it is (or they want say
anonymous and don't know that it's `XX`). Search is done case-insensitively.
Another example use:

``` <!---sh-->
$ ./location -asnv 1 germ
German Democratic Republic ==> DD
Germany ==> DE
```

I, that is Cody, observe that Germany is a country full of 'germ ridden
people' (this is not really true of course but it's a fun pun of mine :-) )

Add `tmp/TODO.md` file with todo items not specific to any open issue.


## Release 1.0.46 2023-08-02

Move `parse_verbosity(3)` to dbg/dbg.c as it is the dbg code that uses the
`verbosity_level` variable. Ran `make test` to verify that this works okay.

Updated `dbg.3` man page to add this function to the man page. Also improved the
man page in some clarification of other functions (call semantics for example).

New version of `dbg` facility and the `jparse` and json parser: respectively
they are `"2.11 2023-08-02"` and for both `jparse` and the json parser `"1.1.4
2023-08-02"`.

Remove `parse_verbosity(3)` from `dyn_array(3)` facility as it is in the
`dbg(3)` facility and the `dyn_array(3)` facility uses the `dbg(3)` facility.

Update `jfmt.1`, `jval.1` and `jnamval.1` for `-F` option. There might be a
colourful update in the three files.

Typo fix in `print_test.c` and make a comment that exists in more than one place
consistent in location.

## Release 1.0.45 2023-08-01

New version of `jfmt`, `jval` and `jnamval`: `"0.0.10 2023-08-01"`.
Add two new format options to `-F` option of `jfmt`, `jval` and `jnamval`:
`1line` which prints output on one line with one space between the tokens and
`nows` for no extra whitespace. There might or might not be a joke somewhere in
here :-)

Fix formatting in `-h` usage string of the above mentioned tools as well.

New version of `jparse_test.sh`: `"1.0.3 2023-08-01"`. The script now runs
`print_test` instead of the Makefile doing it and thus cluttering up the output.
A thought was to have it run a new script `run_print_test.sh` but this was not
necessary and would only add more complexity as the script would just have to
run the tool and check for exit status.

Fix `jparse.3` man page for the change in the `converted` and `parsed` boolean
convenience macros.


## Release 1.0.44 2023-07-31

Add `-F format` option to to `jfmt`, `jval` and `jnamval`. New version for tools
`"0.0.9 2023-07-31"`. `color` is aliased to `colour`. Other options are
`default` and `pedantic`. Debug output (with `DBG_NONE` which has to change -
later on) is added based on the optarg. This option has NOT been added to the
man pages. That will come later either later today or another day.

Add missing setting of variables in `alloc_jnamval()`.


## Release 1.0.43 2023-07-30

Improve, typo fix and bug fix `jparse/test_jparse/print_test.c`. Now checks the
file descriptor versions of the printf functions. Added `not_reached();` call
after an error function was called. New version `"1.0.1 2023-07-30"`.


## Release 1.0.42 2023-07-29

Add `jparse/test_jparse/print_test` test tool to test if various
print function calls fail.  Here "print function call" refers to:

```
print(3), fprintf(3), dprintf(3), vprintf(3), vfprintf(3), vdprintf(3)
```

We also test various print-like functions from `parse/util.c` such as:

```
fprint(), fprstr(), print(), prstr(), para(), fpara(), fpr(), vfpr()
```

Added `chk_stdio_printf_err(stream, ret)` to determine of a
print function call had an error.

All "print function call"s are now followed by a call to
`chk_stdio_printf_err()` to properly detect if there was a
failure in the "print function call".

Add `print_test` execution to `make test` in `jparse/test_jparse/Makefile`.

New versions of `jfmt`, `jval` and `jnamval` with some minor bug fixes and
enhancements: `"0.0.8 2023-07-29"`, `"0.0.8 2023-07-29"` and `"0.0.8
2023-07-29"`. The version change synchronises the version number to keep track
of the versions since the date was the same. This does mean that the version for
both `jfmt` and `jnamval` jumped from 0.0.6 to 0.0.8 but this seems like a
worthy compromise to keep things together especially as the documented change
for the version update that technically took place on 28 July 2023 was or if it
was is no longer documented.

For `jval` and `jnamval` the functions `parse_jval_args()` and
`parse_jnamval_args()` have been updated to take the proper struct, a pointer to
`argc` from `main()` and a pointer to `argv` from `main()`. That is a `int *`
and a `char ***`. The functions increment `argc`/`argv` past the file name so
that `main()` can check if there are any args without having to worry about
`argv[0]`. The function (not `main()` - the parsing ones) now show debug
information about the args shown. Currently the debug level is 0 so that it's
always printed but this will change later on. As there is no implementation yet
(but see below) there is no list or array or searching of any kind with the
args.

Since the ability to check if any args is now easy and since `-i` flag with no
args specified means that there are no matches and that indicates exiting 8
`jval` and `jnamval` now have this check. That is the extent of any
implementation details until we have discussed output, how `jfmt` should format
code, search routines are added and numerous other things. This was just an
aside, one might say.

Added `XXX` comment to the print count function of each tool that points out
that currently the count is not correct. This will be removed in time.

Update `jval(1)` man page - add exit code 8 to list of exit codes.


## Release 1.0.41 2023-07-27

Update `json_util_README.md` document.

Added missing exit code to `jval` tool. Updated version string to: "0.0.6
2023-07-27".

Sequenced exit codes.


## Release 1.0.41 2023-07-26

New version of `jfmt`, `jval` and `jnamval`: "0.0.5 2023-07-26".

Moved code in `jnamval` common to both `jval` and `jnamval` to `json_util.c`
like what was done yesterday for `jval`.

Fixed bug where one could not do:

``` <!---sh-->
echo '"test"' | ./jfmt -
```

and the same thing with `jval` and `jnamval`. This also fixes the bug where one
could not do:

``` <!---sh-->
./jfmt -
"test"
^D
```

and the same thing with the other too tools. The above two fixes changes the use
of `parse_json_stream()` to `parse_json()`. Note that the `FILE *` is still
needed.

Add to `struct json_util` the file path of the JSON file as `json_file_path`.

Check that the path is not an empty file name and exit with error code 3 if it
is in all three tools.

New version of `jparse` and json parser: "1.1.3 2023-07-26".

Improve how `parse_json_stream()` deals with NULL filename.  This means that the
check for NULL is moved higher up in the function so that if the path is NULL we
set it to `-` for stdin. This check was in the function already but the purpose
was to make it so that if NULL it is stdin. If it's NULL though it's likely that
the stream is also NULL which means that the check would never get triggered.
The `stream` can still end up NULL but it shouldn't be a problem if stdin now.

Improve check if file is ready (function `fd_is_ready()`) in
`parse_json_stream()`: don't check if stdin as it should be ready anyway and not
doing this seemed to prevent it from reading the file (in some cases).

## Release 1.0.40 2023-07-25

New version for `jfmt`, `jval` and `jnamval`:  "0.0.4 2023-07-25". Moved common
variables from the structs for each tool (`struct jfmt`, `struct jval` and
`struct jnamval`) to `json_util.h` and prefix with `json_util` rather than the
`(jfmt|jval|jnamval)_`. More work needs to be done here as `jval` and `jnamval`
share variables as well but this will be done in another commit.

The structs `(jfmt|jval|jnamval)_number` are now `json_util_number` in
`json_util.h` and the associated code has been moved to `json_util.c`.

The code for the common `-L` option to the three tools has also been moved to
the `json_util.c` file.

Any other options common to the three tools that I have not mentioned here have
also been moved or so it is believed (it is possible some was missed but these
will be located in time if any are missing).

Move variables and functions common to `jval` and `jnamval` to `json_util`
except that only `jval` currently uses them. `jnamval` has more options not used
by `jval` and so it'll take a bit more work to get it done. Some of the code is
slightly different functionally so that code was not moved over. In particular
some options that are common to both tools have it where `jval` has fewer
option args: `-t` for example. The macros that have been updated now are as the
new ones in `json_util.h`.


## Release 1.0.39 2023-07-24

Minor fixes to JSON convenience macros that check for a valid or parsed node.
Now they check that `item` != NULL first. I kept the `== true` checks that were
added to them for the booleans even though it doesn't match my style simply
because as a macro it might not be clear immediately that it's a boolean so it
seems like it might be a good place to do that.

Fixed `warning: incompatible pointer to integer conversion passing 'struct json
*' to parameter of type 'enum item_type'` in `json_util.c`.

Add `:` suffix to parsed/converted boolean in debug output of `JTYPE_STRING` and
`JTYPE_NUMBER` (via `fprnumber()`) in `vjson_fprint()`. For `parsed` if
converted is also false add a `:` else a `,`. Print a `:` after converted flag.
This separates the parsed/converted status from the actual information of the
type. Here's an example log output for numbers:

```
JSON tree[3]:	lvl: 1	type: JTYPE_NUMBER	{p,c:-Fddildldi}: value:	-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.0
JSON tree[3]:	lvl: 1	type: JTYPE_NUMBER	{p:FE}: value:	1e100000000
```

Note for the first line it has `p,c:` as the number was both parsed and
converted but for the second line it has just `p:` as the number could not
be converted. The flags follow the `:`. Now one might argue that the bools are
part of the numbers but the other data is the number itself.

For strings it's likewise just the rest is string data, not number data:

Updated json parser and jparse version strings to "1.1.2 2023-07-24".

New definition of recently removed macro `CONVERTED_JSON_NODE`:

```c
#define CONVERTED_JSON_NODE(item) ((item) != NULL && (item)->converted == true)
```

Updated `jval` and `jnamval` to parse the options `-n` and `-S` as a list as
each option can be specified more than once. A list seems the most natural
approach though a dynamic array might be considered later.
New version for both tools: "0.0.3 2023-07-24".

## Release 1.0.38 2023-07-23

Fixed `jnum_chk` by correcting the output of `jnum_gen`.
Fixed an incorrect prior use of `make rebuild_jnum_test`
and hand verified the generated `jnum_test.c` file.

Added large e-notation values to the jnum.testset.

Improved `json_alloc()` to explicitly initialize critical booleans
and pointers when allocating a new JSON parse tree node.

Both `json_process_decimal()` and `json_process_floating()` no
longer set `parsed` and `converted` booleans.  That task
if performed by the `json_conv_number()` function.

The `json_conv_number()` now correctly attempts to convert
a given JSON number in floating point, e-notation and
integer depending on if the result of `is_floating_notation()`,
or `is_e_notation()` or `is_decimal()`, respectively.
Those tests are used to set the `is_floating`, `is_e_notatiion`,
and `is_integer` booleans, also respectively.

Improved JSON debug messages from `json_process_decimal()` and
`json_process_floating()`.

Simplified how the `is_valid` boolean arg is used in `jparse/jparse.l`.
Refer to `tree` (instead of "node") when referring to the JSON
tree in `jparse.l`.

Updated convenience macros in `jparse/json_parse.h`.  Now using:

```c
#define PARSED_JSON_NODE(item) ((item)->parsed == true)
#define CONVERTED_PARSED_JSON_NODE(item) (((item)->parsed == true) && ((item)->converted == true))
#define VALID_JSON_NODE(item) (((item)->parsed == true) || ((item)->converted == true))
```

Use of `CONVERTED_JSON_NODE(item)` changed to `CONVERTED_PARSED_JSON_NODE(item)`.

Fixed "copy and paste" typo in `json_tree_print()`.


## Release 1.0.37 2023-07-22

Use of `-H` in `jnamval` implies `-N`.


## Release 1.0.36 2023-07-21

Fix bug in `vjson_fprint()` where numbers that were converted and parsed were
not printed as the check accidentally was using the wrong macro that checks if
converted is true and parsed is false. It should be it checks for both are true
and then the next check in the else if checks if parsed is true and converted is
false.

Move `converted` bool in `struct json_` structs below the `parsed` bool.

Make `jval` and `jnamval` exit with 7 if a number cannot be represented by a C
type. Updated man pages and usage messages. For `jnamval` this means what used
to be 7 is now 8: no matches found.

For number conversions in `jval` and `jnamval` use the macros to check for
converted/parsed booleans.


## Release 1.0.35 2023-07-19

Add initial version of man pages of `jfmt(1)`, `jval(1)` and `jnamval(1)` to
repo.

Minor bug fixes in `jfmt`: set default spaces for `-L` and `-I`options to 4, not
0, and add missing `-q` description to usage message. Updated version to "0.0.3
2023-07-19".

Add the three JSON tools `jfmt`, `jval` and `jnamval` to `bug_report.sh`.


## Release 1.0.34 2023-07-18

New jval version "0.0.2 2023-07-18". Option parsing is complete. All that is
left for this is adding the test code for `-S` and `-n` (see below for why these
options were not completely parsed).

Fix `jval` parsing of `-S op=str` and `-n op=num` so that the json conversion
routines are used in order to check that everything is okay according to the
JSON spec. What is NOT implemented is the routines to run the comparisons: that
will be a longer function and also has to happen after option parsing is done
for all three tools and then the man pages are written and followed by that
discussion.

Note that if the string starts with a '"' we pass in true for the quote
parameter to the conversion routine and otherwise we do not. It's the
responsibility of the user to ensure that the string is properly formed
otherwise. A possible problem, however, that might need to be addressed, is
whether or not it can compare strings with quotes in it. This can be decided
later.

If `jval -c` or `jval -C` is used show a total count of matches. Currently will
be 0 and for `-C` the output will be incorrect but that's okay as that's all we
have available now and it lets us test the options.

Sequenced exit codes.

Remove `any` match type from `jval`. This was added by me as it seemed useful
but since it's added to `jnamval` I presume that it should not be in `jval`.

New version of `jnamval` "0.0.1 2023-07-18". All options should now be parsed
(unless some were missed by accident). Options test code was updated to test the
new bits. Exit codes were changed from the original `json_util_README.md` (which
has been updated) as 1 is already used for unable to write to output file which
is in all tools now (and since 1 was used for jfmt in that way it seems better
to keep them consistent).


## Release 1.0.33 2023-07-17

Updated `jval` version to "0.0.1 2023-07-17".

Add parsing for all options `jval`. Test code for `-n` and `-S` still needs to
be written but everything should be fine.

Removed unused `jval` code except that one function which _might_ be useful
later is a placeholder - does nothing functionally.

Removed unused `jval` structs.

Add check that out file is not stdout before flushing/closing when freeing jfmt
struct.

## Release 1.0.32 2023-07-16

Add initial code for `jfmt`: completely parses options and will dump JSON to
output file if it is not the same as the input file and the file does not
already exist. If not specified it will be stdout.

Add **TEMPORARY PLACEHOLDER** source and header files for `jval` and
`jnamval` tools. The key term that is important is _TEMPORARY PLACEHOLDER_. The
files are actually copies of what were once jprint related but for each tool
jprint / JPRINT were changed to their new tool counterparts. This is done this
way because some of the code is common and this way they are in the repo. A lot
of code will be removed but some will be kept. Much will also be changed. Option
parsing in particular is useful but this will also have some changes. As it will
take more work to get them in order I have kept the other things in too for now
but this **GREATLY** simplifies starting the tools which seems like a worthy
thing to do.  A very nice way that this simplifies starting the tools is that
the parsing of the JSON file is already done. Along with this some of the
options being the same and some of the structs being the same (though they will
have to be modified and some will be removed) it will help get started.

Respective test scripts for each tool have also been added which are also
updated copies of the old script for jprint. These are not run yet because there
is no point in running them yet and it would only slow down make test.

With the exception of the scripts the required changes will be done soon -
hopefully sometime in the coming week (real weeks start on Monday) - but what
this means is that **TEMPORARILY** the tools function as the incomplete no
longer useful jprint. By required changes I mean that the unnecessary code will
be removed and option parsing / usage strings will be updated (whether the
option parsing is completed in the next week is indeterminate).

Updated `jparse/Makefile` and run make depend.

Update .gitignore: fixed references of `jprint` and add the other tools.

## Release 1.0.31 2023-07-15

New JSON parser and jparse version "1.1.0 2023-07-15".

Completely resolve issue #752: 'Enhancement: add parsed boolean to the JSON
parser'. This involved the below changes.

Add more checks to JSON node `parsed` member so that if converted is false but
parsed is true it's not an error (in the cases where parsed is allowed to be
different it is not an error: if they're not allowed to be different we warn
about it if they are different).

Improve printing of string variables and numbers in `vjson_fprint()`.

Add helper macros `CONVERTED_PARSED_JSON_NODE`, ` CONVERTED_JSON_NODE` and
`PARSED_JSON_NODE` and updated jparse.3 man page. The
`CONVERTED_PARSED_JSON_NODE` checks that both the `converted` and `parsed`
booleans are true. The `CONVERTED_JSON_NODE` checks that the boolean `converted`
is true and that the `parsed` boolean is false and the `PARSED_JSON_NODE` checks
that the `parsed` boolean is true and that the `converted` boolean is false.
These macros have been used in in place of checks of `foo->converted` and
`foo->parsed`.

Rename function `sem_node_valid_converted()` to be just `sem_node_valid()` and
make use of appropriate macro: for numbers it uses the `VALID_JSON_NODE` and or
others it uses `CONVERTED_PARSED_JSON_NODE` as both `converted` and `parsed`
must be true for non-number nodes.

Fixes in `json_util_README.md` up through the beginning of `jnamval`. The rest
of the document and a more thorough check of that which has been checked will
come later.

## Release 1.0.30 2023-07-14

New JSON parser and jparse version "1.0.11 2023-07-14".

> Note on version 1.0.10: yesterday, 13 July 2023, the versions of both the JSON
parser and jparse were updated but the date was not updated. The version date in
the changes for 13 July 2023 has been updated below to not cause confusion.

Added placeholder functions `is_floating_notation()`,
`is_floating_notation_str()`, `is_e_notation()` and `is_e_notation_str()` for
issue #752. These functions are NOT called yet except that the `_str()` versions
call the non-`_str()` versions. Both return true without any tests at this time
which is why they cannot be called in the parse functions!

Added to each `struct json_foo` a boolean `parsed` which means that the value
can be parsed but if false it would indicate it cannot be converted. This is
important because a really big number say a googolplex would fail to be
converted in C but it doesn't mean it's not valid JSON. We don't want to tie the
parser to a big number library so other things will have to be done but we
cannot say that it's not valid JSON.

Add back good json file `googolplex.json` as it now is considered valid json: if
the error is a range error then we know it's valid JSON so we can set parsed to
true. If it's something else we cannot. THIS IS TEMPORARY! It could be that
there is both a range error and it's an invalid string. There are XXX comments
about this as it needs to change!

Fix detection of '.' with e notation if a '.' found and also whether 'e' was
found in the jnum tests.

Add code to `is_floating_notation()` and `is_e_notation()` functions. More needs
to be checked and there are still some `XXX` comments to look at. Note that the
code here does the same checks as `json_process_floating()` and this might not
be strictly correct but that can determined later: as it's used only in JSON
parsing it should be okay for the time being.

Check for more than one dot in floating point number strings for JSON and
otherwise. For JSON this is not strictly necessary as the scanner will report an
invalid token but we do it for safety anyway.

Add to function `vjson_fprint()` code that prints out the string of a number if
converted is false but parsed is true.

Add macro `VALID_JSON_NODE` that determines if a `struct json_foo` is valid
which means that either `item->converted` or `item->parsed` is true. This macro
is currently used prior to reporting an error in the conversion functions. This
macro is documented in both `jparse.1` and `jparse.3`.


Add function `sem_node_valid_parsed()` which is like
`sem_node_valid_converted()` but for the parsed boolean instead of the converted
boolean.

Update `jparse.1` man page noting the condition where if a value cannot be
converted but is valid JSON it will still report that it's valid JSON but will
warn that conversion failed.


## Release 1.0.29 2023-07-13

New JSON parser and jparse version "1.0.10 2023-07-13".

Fix regexp for `JSON_NUMBER` in json parser. For exponents it only allowed a
fraction followed by an exponent rather than an integer followed by an exponent.
In other words `1.0e0` was valid but `1e0` was not.

Note that a googolplex is too big for C so the file `good/googolplex.json` was
removed. For reference a googolplex is
`1e10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000`.
We don't have a directory for the jparse test suite that is although valid JSON
is not possible to parse. We do incidentally have this functionality for
info.json and auth.json files though.

A googol (yes this is where the name 'Google' comes from :-) ) is `1e100` and is
not too big for C.

The file `test_ioccc/test_JSON/general.json/bad/info.timestamp-0.json` is
actually valid JSON so it was moved to the good subdirectory. Now make test at
the top level directory works fine.


## Release 1.0.28 2023-07-03

Fixes and improvements applied to the new
[json-README.md](jparse/json-README.md) document (which was not documented here
yesterday when it was added).


## Release 1.0.27 2023-07-02

Bug fixes in `jparse_test.sh` (erroneously it referred to a variable that did
not exist in the function in question - to do with error location tests - and
also had a log message outside the check of the verbosity level).

Add several new good test JSON files under `jparse/test_jparse/test_JSON/good/`.
Add another bad JSON test file for both location reporting and otherwise.

In an effort to write the specifications for command line utilities
that read JSON, and to assist others in understanding the JSON
related code, code comments, data structures and documentation found
in this GitHub repo, we add a first draft of `jparse/json-README.md`.

The modified JSON string encoding rules as given in `jparse/json-README.md`
have been implemented.  The following characters are no longer encoded
and are represented as themselves:

- & is no longer encoded
- / is no longer encoded
- < is no longer encoded
- > is no longer encoded
- [\x80-\xff] is no longer encoded


## Release 1.0.26 2023-06-29

New version of `jprint` "0.0.33 2023-06-29".

Removed 'num' from description of 'simple' for options `-t` and `-Y` and instead
say what it translates to (int,float,exp).

Added four new good test json files under `test_jparse/test_JSON/good`:
`42.json`, `foo.json`, `just_array.json` and `top_level_array.json`.

Sorted (by category) the member variables in `struct jprint` as well as in the
function `alloc_jprint()` (in `jprint_util.c`).

Added options `-o` and `-r` but at this time nothing is done with them: booleans
are set but nothing else. Why is `-o` before `-r`? To spell out 'or' which is
what the `-o` option does! Obviously :-)

Make `bug_report.sh` check that both `jprint` and `jprint_test.sh` are
executable.

Add support of proper matching for JSON booleans and nulls. This does not apply
(just like for the others that are in) for inside arrays - just those by
themselves. Works for `-Y` to search by value and without `-Y` to search by
name, showing the name or value respectively, depending on if a match is found.
The `-p` or `-j` options can override this of course. It should be remembered
though that nulls and booleans have a string name so only string is checked
before adding a match.


## Release 1.0.25 2023-06-28

Add new good JSON test file 42.json under
`jparse/test_jparse/test_JSON/good/42.json`.

New `jprint` version "0.0.32 2023-06-28".

Fixed potential memory leak in `jprint`: although currently none exist the
matches list in the struct jprint (instead of the matches list in each pattern)
was not freed.

Use of `jprint -s` and `jprint -j` now possible.

New jparse and json parser version "1.0.9 2023-06-28". Add new function `extern
char const *json_get_type_str(struct json *node, bool encoded);` to jparse
library to get the matched (in the parser/lexer) name that triggered parsing.
Updated man page.

Make use of `json_get_type_str()` in `jprint` for modularity. This only applies
to strings and number types.

## Release 1.0.24 2023-06-27

New `jprint` version "0.0.30 2023-06-27".

Fix possible NULL pointer dereference. It should never have happened but it was
theoretically possible. This fix involves a slight change in the way
`is_jprint_match()` works in that it takes an additional `char *`: a name that
is the name to match if `pattern` or `pattern->pattern` is NULL. If both are
name and either `pattern` or `pattern->pattern` are NULL it is an error. This
simplifies checking a bit and there is another use in mind that will have to
come with another commit (if it proves useful; as the concept of `name_arg`s is
currently not correct it might be that this won't be useful at all and that the
struct is even removed).

`jprint -Y str` partially works now. It's not perfect in that in some cases it
can end up matching other types but that depends on the JSON file and options.
The function `add_jprint_match()` can now have a NULL pattern and also NULL
`pattern->pattern`. If `pattern == NULL` then the new `jprint->matches` is
iterated through and set up. This change is to facilitate adding matches without
patterns for when printing the entire file but this part has not been
implemented yet. More needs to be tested when calling this function but other
things have to be done before that can be done and once again it might be that
the pattern struct and patterns list is completely removed once certain pending
functions are added to the jparse library.

Add back call to run json check tool. Used after reading in the data block but
prior to parsing json. Fix use after free in the function that does this. It
dereferenced the jparse struct after calling `free_jparse()`. This was not
detected because it happens only when the tool failed and the test case used was
not invalid where I tested it but is in macOS.

Add `struct json *json_tree` to `struct jprint` as a convenience. Some functions
are directly passed this but this could be changed if desired.

Fixes in handling of `jprint -Q`. We have to check not only that it's a string
and printing syntax or quotes are requested but that the type of the match is a
string (`JTYPE_STRING`) as well. Get rid of string bool in `jprint_match`
struct as it's not needed with the `type`.

Improvements in printing some types wrt quotes. Now the function
`add_jprint_match()` takes two types, a name and value type and the `struct
jprint_match` has a `enum item_type name_type` and `enum item_type value_type`.
The determination of which type to use still only has one node at a time so it's
not perfect but it does help somewhat and will be of value later on.

Further update version of `jprint` to "0.0.31 2023-06-27". The handling of
adding matches of strings and numbers with values and names being different (if
they are in the file) has been implemented. This required a lot of extra code
and more modularity needs to be added but that can come later.


## Release 1.0.23 2023-06-26

New `jprint` version "0.0.29 2023-06-26".

Fix handling of `-L` and `-I`. Indenting and printing of levels was messed up.
Doing this I was able to remove a lot of duplicate code too.

Add file `test_jparse/test_JSON/good/JSON_misfeature_number_7.json` from a
GitHub comment.

Minor update to `h2g2.json` file - add a boolean to elements of the person
array.

Added stub code for JSON tree search functions.

Added pre-order (NLR) and post-order (LRN) search option to JSON
tree talking.  Added post_order arg to `json_tree_walk()` and
`vjson_tree_walk()`.

Added depth arg to `json_tree_walk()`.


## Release 1.0.22 2023-06-25

New `jprint` version "0.0.28 2023-06-25". Removed `-G regex` option.

Slight formatting in `jprint` usage string (to do with aliases).

Rename `struct jprint` boolean `substrings_okay` to `use_substrings` as it seems
a better description and it matches the pattern struct as well.

Modularise `vjprint_json_search()` by new function `is_jprint_match()` which
takes the `struct jprint *`, the pattern, node and string and based on the type
does the right checks. Will not return on NULL pointers. Returns true if it is a
match and false otherwise.

Add more safety when checking for matches by checking that `item != NULL` and
that `item->converted` is true and that the string is not NULL.

Clarify that one can use `-p both` or `-p b` but not `-p n`/`-p name` or `-p
v`/`-p value` with `-j`. This was changed yesterday as it's just as easy to
check (and is slightly more user-friendly) that as it is for explicit use of
`-p` full stop.

Move most functions from `jprint.c` to `jprint_util.c` and move structs to
`jprint_util.h`. This is because most functions are utility in nature. The
`jprint_sanity_chks()` remains in `jprint.c` as `usage()` is the same name as
other tools usage function so it's a static function in `jprint.c` but
`jprint_sanity_chks()` uses that function. It could also be renamed to
`jprint_usage()` of course.


## Release 1.0.21 2023-06-24

New `jprint` version at "0.0.27 2023-06-24". If `-j` is used don't make use of
`-p b` or `-p both` an error. It's only an error if printing of just name or
just value is specified (after the `-j` as `-j` will set both). Checking for
this is just as simple as for `-p` being used at all and it seems slightly more
user-friendly to do it this way.

Modularise option checking of `jprint` by moving it to the
`jprint_sanity_chks()` function which now returns a `FILE *`, the file to read
the JSON from. The function will never return a NULL `FILE *`. It will not
return with a command line error. It will check all options and verify that the
right number of args have been specified. `main()` will add the `name_arg`s to
the patterns list and go from there if `jprint_sanity_chks()` returns. As the
`argc` and `argv` have to be shifted in main() they are a `int *` and `char ***`
respectively rather than their usual `int` and `char **`.

Add function `parse_jprint_name_args()` to iterate through command line, looking
for `name_arg`s. This function is called by the `jprint_sanity_chks()` as some
options have to be checked after looking on the command line for `name_arg`s.

Make running JSON check tool more modular which fixes bug of printing output
more than once. The `struct jprint` has the `FILE *json_file`, `char
*file_contents` as well as a `FILE *` for the json check tool stream and `char
*`s for the check tool path and args.

Make `jprint.c` functions in same order as in `jprint.h`.

Implement `jprint -Q`.

Fix `jprint -L` for some types. Not all types have been tested but numbers,
strings and possibly members are correct. When more is done other types will be
verified too.


## Release 1.0.20 2023-06-23

New `jprint` version at "0.0.26 2023-06-23".

Add booleans (including renaming one `uintmax_t` to be a boolean and adding a
new `uintmax_t` in its place) in `struct jprint` to indicate that specific
options were used rather than relying on the `uintmax_t`s being not 0.

Add inclusion of `string.h` and feature test macro `_GNU_SOURCE` to `jprint.h`
for `strcasestr()`.

Fix sign comparison warning in `util.h`.

Fix use after free error in `util.c` (function `vcmdprintf()`).

Fix error in `jprint_run_tests()` causing invalid test result and warning about
unused computed value.

The `jprint_match` struct (which is still a work in progress) now has two
`struct json *`s: the node name and node value. The name is what was matched,
whether or not value was searched or not and the value is what will be printed,
either value or name depending on the options. As both might need to be printed
this can also be a misnomer but I believe having the nodes will be useful at
some point.

Improve (but not yet fixed) the value / name added to matches particularly with
substring matching. The value is still the same as the name in the match struct
but with substring matches the match is the pattern (`name_arg`) specified but
the value (what to be printed) is the actual member found.

Typo fix in debug message. The `jprint -o` option no longer exists but it was
referenced.

New `jparse` version "1.0.8 2023-06-23". Fixed display bug in `vjson_fprint()`
for some of the int types. In particular it cast the booleans to an int rather
than use the `as_type` members of the struct. This would mean that instead of
printing the actual number it would print a boolean as an integer.

More modularity in `jprint` printing matches. Several new functions have been
added. These will be useful later for the printing of not just a JSON file if no
pattern requested but also printing matches. More will be added.

Fix typo in usage string with `-Y` based on the original concept. It takes just
one `name_arg` but is not an option arg but rather an arg to the program itself.
Only the type is an option arg to the option.

Fix unmatched `.RE` in jsemtblgen.8 man page.

Add initial man page for `jprint`. This is currently incomplete. The `BUGS`
section just says that there are too many things to list at this time as the
tool is in progress and there are a number of things that are incorrect even
besides it. There are some examples but with the exception of one that prints
out the file it is not correct output (and it's not correct for the file in full
depending on options specified). The synopsis is complete and the description is
a good start but the options list is incomplete. There are a lot more to be
added but I want to have the file in and I'll continue working on it in the
coming days.

## Release 1.0.19 2023-06-22

New `jprint` version at "0.0.25 2023-06-22".

Modify behaviour of `jprint -c` to completely match `grep -c` (which was my
original idea in the first place). If more than one pattern (as in `name_arg`)
is specified and found it is a total of the matches found.

Implement `jprint -E` to such a degree as is currently possible.

Minor improvements to `jprint` usage string including not hard coding in the
string the default maximum depth to parse, instead using the macro
`JSON_DEFAULT_MAX_DEPTH`.

Implement `jprint -P`. When this option is disabled the spaces specified still
separate the tokens if printing both name and value. Default is 1 space, changed
from 0 (it was always supposed to be 1).

When printing both (without json syntax) print on the same line the name and
value.

Added boolean to indicate that `-b` was used. Currently this is unused but it
will allow not relying on the spaces being not 0 as the default is 1. This fact
is not yet needed but it might be and it's a clear way to indicate if the option
was used or not.

Make matching printing function a bit more modular by introducing a new
function, `jprint_print_match()`. Note that the concept of patterns is incorrect
and has to be fixed. Discussion is pending.

New jparse and json parser version "1.0.7 2023-06-22". Make print tree function
a bit better in that for `null` type it prints not `NULL` but `null` to indicate
it's a JSON `null` to remove any ambiguity from C's `NULL`.

Typo fixes in `jprint` usage string.

## Release 1.0.18 2023-06-21

New `jprint` version "0.0.24 2023-06-21".

`jprint` no longer accepts negative max values for options `-l` and `-N`. Only
`-n` allows a max < 0. For `-N` it makes no sense and for `-l` it's of
questionable use. That can be decided later but let's just say that if it was a
square root it would be too imaginary :-) One of the `jprint -K` tests tests
negative max but no others do. In that code only `-l` was tested but the one for
negative max is now `-n`. A test case for parsing this error in command line
cannot be done because that's a fatal error which would make the program exit.
It might be possible to do this in the test script at a later time.

Implement `-L` option for indenting after levels with `-j` option. The text said
that `-L` has no effect without `-j` but I felt like it might be useful to have
it even without `-j` and the code is already there so for now I have kept it and
removed the text that stated it has no effect without `-j`. This can be undone
if necessary.

Implement `jprint -s` (substring match). This means that the only other kind of
matching that has to be added is regexps. It should be noted, however, that
the matching is not entirely complete and some of it will have to change as
well. This holds for exact matches as well.

Implement `jprint -I` (indent json syntax based on the depth).

Implement `jprint -b`.

## Release 1.0.17 2023-06-20

New `jprint` version "0.0.23 2023-06-20".

Process write only pipe if -S tool specified. To see details check git log and
search for 'Process write only pipe if -S tool specified'.

Implement `-c` option to show only count of matches. Use of `-o` (print entire
file) and `-c` (show only count) cannot be used together. Use of `-c` without
specifying any patterns is also a command line error as there cannot be any
matches.

Change debug level 0 messages to debug level 1.

Implement case-insensitive option `-i`.

New `jparse` and JSON parser version both at "1.0.6 2023-06-20". Added boolean
to `struct json_number` a boolean `is_integer` for integers to simplify checks
in `jprint`. If integer the booleans for floating points are set to false but if
number string is parsed as floating point only the integer boolean is set to
false. This is because the function to parse floating points should take care of
it (there are a lot of paths in the integer conversions) and that function only
returns true or false without giving an indication if it's an exponent or not.

Don't increment the count of matches if the type is not the same. By type this
refers to the `JTYPE_*` enum found in `json_parse.h`.

Add more strict checks for number types based on options.

Disable explicit check for strings in match function as the check there is
problematic anyway and this helps in another way until the function itself can
be fixed. It's very much a work in progress.


## Release 1.0.16 2023-06-19

New `jprint` version "0.0.22 2023-06-19".

Fix level constraints and matching of value versus name constraints. These have
only been tested in very basic JSON files but both now work in principle. There
were a couple problems. See the commit log for details (search for the string,
without quotes, `"0.0.22 2023-06-19"`).

Implement `-L` option.

Implement `-C` and `-B` options if `-j` specified (`-j` is required) and partly
implement `-j` (for simple files). It is not an error if `-j` is not specified:
it simply has no effect. The `struct jprint_match` now has a boolean `string`:
if true it means the match is a string.

Partly process `-Y` option. It is buggy for strings due to the way the function
is called. There is a workaround to let values be found but it also ends up
letting names be found. This has to be fixed at a later time. The number check
for `-Y` is incomplete as well and the simple and compound types are also not
checked yet.


## Release 1.0.15 2023-06-18

New `jprint` version at "0.0.21 2023-06-18". `jprint` now has a matches list per
pattern. The tree traversal functions will search for matches and after that
function finally returns the printing of matches will be found. This will
require additional functions that figure out how to print based on the json type
and jprint options along with the jprint type but for now it prints just the
name and value.

Note that by 'everything is added to the list' mentioned above this means that
name and value will be added as a value. This is something that will be fixed
later but it's the way the tree is traversed that causes this. Be aware also
that for _each_ pattern requested every member of currently supported types will
be added so you can see duplicates. This is because no matching is done yet.

New debug message added. The level is currently 0 so will always be printed but
just like the others this will be changed.

Stop extraneous newlines from being printed.

Add initial support for searching by value or name (value via `-Y` option). This
will only work for simple json files (or I have only tested simple json files at
this time). If value is being searched we print the name. I'm not sure now if
that's correct behaviour. If it is not then the check will have to be changed.

Checks for basic patterns are added! No regexp support yet. This does not mean
that the printing of name and/or value is yet correct. This needs to be worked
on. Right now a match can happen for either name or value even though it's
supposed to be one or the other.

Rename some functions to match what they do better (they do not print but search
for matches).

Added double blank lines before _Release_ markdown headers in this file.

Rename `is_open_stream()` to `is_open_file_stream()` in `jparse/util.c`.
Added `fd_is_ready()` and `flush_tty()` to `jparse/util.c`.
Added `flush_stdin` as new 2nd arg to `shell_cmd()` in `jparse/util.c`.
Changed `pipe_open()` in `jparse/util.c` to only flush stdin if `write_mode`.
Changed JSON parser to use new `fd_is_ready()` function.
Changed `jprint` and `jparse/json_sem` to use new `fd_is_ready()` function.
Updated `mkiocccentry` to not flush stdin when calling shell functions.
Updated `txzchk` to not flush stdin when calling shell functions.


## Release 1.0.14 2023-06-17

New `jprint` version at "0.0.20 2023-06-17".

Fix special handling for -Y option wrt exactly one name arg. The idea behind the
check is that if `-G` is specified then one cannot specify a `name_arg`. If it
is not specified then you must specify one `name_arg`. If both `-G` and a
`name_arg` are not specified it is also an error but there was a bug which
required that `-G` was used.

Don't show that substrings are ignored for patterns that are regexps (in
`jprint`).

Sequenced exit codes in `jprint`.

Add callback function and calling code to `jprint`. Currently it does not check
for any constraints and will print only json nodes which has a string as the
name or value. The name is not ultimately desired without printing json as a
whole but for now the name is printed. This means that both name and value might
be printed. There are too many newlines printed as well. Much more needs to be
done with these functions.

`jprint` now checks level (depth) constraints before printing a json member.
This needs to be tested more carefully with bigger documents.


## Release 1.0.13 2023-06-16

New `jprint` version "0.0.19 2023-06-16" - will print entire file if no pattern
specified OR the new option `-o` is specified but only if valid JSON (for both).
A note that if `read_all()` fails to read in the entire file then there could be
a problem but we only print the file if the data read is not NULL. This change
is important as it means now all that needs to be done is add the handling of
the JSON checker/semantics tool and then the compiling of regexps (if requested)
and then printing for any matches found (the search routines have to be written
too as does the code to traverse the tree - or 'climb down the tree' :-) ). In
other words one of the features of the program is complete! Note that use of
`-o` when a pattern is specified is a command line error.

Changed `pipe_open()` to allow for write mode. Takes a new boolean `write_mode`
which says to open for write mode rather than read mode.

Fix handling of `jprint -S` and `jprint -A` (specifically `-S`: a segfault in
some conditions namely if the path did not exist which was not checked for and
thus not triggered until this change). Verify path is a regular file and is
executable. Check args and if specified make sure not empty and make sure that
`-S` is specified as well. Checks `-S` path first.

If `jprint -S path` is an executable file then attempt to run, first redirecting
stdout and stderr to `/dev/null`, checking the exit code. If exit code is not 0
exit as an error. If it is 0 open a write only pipe. The pipe is not used yet
but will be once it is clear how it should be used. At this time the `-A args`
might be a misnomer or it might be a misunderstanding on my part or it might not
even matter. Currently it's used as options and the options list is terminated
by `--`. It might be that it's supposed to be args to the program, not options.
Or it might be that it doesn't matter. This is TBD later.

Fix minor bug in `txzchk` that prevented a debug message from being shown even
if the debug level requested was high enough. New version "1.0.2 2023-11-06".


## Release 1.0.12 2023-06-15

Change option letters of `jprint` a bit as described next.

`-s` now means search substrings. Conflicts with `-g`. `-s` was `-M`.

`-S` sets the path to the tool to run, which although in concept should be a
semantics tool checker, it can actually be something else like `grep`. This option
was `-s`.

`-A` sets the args to the `-S path`. This option was `-S`. Empty args are not
allowed and use of `-A` requires use of `-S`. The difference besides letters is
that args now must not be empty though this might not be strictly necessary.

Allow for both regular expressions and normal patterns via the `-G regexp`
option. Does not conflict with `-s` because the regexp is added as a regexp but
the `name_arg`s will allow substrings if `-s` specified - but not for the `-G`
regexp. `-G` does not conflict with `-Y` as long as only `-G` is used only once
and no additional `name_arg` is specified at the command line after `file.json`.

Fix bug where the command line could be messed up and the file.json is thought
to be a different arg specified.

Updated `jprint` version to "0.0.18 2023-06-15".


## Release 1.0.11 2023-06-14

Minor fix in `jparse` error location reporting. When the erroneous token was at
the very first column the column reported was column 0 which is incorrect by the
standard (de facto or otherwise) and also editors like vim which could be
confusing. New JSON parser version "1.0.5 2023-06-14". Since this also changes
the `jparse` tool indirectly I also updated the `jparse` version to be "1.0.5
2023-06-14" (skipping 1.0.4 in order to make it match as this seems like a good
time to make them match version and date).

Renamed `test_jparse/test_JSON/bad_loc/comma-eof.json` ->
`test_jparse/test_JSON/bad_loc/braces-comma-eof.json` as it actually has an
otherwise valid json file, `{}`. Added a `comma-eof.json` and respective error
file which has the first character being a comma.

Added several other test files (and their corresponding `.err` file) to have
more tests in JSON error location reporting.

Added another bad json file
`test_ioccc/test_JSON/general.json/bad/third-line-backspace.json`.

Added make rule in `jparse/test_jparse` to simplify adding the
`jparse/test_jparse/test_JSON/bad_loc` error files. To use run from the
`jparse/test_jparse` directory `make rebuild_jparse_err_files`. This rule
deletes the error files prior to regenerating them.

`jprint` version updated to "0.0.17 2023-06-14". `jprint -h` modified for the
`-m max_depth` option.  `jprint -Y` now accepts `simple` type as well.

Rename `jprint -S` to `jprint -M`, add new options `-s` and `-S` for a tool path
and tool args to be run on file.json. Right now all that is done is the old
processing of `-S` is now done for `-M` and `-s` sets a path and `-S` sets args.
It makes sure that if `-S` is specified then so is `-s`. It does not check yet
if the tool path exists, is a regular file and is executable and obviously it
does not execute anything yet either as this tool is to be written later and the
details are not specific yet either.

Make it so that use of both `jprint -j` and `jprint -M` is an error.

Change exit codes for `jprint`. Specifically 7 is no longer reserved for an
allocation error as it's more of an internal error. 7 is now for the JSON check
tool failure which is related to the options `-s` and `-S`. Sequenced exit
codes.

`jprint -Y` requires exactly two args to the command - the file and one
`name_arg`. Anything else is an error.


## Release 1.0.10 2023-06-13

New `jparse` version at "1.0.3 2023-06-12" and json parser version at "1.0.4
2023-06-12". The json parser version was bumped by .2 rather than .1 since it
should have been updated the other day when `jparse` was updated and it might be
that `jparse` itself did not need to be updated: that's not necessarily true
either way. The significant change here is that when encountering a `\t` we
increment the column by 8. This appears to be what a tab is but it might not be
perfect as there might be other cases where a `\t` is some other count. This has
to be tested further.

Update `jprint_test.sh` and add new files under
`jparse/test_jparse/test_JSON/bad_loc`. The new functionality, which is only run
if the option `-L` is passed to the script, tests for the exact match of errors
(in the `*.err` files) of the `*.json` files in this subdirectory. These JSON
files have syntax errors where the purpose is to test location reporting. If
there is not an exact match `make test` will fail. For now there is only one
file but more will be added in time. The purpose of this new feature is to test
the location reporting but we can add more files in time.  An important thing to
note is the new README.md file in `jparse/test_jparse` which explains what one
**MUST** do after adding a new invalid JSON file under
`jparse/test_jparse/test_JSON/bad_loc`. This information is also in the man page
`jparse(8)`.

New `jprint` version "0.0.16 2023-06-13" with new option. Using `-Y type` one
will be able to search by value instead of name. Currently it does not check for
only one value but this will likely change in a future update. Code does not yet
check that the type matches either. In other words if one specifies `-Y int` it
does not verify that the `name_arg` is actually an int. It might not be
necessary either since the JSON parser has the values and names as original
strings. This is TBD later. But an important question has to be answered right
now.

```
/*
 * Why is this option -Y? Why not Y? Because Y, that's why! Why
 * besides, all the other good options were already taken. :-)
 * Specifically the letter Y has a V in it and V would have been the
 * obvious choice but both v and V are taken. This is why you'll
 * have to believe us when we tell you that this is a good enough
 * reason why! :-)
 */
```


## Release 1.0.9 2023-06-12

New `jprint` version "0.0.15 2023-06-12".

Make `jprint -h` exit codes formatting consistent with `jparse`.

Make `struct jprint options` in jprint.c a `struct jprint *jprint` as it will
hold other information besides options including additional structs. Added
`free_jprint(struct jprint *jprint)` function to completely free everything in
it and then itself.

Fix bug in `jprint` checking of `-` for stdin. It shouldn't be just checking the
first char as being `-` but rather the entire arg. Without this it results in
strange behaviour when say `-555` is the file arg.

Added initial `jprint_test.sh` test script. For now it is very much like
`jparse_test.sh` except it doesn't test the strings file as `jprint` doesn't
have an option to read args as a string. Whether it might be good to do this or
to change the script so that it can read from stdin are questions to be answered
at a later date. Something it additionally does is run `jprint -K` test mode.

Fix incorrect exit codes in help string of `jparse_test.sh`.

Add patterns / regexps to `patterns` linked list in `jprint` struct. For now it
only iterates through the list, showing that a pattern or regexp will be
searched for. Functions to add and free the list added. Patterns are appended to
the list so that the first pattern/regexp specified will be looked for first.

Change exit codes in `jprint` a bit, making 7 exclusive for memory allocation
errors and >= 15 for other internal errors.


## Release 1.0.8 2023-06-11

Fix `jparse` column location calculations where error messages just showed the
invalid token at column 1 even when the bad token is not on column 1.

Bumped `jprint` version to "0.0.14 2023-06-11".

When no `name_arg` is passed to `jprint` it will print the entire file. It was
never a condition that would cause anything but exit code 0 (assuming the json
file is valid) but it is now more explicit in the code as of 11 June 2023 that
the entire file will be printed once the printing code is implemented.

It is an error to use `jprint -p {n,v,b}` and `jprint -j` together.

Move the many options in `jprint`'s `main()` to `struct jprint_options` to
organise them and to help with passing to necessary callbacks later on.


## Release 1.0.7 2023-06-10

Release `jprint` version "0.0.13 2023-06-10".

Make `jprint -S -g` a command line error.

Use of `jprint -b [num]t` (or `-b tab`) requires use of `-p b` (or `-p both`)
and it is an error if this condition is true.

Add functions to parse `-I`, `-b` and `-L` options.

Use of `jprint -j` implies `-p b -b 1 -e -Q 4 -t any`.

But fix check of number arguments after options in `jprint`. The comparison was
wrong and could cause invalid number of arguments at command line even when they
are correct.

Clarify use of `jprint` `-I` and `-L` options in help string.

Add more test cases of bits.

Fix potential NULL pointer dereference in parsing of number ranges.


## Release 1.0.6 2023-06-09

New `jprint` version "0.0.10 2023-06-09". This version now correctly handles the
number ranges in the form of e.g. `-l 5:-3` where if there are 10 matches it
will print the fifth match up through the third to last match.

Fixed `seqcexit` and `picky` rules for jprint and sequenced exit codes.

Change semantics of `jprint -L` option. It now takes an option argument which is
the number of spaces or a tab which will be printed after the JSON level is
printed. Like `-b t` this has to be fixed in that we cannot assume that `t` or
`tab` is 8 spaces instead of `\t` but this will change later.

Further version update of `jprint`, now being  "0.0.11 2023-06-09". Semantics of
`-L` and `-b` changed further. Can be `-[b|L] <num>[t|s]` where `s` is optional.
If a number is specified without `t` it implies spaces. If a `s` is specified it
implies spaces and if nothing is specified it implies spaces. Anything else is a
syntax error.

Add initial testing of `-t type` option after fixing an issue with the bitwise
test functions. See `jprint_test.c` for details.

Release `jprint` version "0.0.12 2023-06-09": change semantics of `-I` option to
match that of `-b` and `-L`. Finished -t type test code.


## Release 1.0.5 2023-06-08

`jprint` version "0.0.9 2023-06-08". At this time I (Cody) believe all known
checks for `jprint` options have been added. Also test functions for the number
range code and print mode have been added and run successfully! To run test code
pass in `-K` to `jprint`.

The next step is to write some test functions (likely printing debug
messages for different options and their option arguments). Currently (as of
version 0.0.8) the number range test functions are successful.

If all is OK the code to traverse the tree to look for simple matches (this does
not mean the `JPRINT_TYPE_SIMPLE`) can be added. At first the tool will not
check for the constraints but rather just print the name and value (even though
the default is value (`JPRINT_PRINT_VALUE`) only I want to make sure that the
ability to to print both is there).  This will help make sure that the
traversing works okay before constraints are added. Prior to the following step,
described below, the grep-like functionality, using `regex.h`, can be added (it
might be better to instead add the grep-like functionality after the below - the
constraints - are added but this will be determined at the time).

Once the above is okay the constraints can be added. The tests should be easier
to do than the traversing and following parts but my hope is that in the coming
days more than tests can be added.

Once this is all done a comprehensive test script can be added to the repo that
is called by the entire test suite (including `bug_report.sh`) so that we can
verify that `jprint` works as expected. Then any issues can be fixed and the
tests can be run again. This might take more than a few days but hopefully
things will move along nicely.


## Release 1.0.5 2023-06-05

`jprint` now accepts a `-m max_depth` option to allow for one to specify maximum
depth to traverse the json tree, assuming it's valid JSON. Defaults to
`JSON_DEFAULT_MAX_DEPTH == 256`. With debug level `DBG_NONE` (this will change
to a higher level once the tool is complete or closer to being complete) says
what the limit is, indicating that 0 is no limit and 256 is the default.

`jprint` now parses the `-t type` comma-separated option. Currently is a
bitvector but this might change as more is developed (bitvector was not the
first thought).

New `jprint` version "0.0.6 2023-06-07". It now parses all options and most test
functions for options being set are added as well. This version is backdated to
7 June because this was done on the 7th of June but the version was not updated.


## Release 1.0.4 2023-06-04

Split the location table into `soup/location_tbl.c` and
the location table related utilities into c`soup/location_util.c`.

Updated location table `tbl[]` in `soup/location_tbl.c`
as of 2023 June 05.

Added `soup/location` utility to look up locations ISO 3166 code(s),
location names, of print the entire location table.

Release initial `jprint` tool. Current version "0.0.3 2023-06-03". Current
functionality is limited to:

- accepting SOME Of the options but not parsing any except `-h`.
- if not enough args (at least 1 arg - the file to parse) it prints usage string
(same as `-h` but with error message about not enough args).
- checks that first arg exists and is a regular file (or `stdin`) and if so it
tries to parse it as a JSON file. Depending on status of whether it's valid or
json it will report an error or report valid json. Later it will only report
valid JSON as a debug message.
- parsing options except that only the basic option arguments are parsed.
- note that `jprint` will make use of the `regex(3)` library.
- frees the parse tree prior to exiting.

Updated [jparse/README.md](jparse/README.md) to note the `jprint` tool.

**MUCH MORE** has to be done but this is a great start for a very important tool. We
believe that after the tool is finished and tested (including added to the
overall test suite + the `bug_report.sh` script!) we (i.e. Cody/@xexyl) can
import the jparse parser and tools into the [jparse
repo](https://github.com/xexyl/jparse).


## Release 1.0.3 2023-05-22

Added [remarks.example.md](remarks.example.md) which has instructions for
writing `remarks.md` files in the same vein as the
[Makefile.example](Makefile.example).

Fix `bug_report.sh` and `make clobber` (in `jparse/`) to remove bogus file
`jparse/lex.jparse_.c` after completion of one of the steps.


## Release 1.0.2 2023-04-14

Fix mkiocccentry to write past winner and author handle to the answer file. It
already read from these from the file but did not write them and therefore using
a new answers file did not work at the point that these fields were added to the
tool.

Improved the algorithm for UTF-8 strings to ASCII POSIX safe characters.
The `utf8_posix_map hmap[]` was changed to no longer map UTF-8 to
ASCII characters '.' NOR '+' NOR '-'.  The `default_handle()` function
was changed to NOT produce more than one '_' (underscore) in a row.
Before these changes, the function produced handles of the form:

```re
^[0-9a-z][0-9a-z._+-]*$
```

Now, handles conform to the following regexp:

```re
^[0-9a-z][0-9a-z_]*$
```

Moreover, default handles will NOT contain 2 or more '_' (underscore)
characters in a row.


## Release 1.0.1 2023-02-14

Fix minor shellcheck nits in `bug_report.sh` and
`jparse/test_jparse/jparse_test.sh` so that the `make prep` phase
_25 shellcheck_ will pass as OK.

Test if ctags is available before attempting to run ctags in Makefile rules.

The `jparse/Makefile` `make clobber` rule will call the same rule in
`jparse/Makefile/test_jparse` as well.

Make `hostchk.sh` grep usage POSIX compliant. This means it no longer uses the
either of the `-o` or `-h` options.

Corrected the comment about the Rule 2b value 2503.  The previous
comment, as noted by Nicholas Carlini (GitHub user @carlini), was
an out of date comment related to the previous prime 2053 value.

Fix issue in jparse/Makefile where sometimes one would see:

	make[2]: `jparse.tab.h' is up to date.
        make[2]: `jparse.lex.h' is up to date.

Added `jparse_` prefix to json parser so that multiple parsers can exist in
programs that link in the library. New jparse version is 1.0.1 2023-03-10.

Fix jparse/Makefile to have a dependency line for jparse.c (jparse.o). Run make
depend at the root directory.

Change `malloc_path()` to `calloc_path()` as it uses `calloc(3)` not
`malloc(3)`.

Rename macro `MAX_NUL_REPORTED` -> `MAX_NUL_BYTES_REPORTED` and allow overriding
it (default is 5; uses `#if !defined (MAX_NUL_BYTES_REPORTED)`.

Rename macro `MAX_LOW_REPORTED` `MAX_LOW_BYTES_REPORTED` and allow overriding it
(default is 5; uses `#if !defined (MAX_LOW_BYTES_REPORTED)`.

Fix version of jparse; the prefix added was supposed to be `1.0.1 2023-03-10`
and was changed initially but when trying to get a problem resolved it must have
been undone. Now it's instead `1.0.2 2023-03-11`.


## Release 1.0.0 2023-02-04

Resolved all macOS -Weverything issues worth addressing.  See
`test_ioccc/static_analysis.md` for details and to see how you can also help as
well as the fixes we applied, what might need to be fixed and what we ignore
(and why we choose to ignore them). Note that originally the commit ids were
bogus and although we fixed those in that file the git commit log obviously has
the bogus ids.

Restore Makefile use -Werror and use of -O0. With the exception of the dbg code,
use of -Werror and compiling with -O0 -g is restored in various Makefiles while
version 1.0.0 is still in development.

Fix some memory leaks detected with `valgrind`. See
`test_ioccc/dynamic_analysis.md` for details and to see how you might also help.

Exit codes checked for consistency across usage strings, man pages and the tools
themselves.

Changed DBG_VERSION from "2.6 2022-11-09" to "2.8 2023-01-29".
Minor improvements made on when warning messages are printed.
Added dbg_version as a global string for users of the dbg.a library.

Changed DYN_ARRAY_VERSION from "2.0 2022-06-09" to "2.1 2023-01-21" and then
from "2.1 2023-01-21" to "2.2 2023-01-22".
Added dyn_array_version as a global string for users of the dyn_array.a library.

Changed JSON_PARSER_VERSION from "0.12 2022-11-09" to "0.13 2023-01-21".
Added json_parser_version as a global string for users of the jparse.a library.

Moved JPARSE_VERSION "0.11 2022-07-04" from jparse_main.h to jparse.h.
Added jparse_version as a global string for users of the jparse.a library.

Changed JNUM_GEN_VERSION from "0.7 2022-05-01" to "0.8 2023-01-22".
Current JNUM_CHK_VERSION is "0.8 2022-05-01".
Fixed bug where jnum_gen.c had JNUM_CHK_VERSION.

Added SOUP_VERSION as "1.0 2023-01-21".
Added soup_version as a global string for users of the soup.a library.

Changed jsemtblgen so that -P prefix will always be used on function names.
Use of -M func will override any specific member name.
Changed JSEMTBLGEN_VERSION from "0.4 2022-07-13" to "0.6 2023-01-22".
The jsemtblgen tool, with the -I option, will not print duplicate
extern function declarations.

Fixed a number of tests relating to conversion of floating point values.
Corrected `jparse/test_jparse/jnum_test.c` test results for
the `as_double is an integer` test cases.

Added a system to notice "Notice:" messages and count then for `make prep`.
The `test_ioccc/prep.sh` tool removed the generated bug report log file
if the `bug-report.sh` tool does an exit 0, even if there are notices.
Changed BUG_REPORT_VERSION from "0.12 2023-01-24" to "0.13 2023-01-24".
Then with more improvements (see below) it was changed to "0.14 2023-01-30". The
improvements: check for Makefile in all subdirectories, check for overriding
makefile.local in all subdirectories and check that all subdirectories exist and
are readable and are executable.

Changed PREP_VERSION from "0.3 2023-01-24" to "0.4 2023-01-24".

Moved static variables from header to source files.

Fix segfault when author length is too long from NULL pointer dereference: due
to an else being left off from an if after rejecting the author, freeing and
setting to NULL the code went on to check for unique author names which would
then dereference the just rejected NULL author name. See commit
48ea6cc0bda39b2aa1e31fe42aee781197fe8b18.

Go through all files and fix typos and do a check for consistency in messages as
well (error, debug, general, warnings etc.). This excludes .json,  .txt and .err
files under test_ioccc/ as there's no point in checking them (some of the .txt
files do have text that would trigger spellchecking problems but they're part of
invalid tarball listings). The iocccsize related files have not been checked.
Where the alternative and incorrect English (that is American English) spelling
was in the repo it was untouched as it is after all the International Obfuscated
C Code Contest, not the Obfuscated C Code Contest for people who use proper
English. :-)

Fix some grammar errors. More could probably be done later but whether this is
worth it can be TBD at another time. There are some places that are problematic
in fixing and it's time consuming to find all the remaining issues. Besides,
this is for the IOCCC so if a few things here and there are misleading or
confusing it's probably not all that bad anyway! :-)

Improved man pages, error messages and documentation.  Went through all
documentation and fixed noticed problems, typos, formatting issues etc.

Add CodeQL Analysis for GitHub commits: added codeql.yml as a new GitHub
workflow.

Fix internal Makefile rules to do with the
[dbg repo](https://github.com/lcn2/dbg).
Fold in .gitignore from dbg repo.

Improve Makefile #! lines.

Fix shellcheck errors under macOS.

Moved utf8_test(8) to test_ioccc/man/man8.

Improve getopt() calls. We now control the error messages for better output like
removing "-h help mode" when -h is specified. Compress the '?', ':' and default
cases by way of new function check_invalid_option().

Fix bug_report.sh options -l and -x.

Improve dyn_alloc code robustness.

Fix several implicit signed conversion issues and double precision floating
point tests. Improve other fixes to various warnings.

Change remaining calls to strtok() to strtok_r().

Remove duplicate object in jparse Makefile.

Update README.md for information on how to help (for when we're ready for such
help). The note that we're not yet ready will be removed once we are.

Fixed debug calls in jparse.y to match the functions being called that changed
due to making the parser re-entrant.

Fixed various compile errors in systems that were not initially tested (debian
and linux in general depending on the issue).

Improve prep.sh so that issues reported are more specific. In particular it will
show which rule failed rather than just that a make action failed.

Change in txzchk the txzpath to tarball_path to be clearer what it is.

Fix double notices by bug_report.sh for unknown versions of tools.

Improve how bug_report.sh reports on missing commands.

Fix man pages to use formatting macros throughout and add them to the SYNOPSIS
and OPTIONS sections. This makes run_usage.sh (which was always a quick, dirty
and ugly hack) even less useful but not entirely useless. Update checknr call
in Makefiles so that checknr is aware of the additional macros.

Fix (prevent) stray jparse related files after bug_report.sh.

Fix potential NULL pointer dereferences.

Improve make clobber so that specific files and directories that were being
erroneously being removed no longer are removed.

Make it easier to make soup. :-) Yes this is for fun only and that sentence has
a slight hint.

Typo fix in soup/location.c: although ions have their place when peroxide ions
in particular combine with water and diluted acids it forms hydrogen peroxide
which has ended up killing people when those unaware of the dangers
misunderstand the request that someone would like a glass of H2O too. Thus we
should get rid of stray ions in the repo and now have. See commit
532d33773a198058a3d1d596ad1192cb7f3b35d0 for details.

Make parameters in usage() functions consistent across all files. Add usage() to
utf8_test.c and improve usage message. Change VERSION to UTF8_TEST_VERSION in
utf8_test.c.

Change boldness of foo([138]) in man pages so that only foo is in bold.

Fix usage message in fnamchk.

Change some local variables to not be the name of functions in the dbg library
(which the files in question used).

Improve iocccsize -h output. This includes adding version information to the
message.

Add version information to jsemcgen -h. More generally fix shell script -h
version in several scripts to print script name and version string.

Remove extra spaces in all_sem_ref.sh -h output.

Remove trailing blank lines to various scripts -h output.

Clarify comments about rule 2a and rule 2b tests.

Improve jsemcgen(8), jsemtblgen(8) and all_sem_ref(8).
Improve output of `jsemtblgen -h`.

Add FAQ.md for information on getting help with various problems.

Bug fix hostchk.sh. In particular in some cases it would be that the non-zero
exit code would be reported as 0.

Update dbg/README.md to refer to other dbg(3) man pages (the main files not the
symlinks). The jparse/README.md does not refer to an example yet (for the
library) as an example program does not exist. The dyn_array/README.md does not
explain how to link in the library yet because it depends on the dbg repo and it
is not yet determined how this will be approached. Similar can be said for
jparse though we do have json_dbg() and other related functions.

In order to make it easier to run tools from the top level directory, we
changed the default path of `fnamchk` from "./fnamchk" to "./test_ioccc/fnamchk",
and the path of `jparse` from "./jparse" to "./jparse/jparse".

Check return value of function `yylex_init_extra()` and warn with errno
information if non-zero return value. Sequenced exit codes, updated backup
parser files.

Remove unused function string_to_intmax() and change string_to_intmax2() to be
string_to_intmax().

Added `IOCCC_NEWS_URL` to `mkiocccentry.h` to define the URL to
read IOCCC news.  Added `IOCCC_REGISTRATION_READY` to `mkiocccentry.h`
to indicate if the registration process is ready independently from
the submit server.

Fixed coredump in verge when a version string starts with a non-digit
(`!isascii() && !isdigit()`).

Make verge use string_to_intmax() (instead of string_to_long()) and get rid of
the following unused functions:

        extern long string_to_long(char const *str);
        extern long long string_to_long_long(char const *str);
        extern int string_to_int(char const *str);
        extern unsigned long string_to_unsigned_long(char const *str);
        extern unsigned long long string_to_unsigned_long_long(char const *str);
        extern uintmax_t string_to_uintmax(char const *str);
        extern long double string_to_float(char const *str);

Exit codes were resequenced.

The `make tags` now forms both local directory tags files by the name of
.local.dir.tags as well as a tags file from the accumulation of tags from
related source directories.

Changed `soup/vermod.sh` default JSON tree from "../test_ioccc/test_JSON" to "test_ioccc/test_JSON" and default limit.sh from "./limit_ioccc.sh" to "soup/limit_ioccc.sh" so that `soup/vermod.sh` may be executed from the top level source directory without the need for using `-d test_dir` nor `-i limit.sh`.

Update versions prior to code freeze.

Changed ALL_SEM_REF_VERSION from "1.1 2022-12-30" to "1.2 2023-02-04".
Changed AUTH_VERSION from "1.18 2022-11-30" to "1.19 2023-02-04".
Changed BUG_REPORT_VERSION from "0.14 2023-01-30" to "1.0 2023-02-04".
Changed CHKENTRY_TEST_VERSION from "0.2 2022-11-04" to "1.0 2023-02-04".
Changed CHKENTRY_VERSION from "0.4 2022-11-30" to "1.0 2023-02-04".
Changed DBG_VERSION from "2.8 2023-01-29" to "2.9 2023-02-04".
Changed DYN_ARRAY_VERSION from "2.2 2023-01-22" to "2.3 2023-02-04".
Changed DYN_TEST_VERSION from "1.8 2023-01-2" to "1.9 2023-02-04".
Changed FNAMCHK_VERSION from "0.6 2022-03-15" to "1.0 2023-02-04".
Changed HOSTCHK_VERSION from "0.3 2022-10-27" to "1.0 2023-02-04".
Changed INFO_VERSION from "1.13 2022-10-28" to "1.14 2023-02-04".
Changed IOCCC_YEAR from 2022 to 2023.
Changed IOCCCSIZE_VERSION from "28.12 2022-10-17" to "28.13 2023-02-04".
Changed IOCCC_TEST_VERSION from "0.5 2022-11-04" to "1.0 2023-02-04".
Changed JNUM_CHK_VERSION from "0.8 2022-05-01" to "1.0 2023-02-04".
Changed JNUM_GEN_VERSION from "0.8 2023-01-22" to "1.0 2023-02-04".
Changed JPARSE_TEST_VERSION from "0.5 2022-12-31" to "1.0 2023-02-04".
Changed JPARSE_VERSION from "0.11 2022-07-04" to "1.0 2023-02-04".
Changed JSEMCGEN_VERSION from "1.2 2022-10-23" to "1.3 2023-02-04".
Changed JSEMTBLGEN_VERSION from "0.6 2023-01-22" to "1.0 2023-02-04".
Changed JSON_PARSER_VERSION from "0.13 2023-01-21" to "1.0 2023-02-04".
Changed JSTRDECODE_VERSION from "0.5 2022-04-17" to "1.0 2023-02-04".
Changed JSTRENCODE_VERSION from "0.5 2022-04-17" to "1.0 2023-02-04".
Changed JSTR_TEST_VERSION from "0.4 2022-11-05" to "1.0 2023-02-04".
Changed MKIOCCCENTRY_TEST_VERSION from "0.3 2022-11-05" to "1.0 2023-02-04".
Changed MKIOCCCENTRY_VERSION from "0.44 2022-11-30" to "1.0 2023-02-04".
Changed MIN_TIMESTAMP from 1662145368 to 1662145368.
Changed PREP_VERSION from "0.4 2023-01-24" to "1.0 2023-02-04".
Changed RESET_TSTAMP_VERSION from "0.4 2022-04-23" to "1.0 2023-02-04".
Changed RUN_BISON_VERSION from "0.4 2023-01-05" to "1.0 2023-02-04".
Changed RUN_FLEX_VERSION from "0.5 2023-01-05" to "1.0 2023-02-04".
Changed RUN_USAGE_VERSION from "0.2 2022-11-06" to "1.0 2023-02-04".
Changed SOUP_VERSION from "1.0 2023-01-21" to "1.1 2023-02-04".
Changed TEST_IOCCCSIZE_VERSION from "1.3 2023-01-17" to "1.4 2023-02-04".
Changed TXZCHK_TEST_VERSION from "0.5 2022-11-04" to "1.0 2023-02-04".
Changed TXZCHK_VERSION from "0.13 2022-10-09" to "1.0 2023-02-04".
Changed UTF8_TEST_VERSION from "1.1 2022-10-17" to "1.3 2023-02-04".
Changed VERGE_VERSION from "0.1 2022-04-02" to "1.0 2023-02-04".
Changed VERMOD_VERSION from "0.1 2022-03-15" to "1.0 2023-02-04".

Updated test_ioccc/test_JSON test files to account for changes
in the following versions:

- AUTH_VERSION
- CHKENTRY_VERSION
- FNAMCHK_VERSION
- INFO_VERSION
- IOCCCSIZE_VERSION
- IOCCC_YEAR
- MKIOCCCENTRY_VERSION
- TXZCHK_VERSION

Updated selective `test_ioccc/test_txzchk` test files to account
for changes in MIN_TIMESTAMP made by prior use of the
`soup/reset_tstamp.sh` tool.

Added MKIOCCCENTRY_REPO_VERSION "1.0.0 2023-02-04" to the
`soup/version.h` file.

Fix `soup/Makefile` _clobber_ rule to remove the `soup/ref` tree.

Have the `test_ioccc/Makefile` _clobber_ rule remove
`test_iocccsize/`, `test_src/`, and `test_work/` directory
trees from from under the `test_ioccc/` directory.

Improved mkiocccentry_test.sh so that one can specify the path to various tools
that mkiocccentry needs.
Made txzchk_test.sh -t/-t consistent with mkiocccentry.
Added option to ioccc_test.sh to let one specify path to tar.
Changed IOCCC_TEST_VERSION from "1.0 2023-02-04" to "1.0.1 2023-02-05".
Changed MKIOCCCENTRY_TEST_VERSION from "1.0 2023-02-04" to "1.0.1 2023-02-05".
Changed TXZCHK_TEST_VERSION from "1.0 2023-02-04" to "1.0.1 2023-02-05".

Fixed chkentry_test.sh so it will work when running it directly.

Major updates to CHANGES.md. See
[CHANGES.md](https://github.com/ioccc-src/mkiocccentry/blob/master/CHANGES.md)
for details. :-)

Version 1.0.0 code freeze.

Formal version 1.0.0 release.


## Release 0.9.9 2022-12-31

Release just prior to version 1.0: the "public comment version".

The `make prep` and `make release` rules were made much less verbose.

Updated and fixed .gitignore file.

The jparse/ directory only uses and depends on dbg/ and dyn_alloc/.

Made changes to version.h so that the JSON parser code under
jparse/ does not have to use it.

Tuned the verbosity of Makefiles with various Makefile variables such as
${Q}, ${V}, ${S}, ${M}, ${INSTALL_Q} ${INSTALL_V}, and ${MAKE_CD_Q}.

Moved all man pages under their respective sub-directories.  The build_man
rule create a man sub-directory with copies of all other man pages.

Numerous updates and fixes to the man page set.  Improved how man page
macros are used.

Added man symlinks in place where commands are in a given man page.

Change IOCCC use of twitter to mastodon.  The mkiocccentry tool now asks for
an optional mastodon account.  The .auth.json file contains entries for
an optional mastodon instead of twitter.

The Makefile set, and the way they call each other has been improved.
Programs link to one or more libraries such as dbg.a, dyn+array.a,
jparse.a and soup.a.

Make the Makefile set reasonably consistent in terms of style and comments.

Removed use of man2html.  No longer are html files made from man pages.

Removed the checknr rule form Makefiles.  Instead, the check_man rule
tests man pages.

Fix the system of multiple Makefiles (more work might need to be done like
installing man pages with `make install` as well as other possible issues -
several have been discovered and fixed since this issue was 'closed'). Other
miscellany fixes have been made in the Makefiles.

Improve `bug_report.sh`: with `-l` (only write to log file) it is completely
silent and with `-t` fewer tests are run (don't invoke any of the make checks)
making it faster (note though if our programs are not compiled it will cause
some tests to fail: using via `make prep` will solve this problem). The `-x`
option will cause it to delete the report if no issues are found.

Improve `prep.sh`: make it much quieter if `-l logfile` (write to log file and
write less information to stdout) is used. Now calls `bug_report.sh` with
options `-t -x -l` via Makefile rule `bug_report-txl`. This allows for the
`bug_report.sh -t` to work without causing any problems. For reporting bugs one
should call `make bug_report`: `prep.sh` is for the maintainers of the repo.

The JSON test files used to test the chkentry tool were moved to the
test_ioccc/test_JSON directory.

Updated copyright to include 2023 (and a 'Happy New Year' to one and all!).

Add separate man pages for dbg(3) functions. Improve several of the man pages
more generally. TODO: make all man pages installed via `make install`.

Moved jparse to subdirectory.

Removed jparse.md and dbg.md in favo(u)r of README.md files in sub-directories.

Fixed dyn_array bug.

Moved dyn_array code to dyn_array/ subdirectory.

Fixed obscure bug with bool typedef.


## Release 0.8.1 2022-11-19

This "roll-up" release was added so that it could be referenced
in the https://www.ioccc.org/index.html news section.

Updated legacy_clobber rule to remove older directories no longer used.

Moved test directory to test_ioccc to avoid Makefile confusion
with the test rule.

Restructure dbg facility and Makefile in accordance with
the new method of copying code from external GitHub repositories.

Improve formatting with the hostchk_warning rule.

Added initial dyn_array.md documentation.

Typo fixes to the verge.8 man page.

Minor fixes to jparse.md.

Updated txzchk comments (source, header, man page).

Improved and updated README.md documentation.

Improved bug_report.sh: in particular in reporting versions.

Bug fixes and improvements to several shell scripts.
Made changes to satisfy shellcheck as well.

Added rules in the Makefile to make is easier to import code from
external GitHub repositories.

```
This repo is designed to be a standalone repo.  Even though we use other
repositories, we prefer to NOT clone them.  We want this repo to depend
on a specific version of such code so that a change in the code of those
external repositories will NOT impact this repo.

For that reason, and others, we maintain a private copy of an external
repository as clone.repo.  The clone.repo directory is excluded from
out repo via the .gitignore file.  We copy clone.repo/ into repo/ and
check in those file directly into this repo.
```

The dbg code is now a copy of the code from the dbg repo.

Update JSON parser version.  Changed JSON_PARSER_VERSION from "0.11
2022-11-04" to "0.12 2022-11-09".  There was an API change (an
addition of a filename argument in some calls) that suggested version change.


## Release 0.8 2022-11-09

Improve the situation with some systems requiring some feature test macros.
Removed all references to `${TIMEGM_PROBLEM}` in Makefile.
Removed all references to `${D_LEGACY}` in Makefile.
Cleaned out the `$(WARN_FLAGS}` list (from legacy hosts).
These changes allow for cleanly compiling under CentOS without as many
complexities (it was also important for modern systems like Fedora).

Made a number of important fixes to `chkentry`.

Made a number of important fixes to JSON parser.
Pre-scan documents and strings for problematic bytes prior to trying to parse it
as json.

The JSON scanner and parser are now re-entrant!

Improved JSON parser error messages by adding filename and location (lines and
columns).

Added filename argument to top level parser functions:

```c
extern struct json *parse_json(char const *ptr, size_t len, char const *filename, bool *is_valid);
extern struct json *parse_json_stream(FILE *stream, char const *filename, bool *is_valid);
```

This was one part of making the scanner and parser re-entrant. See the git log
and diffs of `jparse.l` and `jparse.y` for more details.

The documentation of the JSON parser is not complete and it will not be
completed until after IOCCCMOCK.

Fixed several memory leaks.

Fixed a number of typos in comments.

Many man pages updated and/or fixed.
Removed `limit_ioccc(8)` man page.

Made a number of shell script fixes with respect to the `mktemp(1)` command.

Experimenting with a system independent `make depend` tool: work in progress.


## Release 0.7 2022-11-05

Released IOCCC entry toolkit v0.7 2022-11-05

Changed BUG_REPORT_VERSION from "0.5 2022-11-03" to "0.6 2022-11-06".
Changed FMT_DEPEND_VERSION from "1.0 2022-09-28" to "1.1 2022-11-05".
Changed CHKENTRY_TEST_VERSION from "0.1 2022-10-11" to "0.2 2022-11-04".
Changed IOCCC_TEST_VERSION from "0462202-04-23" (sic) to "0.5 2022-11=04".
Changed TEST_IOCCCSIZE_VERSION from "1.1 2022-10-16" to ="1.2 2022-11-04".
Changed JPARSE_TEST_VERSION from "0.3 2022-10-22" to "0.4 2022-11-04".
Changed JSTR_TEST_VERSION from "0.4 2022-09-28" to "0.4 2022-11-05".
Changed TXZCHK_TEST_VERSION from "0.4 2022-10-20" to "0.5 2022-11-04".

The `chkentry(1)` tool is now code complete!  Added test suite for
`chkentry` called test/chkentry_test.sh.  The `chkentry(1)` tool
passes all tests.

Removed prefix in bison and flex code. The programmer's apology and
warning are still in `sorry.tm.ca.h`.

A test suite for `txzchk` was added.

Removed the `MAX_DIR_KSIZE` limit. There still is a maximum tarball
size but now there also is a limit of the number of files in the
tarball. `txzchk` and `mkiocccentry` were updated for this.

We're one step closer to being able to invite the public to
review these tools.

Created the test sub-directory.  Moved many test tools under the
test sub-directory.  Moved log files under test.

Created the man sub-directory.  Moved all man pages under the
man sub-directory.  The html files are also build under man.

Added the soup sub-directory.  Moved tools related to building semantic
test tables for chkentry under the soup sub-directory.  Moved ref
under soup.

Added fmt_depend.sh under the soup sub-directory to format
the make depend lines in a system independent way.

Added -Z topdir flags to soup/fmt_depend.sh, test/ioccc_test.sh,
test/jstr_test.sh, test/mkiocccentry_test.sh, and test/txzchk_test.sh.
By default these tools cd to the top level directory where the
mkiocccentry.c source file resides.  By use of -Z topdir, one can
force the top level directory location.

Experimenting with compiling code in sub-directories such as soup
and test.


## Release 0.6 2022-09-02

Released IOCCC entry toolkit v0.6 2022-09-02

Updated CHANGES.md for v0.6 2022-09-02.

Changed `MIN_TIMESTAMP` from 1655958810 to 1662145368.

Updated `mkiocccentry` from "0.40 2022-03-15" to "0.41 2022-09-02".
Updated `.info.json` version from "1.10 2022-06-22" to "1.11 2022-09-02".
Updated `.author.json` version from "1.13 2022-06-22" to "1.14 2022-09-02".

Improved code to use new facilities for output to a buffer from dbg release of
v2.5 2022-07-23.

The `chk_foo()` functions in `chk_validate.c` and the `test_foo()` functions in
`entry_util.c` are code complete, although they remain untested and unused.  The
`chkentry` tool, however, is not yet code complete. Later releases of tested
JSON semantic code will no doubt modify these functions.

Improved a number of the ways that JSON field values are checked.  In a number
of cases, code form `mkiocccentry.c` was moved into `test_foo()` functions so
that they could be used by other tools such as the JSON semantic test code.

Added code to generate JSON semantic tables from JSON reference files for
`.info.json` and `.author.json`.  The `jsemcgen.sh` tool manages this by way of
the `jsemtblgen` code generator and header, patch and trailer files (see
`chk.auth.*` and `chk.info.*` files).

Avoided the appearance of attacking any particular individual.  It was not our
intention to disrespect anyone, even though we disagree with some of the
technical decisions.  Where we have fundamental technical disagreements, we
attempted to express those technical disagreements with humour in hopefully a
more fun way.  As also now apologize for how `bison` and `flex` generated code
may look, instead of simply calling it ugly.  As such, we hopefully improved
some of the humour in our code while trying to be nice and friendly to others.

For example now the adjusted dbg levels in JSON parser are:

```
*     At -J 3, only the top level return type and top level tree are printed.
*     At -J 5, intermediate tree return types and tree are printed too.
*     At -J 7, also print grammar progress.
*     At -J 9, also print sorry_text and sorry_lang grammar values.
```

Removed a number of files and added a number of files under the `test_JSON/`
tree.  When the JSON semantic code is being tested in a future release, we
expect more such `test_JSON/` tree changes.

Improved / add a number of man pages.  Updated `README.md`.

Improved and expanded `txzchk`.

Added more test code.  We attempt to detect feathers in tarballs.  :-)

We will neither confirm nor deny the presence of an "Easter egg".
To to so would be "foolish".  :-)

Improved and fixed `vermod.sh` and `reset_tstamp.sh`.  Tested this
code by changing the `MIN_TIMESTAMP` as noted above.  The `MIN_TIMESTAMP`
needed to up updated anyway due to changes in the `.info.json` and
`.author.json` formats.

Made numerous improvements and bug fixes to the Makefile.

Fixed how `picky` is used in by the `make picky` rule for a few special files.

Added multiple rules to the Makefile including but not limited to `make mkref`,
`make legacy_clobber`, and `make legacy_clean` rules.  Applied multiple bug
fixes to the Makefile.

Improved the Makefile to be less impacting for modern systems while trying to
maintain, for as long as we can, compatibility with some older systems.

Attempted to improve compatibility with reasonably modern systems.  We managed
to keep CentOS 7 somewhat supported for now, although we may be forced to drop
support of such an old system before 2024. But if we do reach 2024 we can only
say that given that CentOS 7 will reach EOL in 2024 and CentOS 8 support was
unduly dropped and since < CentOS 7 already was at EOL this should not be a
problem. :-)

Users of very out of date systems can still enter and submit entries to the
IOCCC.  They just might need to find a more modern system to package and submit
their IOCCC entry, however.

Added stub code for `hostchk.sh`.  A future release will include mode tests for
the given hosts.  Future releases will also include a bug report system that
will also use `hostchk.sh`.

Improved the `no-comment` directive in `.info.json` and `.author.json` files.

Improved how `time_t` values are used and printed.  We no longer assume that
`time_t` is signed nor assume it is unsigned: we only assume it's time. :-)

Improved comments in C code about special `seqcexit` comment directives.

Make numerous bug fixes and fixed a fair number of typos.  Nevertheless we claim
this is *NOT* complete. :-)


## Release 0.5 2022-07-10

The `jsemtblgen` tool is code complete.

Added `is_reserved()` function to `rule_count.c`.

Changed the Official iocccsize version from 28.10 2022-03-15 to 28.11
2022-07-10. Updated `test_JSON` tree and `json_teststr.txt` to reflect the new
`IOCCCSIZE_VERSION` value.

Added and improved a number of man pages.

Added `Makefile` rule to build HTML files from all man pages.

Both the `parse_json()` and `parse_json_file()` top level functions of the JSON
parser return a JSON node parse tree when the JSON is valid.

Trimmed the JSON parser to not include code that does not strictly belong to the
generic JSON parser.  Removed dead C code.

Updated dbg facility to version 2.4 2022-06-26.  Version 2.4 2022-06-26 contains
functions to write (form) a message, debug diagnostic message, warning, or error
message into a string buffer:

```c
extern void snmsg(char *str, size_t size, char const *fmt, ...)
extern void sndbg(char *str, size_t size, int level, char const *fmt, ...);
extern void snwarn(char *str, size_t size, char const *name, char const *fmt, ...);
extern void snwarnp(char *str, size_t size, char const *name, char const *fmt, ...);
extern void snwerr(int error_code, char *str, size_t size, char const *name, char const *fmt, ...);
extern void snwerrp(int error_code, char *str, size_t size, char const *name, char const *fmt, ...);
```

Simplify command for jparse and jsemtblgen.  The -s is now just a flag.  Only
one arg is allowed.

Adjusted dbg levels in JSON parser:

```
*     At -J 3, only the top level return type and top level tree are printed.
*     At -J 5, intermediate tree return types and tree are printed too.
*     At -J 7, also print grammar progress.
*     At -J 9, also print ugly_text and ugly_lang grammar values.
```

Improved top level JSON parser debug & warn layering.

The `json_dbg()` functions now returns void, like `dbg()`.  The `json_vdbg()`
functions now returns void, like `vdbg()`.  The `json_dbg_tree_print()` function
now returns void.  JSON parser now uses `werr()` instead of `warn()`.

Now using `seqcexit` tool, version 1.8 2022-06-02 to sequence `snwerr()` and
`snwerrp()` calls.

Fixed a number of typos in comments and documentation.

Improved a number of C comments.  Some comments are less humor impaired and
might even make the reader prone to mirth and laughter.  :-)

Sequenced exit codes.

Improved C code format consistency.

The `-V` flag will only print the version string and exit. The `-h` flag may
print one or more version strings preceded by an explanatory string explaining
what the version is about.

Added date strings to release strings in `CHANGES.md`.

Updated `CHANGES.md`.


## Release 0.4 2022-06-23

The JSON parser is code complete!

The following JSON parses now work:

``` <!---sh-->
./jparse -J 3 -s '{ "curds" : "whey", "foo" : 23209 }'
./jparse -J 3 test_JSON/info.json/good/info.good.json
./jparse -J 3 test_JSON/author.json/good/author.default.json
```

Replaced `chkinfo` (was `jinfochk`) and `chkauth` (was `jauthchk`)
with a stub for a single `chkentry`.

Files of the form `chk\*` have been renamed `old.chk\*` and are no longer
compiled, nor referenced by `Makefile`.  Some of the code in these `old.chk\*`
files may be used in the formation of the new `chkentry` tool.

In `.info.json` the `chkinfo_version` was replaced by `chkentry_version`.

In `.author.json` the `chkauth_version` was replaced by `chkentry_version`.

The `IOCCC_info_version` in `.info.json` was changed from "1.9 2022-03-15"
to "1.10 2022-06-22".  The `INFO_VERSION` in `version.h` was changed accordingly.

The `IOCCC_author_version` in `.author.json` was changed from "1.12 2022-03-15"
to "1.13 2022-06-22".  The `AUTHOR_VERSION` in `version.h` was changed accordingly.

The `MIN_TIMESTAMP` in `version.h has was changed from 1652598666 to 1655958810.


## Release 0.3 2022-06-13

Improved JSON parser diagnostic messages.  Now, `./jparse -s 0` just prints
`invalid JSON`, whereas `./jparse -J 7 -s 0` prints a bunch of parser
diagnostics and  `./jparse -J 9 -s 0` turns on `bison` debug messages.

Updates to use new debug facility features from version 2.2 2022-06-08.

Fixed lots typos.  Make a number of text improvements in comments,
shell scripts, and documentation (both man pages and markdown documents).

Renamed jauthchk to chkauth.  Renamed jinfochk to chkinfo.
Renamed associated header files, man pages and comment references.
Adjusted shell scripts to use the new names accordingly.
Renamed `JAUTHCHK_VERSION` to `CHKAUTH_VERSION`.
Renamed `JINFOCHK_VERSION` to `CHKINFO_VERSION`.
Renamed `json_chk.c` to `chk_util.c`.
Renamed `json_chk.h` to `chk_util.h`.
Renamed `json_err.codes` to `chk_err.codes`.
Renamed `json_warn.codes` to `chk_warn.codes`.
Renamed `jcodechk.sh` to `chkcode.sh`.


## Release 0.2 2022-05-17

Several new tools and man pages have been added. Additionally the JSON parser is
being worked on with reasonable progress. The following tools have been added:

- `jinfochk` - check .info.json file (incomplete - dependent on JSON parser)
- `jauthchk` - check .author.json file (incomplete - dependent on JSON parser)
- `verge` - check that a version is greater than or equal to another
- `jparse`  - the JSON parser (incomplete as noted)
- `jnum_chk` - check JSON number string conversions
- `jnum_gen` - generate JSON number string conversion test data
- `utf8_test` - test translate UTF-8 into POSIX portable filename and + chars

See the following additional man pages:

- `jinfochk.1`
- `jauthchk.1`
- `jstrdecode.1`
- `jstrencode.1`
- `verge.1`
- `jparse.1`
- `limit_ioccc.1`
- `utf8_test.1`


The `-T` option to the tools has been removed as it was decided it is an
obsolete concept.

NOTE: The `iocccsize` is not listed as a new tool in this file as it was always
here.


## Release 0.1 2022-02-21

Created this CHANGES.md markdown document.

Added `-T` flag to `mkiocccentry`, `fnamchk`, `txzchk`, `jstrencode` and
`jstrdecode` to print the IOCCC entry tool set release tag:


``` <!---sh-->
./mkiocccentry -T
./fnamchk -T
./txzchk -T
./jstrencode -T
./jstrdecode -T
```

## Release 0.0

Released early versions of the following tools:

- `jstrdecode`
- `jstrencode`
- `mkiocccentry`
- `txzchk`

See these man pages for details:

- `fnamchk.1`
- `iocccsize.1`
- `mkiocccentry.1`
- `txzchk.1`
