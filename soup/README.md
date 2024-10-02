# Requirements for repo maintainers/developers

**IMPORTANT NOTE:** if you are submitting to the IOCCC, you do **NOT** need to
meet the requirements below!

For those who need to develop and maintain this repo, **in addition to the
list of requirements in
[test_ioccc/README.md](https://github.com/ioccc-src/mkiocccentry/blob/master/test_ioccc/README.md)**,
you will need:

* bison version 3.8.2 or later[^0]
* flex version 2.6.4 or later[^0]
* `independ`, version 1.00 2022-12-27 or later (see [independ
repo](https://github.com/lcn2/independ)).
* `shellcheck` version 0.10.0 or later (see [shellcheck GitHub
repo](https://github.com/koalaman/shellcheck.net)).
* `seqcexit(1)` version 1.12 2022-11-09 or later (see [seqcexit
repo](https://github.com/lcn2/seqcexit)).
* `picky` version 2.6 or later (see [picky
repo](https://github.com/lcn2/picky)).
* a sense of humour :-)


[^0]: With respect to `bison` and `flex`: strictly speaking, you do **NOT** need the
required versions of flex/bison for the JSON parser `jparse`, or even `bison`
and `flex` at all, as only the maintainers of the [jparse
repo](https://github.com/xexyl/jparse/), and only when modifying certain files,
need these tools. This is because this repo, which has a clone of the jparse
repo, has backup files of the generated code, for those who do not have a recent
enough version; and since only maintainers of the jparse repo will create the
backup files (when necessary), one does not need to have the tools to maintain
this repo.
