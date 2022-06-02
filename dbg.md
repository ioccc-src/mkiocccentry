# dbg - info, debug, warning, error and usage message facility

When linked into your program, the `dbg` facility provides a way to write
informative messages, debug messages, warning messages, fatal error messages and
usage messages to a stream such as `stderr`.

The `dbg` function calls are easy to add to your C code. There are a number of
ways to control them including the ability to silence them by default and in the
case of debug messages, turn on increasing levels of verbosity.


# TL;DR - too long; didn't read

## To setup:

1. Compile `dbg.c` to produce `dbg.o`.
2. Add `#include "dbg.h"` to the C source files that you wish to use one or more
   of the `dbg` functions in.
3. Set `verbosity_level` to some verbosity level such as `DBG_LOW` (1) or
   `DBG_MED` (3) (see `dbg.h` for other levels).
4. Compile your source file(s) and link in `dbg.o`.


# General notes for all functions

## In case of NULL fmt:

If `fmt == NULL`, then the following format string will be used:

```
((NULL fmt))
```

and the following warning, preceded by a newline, will be issued:

```
Warning: foo: fmt is NULL, forcing fmt to be: ((NULL fmt))
```

where `foo` refers to the function in question (msg(), warn() etc.).

When `fmt == NULL` or contains no `%` specifiers, the arguments following `fmt`
are ignored.

## In case of NULL name

In the functions that accept a name (say the calling function) if `name ==
NULL`, then the following string will be used instead:

```
((NULL name))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: foo: name is NULL, forcing name to be: ((NULL name))
```

where `foo` refers to the function in question (msg(), warn() etc.).


## Error checking

All writes are checked for errors. Write error messages are written to stderr.
However, a persistent problem writing to the stream (such as if the stream
being written to was previously closed) will likely prevent such an error from
being seen.



## Examples:

```c
/* ... */

#include <unistd.h>

/* ... */

/*
 * dbg - info, debug, warning, error and usage message facility
 */
#include "dbg.h"

...

#define VERSION_STRING "61.0 2022-06-01"

static char const * const usage =
"usage: %s [-h] [-v level]\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: 0)\n"
"\n"
"Version: %s";

...

    int i;

    while ((i = getopt(argc, argv, "hv:")) != -1) {
	switch (i) {
	case 'h':       /* -h - write help to stderr and exit 0 */
	    /* exit(0); */
	    fprintf_usage(0, stderr, usage, program, DBG_VERSION); /*ooo*/
	    not_reached();
	    break;
	case 'v':       /* -v verbosity */
	    /* parse verbosity */
	    errno = 0;                  /* pre-clear errno for errp() */
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		/* exit(1); */
		errp(1, __func__, "cannot parse -v arg: %s", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	default:
	    fprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
	    fprintf_usage(3, stderr, usage, program, VERSION_STRING); /*ooo*/
	    not_reached();
	}

...

    dbg(DBG_LOW, "starting critical section");
    dbg(DBG_MED, "file: %s has length: %ld", filename, length);
    if (buf[0] < MIN_BYTE) {
        dbg(DBG_VVHIGH, "Leading buffer byte: 0x%02x < minimum: 0x%02x", buf[0], MIN_BYTE);
    }

...

    errno = 0;                      /* pre-clear errno for warnp() */
    ret = fprintf(answerp, "%s\n", info.title);
    if (ret <= 0) {
        warnp(__func__, "fprintf error writing title to the answers file");
    }

...

    errno = 0;
    tarball = strdup(tarball_path);
    if (tarball == NULL) {
        errp(9, __func__, "strdup() tarball path %s failed", tarball_path);
        not_reached();
    }

```





# To use:

Compile `dbg.o`:

```sh
$ make dbg.o
```

Include `dbg.h` in your C code:

```c
/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"
```

And finally link dbg.o into your binary:

```
$ cc your_code.o ... dbg.o -o your_code
```


# msg - write a generic message

The `msg()` function will write an informational message to `stderr`.

The `fmsg()` function will write an informational message to a stream.

## Synopsis:

```c
/*
 * msg - write a generic message, to stderr
 *
 * given:
 *      fmt     printf format
 *      ...
 */
extern void msg(const char *fmt, ...);

/*
 * fmsg - write a generic message, to a stream
 *
 * given:
 *      stream  open stream to use
 *      fmt     printf format
 *      ...
 */
extern void fmsg(FILE *stream, char const *fmt, ...);
```

The `fmt` argument is a printf-style format. If the format requires arguments,
then such arguments may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%` directives in `fmt`, and
any arguments that may follow is checked by the format attribute facility.  Thus
having too many arguments, too few arguments, or arguments of the wrong type will
result in compiler warnings.

Special care is made in these functions to restore the caller's original value
of `errno` on return.


## Examples:

```c
msg("This is an informative message.");
msg("foo: %d bar: <%s> baz: %g", foo, bar, baz);

/* ... */

fmsg(stdout, "This is an informative message to an open file: %s", debug_filename);
```

A newline (i.e., `\n`) is written after the message and thus the `fmt` argument
does **NOT** need to end in a newline (though it can if you want it to).

## Output control:

The following global variables have impact output on the `msg()` functions:

```c
extern int verbosity_level;		/* maximum debug level for debug messages */
extern bool msg_output_allowed;		/* false ==> disable informational messages */
extern bool msg_warn_silent;		/* true ==> silence info & warnings if verbosity_level <= 0 */
```

Any of the following conditions, if true at the time the function is called,
will **disable** the writes.

```c
1. msg_output_allowed == false
2. msg_warn_silent == true && verbosity_level <= 0
```


## Alternative `va_list` interface:

The `vmsg()` function is an alternative interface to `msg()` in `va_list` form.

The `vfmsg()` function is an alternative interface to `fmsg()` in `va_list`
form.


```c
/*
 * vmsg - write a generic message, to stderr, in va_list form
 *
 * given:
 *      fmt     format of the warning
 *      ap      variable argument list
 */
extern void vmsg(char const *fmt, va_list ap);

/*
 * vfmsg - write a generic message, to a stream, in va_list form
 *
 * given:
 *      stream  open stream to use
 *      fmt     format of the warning
 *      ap      variable argument list
 */
extern void vfmsg(FILE *stream, char const *fmt, va_list ap);
```

The state of the `va_list ap` is **NOT** modified by this alternative interface
function.

NOTE: The arguments referenced by the `va_list ap` argument are **NOT** checked
for consistency like they are using the primary interface.  For this reason, the
primary interface `msg()` or `fmsg()` is recommended.





# dbg - write a verbosity level allowed debug message

The `dbg()` function will write a debug message, if the verbosity level is high
enough, to a stream, such as `stderr`.

The `fdbg()` function will write a debug message, if the verbosity level is high
enough, to a stream.

## Synopsis:

```c
/*
 * dbg - write a verbosity level allowed debug message
 *
 * given:
 *      level   write message if >= verbosity level
 *      fmt     printf format
 *      ...
 */
extern void dbg(int level, const char *fmt, ...);

/*
 * fdbg - write a verbosity level allowed debug message, to a stream
 *
 * given:
 *      stream  open stream to use
 *      level   write message if >= verbosity level
 *      fmt     printf format
 *      ...
 */
extern void fdbg(FILE *stream, int level, char const *fmt, ...);
```

The `level` argument determines if the debug message is written.  If `level` is
above the `verbosity_level` level, nothing is written to the stream.

It is recommended that the calling program set the global `verbosity_level` by
use of a command line option such as `-v level`.  For example:

```c
    ...
    while ((i = getopt(argc, argv, "hv:")) != -1) {
        switch (i) {
        case 'h':       /* -h - write help to stderr and exit 0 */
            /* exit(0); */
            fprintf_usage(0, stderr, usage, program, DBG_VERSION); /*ooo*/
            not_reached();
            break;
        case 'v':       /* -v verbosity */
            /* parse verbosity */
            errno = 0;
            verbosity_level = (int)strtol(optarg, NULL, 0);
            if (errno != 0) {
                /* exit(1); */
                err(1, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno)); /*ooo*/
                not_reached();
            }
            break;
	/* ... */
```

The `fmt` argument is a printf-style format.  If the format requires arguments,
then such arguments may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%` directives in `fmt` and
any arguments that may follow is checked by the format attribute facility.  Thus
having too many arguments, too few arguments, or arguments of the wrong type
will result in compiler warnings.

The `dbg.h` header file defines the following constants that are useful to help
establish verbosity levels:

```c
#define DBG_NONE (0)            /* no debugging */
#define DBG_LOW (1)             /* minimal debugging */
#define DBG_MED (3)             /* somewhat more debugging */
#define DBG_HIGH (5)            /* verbose debugging */
#define DBG_VHIGH (7)           /* very verbose debugging */
#define DBG_VVHIGH (9)          /* very very verbose debugging */
#define DBG_VVVHIGH (11)        /* very very very verbose debugging */
#define DBG_VVVVHIGH (13)       /* very very very very verbose debugging */
```

Special care is made in this function to restore the caller's original value of
`errno` on return.

## Examples:

```c
dbg(DBG_LOW, "Starting critical code section");
dbg(DBG_MED, "Current line numner: %ld", lineno);
dbg(DBG_VVHIGH, "Leading buffer byte: 0x%02x < minimum: 0x%02x", buf[0], MIN_BYTE);

/* ... */

fdbg(stdout, DBG_HIGH, "loop value: %d", i);
```

The `level` argument helps determine if the debug message is written or ignored.
If the global `verbosity_level` is high enough, then the debug message is
written to a stream such as `stderr`.

For example, if `verbosity_level == 5`, then the first two examples above
will write to a stream such as `stderr`, while the third example will not.

The debug message will be preceded by `debug[#]:` followed by a space, where `#`
is the numeric value of the `level` argument.

A newline (i.e., `\n`) is written after the message, and thus the `fmt` argument
does **NOT** need to end in a newline (but you can add one if you want).

If `verbosity_level >= 11` the following output on a stream such as `stderr` is
possible:

```
debug[1]: Starting critical code section
debug[3]: Current line numner: 7
debug[11]: Leading buffer byte: 0x45 < minimum: 0x7e

..

debug[5]: loop value: 23209
```

## Output control:

The following global variables have impact output:

```c
extern int verbosity_level;		/* maximum debug level for debug messages */
extern bool dbg_output_allowed;         /* false ==> disable debug messages */
```

Any of the following conditions, if true at the time the function is called,
will **disable** the writes.

```c
1. dbg_output_allowed == false
2. verbosity_level > level
```

In the above, `level` is a value passed to the function.

## Alternative `va_list` interface:

The `vdbg()` function is an alternative interface to `dbg()` in `va_list` form.

The `vfdbg()` function is an alternative interface to `fdbg()` in `va_list`
form.

```c
/*
 * vdbg - write a verbosity level allowed debug message, to stderr, in va_list form
 *
 * given:
 *      level   write message if >= verbosity level
 *      fmt     format of the warning
 *      ap      variable argument list
 */
extern void vdbg(int level, char const *fmt, va_list ap);

/*
 * vfdbg - write a verbosity level allowed debug message, to a stream, in va_list form
 *
 * given:
 *      stream  open stream to use
 *      level   write message if >= verbosity level
 *      fmt     format of the warning
 *      ap      variable argument list
 */
 extern void vfdbg(FILE *stream, int level, char const *fmt, va_list ap);
```

The state of the `va_list ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the `va_list ap` argument are **NOT** checked
for consistency like they are using the primary interface.  For this reason, the
primary interface `dbg()` or `fdng()` is recommended.





# warn - write a warning message

The `warn()` function will write warning a message to `stderr`.

The `fwarn()` function will write warning a message to a stream.

## Synopsis:

```c
/*
 * warn - write a warning message to stderr
 *
 * given:
 *      name    name of function issuing the warning
 *      fmt     format of the warning
 *      ...     optional format args
 */
extern void warn(char const *name, char const *fmt, ...);

/*
 * fwarn - write a warning message to a stream
 *
 * given:
 *      stream  open stream to use
 *      name    name of function issuing the warning
 *      fmt     format of the warning
 *      ...     optional format args
 */
extern void fwarn(FILE *stream, char const *name, char const *fmt, ...);
```

The `name` argument should be the name of the calling function or some string
that helps identify the calling context.  For modern C compilers, the value
`__func__` is the name of the calling function, and thus is a good choice for a
first argument.

The `fmt` argument, is a printf-style format.  If the format requires arguments,
then such arguments may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

Special care is made in this function to restore the
caller's original value of `errno` on return.

## Examples:

```c
warn(__func__, "unexpected foobar: %d", value);

...

fwarn(stdout, __func__, "unexpected curds: %d", cvalue);
```
The warning message will be preceded by `Warning:`,
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

The following are possible output of the above example:

```
Warning: warn: unexpected foobar: 42

..

Warning: fwarn: unexpected curds: 55
```

## Output control:

The following global variables have an impact output:

```c
extern int verbosity_level;		/* maximum debug level for debug messages */
extern bool warn_output_allowed;	/* false ==> disable warning messages */
extern bool msg_warn_silent;		/* true ==> silence info & warnings if verbosity_level <= 0 */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. warn_output_allowed == false
2. msg_warn_silent == true && verbosity_level > 0
```

## In case of error:

If `fmt == NULL`, then the following format string will be used:

```
((NULL fmt))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warn: fmt is NULL, forcing fmt to be: ((NULL fmt))
```

When `fmt == NULL`, any arguments following `fmt` are ignored.


## Alternative va_list interface:

The `vwarn()` function is an alternative interface to `warn()`
in `va_list` form.

The `vfwarn()` function is an alternative interface to `fwarn()`
in `va_list` form.

```c
/*
 * vwarn - write a warning message, to stderr, in va_list form
 *
 * given:
 *      name    name of function issuing the warning
 *      fmt     format of the warning
 *      ap      variable argument list
 */
extern void vwarn(char const *name, char const *fmt, va_list ap);

/*
 * vfwarn - write a warning message to a stream, in va_list form
 *
 * given:
 *      stream  open stream to use
 *      name    name of function issuing the warning
 *      fmt     format of the warning
 *      ap      variable argument list
 */
 extern void vfwarn(FILE *stream, char const *name, char const *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `warn()` or `fwarn()` is recommended.





# warnp - write a warning message with errno details

The `warnp()` function will write a warning message that
includes errno details to `stderr`.

The `fwarnp()` function will write a warning message that
includes errno details to a stream.

## Synopsis:

```c
/*
 * warnp - write a warning message with errno details, to stderr
 *
 * given:
 *      name    name of function issuing the warning
 *      fmt     format of the warning
 *      ...     optional format args
 */
extern void warnp(char const *name, char const *fmt, ...);

/*
 * fwarnp - write a warning message with errno details to a stream
 *
 * given:
 *      stream  open stream to use
 *      name    name of function issuing the warning
 *      fmt     format of the warning
 *      ...     optional format args
 */
extern void fwarnp(FILE *stream, char const *name, char const *fmt, ...);
```

The `name` argument should be the name of the calling function,
or some string that helps identify the calling context.
For modern C compilers, the value `__func__` is the name of
the calling function, and thus is a good choice for a first argument.

The `fmt` argument, is a printf-style format.
If the format requires arguments, then such arguments
may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

The `warnp()` function is similar to `warn()` with the
additional writing of `errno` related information.
The `fwarnp()` function is similar to `fwarn()` with the
additional writing of `errno` related information.
For this reason, this function may be used to
report on a failure of a library function or system call
that sets `errno`.

Because some previous library function or system call may
have set `errno` previously, it is recommended
that you set `errno` to 0 before calling a
given library function or system call.

For example:

```c
#include <errno.h>

...

errno = 0;		/* pre-clear errno for warnp() */
ret = fprintf(answerp, "%s\n", info.common.ioccc_id);
if (ret <= 0) {
    warnp(__func__, "fprintf error writing IOCCC contest id to the answers file");
    ++answers_errors;
}
```

Special care is made in this function to restore the
caller's original value of `errno` on return.

## Examples:

```c
warnp(__func__, "unable to open file: %s", filename);

...

fwarnp(stderr, __func__, "unable to write to file: %s", filename);
```

The warning message will be preceded by `Warning:`,
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

After the `fmt` message is written to a stream
such as `stderr`, information about `errno` is written.
The `errno` information written, preceded by a space
is of the form `errno[#]: string` where `#` is the
value of `errno` and `string` is the output of
`strerror(errno)` and is based on the caller's value of `errno`.

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

The following are possible output of the above example:

```
Warning: function_name: unable to open file: curds: errno[2]: No such file or directory
```

## Output control:

The following global variables have an impact output:

```c
extern int verbosity_level;		/* maximum debug level for debug messages */
extern bool warn_output_allowed;	/* false ==> disable warning messages */
extern bool msg_warn_silent;		/* true ==> silence info & warnings if verbosity_level <= 0 */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. warn_output_allowed == false
2. msg_warn_silent == true && verbosity_level > 0
```


## Alternative `va_list` interface:

The `vwarnp()` function is an alternative interface to `warnp()`
in `va_list` form.

The `vfwarnp()` function is an alternative interface to `fwarnp()`
in `va_list` form.

```c
/*
 * vwarnp - write a warning message with errno details, to stderr, in va_list form
 *
 * given:
 *      name    name of function issuing the warning
 *      fmt     format of the warning
 *      ap      variable argument list
 */
extern void vwarnp(char const *name, char const *fmt, va_list ap);

/*
 * vfwarnp - write a warning message with errno details to a stream, in va_list form
 *
 * given:
 *      stream  open stream to use
 *      name    name of function issuing the warning
 *      fmt     format of the warning
 *      ap      variable argument list
 */
extern void vfwarnp(FILE *stream, char const *name, char const *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `warnp()` or `fwarnp()` is recommended.





# err - write a fatal error message before exiting

The `errp()` function will write an error message before exiting to `stderr`.

The `ferrp()` function will write an error message before exiting to a stream.

## Synopsis:

```c
/*
 * err - write a fatal error message before exiting, to stderr
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the error
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void err(int exitcode, char const *name, char const *fmt, ...);

/*
 * ferr - write a fatal error message before exiting to a stream
 *
 * given:
 *      exitcode        value to exit with
 *      stream          open stream to use
 *      name            name of function issuing the error
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void ferr(int exitcode, FILE *stream, char const *name, char const *fmt, ...);
```

The `exitcode` argument is the exit code to exit with.

The `name` argument should be the name of the calling function,
or some string that helps identify the calling context.
For modern C compilers, the value `__func__` is the name of
the calling function, and thus is a good choice for a firsts argument.

The `fmt` argument, is a printf-style format.
If the format requires arguments, then such arguments
may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

Because this function calls `exit()`, this function will **not** return.

See `man 3 exit` for details on the `exit()` call.

For modern C compilers, the compiler will know that
this function will not return and will treat any code
following `err()` as unreachable code.

For backward compatibility, you should follow the `err()`
call with a call to the function `not_reached()`.

For modern C compilers, the `not_reached()` call becomes a
call to `__builtin_unreachable()` and no compiler warning
will be used.  For all other compilers, the `not_reached()`
call becomes a call to `abort()`.  Because the `err()` call
will never return, the `abort()` function will not be reached.
Nevertheless in both cases the compiler will know that
follows the  `err()` call will not be reached.

For example:

```c
if (filename == NULL) {
    err(31, __func__, "filename is NULL!");
    not_reached();
}
filename_len = strlen(filename);
```

In the above example, the compiler will know
that the call to `strlen()` is safe.
For both modern and older C compilers,
the compiler knows that the `NULL` pointer
case does **NOT** fall through to the
`strlen()` call.

## Examples:

```c
err(1, __func__, "bad foobar: %s", message);
not_reached();

...

ferr(1, stdout, __func__, "improper curds value: %d", cvalue);
not_reached();
```

The error message will be preceded by `FATAL[#]:`
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

The following are possible output of the above example:

```
FATAL[1]: function_name: bad foobar: fizzbin
```

## Output control:

The following global variables have an impact output:

```c
extern bool err_output_allowed;		/* false ==> disable error messages */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. err_output_allowed == false
```

## In case of error:

If `exitcode < 0`, then the following exit code 255 will be used.

If this case the following warning, preceded by a newline, will be issued:

```
Warning: err: exitcode < 0: #
```

Here `#` will be the original  `exitcode` value.

Another second warning will be written of the form:

```
Warning: err: forcing use of exit code: 255
```


## Alternative va_list interface:

The `verr()` function is an alternative interface to `err()`
in `va_list` form.

The `vferr()` function is an alternative interface to `efrr()`
in `va_list` form.

```c
/*
 * verr - write a fatal error message before exiting, to stderr, in va_list form
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the error
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void verr(int exitcode, char const *name, char const *fmt, va_list ap);

/*
 * vferr - write a fatal error message before exiting to a stream, in va_list form
 *
 * given:
 *      exitcode        value to exit with
 *      stream          open stream to use
 *      name            name of function issuing the error
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vferr(int exitcode, FILE *stream, char const *name, char const *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `err()` or `ferr()` is recommended.





# errp - write a fatal error message with errno details before exiting

The `errp()` function will write an error message that includes errno details
before exiting to `stderr`.

The `ferrp()` function will write an error message that includes errno details
before exiting to a stream.

## Synopsis:

```c
/*
 * errp - write a fatal error message with errno details before exiting, to stderr
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the warning
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void errp(int exitcode, char const *name, char const *fmt, ...);

/*
 * ferrp - write a fatal error message with errno details before exiting to a stream
 *
 * given:
 *      exitcode        value to exit with
 *      stream          open stream to use
 *      name            name of function issuing the warning
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void ferrp(int exitcode, FILE *stream, char const *name, char const *fmt, ...);
```

The first argument is the exit code to exit with.

The `name` argument should be the name of the calling function,
or some string that helps identify the calling context.
For modern C compilers, the value `__func__` is the name of
the calling function, and thus is a good choice for a first argument.

The `fmt` argument, is a printf-style format.
If the format requires arguments, then such arguments
may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

The `errp()` function is similar to `err()` with the
additional writing of `errno` related information.
The `ferrp()` function is similar to `ferr()` with the
additional writing of `errno` related information.
For this reason, this function may be used to
report on a failure of a library function or system call
that sets `errno`.

Because some previous library function or system call may
have set `errno` previously, it is recommended
that you set `errno` to 0 before calling a
given library function or system call.

For example:

```c
#include <errno.h>

...

errno = 0;              /* pre-clear errno for errp() */
answerp = fopen(answers, "r");
if (answerp == NULL) {
    errp(8, __func__, "cannot open answers file");
    not_reached();
}
```

Because this function calls `exit()`, this function will **not** return.

See `man 3 exit` for details on the `exit()` call.

For modern C compilers, the compiler will know that
this function will not return and will treat any code
following `errp()` as unreachable code.

For backward compatibility, you should follow the `err()`
call with a call to the function `not_reached()`.

For modern C compilers, the `not_reached()` call becomes a
call to `__builtin_unreachable()` and no compiler warning
will be used.  For all other compilers, the `not_reached()`
call becomes a call to `abort()`.  Because the `errp()` call
will never return, the `abort()` function will not be reached.
Nevertheless in both cases the compiler will know that
follows the  `errp()` call will not be reached.

For example:

```c
errno = 0;                  /* pre-clear errno for errp() */
filename = strdup(arg);
if (filename == NULL") {
    errp(31, __func__, "cannot strdup filename");
    not_reached();
}
filename_len = strlen(filename);
```

In the above example, the compiler will know
that the call to `strlen()` is safe.
For both modern and older C compilers,
the compiler knows that the `NULL` pointer
case does **NOT** fall through to the
`strlen()` call.

## Examples:

```c
errp(1, __func__, "printf function error, returned: %d", err);
not_reached();

...

ferrp(1, stdout, __func__, "strdup() failed");
not_reached();
```

The error message will be preceded by `FATAL[#]:`
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

The following are possible output:

```
FATAL[1]: function_name: bad foobar: fizzbin errno[2]: No such file or directory
```

## Output control:

The following global variables have an impact output:

```c
extern bool err_output_allowed;		/* false ==> disable error messages */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. err_output_allowed == false
```

## In case of error:

If `exitcode < 0`, then the following exit code 255 will be used.

If this case the following warning, preceded by a newline, will be issued:

```
Warning: errp: exitcode < 0: #
```

Here `#` will be the original  `exitcode` value.

Another second warning will be written in the following form:

```
Warning: errp: forcing use of exit code: 255
```


## Alternative va_list interface:

The `verrp()` function is an alternative interface to `errp()`
in `va_list` form.

```c
/*
 * verrp - write a fatal error message with errno details before exiting, to stderr, in va_list form
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the warning
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void verrp(int exitcode, char const *name, char const *fmt, va_list ap);

/*
 * vferrp - write a fatal error message with errno details before exiting, to a stream, in va_list form
 *
 * given:
 *      exitcode        value to exit with
 *      stream          open stream to use
 *      name            name of function issuing the warning
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vferrp(int exitcode, FILE *stream, char const *name, char const *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `errp()` or `ferrp()` is recommended.





# werr - write an error message w/o exiting

The `werr()` function will write an error message w/o exiting to `stderr`.

The `fwerr()` function will write an error message w/o exiting to a stream.

## Synopsis:

```c
/*
 * werr - write an error message w/o exiting, to stderr
 *
 * given:
 *      error_code      error code
 *      name            name of function issuing the error
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void werr(int error_code, char const *name, char const *fmt, ...)

/*
 * fwerr - write an error message w/o exiting to a stream
 *
 * given:
 *      error_code      error code
 *      stream          open stream to use
 *      name            name of function issuing the error
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void fwerr(int error_code, FILE *stream, char const *name, char const *fmt, ...);
```

The `werr()` function writes the same thing as `err()` but does
NOT call `exit()` nor does it perform a bounds check on `error_code`.

The `wferr()` function writes the same thing as `efrr()` but does
NOT call `exit()` nor does it perform a bounds check on `error_code`.

The reason why the first argument is `error_code` (and not 'exitcode`)
is that this argument is simply an arbitrary integer that is
to be written after "`FATAL[`" and before the "`]'".

The `name` argument should be the name of the calling function,
or some string that helps identify the calling context.
For modern C compilers, the value `__func__` is the name of
the calling function, and thus is a good choice for a first argument.

The `fmt` argument, is a printf-style format.
If the format requires arguments, then such arguments
may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

Special care is made in this function to restore the
caller's original value of `errno` on return.

## Examples:

```c
werr(21701, __func__, "bad foobar: %s", message);

...

fwerr(23209, __func__, "unexpected whey: %s", cwhey);
```

The error message will be preceded by `FATAL[#]:`
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

The following is the possible output of the above example:

```
FATAL[21701]: function_name: bad foobar: fizzbin
```

## Output control:

The following global variables have an impact output:

```c
extern bool err_output_allowed;		/* false ==> disable error messages */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. err_output_allowed == false
```

## In case of error:

NOTE: No bounds check is made for the first argument (`error_code`).


## Alternative va_list interface:

The `vwerr()` function is an alternative interface to `werr()`
in `va_list` form.

```c
/*
 * vwerr - write an error message w/o exiting, to stderr, in va_list form
 *
 * given:
 *      error_code      error code
 *      name            name of function issuing the error
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vwerr(int error_code, char const *name, char const *fmt, va_list ap);

/*
 * vfwerr - write an error message w/o exiting, to a stream, in va_list form
 *
 * given:
 *      error_code      error code
 *      stream          open stream to use
 *      name            name of function issuing the error
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vfwerr(int error_code, FILE *stream, char const *name, char const *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `werr()` or `fwerr()` is recommended.





# werrp - write an error message with errno details w/o exiting

The `werrp()` function will write an error message that includes errno details to `stderr`.

The `fwerrp()` function will write an error message that includes errno details to a stream.

## Synopsis:

```c
/*
 * werrp - write an error message with errno details w/o exiting, to stderr
 *
 * given:
 *      error_code      error code
 *      name            name of function issuing the warning
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void werrp(int error_code, char const *name, char const *fmt, ...);

/*
 * fwerrp - write an error message with errno details w/o exiting to a stream
 *
 * given:
 *      error_code      error code
 *      stream          open stream to use
 *      name            name of function issuing the warning
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void fwerrp(int error_code, FILE *stream, char const *name, char const *fmt, ...);
```

The `werrp()` function writes the same thing as `err()` but does
NOT call `exit()` nor does it perform a bounds check on `error_code`.

The `fwerrp()` function writes the same thing as `ferr()` but does
NOT call `exit()` nor does it perform a bounds check on `error_code`.

The reason why the first argument is `error_code` (and not 'exitcode`)
is that this argument is simply an arbitrary integer that is
to be written after "`FATAL[`" and before the "`]'".

The `name` argument should be the name of the calling function,
or some string that helps identify the calling context.
For modern C compilers, the value `__func__` is the name of
the calling function, and thus is a good choice for a first argument.

The `fmt` argument, is a printf-style format.
If the format requires arguments, then such arguments
may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

The `werrp()` function is similar to `werr()` with the
additional writing of `errno` related information.
For this reason, this function may be used to
report on a failure of a library function or system call
that sets `errno`.

Because some previous library function or system call may
have set `errno` previously, it is recommended
that you set `errno` to 0 before calling a
given library function or system call.

For example:

```c
#include <errno.h>

...

errno = 0;              /* pre-clear errno for werrp() */
answerp = fopen(answers, "r");
if (answerp == NULL) {
    werrp(8, __func__, "cannot open answers file");
    fwerrp(8, socket, __func__, "cannot open answers file");
}
```

Special care is made in this function to restore the
caller's original value of `errno` on return.

## Examples:

```c
werrp(391581, __func__, "malloc() failed");

...

fwerrp(216091, stdout, __func__, "calloc() failed");
```

The error message will be preceded by `FATAL[#]:`
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

The following are possible output of the above example:

```
FATAL[391581]: function_name: malloc() failed: errno[12]: Cannot allocate memory

...

FATAL[216091]: function_name: calloc() failed: errno[14}; Bad address
```

## Output control:

The following global variables have an impact output:

```c
extern bool err_output_allowed;		/* false ==> disable error messages */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. err_output_allowed == false
```

## In case of error:


NOTE: No bounds check is made for the first argument (`error_code`).


## Alternative va_list interface:

The `vwerrp()` function is an alternative interface to `werrp()`
in `va_list` form.

```c
/*
 * vwerrp - write an error message with errno info w/o exiting, to stderr, in va_list form
 *
 * given:
 *      error_code      error code
 *      name            name of function issuing the warning
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vwerrp(int error_code, char const *name, char const *fmt, va_list ap);

/*
 * vfwerrp - write an error message with errno details w/o exiting to a stream, in va_list form
 *
 * given:
 *      error_code      error code
 *      stream          open stream to use
 *      name            name of function issuing the warning
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vfwerrp(int error_code, FILE *stream, char const *name, char const *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `werrp()` or `fwerrp()` is recommended.





# warn_or_err - write a warning or error message before exiting, depending on an arg

The `warn_or_err()` function will write either a warning message, or
an error message before exiting, depending on an arg to `stderr`.

The `fwarn_or_err()` function will write either a warning message, or
an error message before exiting, depending on an arg, to a stream.

## Synopsis:

```c
/*
 * warn_or_err - write a warning or error message before exiting, depending on an arg,
 *               to stderr
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the warning
 *      warning         true ==> write a warning, false ==> error message before exiting
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void void warn_or_err(int exitcode, const char *name, bool warning, const char *fmt, ...);

/*
 * fwarn_or_err - write a warning or error message before exiting, depending on an arg,
 *                to a stream
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the warning
 *      warning         true ==> write a warning, false ==> error message before exiting
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void fwarn_or_err(int exitcode, FILE *stream, const char *name, bool warning, const char *fmt, ...);
```

The `warn_or_err()` function writes the same thing as `warn()` if `warning == true`,
or writes the same thing as `err()` if `warning == false`, to `stderr`.

The `fwarn_or_err()` function writes the same thing as `warn()` if `warning == true`,
or writes the same thing as `err()` if `warning == false`, to a stream.

The `warning` argument controls if this function writes a warning (if `true`), or
writes error message before exiting (if `false`).  When `warning == true`
the `exitcode` is ignored and the function returns.  When `warning == false`
then the function will call `exit()` and the function does not return.

The `name` argument should be the name of the calling function,
or some string that helps identify the calling context.
For modern C compilers, the value `__func__` is the name of
the calling function, and thus is a good choice for a first argument.

The `fmt` argument, is a printf-style format.
If the format requires arguments, then such arguments
may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

Special care is made in this function to restore the
caller's original value of `errno` on return.

## Examples:

```c
warn_or_err(21701, __func__, true, "bad foobar: %s", message);

...

fwarn_or_err(23209, stdout. __func__, false, "unexpected whey: %s", cwhey);
```

When `warning == true`, the warning message will be preceded by `Warning:`,
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

When `warning == false`, the error message will be preceded by `FATAL[#]:`
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

The following is the possible output of the above example:

```
FATAL[21701]: function_name: bad foobar: fizzbin

...

Warning: function_name: unexpected whey: curds
```

## Output control:

### When warning == false:

The following global variables have an impact output:

```c
extern int verbosity_level;		/* maximum debug level for debug messages */
extern bool warn_output_allowed;	/* false ==> disable warning messages */
extern bool msg_warn_silent;		/* true ==> silence info & warnings if verbosity_level <= 0 */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. warn_output_allowed == false
2. msg_warn_silent == true && verbosity_level > 0
```

### When warning == true:

The following global variables have an impact output:

```c
extern bool err_output_allowed;		/* false ==> disable error messages */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. err_output_allowed == false
```

## In case of error:

### When warning == false:

If ` name == NULL`, then the following format string will be used:

```
((NULL name))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warn_or_err: name is NULL, forcing name to be: ((NULL name))
```

If `fmt == NULL`, then the following format string will be used:

```
((NULL fmt))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warn_or_err: fmt is NULL, forcing fmt to be: ((NULL fmt))
```

When `fmt == NULL`, any arguments following `fmt` are ignored.


### When warning == true:

If `exitcode < 0`, then the following exit code 255 will be used.

If this case the following warning, preceded by a newline, will be issued:

```
Warning: warn_or_err: exitcode < 0: #
```

Here `#` will be the original  `exitcode` value.

A second warning will be written in the form:

```
Warning: warn_or_err: forcing use of exit code: 255
```

If ` name == NULL`, then the following format string will be used:

```
((NULL name))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warn_or_err: name is NULL, forcing name to be: ((NULL name))
```

If `fmt == NULL`, then the following format string will be used:

```
((NULL fmt))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warn_or_err: fmt is NULL, forcing fmt to be: ((NULL fmt))
```

When `fmt == NULL`, any arguments following `fmt` are ignored.


## Alternative va_list interface:

The `vwarn_or_err()` function is an alternative interface to `warn_or_err()`
in `va_list` form.

The `vfwarn_or_err()` function is an alternative interface to `fwarn_or_err()`
in `va_list` form.

```c
/*
 * vwarn_or_err - write a warning or error message before exiting, depending on an arg,
 *                to stderr, in va_list form
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the warning
 *      warning         true ==> write a warning, false ==> error message before exiting
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vwarn_or_err(int exitcode, const char *name, bool warning, const char *fmt, va_list ap);

/*
 * vfwarn_or_err - write a warning or error message before exiting, depending on an arg,
 *                 to a stream, in va_list form
 *
 * given:
 *      exitcode        value to exit with
 *      stream          open stream to use
 *      name            name of function issuing the warning
 *      warning         true ==> write a warning, false ==> error message before exiting
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vfwarn_or_err(int exitcode, FILE *stream, const char *name, bool warning, const char *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `warn_or_err()` or `fwarn_or_err()` is recommended.





# warnp_or_errp - write a warning or error message before exiting, depending on an arg, w/errno details

The `warnp_or_errp()` function will write either a warning message, or
an error message before exiting, depending on an arg, w/errno details, to `stderr`.

The `fwarnp_or_errp()` function will write either a warning message, or
an error message before exiting, depending on an arg, w/errno details, to a stream.

## Synopsis:

```c
/*
 * warnp_or_errp - write a warning or error message before exiting, depending on an arg,
 *                 w/errno details, to stderr
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the warning
 *      warning         true ==> write a warning, false ==> error message before exiting
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void warnp_or_errp(int exitcode, const char *name, bool warning, const char *fmt, ...);

/*
 * fwarnp_or_errp - write a warning or error message before exiting, depending on an arg,
 *                  w/errno details, to a stream
 *
 * given:
 *      exitcode        value to exit with
 *      stream          open stream to use
 *      name            name of function issuing the warning
 *      warning         true ==> write a warning, false ==> error message before exiting
 *      fmt             format of the warning
 *      ...             optional format args
 */
extern void fwarnp_or_errp(int exitcode, FILE *stream, const char *name, bool warning, const char *fmt, ...);
```

The `warnp_or_errp()` function writes the same thing as `warnp()`
with errno details, if `warning == true`, or writes the same thing
as `err()` if `warning == false`, to `stderr`.

The `fwarnp_or_errp()` function writes the same thing as `warnp()`
with errno details, if `warning == true`, or writes the same thing
as `err()` if `warning == false`, to a stream.

The `warning` argument controls if this function writes a warning (if `true`), or
writes error message before exiting (if `false`).  When `warning == true`
the `exitcode` is ignored and the function returns.  When `warning == false`
then the function will call `exit()` and not return.  In error case,
errno details are also written.

The `name` argument should be the name of the calling function,
or some string that helps identify the calling context.
For modern C compilers, the value `__func__` is the name of
the calling function, and thus is a good choice for a first argument.

The `fmt` argument, is a printf-style format.
If the format requires arguments, then such arguments
may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

The `werrp()` function is similar to `werr()` with the
additional writing of `errno` related information.
For this reason, this function may be used to
report on a failure of a library function or system call
that sets `errno`.

Because some previous library function or system call may
have set `errno` previously, it is recommended
that you set `errno` to 0 before calling a
given library function or system call.

For example:

```c
#include <errno.h>

...

errno = 0;              /* pre-clear errno for werrp() */
answerp = fopen(answers, "r");
if (answerp == NULL) {
    werrp(8, __func__, "cannot open answers file");
    fwerrp(8, socket, __func__, "cannot open answers file");
}
```

Special care is made in this function to restore the
caller's original value of `errno` on return.

## Examples:

```c
werrp(391581, __func__, "malloc() failed");

...

fwerrp(216091, stdout, __func__, "calloc() failed");
```

When `warning == true`, the warning message will be preceded by `Warning:`,
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

When `warning == false`, the error message will be preceded by `FATAL[#]:`
followed by a space, followed by the value of the `name` string,
followed by a `:` followed by a space.

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

The following are possible output of the above example:

```
FATAL[391581]: function_name: malloc() failed: errno[12]: Cannot allocate memory

...

FATAL[216091]: function_name: calloc() failed: errno[14}; Bad address
```

## Output control:

### When warning == false:

The following global variables have an impact output:

```c
extern int verbosity_level;		/* maximum debug level for debug messages */
extern bool warn_output_allowed;	/* false ==> disable warning messages */
extern bool msg_warn_silent;		/* true ==> silence info & warnings if verbosity_level <= 0 */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. warn_output_allowed == false
2. msg_warn_silent == true && verbosity_level > 0
```

### When warning == true:

The following global variables have an impact output:

```c
extern bool err_output_allowed;		/* false ==> disable error messages */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. err_output_allowed == false
```

## In case of error:

### When warning == false:

If ` name == NULL`, then the following format string will be used:

```
((NULL name))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warnp_or_errp: name is NULL, forcing name to be: ((NULL name))
```

If `fmt == NULL`, then the following format string will be used:

```
((NULL fmt))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warnp_or_errp: fmt is NULL, forcing fmt to be: ((NULL fmt))
```

When `fmt == NULL`, any arguments following `fmt` are ignored.


### When warning == true:

If `exitcode < 0`, then the following exit code 255 will be used.

If this case the following warning, preceded by a newline, will be issued:

```
Warning: warnp_or_errp: exitcode < 0: #
```

Here `#` will be the original  `exitcode` value.

Another second warning will be written in the following form:

```
Warning: warnp_or_errp: forcing use of exit code: 255
```

If ` name == NULL`, then the following format string will be used:

```
((NULL name))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warnp_or_errp: name is NULL, forcing name to be: ((NULL name))
```

If `fmt == NULL`, then the following format string will be used:

```
((NULL fmt))
```

In this case the following warning, preceded by a newline, will be issued:

```
Warning: warnp_or_errp: fmt is NULL, forcing fmt to be: ((NULL fmt))
```

When `fmt == NULL`, any arguments following `fmt` are ignored.


## Alternative va_list interface:

The `vwarnp_or_errp()` function is an alternative interface to `warnp_or_errp()`
in `va_list` form.

The `vfwarnp_or_errp()` function is an alternative interface to `fwarnp_or_errp()`
in `va_list` form.

```c
/*
 * vwarnp_or_errp - write a warning or error message before exiting, depending on an arg,
 *                  w/errno details, to stderr, in va_list form
 *
 * given:
 *      exitcode        value to exit with
 *      name            name of function issuing the warning
 *      warning         true ==> write a warning, false ==> error message before exiting
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vwarnp_or_errp(int exitcode, const char *name, bool warning, const char *fmt, va_list ap);

/*
 * vfwarnp_or_errp - write a warning or error message before exiting, depending on an arg,
 *                   w/errno details, to a stream, in va_list form
 *
 * given:
 *      exitcode        value to exit with
 *      stream          open stream to use
 *      name            name of function issuing the warning
 *      warning         true ==> write a warning, false ==> error message before exiting
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vfwarnp_or_errp(int exitcode, FILE *stream, const char *name, bool warning, const char *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `wwarnp_or_errp()` or `fwarnp_or_errp()` is recommended.





# printf_usage - write command line usage and perhaps exit

The `printf_usage()` function will write an usage message, to `stderr`.

The `fprintf_usage()` function will write an usage message, to a stream.

## Synopsis:

```c
/*
 * printf_usage - write command line usage and perhaps exit, to stderr
 *
 * given:
 *      exitcode        - >= 0, exit with this code
 *                        < 0, just return
 *      fmt             - format of the usage message
 *      ...             - potential args for usage message
 */
extern void printf_usage(int exitcode, char const *fmt, ...);

/*
 * fprintf_usage - write command line usage and perhaps exit, to a stream
 *
 * given:
 *      exitcode        - >= 0, exit with this code
 *                        < 0, just return
 *      stream          - stream to write on
 *      fmt             - format of the usage message
 *      ...             - potential args for usage message
 */
extern void fprintf_usage(int exitcode, FILE *stream, char const *fmt, ...);
```

If `exitcode >= 0` then the function will call `exit()`.
If `exitcode < 0` then the function will return.

For your convenience, `dbg.h` defines the following constant:

```c
#define DO_NOT_EXIT (-1)        /* do not let the usage printing function exit */
```

When `exitcode` is `DO_NOT_EXIT`, the function will return.

The `fmt` argument, is a printf-style format.
If the format requires arguments, then such arguments
may be given after the `fmt`.

See `man 3 printf` for details on a printf format.

For modern C compilers, the agreement between any `%`
directives in `fmt`, and any arguments that may follow
is checked by the format attribute facility.
Thus having too many arguments, too few arguments,
or arguments of the wrong type will result in
compiler warnings.

Special care is made in this function to restore the
caller's original value of `errno` on return.

## Examples:

```c
#define VERSION_STRING "61.0 2021-10-28"

static char const * const usage =
"usage: %s [-h] [-v level]\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: 0)\n"
"\n"
"Version: %s";

...

printf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
printf_usage(3, stderr, usage, program, VERSION_STRING); /*ooo*/

...

fprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
fprintf_usage(3, stderr, usage, program, VERSION_STRING); /*ooo*/
```

A newline (i.e., `\n`) is written after the message,
and thus the `fmt` argument does **NOT** need to end in
a newline.

## Output control:

The following global variables have an impact output:

```c
extern bool usage_output_allowed;	/* false ==> disable usage messages */
```

Any of the following conditions, if true at the time the
function is called, will **disable** the writes.

```c
1. usage_output_allowed == false
```

## In case of error:

If `fmt == NULL`, then the following format string will be used:

```
((NULL fmt))
```

and following warning, preceded by a newline, will be issued:

```
Warning: printf_usage: fmt is NULL, forcing fmt to be: ((NULL fmt))
```

When `fmt == NULL`, any arguments following `fmt` are ignored.


## Alternative va_list interface:

The `vprintf_usage()` function is an alternative interface to `printf_usage()` in `va_list` form.

The `vfprintf_usage()` function is an alternative interface to `fprintf_usage()` in `va_list` form.

```c
/*
 * vprintf_usage - write command line usage and perhaps exit, to stderr, in va_list form
 *
 * given:
 *      exitcode        >= 0, exit with this code
 *                        < 0, just return
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vprintf_usage(int exitcode, char const *fmt, va_list ap);

/*
 * vfprintf_usage - write command line usage and perhaps exit, to a stream, in va_list form
 *
 * given:
 *      exitcode        >= 0, exit with this code
 *                        < 0, just return
 *      stream          stream to write on
 *      fmt             format of the warning
 *      ap              variable argument list
 */
extern void vfprintf_usage(int exitcode, FILE *stream, char const *fmt, va_list ap);
```

The state of the va_list `ap` is **NOT** modified by this
alternative interface function.

NOTE: The arguments referenced by the va_list `ap` argument
are **NOT** checked for consistency like they are using
the primary interface.  For this reason, the primary
interface `printf_usage()` or `fprintf_usage()` is recommended.





# global message control variables

The following global variables are declared in `dbg.c` (and thus
are linked in code via `dbg.o`):

```c
int verbosity_level = DBG_DEFAULT;      /* maximum debug level for debug messages */
bool msg_output_allowed = true;         /* false ==> disable informational messages */
bool dbg_output_allowed = true;         /* false ==> disable debug messages */
bool warn_output_allowed = true;        /* false ==> disable warning messages */
bool err_output_allowed = true;         /* false ==> disable error messages */
bool usage_output_allowed = true;       /* false ==> disable usage messages */
bool msg_warn_silent = false;           /* true ==> silence info & warnings if verbosity_level <= 0 */
```

The above C code also shows the defaults for these global control variables.
