# cpath

**TL;DR**: Convert a path string into a short string.

Canonicalize path strings: `cpath(1)` utility, `canon_path(3)` function in `libcpath.a`.

Canonical (adjective):

```
    ...
    2. conforming to a general rule or acceptable procedure,
    ...
    4. reduced to the canonical form
```

Convert path strings to a canonical path.


### Paths do not have to exist

The paths do **NOT** have to exist.  The local filesystem is **NOT**
accessed by the `cpath(1)` command, **NOR** the `canon_path(3)` library
function.  **NO** filesystem checks are performed.  Symbolic links are
**NOT** considered.


### If you want to also check the filesystem

To check the filesystem, after using `cpath(1)` to canonicalize a path,
use the `realpath(1)` command to check the filesystem. Similarly, after using
`canon_path(1)` to canonicalize a path, use `realpath(3)`.


# Table of Contents

- [Dependencies](#dependencies)
- [Compiling](#compiling)
- [Installing](#installing)
- [Canonicalization details](#canonicalization-details)
- [Uninstalling](#uninstalling)
- [Optional canonicalization functionality](#optional-canonicalization-functionality)
- [Command line use](#command-line-use)
- [Library function use](#library-function-use)
- [History](#history)
- [Reporting Security Issues](#reporting-security-issues)


<div id="dependencies"></div>

# Dependencies

In order to compile and use `cpath` (the applications and the library) you will
need to download, compile and install the [dbg repo](https://github.com/lcn2/dbg),
the [dyn_array repo](https://github.com/lcn2/dyn_array), and the
[pr repo](https://github.com/lcn2/pr).

To clone, make and install these dependencies:

```sh
    # clone, compile and install dbg:

    git clone https://github.com/lcn2/dbg
    cd dbg && make all
    sudo make install

    # clone, compile and install dyn_array:

    git clone https://github.com/lcn2/dyn_array
    cd dyn_array
    make all
    sudo make install

    # clone, compile and install pr:

    git clone https://github.com/lcn2/pr
    cd pr
    make all
    sudo make install
```

The default `PREFIX` to the install and uninstall rules for these libraries is
`/usr/local` but if you need to change this, say due to a system policy, you can
do so with the `PREFIX` variable. For instance if you need or want to install
the libraries to `/usr/lib`, the binaries to `/usr/bin` etc. you can do instead:

```sh
    sudo make PREFIX=/usr install
```

Of course, you can specify a different `PREFIX` than `/usr` if you wish.
Remember if you do this though, that if you uninstall them, you will have to
specify the same `PREFIX`. Also, depending on where you install, you might have
to modify your command line/Makefile when compiling and linking in the
libraries.


<div id="compiling"></div>

# Compiling

After [installing the dependencies](#dependencies):

To compile from source:

```sh
    make clobber all
```

**IMPORTANT NOTE**: the Makefiles allow one to override certain things by having
a file `Makefile.local` in the same directory. This can cause compilation errors
so if you do have such a file you should use so with caution.


<div id="installing"></div>

# Installing

After [installing the dependencies](#dependencies) and
[compiling from source](#compiling):

To install the `cpath(1)` command and `libcpath.a` library:

```sh
    sudo make install
```

We also support the `PREFIX` standard so if you need to install the binaries to
`/usr/bin`, library to `/usr/lib`, the header files to `/usr/include` and
the man pages to `/usr/share/man/man[138]`, then do:

```sh
    sudo make PREFIX=/usr install
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
    sudo make uninstall
```

If you installed to a different `PREFIX` than the default then you must specify
that same `PREFIX`. For instance if you installed with the `PREFIX` of `/usr`
then you must do instead:

```sh
    sudo make PREFIX=/usr uninstall
```


<div id="canonicalization-details"></div>

# Canonicalization details

Unlike the `realpath(1)` command and `realpath(3)` that operates on the
filesystem, cpath operating on strings.  In particular **the string does
NOT have to exist in a filesystem**.

Among the actions performed during canonicalization are:

0. Remove extraneous trailing "/"s (slashes)

```
    foo/ becomes foo
    /// becomes /
    / remains /
```

1. Replace multiple consecutive "/"s (slashes) with a single "/" (slash)

```
    /var//tmp///test becomes /var/tmp/test
    a/b//c///d becomes a/b/c/d
```

2. Remove extraneous "./ (dot) path components

```
    ./bar becomes bar
    ./ becomes .
    whey/. becomes whey
    ./a/./b/././c/. becomes a/b/c
```

3. Where possible, remove ".."s (dot-dot) path components

```
    /usr/lib/../bin/make becomes /usr/bin/make
    a/b/c/../../d/e/.. becomes a/d
    /.. becomes /
    ./.. becomes ..
    a/b/../../../c becomes ../c
```


<div id="optional-canonicalization-functionality"></div>

# Optional canonicalization functionality

While not performed by default, you may use the following functionality:


## Set a maximum path length

By default, the length of the path is unlimited.

One may optionally set a maximum allowed path length.  In this case the
path length is set after the path has been canonicalized.


## Set a maximum length of any canonicalized path component

A "_path component_" is a part of a path.  For example, the canonicalized path `/usr/bin/make`
contains a path components: `usr`, `bin`, and `make`.

By default, the length of any canonicalized path component is unlimited.
One may optionally set a maximum path component length.


## Set a maximum path depth

By default, the path depth is unlimited.

The path depth is the depth from the top level directory or "_topdir_".
If the path is an absolute path (starts with "/" (slash), then the
"_topdir_" is "/" (slash), other the "_topdir_" is assumed to be
"." (dot).

The "_topdir_" level is **0**.  A "_path component_" directly under _topdir_"
is at level **1**.

Absolute path depths:

- level 0: `/`
- level 1: `/aaa`
- level 2: `/aaa/bbb`
- level 3: `/aaa/bbb/ccc`

Relative path depths:

- level 0: `.`
- level 1: `curds`
- level 2: `curds/and`
- level 3: `curds/and/whey`


## Require the path to be relative

By default, **both** absolute and relative paths are allowed.

One may require the path to be relative and flag absolute paths as
an error.


## Convert paths to lower case

By default, the case of letters in paths are **NOT** changed.

One may, as part of the canonicalization process, optionally convert all
**UPPER CASE** letters into lower case.  Converting to lower case may be
useful with considering paths for filesystems that are case insensitive:
i.e., where `foo` and `Foo` and `FOO` refer to the same file.


## Require canonicalized path components to be safe

By default, canonicalized paths may contain any character other the `NUL` (`'\0'`
or 0 byte) character.

One may require all canonicalized path components to conform to an safety check extended regular expression.

By default, the safety check is perform by the following extended regular expression:

```
    ^[0-9A-Za-z._][0-9A-Za-z._+-]*$
```

**IMPORTANT**: We recommend that extended regular expression begin with
a **^** character and end with a **$ &**character.  This will force
the extended regular expression to match the entire canonicalized path component.

The path component safety check is performed after canonicalization.
For example, `a/-b/../c` is considered safe because after canonicalization
the canonicalized path `a/c` contains only safe path components.

**NOTE**: The term safe is used in what may be used in an application
that performs file name pattern matching is as that used by a shell.
There is nothing inherently dangerous about a path, per se, especially
when the application is the canonicalized path to most functions, such
as **fopen(3)**.  Nevertheless, an application may wish to restrict the
length, depth, and character set used in paths.


<div id="command-line-use"></div>

# Command line use

The following `cpath(1)` examples assume that `cpath` is in your
`$PATH` search path, after [installing](#installing) of course.


## cpath(1) command line examples

Single command line argument:

```sh
    $ cpath a/b/c/../../d/e/..
    a/d
```

Multiple command line arguments:

```sh
    $ cpath foo/ /// / /var//tmp///test a/b//c///d . ./bar ./ whey/.
    foo
    /
    /
    /var/tmp/test
    a/b/c/d
    .
    bar
    .
    whey
```

Without and path arguments, `cpath(1)` reads paths from standard input (stdin).
In this mode, lines that begin with "#" (hash) and empty lines are ignored:

```sh
    $ cat paths
    # comment lines are ignored as well as empty lines
    # one path per line

    ./a/./b/././c/.
    /usr/lib/../bin/make
    a/b/c/../../d/e/..

    $ cpath < paths
    a/b/c
    /usr/bin/make
    a/d
```


## cpath(1) usage message

```sh
usage: ./cpath [-h] [-v level] [-V] [-m max_path] [-M max_file] [-d max_depth] [-D] [-r] [-l] [-s] [-S regex] [path ...]

    -h              print help message and exit
    -v level        set verbosity level (def level: 0)
    -V              print version string and exit

    -m max_path     max canonicalized path length, 0 ==> no limit (def: 0)
    -M max_file     max length of any canonicalized path component, 0 ==> no limit (def: 0)
    -d max_depth    max canonicalized path depth where 0 is the topdir, 0 ==> no limit (def: 0)

    -D              error if .. (dot-dot) moves above the beginning of path (def: not an error)
    -r              path must be relative (def: absolute paths allowed)
    -l              convert to lower case (def: don't change the path case)
    -s              require all canonicalized path components to be safe (def: don't check)
    -S regex        safe match extended regex (implies -s) (def: safe match ^[0-9A-Za-z._][0-9A-Za-z._+-]*$)

    [path ...]      canonicalize path args (def: read paths from stdin)

                        When reading paths from stdin, use one path per line.
                        Empty lines and lines that begin with # (hash) are ignored.

Exit codes:
    0   all is OK
    1   canonicalized path starts with /, and -r was used
    2   -h and help string printed or -V and version string printed
    3   command line error
    4   canonicalized path byte length exceeds -m max_path limit
    5   canonicalized component byte length path exceeds -M max_file limit
    6   canonicalized path depth exceeds -d max_depth limit
    7   canonicalized is not safe, and -s was used, or -S regex is an invalid extended regex
    8   -D and use of .. (dot-dot) attempted to move before start of path
    9   path is empty
 >=10   internal error
```


<div id="library-function-use"></div>

# Library function use

The canonicalization process is handled by the `canon_path()` function,
which is available in the `libcpath.a` static library and `cpath.h`
after you have [installed the cpath dependencies](#dependencies) and
[installed cpath](#installing).

After [installing the cpath dependencies](#dependencies) and
after you have [installed this cpath repo](#installing),
compile C code such as the code below using:

```sh
    cc foo.c -ldbg -ldyn_array -lpr -lcpath -o foo
```

Consider the following C code framework:

```c
    #include <cpath.h>

    char *path;                                 /* path to canonicalize */
    char *cpath;                                /* canonicalized path or NULL */
    enum path_sanity sanity = PATH_ERR_UNSET;   /* canon_path() path error code, or PATH_OK */

    /* ... */

    /*
     * set path to be a pointer to some path string
     *
     * In this example we hard code a path.
     */
    path = "./some/./dot-dot-will-delete-me/../path/.";

    /*
     * canonicalize path without an limits, checks, no case conversion
     */
    cpath = canon_path(path, 0, 0, 0, &sanity, NULL, NULL, false, false, false, false, NULL);
    if (cpath == NULL) {

        /* write canonization error to stderr and exit(10) */
        err(10, __func__, "failed to canonicalize path: %s error: %s (%s)",
                          path, path_sanity_name(sanity), path_sanity_error(sanity));
        not_reached();

    } else {

        /* report on canonicalized path to stdout using the libpr print(3) function */
        print("path: %s is canonicalized into: %s\\n", path, cpath);

        /* free malloced canonicalized path storage */
        free(cpath);
        cpath = NULL;
    }

    /* ... */

    char *path2;                                /* another path to canonicalize */
    char *cpath2;                               /* canonicalized path2 or NULL */
    enum path_sanity sanity2 = PATH_ERR_UNSET;  /* canon_path() path2 error code, or PATH_OK */
    size_t max_path_len = 32;                   /* maximum canonicalized path2 length */
    size_t max_filename_len = 14;               /* maximum canonicalized path2 component length */
    int32_t max_depth = 5;                      /* maximum canonicalized path2 depth */
    size_t len2;                                /* length of canonicalized path2 */
    size_t depth2;                              /* length of canonicalized path depth */

    /*
     * set path2 to be a pointer to some path string
     *
     * In this example we hard code a path.
     */
    path2 = "This/./Path/Is/A/Rather_long_name_in_the-but-dot-dot-DELETES-it/../Path";

    /*
     * canonicalize path2 with size limits, relative path, lower case conversion,
     * error if .. (dot-dot) moves above the beginning of the path,
     * default extended regular expression path component safety check of:
     *
     *    ^[0-9A-Za-z._][0-9A-Za-z._+-]*$
     */
    cpath2 = canon_path(path2, max_path_len, max_filename_len, max_depth,
                        &sanity2, &len2, &depth2, true, true, true, true, NULL);
    if (cpath2 == NULL) {

        /* write canonization error to stderr and exit(11) */
        err(11, __func__, "failed to canonicalize path2: %s error: %s (%s)",
                          path2, path_sanity_name(sanity2), path_sanity_error(sanity2));
        not_reached();

    } else {

        /* report on canonicalized path2 to stdout using the libpr print(3) function */
        print("path2: %s is canonicalized into: %s", path2, cpath2);
        print("canonicalized path2 length: %zu path2 depth: %d", len2, depth2);

        /* free malloced canonicalized path storage */
        free(cpath2);
        cpath2 = NULL;
    }

    /* ... */

    char *path3;                                /* yet another path to canonicalize */
    char *cpath3;                               /* canonicalized path3 or NULL */
    enum path_sanity sanity2 = PATH_ERR_UNSET;  /* canon_path() path3 error code, or PATH_OK */
    char *regex = "^[0-9A-Za-z]+$";             /* alphanumeric only extended regular expression */
    regex_t reg;                                /* compiled extended regular expression */
    int regcomp_ret = -1;                       /* regcomp(3) return value or -1 (REG_ENOSYS) */

    /*
     * set path3 to be a pointer to some path string
     *
     * In this example we hard code a path.
     */
    path3 = "./a/b2////c333/Four";

    /*
     * compile an extended regular expression
     */
    regcomp_ret = regcomp(&reg, regex, REG_EXTENDED);
    if (regcomp_ret != 0)
        char errbuf[BUFSIZ+1];  /* regerror() message buffer */

        /* write regular expression compile error to stderr and exit(12) */
        memset(errbuf, 0, sizeof(errbuf));
        (void) regerror(regcomp_ret, &reg, errbuf, BUFSIZ);
        err(12, __func__, "invalid regular expression: %s: error: %s", regex, errbuf);
        not_reached();
    }

    /*
     * canonicalize path3 with a non-default extended regular expression path component safety check
     */
    cpath3 = canon_path(path3, max_path_len, max_filename_len, max_depth,
                        &sanity3, NULL, NULL, false, false, true, false, &reg);
    regfree(&reg); /* free the regular extended regular expression storage */
    if (cpath3 == NULL) {

        /* write canonization error to stderr and exit(13) */
        err(13, __func__, "failed to canonicalize path3: %s error: %s (%s)",
                          path3, path_sanity_name(sanity3), path_sanity_error(sanity3));
        not_reached();

    } else {

        /* report on canonicalized path3 to stdout using the libpr print(3) function */
        print("path3: %s is canonicalized into: %s", path3, cpath3);

        /* free malloced canonicalized path storage */
        free(cpath3);
    }
```

The `canon_path()` function is defined in `cpath.h` is:

```c
    /*
     * canon_path - canonicalize a path
     *
     * given:
     *      orig_path           - path to canonicalize
     *      max_path_len        - max canonicalized path length, 0 ==> no limit
     *      max_filename_len    - max length of each component of path, 0 ==> no limit
     *      max_depth           - max depth of subdirectory tree, 0 ==> no limit, <0 ==> reserved for future use
     *      sanity_p            - NULL ==> don't save canon_path path_sanity error, or PATH_OK
     *                            != NULL ==> save enum path_sanity in *sanity_p
     *      len_p               - NULL ==> don't save canonical path length,
     *                            != NULL ==> save canonical path length in *len_p
     *      depth_p             - NULL ==> don't save canonical depth,
     *                            != NULL ==> record canonical depth in *depth_p
     *      rel_only            - true ==> path from "/" (slash) NOT allowed, path depth counted from implied "." (dot)
     *                            false ==> path from "/" (slash) allowed, path depth counted from /
     *      lower_case          - true ==> convert UPPER CASE to lower case during canonicalization
     *                            false ==> don't change path case
     *      safe_chk            - true ==> test each canonical path component using safe_path_str(str, true, false)
     *                            false ==> do not perform path safety tests on the path
     * returns:
     *      NULL ==> invalid path, internal error, or NULL pointer used
     *      != NULL ==> malloced path that has been canonicalized and
     *                  if sanity_p != NULL then *sanity_p is set to PATH_OK
     */

    extern char * canon_path(char const *orig_path,
                             size_t max_path_len, size_t max_filename_len, int32_t max_depth,
                             enum path_sanity *sanity_p, size_t *len_p, int32_t *depth_p,
                             bool rel_only, bool lower_case, bool safe_chk);
```


<div id="history"></div>

# History

This code was written by [Landon Curt Noll](https://github.com/lcn2).

Various parts of the canonicalization code dates back as early as 1991.


<div id="reporting-security-issues"></div>

# Reporting Security Issues

To report a security issue, please visit "[Reporting Security Issues](https://github.com/lcn2/cpath/security/policy)".
