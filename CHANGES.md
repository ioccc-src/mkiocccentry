# Major changes to the IOCCC entry toolkit


## Release 0.4

JSON parser is code complete!

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

In `.info.json` the `chkinfo\_version` was replaced by `chkentry\_version`.

In `.author.json` the `chkauth\_version` was replaced by `chkentry\_version`.

The `IOCCC\_info\_versioN` in `.info.json` was changed from "1.9 2022-03-15"
to "1.10 2022-06-22".  The INFO\_VERSION in `version.h` was changed accordingly.

The `IOCCC\_author\_version` in `.author.json` was changed from "1.12 2022-03-15"
to "1.13 2022-06-22".  The `AUTHOR\_VERSION` in `version.h` was changed accordingly.

The `MIN\_TIMESTAMP` in `version.h has was changed from 1652598666 to 1655958810.


## Release 0.3

Improved JSON parser diagnostic messages.  Now, `./jparse -s 0` just prints
`invalid JSON`, whereas `./jparse -J 7 -s 0` prints a bunch of parser
diagnostics and  `./jparse -J 9 -s 0` turns on `bison` debug messages.

Updates to use new debug facility features from version 2.2 2022-06-08.

Fixed lots typos.  Make a number of text improvements in comments,
shell scripts, and documentation (both man pages and markdown documents).

Renamed jauthchk to chkauth.  Renamed jinfochk to chkinfo.
Renamed associated header files, man pages and comment references.
Adjusted shell scripts to use the new names accordingly.
Renamed `JAUTHCHK\_VERSION` to `CHKAUTH\_VERSION`.
Renamed `JINFOCHK\_VERSION` to `CHKINFO\_VERSION`.
Renamed `json\_chk.c` to `chk\_util.c`.
Renamed `json\_chk.h` to `chk\_util.h`.
Renamed `json\_err.codes` to `chk\_err.codes`.
Renamed `json\_warn.codes` to `chk\_warn.codes`.
Renamed `jcodechk.sh` to `chkcode.sh`.


## Release 0.2

Several new tools and man pages have been added. Additionally the JSON parser is
being worked on with reasonable progress. The following tools have been added:

- `jinfochk` - check .info.json file (incomplete - dependent on JSON parser)
- `jauthchk` - check .author.json file (incomplete - dependent on JSON parser)
- `verge` - check that a version is greater than or equal to another
- `jparse`  - the JSON parser (incomplete as noted)
- `jnum\_chk` - check JSON number string conversions
- `jnum\_gen` - generate JSON number string conversion test data
- `utf8_test` - test translate UTF-8 into POSIX portable filename and + chars

See the following additional man pages:

- `jinfochk.1`
- `jauthchk.1`
- `jstrdecode.1`
- `jstrencode.1`
- `verge.1`
- `jparse.1`
- `limit\_ioccc.1`
- `utf8\_test.1`


The `-T` option to the tools has been removed as it was decided it is an
obsolete concept.

NOTE: The `iocccsize` is not listed as a new tool in this file as it was always
here.


## Release 0.1

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
