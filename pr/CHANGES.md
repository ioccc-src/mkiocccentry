# Major changes to the IOCCC entry toolkit


## Release 1.1.6 2026-06-13

Remove calls to `isascii()` as this function was removed from POSIX.1-2024 (it
was deprecated in POSIX.1-2008).

For the `to*()` functions/macros of `ctype.h`, although the arg is an `int`, it
is still undefined if it is not representable as either `EOF` or an `unsigned
char`, thus in calls to these functions/macros, we cast to an unsigned char the
arg. This change was tested in the jparse repo (i.e. `make ; sudo make install`
first) and it all seems good.

Also fixed date range in copyright in `pr.c`: 2008,2022-2026. The 2022 is just a
guess. It is not entirely possible (without digging through old email) to
determine it due to how it was first in the mkiocccentry repo under root and
then it was moved to jparse/ and that was then moved to its own repo. However as
mkiocccentry started in late 2021 I believe util.c (which is a significant
source of `pr.c`) I think 2022 is a fair guess.



## Release 1.1.5 2025-10-02

Fixed possible race condition was possible between when a file was
checked for readability, and when it was opened by `open_dir_file()`.

Fixed possible race condition was possible between when a file was
checked for readability, and when it was opened by `open_dir_file()`.

Fixed possible race condition when `open_dir_file()` when a directory
is checked for being readable + searchable, and the function temporarily
changes the current directory.

Fixed security policy link in `README.md`.

Fixed comment typo in `codeql.yml`.

Added GitHub actions and templates.

Sorting tags using the C locale.

The printf format used on debug, and warning messages now handles the
correct formats for the values printed.

Added `fd_is_ready()` and `flush_tty()` to `libpr`.  The `flush_tty()`
helps to support stdio operations.

Improved `make tags`.

Improved `make clobber` to remove a temporary created by `make tags`.

Improved the `pr(3)` man page.

Added stub for `pr_test.c` test code.  The `pr_test.c` is just a framework
for a real test case.

Added use of `LD_DIR` to manage linking `pr_test`.

Fixed and improved `Makefile` comments.  Fixed `MAN3_BUILT` in `Makefile`
to avoid double install.

Updated the `CHANGES.md` file that had not been updated since the
"Release 1.0.0 2025-09-19".

Changed `PR_VERSION` to "1.1.5 2025-10-02".


## Release 1.0.0 2025-09-19

Initial release of `pr.c`, `pr.h`, and  `Makefile`.

Code has been copied over from [mkiocccentry
repo](https://github.com/ioccc-src/mkiocccentry) and from the [jparse
repo](https://github.com/xexyl/jparse).

The code in this from repo was copied out of the [mkiocccentry toolkit
repo](https://github.com/ioccc-src/mkiocccentry) and out of the  [jparse
repo](https://github.com/xexyl/jparse).

The origin of libpr dates back to code written by Landon Curt Noll
around 2008.  That 2008 code was copied into the jparse repo, and the
mkiocccentry toolkit repo by Landon Curt Noll.  While in the jparse repo,
both Landon Curt Noll and Cody Boone Ferguson added to and improved this
code base.
