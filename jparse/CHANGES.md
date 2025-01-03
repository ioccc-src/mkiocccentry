# Significant changes in the JSON parser repo

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
