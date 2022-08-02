# Major changes to the IOCCC entry toolkit


## Release 0.5 2022-07-10

The jsemtblgen tool is code complete.

Added `is_reserved()` function to `rule_count.c`.

Changed the Official iocccsize version from 28.10 2022-03-15 to
28.11 2022-07-10. Updated `test_JSON` tree and `json_teststr.txt` to reflect the
new `IOCCCSIZE_VERSION` value.

Added and improved a number of man pages.

Added Makefile rule to build HTML files from all man pages.

Both the `parse_json()` and `parse_json_file()` top level functions
of the JSON parser return a JSON node parse tree when the JSON is
valid.

Trimmed the JSON parser to not include code that does not strictly
belong to the generic JSON parser.  Removed dead C code.

Updated dbg facility to version 2.4 2022-06-26.  Version 2.4
2022-06-26 contains functions to write (form) a message, debug
diagnostic message, warning, or error message into a string buffer:

```c
extern void snmsg(char *str, size_t size, char const *fmt, ...)
extern void sndbg(char *str, size_t size, int level, char const *fmt, ...);
extern void snwarn(char *str, size_t size, char const *name, char const *fmt, ...);
extern void snwarnp(char *str, size_t size, char const *name, char const *fmt, ...);
extern void snwerr(int error_code, char *str, size_t size, char const *name, char const *fmt, ...);
extern void snwerrp(int error_code, char *str, size_t size, char const *name, char const *fmt, ...);
```

Simplify command for jparse and jsemtblgen.  The -s is now just a
flag.  Only one arg is allowed.

Adjusted dbg levels in JSON parser:

```
*     At -J 3, only the top level return type and top level tree are printed.
*     At -J 5, intermediate tree return types and tree are printed too.
*     At -J 7, also print grammar progress.
*     At -J 9, also print ugly_text and ugly_leng grammar values.
```

Improved top level JSON parser debug & warn layering.

The `json_dbg()` functions now returns void, like dbg().
The `json_vdbg()` functions now returns void, like vdbg().
The `json_dbg_tree_print()` function now returns void.
JSON parser now uses werr() instead of warn().

Now using seqcexit tool, version 1.8 2022-06-02 to sequence snwerr()
and snwerrp() calls.

Fixed a number of typos in comments and documentation.

Improved a number of C comments.  Some comments are less humor impaired
and might even make the reader prone to mirth and laughter.  :-)

Sequenced exit codes.

Improved C code format consistency.

The -V flag will only print the version string and exit.  The -h
flag may print one or more version strings preceded by an
explanatory string explaining what the version is about.

Added date strings to release strings in CHANGES.md.

Updated CHANGES.md.


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

Files of the form "chk\*" have been replaced to "old.chk\*' and
are no longer compiled, not referenced by `Makefile`.  Some of the
code in these "old.chk\*' files may be used in the formation
of the new `chkentry` tool.

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
