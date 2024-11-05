# Information on the jparse C library

In this document we describe the `jparse` C library so you can get an idea of
how to use it in a C application that needs to parse and process valid JSON
documents, whether in a file (on disk or stdin, or even over a network) or a
string.

For information on the `jparse` repo, see the [jparse repo
README.md](https://github.com/xexyl/jparse/blob/master/README.md).

For information on the `jparse` utilities see
[jparse_util_README.md](https://github.com/xexyl/jparse/blob/master/jparse_util_README.md).

For information on the testing suite see
[test_jparse/README.md](https://github.com/xexyl/jparse/blob/master/test_jparse/README.md).

We also do recommend that you read the
[json_README.md](https://github.com/xexyl/jparse/blob/master/json_README.md)
document to better understand the JSON terms used in this repo.

# Table of Contents

- [jparse library](#jparse-library)
- [jparse library example](#jparse-library-example)
- [jparse header files](#jparse-header-files)
- [Linking in the jparse library](#linking-jparse)
- [Re-entrancy](#re-entrancy)
- [jparse API overview](#jparse-api-overview)


<div id="jparse-library"></div>

# jparse library

As a library, `jparse` is much more useful that the `jparse(1)` tool, as it
allows one to parse JSON in their application and then interact with the parsed
tree.

In order to use the library, you will need to `#include` the necessary header
files and then link in the libraries (jparse and the dependency libraries).

Before we give you information about header files and linking in the libraries,
we will give you an example (or at least refer you to a simple example). This
way, you can hopefully follow it a bit better.


<div id="jparse-library-example"></div>

# jparse library example

For a relatively simple example program that uses the library, take a look at
[jparse_main.c](https://github.com/xexyl/jparse/blob/master/jparse_main.c). As
we already gave details on how to use it, we will not do that here. It is,
however, a nice example program to give you a basic idea of how to use the
library, especially as it is commented nicely.

As you will see, in the case of this tool, we include
[jparse_main.h](https://github.com/xexyl/jparse/blob/master/jparse_main.h),
which includes the two most useful header files,
[jparse.h](https://github.com/xexyl/jparse/blob/master/jparse.h) and
[util.h](https://github.com/xexyl/jparse/blob/master/util.h), the former of
which is required (in actuality, `jparse.h` includes it, but it does not hurt to
include it anyway due to inclusion guards).

Below we give finer details on using the library.


<div id="jparse-header-files"></div>

# jparse header files

For this we will assume that you have installed `jparse` into a standard
location. If you wish to not install it, then you will have to change how you
`#include` the files a bit, as well as how you link in the libraries.

While we do not (yet?) show every header file that is installed, the two most
useful ones are `jparse.h` and `util.h`, found in the `jparse/` subdirectory
(again, when installed).

Thus in your program source you might have:

```c
#include <jparse/jparse.h>
#include <jparse/util.h>
```

Again, if you need a simple example program that uses the library, see the
`jparse(1)` source code,
[jparse_main.c](https://github.com/xexyl/jparse/blob/master/jparse_main.c).


<div id="linking-jparse">

# Linking in the jparse library

In order to use the library you will have to link the static libraries (the
`jparse(3)` library as well as the `dbg` and `dyn_array` libraries) into your
program.

To do this you should pass to the compiler `-ljparse -ldbg -ldyn_array`. For
instance to compile
[json_main.c](https://github.com/xexyl/jparse/blob/master/jparse_main.c), with
the `#include` lines changed to:

```c
#include <jparse/jparse.h>
#include <jparse/util.h>
```

we can compile it like:

```sh
cc jparse_main.c -o jparse -ljparse -ldbg -ldyn_array
```

and expect to find `jparse` in the current working directory.

If you need an example for a Makefile, take a look at the
[Makefile](https://github.com/xexyl/jparse/blob/master/Makefile)'s
`jparse_main.o` and `jparse` rules, to give you an idea.

Once your code has been compiled and linked into an executable, you should be
good to go, although it naturally will obfuscate your code a bit! :-)


<div id="re-entrancy"></div>
<div id="reentrancy"></div>

# Re-entrancy

Although the scanner and parser are both re-entrant, only one parse at the same
time in a process has been tested. The testing of more than one parse at the
same time might be done at a later time but that will likely only happen if a
tool requires it.

If it's not clear: this means that having more than one parse active in the same
process at the same time is untested so even though it should be okay there
might be some issues that have yet to be discovered.


<div id="jparse-api-overview"></div>

# jparse API overview

To get an overview of the API, try from the repo directory:

```sh
man ./man/man3/jparse.3
```

or if installed:

```sh
man 3 jparse
```

which gives more information about the most important functions.
