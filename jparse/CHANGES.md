# Significant changes in the JSON parser repo

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
