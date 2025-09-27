# `dyn_array` - dynamic array facility

When linked into your program, the `dyn_array` facility will give you a way to have a
general purpose dynamic array in your program.

## Dependencies

In order to use `dyn_array` you will need to download, compile and
install the [dbg repo](https://github.com/lcn2/dbg).

To do this, you might try:

```sh
    git clone https://github.com/lcn2/dbg
    cd dbg && make clobber all test
    # then as root or via sudo:
    make install
```

and then proceed with the below steps.


## Set up

For more information including an example see the next section.


### If you do not wish to install the library:

0. Compile `dyn_array.c` to produce `dyn_array.o`.
1. Add `#include "dyn_array.h"` to the C source files that you wish to use the
   facility in.
2. Compile your source file(s) and link in `dyn_array.o`.


### Installing the library:

First, compile and test the library:

```sh
    make clobber all test
```

Next, install the library (as root or via sudo):

```sh
    make install
```

If you need or want to install to a different location than the default
`/usr/local` you can do so by changing the `PREFIX` like:

```sh
    make PREFIX=/usr install
```


Then, set up the code kind of like above, but with these changes:

0. Add `#include <dyn_array.h>` to the C source files that you wish to use the
   facility in.
1. Compile your source file(s) and link in `dyn_array.a` (e.g. pass to the
compiler `-ldyn_array`).

## Uninstalling

If you wish to uninstall everything from this repo you can do like:

```sh
    make uninstall
```

If you changed the `PREFIX` when installing make sure to specify that. For
instance if you installed with `PREFIX=/usr` you should do:


```sh
    make PREFIX=/usr uninstall
```



## The `dyn_array` API

For an overview of all the functions and macros, please see the man page
`dyn_array.3`. You can render it by:

```sh
man ./dyn_array/dyn_array.3
```

A good example of making use of the dynamic array facility is
[example.c](blob/master/example.c).
In that example, a dynamic array of pointers to strings is
created, added to, then the entire list of strings are
printed, and finally freed.

An another example program making use of some of the functions is
[dyn_test.c](blob/master/dyn_test.c).

**PLEASE NOTE**: because `dyn_test.c` needs to be a standalone test,
the code had to "import" macros and functions the
[dbg repo](https://github.com/lcn2/dbg).
As such, code that is excluded unless **USE_DBG** is defined
should be **ignored** from the perspective of an example.


## Source Code History

This [dynamic array facility repo](https://github.com/dyn_alloc)
was created from a file copy of the _dynamic array facility_ code
found in the `dyn_alloc` subdirectory of the [mkiocccentry GitHub
repo](https://github.com/ioccc-src/mkiocccentry) on 2023 June 18.
At the file copy, `DYN_ARRAY_VERSION` was "2.3 2023-02-04", and
`DYN_TEST_VERSION` was "1.9 2023-02-04".

The last commit into the `dyn_alloc` subdirectory of the [mkiocccentry
GitHub repo](https://github.com/ioccc-src/mkiocccentry) was [commit
187a6ac4ac081fa915799fa9f8e3dbcbd950833c](https://github.com/ioccc-src/mkiocccentry/commit/187a6ac4ac081fa915799fa9f8e3dbcbd950833c)
on 2023 Feb 19.

The  `dyn_alloc` subdirectory of the [mkiocccentry GitHub
repo](https://github.com/ioccc-src/mkiocccentry) was formed via
[commit
daaa9fb68240ca85cbb4bea24a3e36be7713d676](https://github.com/ioccc-src/mkiocccentry/commit/daaa9fb68240ca85cbb4bea24a3e36be7713d676)
when the _dynamic array facility_ code was moved to the `dyn_array`
subdirectory on 2022 Nov 20.  Prior to the creation of the `dyn_alloc`
subdirectory, the dynamic array facility was found in the top
directory of the [mkiocccentry GitHub repo as late as commit
fb8203ac5de4c5e19a44a785f436236fae89bac3](https://github.com/ioccc-src/mkiocccentry/commit/fb8203ac5de4c5e19a44a785f436236fae89bac3)
also on 2022 Nov 20.

The dynamic array facility code first appeared [mkiocccentry GitHub
repo](https://github.com/ioccc-src/mkiocccentry) via [commit
16ae660339cd3729652ce3221785042c2e2d07aa](https://github.com/ioccc-src/mkiocccentry/commit/16ae660339cd3729652ce3221785042c2e2d07aa)
on 2022 Apr 17.

According to [commit
16ae660339cd3729652ce3221785042c2e2d07aa](https://github.com/ioccc-src/mkiocccentry/commit/16ae660339cd3729652ce3221785042c2e2d07aa),
it was a checkpoint of the _dynamic array facility_ of "_v1.4
2022-04-17_".

The  _dynamic array facility_ of "_v1.4 2022-04-17_" `dyn_alloc.c`
and `dyn_alloc.h` files came from the [arcetri sts
repo](https://github.com/arcetri/sts) on 2022 Apr 17.  In particular,
`dyn_alloc.c` file was copied from [commit
2d63fe8438857badc9dbc20a48bde4689ca1af9e](https://github.com/arcetri/sts/commit/2d63fe8438857badc9dbc20a48bde4689ca1af9e)
of 2017 Apr 17.  The `dyn_alloc.h` file was was copied from [commit
2b8cefa1e4556caa6e0b17f9c1017534fc9ffd6b](https://github.com/arcetri/sts/commit/2b8cefa1e4556caa6e0b17f9c1017534fc9ffd6b)
of 2016 Dec 20.  The other files that were copied into the [dynamic
array facility repo](https://github.com/dyn_alloc) were created as
part of the [mkiocccentry GitHub
repo](https://github.com/ioccc-src/mkiocccentry).

The `dyn_alloc.c` file of the [arcetri sts
repo](https://github.com/arcetri/sts) were added as part of sts
version 2.1.2.1 on 2016 Dec 20 as part of [commit
a29afbbf3d852cbf97908e05a0755b3bfaa1fbc9](https://github.com/arcetri/sts/commit/a29afbbf3d852cbf97908e05a0755b3bfaa1fbc9).
This file was created by [Landon Curt
Noll](http://www.isthe.com/chongo/index.html) back in 2014.

The `dyn_alloc.h` file of the [arcetri sts
repo](https://github.com/arcetri/sts) were added as part of sts
version 2.1.2.2 on 2016 Dec 20 as part of [commit
f124cb6e2b4ce7b07edf6fa3b4c444e32746ec88](https://github.com/arcetri/sts/commit/f124cb6e2b4ce7b07edf6fa3b4c444e32746ec88).
That file was created from the above mentioned `dyn_alloc.c` file
that was in turn created by [Landon Curt
Noll](http://www.isthe.com/chongo/index.html) back in 2014.


# Reporting Security Issues

To report a security issue, please visit "[Reporting Security Issues](https://github.com/lcn2/dyn_array/security/policy)"
