# Major changes to the IOCCC entry toolkit


## Release 1.0.0 2023-01-21

XXX - add more details here as more is done or remembered (what has already been done).

Fix many warnings triggered by clang `-Weverything`. See
`test_ioccc/static_analysis.md` for details and to see how you can also help as
well as fixes, what might need to be fixed and what we ignore (and why).

Fix some memory leaks detected with `valgrind`. See
`test_ioccc/dynamic_analysis.md` for details and to see how you might also help.

Exit codes checked for consistency across usage strings, man pages and the tools
themselves.

Improving man pages, error messages and documentation is being worked on but
this will take more time yet.

Added dbg_version as a global string for users of the dbg.a library.
Added dyn_array_version as a global string for users of the dyn_array.a library.
Added json_parser_version as a global string for users of the jparse.a library.
Added jparse_version as a global string for users of the jparse.a library.
Added soup_version as a global string for users of the soup.a library.

Changed DBG_VERSION from "2.6 2022-11-09" to "2.7 2023-01-21".
Changed DYN_ARRAY_VERSION from "2.0 2022-06-09" to "2.1 2023-01-21".
Changed JSON_PARSER_VERSION from "0.12 2022-11-09" to "0.13 2023-01-21".
Added SOUP_VERSION as "1.0 2023-01-21".

Changed jsemtblgen so that -P prefix will always be used on function names.
Use of -M func will override any specific member name.
Changed JSEMTBLGEN_VERSION from "0.4 2022-07-13" to "0.6 2023-01-22".
The jsemtblgen tool, with the -I option, will not print duplicate
extern function declarations.

Fixed a number of tests relating to conversion of floating point values.
Corrected `jparse/test_jparse/jnum_test.c1 test results for
the `as_double is an integer` test cases.

Changed JNUM_GEN_VERSION from "0.7 2022-05-01" to "0.8 2023-01-22".

Added a system to notice "Notice:" messagees and count then for `make prep`.
The `test_ioccc/prep.sh` tool removed the generated bug report log file
if the `bug-report.sh` tool does an exit 0, even if there are notices.
Changed BUG_REPORT_VERSION from "0.12 2023-01-24" to "0.13 2023-01-24".
Changed PREP_VERSION from "0.3 2023-01-24" to "0.4 2023-01-24".

Moved static variables from header to source files.

Fix segfault when author length is too long from NULL pointer dereference: due
to an else being left off from an if after rejecting the author, freeing and
setting to NULL the code went on to check for unique author names which would
then dereference the just rejected NULL author name.


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

```sh
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


```sh
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
