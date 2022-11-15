# `dyn_array` - dynamic array facility

When linked into your program, the `dyn_array` facility will give you a way to have a
general purpose dynamic array in your program.


# Set up

1. Compile `dyn_array.c` to produce `dyn_array.o`.
2. Add `#include "dyn_array.h"` to the C source files that you wish to use the
   facility in.
4. Compile your source file(s) and link in `dyn_array.o`.


For more information including an example see the next section.


# The `dyn_array` API

For an overview of all the functions and macros, please see the man page
`dyn_array.3`. You can render it by:

```sh
man ./dyn_array.3
```

An example program making use of some of the functions is `dyn_test.c`. How to
use this example will be added once the facility does not depend on the `dbg`
facility.
