# dbg - info, debug, warning, error and usage message facility

When linked into your program, the `dbg` facility provides a way to write
informative messages, debug messages, warning messages, fatal (and non-fatal)
error messages and usage messages to a stream such as `stderr`, an open file or
a buffer of a fixed size.

The `dbg` function calls are easy to add to your C code. There are a number of
ways to control them including the ability to silence them by default and in the
case of debug messages, turn on increasing levels of verbosity.


# Set up

1. Compile `dbg.c` to produce `dbg.o`.
2. Add `#include "dbg.h"` to the C source files that you wish to use one or more
   of the `dbg` functions in.
3. Set `verbosity_level` to some verbosity level such as `DBG_LOW` (1) or
   `DBG_MED` (3) (see `dbg.h` for other levels).
4. Compile your source file(s) and link in `dbg.o`.


For more information including an example see the next section.


# The dbg API

For an overview of all the functions, how to control output, general notes and
other information, please see the man page `dbg.3`. You can render it by:

```sh
man ./dbg.3
```

An example program making use of some of the functions is `dbg_example.c`. In
the man page `dbg.3` it shows you how to compile it and it tells you the
expected output as well as why that output is expected.
