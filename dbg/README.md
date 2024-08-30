# TL;DR Try dbg


## TL;DR Compile and test dbg

```sh
make clobber all test
```


## TL;DR Install dbg

```sh
sudo make install
```


# dbg - info, debug, warning, error and usage message facility

When linked into your program, the `dbg` facility provides a way to
write informative messages, debug messages, warning messages, fatal
(and non-fatal) error messages and usage messages to a stream such as
`stderr`, an open file or a buffer of a fixed size.

The `dbg` facility consists of function calls with argument checking,
as well as attempting to do reasonable things when given NULL pointers,
bogus values, or when stderr is NULL.

For modern compilers, `dbg` facility function calls that use a printf-like
format strings are checked for format / parameter mismatches.  A type
mismatch between a format string and types of arguments will result in
a compiler warning message.

The `dbg` facility function calls are easy to add to your C code. There
are several ways to control them including the ability to silence
message types, and in the case of debug messages, turn on increasing
levels of verbosity.


# Set up

For more information and an example see the [dbg API](#dbg-api) section.

## If you do not wish to install the library:

0. Compile `dbg.c` to produce `dbg.o`.
2. Add `#include "dbg.h"` to the C source files that you wish to use one or more
   of the `dbg` functions in.
2. Set `verbosity_level` to some verbosity level such as `DBG_LOW` (1) or
   `DBG_MED` (3) (see `dbg.h` for other levels).
3. Compile your source file(s) and link in `dbg.o`.


## Installing the library:

First, compile the library:

```sh
    make clobber all
```

Next, install the library (as root or via sudo):

```sh
    make install
```

Then, set up the code kind of like above, but with these changes:

0. Add `#include <dbg.h>` to the C source files that you wish to use one or more
of the `dbg` functions in.
1. Set the `verbosity_level` to some verbosity level such as `DBG_LOW` (1) or
`DBG_MED` (3) (see `dbg.h` for other levels and the example further below).
2. Compile your source file(s) and link in `libdbg.a` (e.g. pass to the compiler
`-ldbg`).


# The dbg API

For an overview of all the functions, how to control output, general notes and
other information, please see the man pages. You can render them like:

```sh
man ./man/man3/dbg.3
man ./man/man3/msg.3
man ./man/man3/printf_usage.3
man ./man/man3/warn.3
man ./man/man3/werr.3
man ./man/man3/err.3
man ./man/man3/warn_or_err.3
```

An example program making use of some of the functions is `dbg_example.c`. In
the man page `dbg.3` it shows you how to compile it and it tells you the
expected output as well as why that output is expected.
