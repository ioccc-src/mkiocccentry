# Major changes to the IOCCC entry toolkit

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
Renamed `JAUTHCHK_VERSION` to `CHKAUTH_VERSION`.
Renamed `JINFOCHK_VERSION` to `CHKINFO_VERSION`.
Renamed `json_chk.c` to `chk_util.c`.
Renamed `json_chk.h` to `chk_util.h`.
Renamed `json_err.codes` to `chk_err.codes`.
Renamed `json_warn.codes` to `chk_warn.codes`.
Renamed `jcodechk.sh` to `chkcode.sh`.


## Release 0.2

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
