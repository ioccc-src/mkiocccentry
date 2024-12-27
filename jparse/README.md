# jparse -  JSON parser, library and tools written in C

`jparse` is a JSON parser (as a stand-alone tool and a library) written in C
with the help of `flex(1)` and `bison(1)`. This library, and all the tools, some
of which use the library and others which do not, were were co-developed in
2022-2024 by:

*@xexyl* (**Cody Boone Ferguson**, [https://xexyl.net](https://xexyl.net),
[https://ioccc.xexyl.net](https://ioccc.xexyl.net))

and:

*chongo* (**Landon Curt Noll**, [http://www.isthe.com/chongo/index.html](http://www.isthe.com/chongo/index.htm)) /\oo/\

in support of the **IOCCC** ([International Obfuscated C Code
Contest](https://www.ioccc.org)), originally in the [mkiocccentry
repo](https://github.com/ioccc-src/mkiocccentry), but we provide it here so that
anyone can use it.

This document discusses the dependencies (and how to obtain, compile and install
them), how to compile and install (as well as uninstall, should you need to deobfuscate
your system :-) ) the `jparse` tools and library, how to report bugs and a brief
history on `jparse` as a whole.

For information on our utilities, including `jparse(1)` but also tools to
decode/encode JSON encoded/decoded strings (others of different applications are
in the pipeline as well), we refer you to the
[jparse_utils_README.md](https://github.com/xexyl/jparse/blob/master/jparse_utils_README.md)
file.

As a stand-alone tool, `jparse` itself is less useful, except for validating
JSON documents (as a file on disk or stdin or even over a network, or as a
string), and as a simple example that uses the library.

The `jparse` library is much more useful for C programmers because you can
integrate it into your own applications and work with the parsed tree(s). For
information on using the `jparse` library, we refer you to the file
[jparse_library_README.md](https://github.com/xexyl/jparse/blob/master/jparse_library_README.md).

We also do recommend that you read the
[json_README.md](https://github.com/xexyl/jparse/blob/master/json_README.md)
document to better understand the JSON terms used in this repo.

For information on our testing suite, we refer you to the
[test_jparse/README.md](https://github.com/xexyl/jparse/blob/master/test_jparse/README.md),
but this is mostly for repo maintainers or those who are interested in how we
make sure things are correct.


# Table of Contents

- [Dependencies](#dependencies)
- [Compiling](#compiling)
- [Installing](#installing)
- [Uninstalling](#uninstalling)
- [Reporting bugs](#reporting-bugs)
- [See also](#see-also)
- [History](#history)


<div id="dependencies"></div>

# Dependencies

In order to compile and use `jparse` (the applications and the library) you will
need to download, compile and install the [dbg
repo](https://github.com/lcn2/dbg) and the [dyn_array
repo](https://github.com/lcn2/dyn_array).

To clone, make and install these dependencies:

```sh
# clone, compile and install dbg:

git clone https://github.com/lcn2/dbg
cd dbg && make all
# then as root or via sudo(8):
make install


# clone, compile and install dyn_array:

git clone https://github.com/lcn2/dyn_array
cd dyn_array
make all
# then as root or via sudo(8):
make install
```

The default `PREFIX` to the install and uninstall rules for these libraries is
`/usr/local` but if you need to change this, say due to a system policy, you can
do so with the `PREFIX` variable. For instance if you need or want to install
the libraries to `/usr/lib`, the binaries to `/usr/bin` etc. you can do instead:

```sh
make PREFIX=/usr install
```

Of course, you can specify a different `PREFIX` than `/usr` if you wish.
Remember if you do this though, that if you uninstall them, you will have to
specify the same `PREFIX`. Also, depending on where you install, you might have
to modify your command line/Makefile when compiling and linking in the
libraries.

If there are any issues with compiling or installing either of the dependencies,
then please open an issue in the respective repo, not here.


<div id="compiling"></div>

# Compiling

The lexer/parser uses `flex(1)`/`bison(1)` but we determine if you have a recent
enough version of each, and if you do not we use the backup files (the ones we
generate when either of
[jparse.l](https://github.com/xexyl/jparse/blob/master/jparse.l)
[jparse.y](https://github.com/xexyl/jparse/blob/master/jparse.y) are modified).

To compile:


```sh
make all
```

**IMPORTANT NOTE**: the Makefiles allow one to override certain things by having
a file `Makefile.local` in the same directory. This can cause compilation errors
so if you do have such a file you should use so with caution.



<div id="installing"></div>

# Installing

If you wish to install the tools, library, header files etc., which is
**HIGHLY** recommended, especially if you want to use the library, you can do as
root or via `sudo(8)`:

```sh
make install
```

We also support the `PREFIX` standard so if you need to install the binaries to
`/usr/bin`, library to `/usr/lib`, the header files to `/usr/include/jparse` and
the man pages to `/usr/share/man/man[138]`, then do:

```sh
make PREFIX=/usr install
```

Of course, you can specify a different `PREFIX` than `/usr` if you wish. But
just like with `dbg` and `dyn_array`, if you do this, then depending on the
`PREFIX`, you might have to modify your command line/Makefile to compile and
link in the library.


<div id="uninstalling"></div>

# Uninstalling

If you wish to deobfuscate your system a bit :-), you can uninstall the programs,
library, header files and man pages by running as root or by `sudo(8)`:

```sh
make uninstall
```

If you installed to a different `PREFIX` than the default then you must specify
that same `PREFIX`. For instance if you installed with the `PREFIX` of `/usr`
then you must do instead:

```
make PREFIX=/usr uninstall
```

as either root or via `sudo(8)`.


<div id="bug-reporting"></div>
<div id="reporting-bugs"></div>
<div id="bugs"></div>

# Reporting bugs

If you have a problem with the library or tools in some form, for example you
cannot compile it in your system, or if you think there is a bug of some kind,
please kindly run from the repo directory:

```sh
make bug_report
```

and attach the log file (it will tell you what the name is) to a new issue at
the [jparse issues page](https://github.com/xexyl/jparse/issues). If it's a bug
please select the bug template.

Note that the script,
[jparse_bug_report.sh](https://github.com/xexyl/jparse/blob/master/jparse_bug_report.sh),
will tell you if it finds any problems and if it does not it tells you that you
can safely delete it. Of course just because it does not find any problems does
not necessarily mean there is not a problem. On the other hand, just because it
tells you that there is a problem does not mean that there is a problem with the
repo; it could be your environment or something else entirely.

To report an issue as a bug, please [open a bug issue on the GitHub issues
page](https://github.com/xexyl/jparse/issues/new?assignees=&labels=bug&projects=&template=bug_report.yml&title=Bug%3A+%3Cbug+title%3E).


Please do **NOT** report a problem with JSON as we can only do what the
[so-called JSON
spec](https://github.com/xexyl/jparse/blob/master/json_README.md#so-called-json-spec)
mandates.

<div id="see-also"></div>

# See also

For more information, try from the repo directory:

```sh
man ./man/man1/jparse.1
man ./man/man3/jparse.3
man ./man/man1/jstrdecode.1
man ./man/man1/jstrencode.1
```

or if you have installed everything (i.e. you ran `make install` as root or via
`sudo(8)`) then you can do:

```
man 1 jparse
man 3 jparse
man jstrdecode
man jstrencode
```

**NOTE**: the library man page does have an example but you can always look
at [jparse_main.c](https://github.com/xexyl/jparse/blob/master/jparse_main.c)
for a relatively simple example (the source code for `jparse(1)` itself, as
described in the
[jparse_utils_README.md](https://github.com/xexyl/jparse/blob/master/jparse_utils_README.md)
file.

<hr>

<div id="history"></div>

# History

It was way back in 1692 when grandpa Landon decided that the **IOCCC**
([International Obfuscated C Code Contest](https://www.ioccc.org)) should use
JSON for data files. Initially a rudimentary parser was in the works but it was
decided that a real parser would be needed and Cody volunteered to help
(actually he was helping before this). We decided to use `flex` and `bison` but
we still had to write thousands of lines of code ourselves. In the end it came
to be a solid parser, both a tool and a library, along with other useful tools.

If you need or just want more details on what happened, and how it progressed,
we suggest you check out the `CHANGES.md` file here and at the [mkiocccentry
repo](https://github.com/ioccc-src/mkiocccentry).

If you really wish to go further than that you can read the GitHub git log in
the `mkiocccentry` repo under the `jparse/` subdirectory, plus the git log at
this jparse repo,  as well as reading the source code. There is a lot to read,
however, so you probably will not want to do that.

If you do read the source code we **STRONGLY** recommend you read the
[jparse.l](https://github.com/xexyl/jparse/blob/master/jparse.l) and
[jparse.y](https://github.com/xexyl/jparse/blob/master/jparse.y) files and
**NOT** the bison or flex generated code! This is because the generated code
might give you nightmares and cause other horrible symptoms. :-) See
[sorry.tm.ca.h](https://github.com/xexyl/jparse/blob/master/sorry.tm.ca.h) for
more details on this. Of course if you're brave enough to read the generated
code you're welcome to but don't say we did not warn you! :-)

Of course, the code we wrote is well commented and might or might not be worth
reading as well.
