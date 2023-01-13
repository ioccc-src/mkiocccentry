# Static analysis of code

In this file we document how to correctly modify the Makefile (or rather its
variables) in case (for example) you wish to enable additional warnings or make
other kinds of changes. We will document fixes to some of the warnings enabled
by `clang -Weverything` as well as make some general notes. We will also
document warnings that can be safely ignored and explain why.

Although it might seem like such a document and title would discuss lints we
don't for reasons we care not to get into. :-)


## Making modifications to the Makefiles: an example

Rather than changing the Makefiles themselves we provide a way to override
variables in each directory's Makefile. To explain how to do this we provide
this example which we'll actually use for much of this document.

Let's say you wish to change the variables `CFLAGS` (or rather the warning flags
which are included in the `CFLAGS` as `WARN_FLAGS`) and `CC` so that you can use
`-Weverything` of `clang` you can do the following:


```sh
$ cat > makefile.local << "EOF"
CC= clang
WARN_FLAGS += -Weverything -Wno-error
EOF
```

You can do this in any directory with a Makefile to override the warning flags
and the compiler which we'll use for much of this document. You can copy this
file to the other subdirectories or you can make a symlink e.g. something like:


```sh
$ cd soup
$ ln -s ../makefile.local .
```

After this the `soup/` subdirectory will also use the modifications (we do not
pass the CFLAGS to other Makefiles though maybe we should).

Note: we throw in the option `-Wno-error` to prevent the compiler from aborting
prematurely. This way we can get a list of all warnings like:


```sh
$ make all > warnings.log 2>&1
```


Now one can look at the `warnings.log` file and analyse it all at once without
having to scroll up and down.

In a moment we'll use this file.


### A note about ASAN

If you want to use ASAN make sure to set this environment var:

	ASAN_OPTIONS= "detect_stack_use_after_return=1"

and also modify the CFLAGS a bit as well:

	CFLAGS+= -O0 -g -pedantic ${WARN_FLAGS} -fsanitize=address -fno-omit-frame-pointer

To do this you may use the feature above with the `makefile.local`.  Note that
there might be some redundancy in your `CFLAGS` but we specify these explicitly
as some of the flags will eventually change.

### IMPORTANT REMINDER: test your changes

Please remember to ALWAYS run `make clobber all test` after you make a change:
just like all scientists test there ideas so too must you when making a change
to the code.


## Analysing `clang -Weverything`

Here we'll analyse the warnings (over some days) that we generated in the
section about modifying the Makefiles.

Tip: if there are a lot of warnings you might want to, when going through the
warnings log, disable the `-Weverything` and selectively enable the class of
warnings. For instance you might do instead `-Wsign-conversion` to help
identify the sign conversion warnings.



## Issue: padding struct 'struct foo' with N bytes to align
### Status: ignore
### Example:

```c
./json_parse.h:129:13: warning: padding struct 'struct json_number' with 1 byte to align 'as_int32' [-Wpadded]
    int32_t as_int32;           /* JSON integer value in int32_t form, if int32_sized == true */
```


### Solution

This warning can be safely ignored so we can add to the `WARN_FLAGS`
`-Wno-padded` to prevent us from being bothered by it further.


## Issue: macro is not used
### Status: varies (see discussion below)
### Examples:

```c
jparse.tab.c:230:11: warning: macro is not used [-Wunused-macros]
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
          ^
```

### Solution

When it comes to flex and bison generated code one can ignore this warning
without any problems. If however it's somewhere else it might or might not want
to be fixed.

There are two other places where it can be ignored:


```c
jstrdecode.c:40:9: warning: macro is not used [-Wunused-macros]
#define REQUIRED_ARGS (0)       /* number of required arguments on the command line */

jstrencode.c:40:9: warning: macro is not used [-Wunused-macros]
#define REQUIRED_ARGS (0)       /* number of required arguments on the command line */
```

These can be ignored because we use this macro in all tools to be consistent. In
the case of `jstrdecode` and `jstrencode if there are no args after the options
we read from `stdin`. If however there are args we process those as strings.
Thus there is no safe way to use this macro but nevertheless we want to be
consistent.

At this time (12 January 2023) all the places that this warning is triggered can
be ignored but this does not necessarily mean that it always should be.


## Issue: warning: 'break' will never be executed
### Status: ignore
### Example

```c
jstrencode.c:181:6: warning: 'break' will never be executed [-Wunreachable-code-break]
            break;
            ^~~~~
```

### Solution

If you look at the code you will see:

```c
        case 't':               /* -t - validate the contents of the jenc[] table */
            printf("%s: Beginning jencchk test on code JSON encode/decode functions ...\n", program);
            jencchk();
            printf("%s: ... passed JSON encode/decode test\n", program);
            exit(0); /*ooo*/
            not_reached();
            break;
```


We can see indeed that the `break` will never be reached but there's a problem
with changing this: some code might be changed which could break the code if the
break statement was removed. A good example has actually happened in the past.

Choosing to ignore this very warning a long while back I (@xexyl) wrote some
code (that was obsoleted by the json parser) that had a return prior to a break.
Later on this actually happened as the return was removed and moved to the end
of the file. But if there was another case in the switch block it would have
fallen through and caused an error.


## Issue: warning: no previous prototype for function
### Status: fixed
### Examples

The first example is due to a function that is no longer used.

```c
util.c:2965:1: warning: no previous prototype for function 'find_matching_quote' [-Wmissing-prototypes]
find_matching_quote(char *q)
^
util.c:2964:1: note: declare 'static' if the function is not intended to be used outside of this translation unit
char *
^
static 
```

### Solution

In this case we can safely get rid of this function.

### Another example

```c
entry_time.c:67:1: warning: no previous prototype for function 'timestr_eq_tstamp' [-Wmissing-prototypes]
timestr_eq_tstamp(char const *timestr, time_t timestamp)
^
entry_time.c:66:1: note: declare 'static' if the function is not intended to be used outside of this translation unit
bool
^
static
```

### Solution

In this case the prototype was missing so we add it to the approprate file and
recompile and, assuming no compilation errors, run `make clobber all test` to
make sure everything is okay.

### See also

This was fixed in commit cd991fae57ad4ac358c899ec2967aca8f2f2f224.


## Issue: warning: format string is not a string literal
### Status: ignored (see discussion below)
### Example

```c
dbg.c:220:28: warning: format string is not a string literal [-Wformat-nonliteral]
    ret = vfprintf(stream, fmt, ap);
                           ^~~
```


### Solution

While normally a cause for concern, in this case the format string is passed as
a literal to the debug function (or actually `fmsg_write()` from the `msg()`
functions) where in turn the variable triggers the warning.

In our case there is no place in the code that this is triggered that is a
problem as it's all like the above example.
