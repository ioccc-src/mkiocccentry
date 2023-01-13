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


## Issue: warning: function 'foo' could be declared with attribute 'noreturn'
### Status: fixed
### Example

```c
dbg.c:3123:1: warning: function 'verr' could be declared with attribute 'noreturn' [-Wmissing-noreturn]
{
^
```

### Solution

Add the `noreturn` attribute, changing the declaration to be:


```c
extern void verr(int exitcode, char const *name, char const *fmt, va_list ap) \
           __attribute__((noreturn));
```

### See also

This was fixed in commit 5971613375469005ed252dc9bb2b63fe1ad1891d.


## Issue: warning: no previous extern declaration for non-static variable
### Status: fixed
### Examples

```c
./jparse.l:49:17: warning: no previous extern declaration for non-static variable 'bs' [-Wmissing-variable-declarations]
YY_BUFFER_STATE bs;
                ^
./jparse.l:49:1: note: declare 'static' if the variable is not intended to be used outside of this translation unit
YY_BUFFER_STATE bs;
^
```

### Solution

In this case we can make the variable static:


```c
static YY_BUFFER_STATE bs;
```

but in this case we must do a bit more to make sure this is okay. Instead of
just running `make all` we must do `make parser-o all`. This will force it to
not use the backup files which means the generated code must be regenerated. If
`make parser-o` fails (for instance because you do not have an up to date flex
version) then it will fail.

### Example

```c
./jparse.y:63:10: warning: no previous extern declaration for non-static variable 'num_errors' [-Wmissing-variable-declarations]
unsigned num_errors = 0;                /* > 0 number of errors encountered */
         ^
./jparse.y:63:1: note: declare 'static' if the variable is not intended to be used outside of this translation unit
unsigned num_errors = 0;                /* > 0 number of errors encountered */
^
```

### Solution

In this case the `num_errors` is no longer used so we can delete it. But here
too we must use `make parser-o`.


### Example

```c
./jparse.y:64:13: warning: no previous extern declaration for non-static variable 'filename' [-Wmissing-variable-declarations]
char const *filename = NULL;            /* if != NULL this is the filename we're parsing */
            ^
./jparse.y:64:1: note: declare 'static' if the variable is not intended to be used outside of this translation unit
char const *filename = NULL;            /* if != NULL this is the filename we're parsing */
^
```

### Solution

When we add `static` we discover something else: it's actually not used. Thus we
can also delete this variable and once more run `make parser-o`.

### A non-JSON example

```
In file included from txzchk.c:55:
./txzchk.h:77:6: warning: no previous extern declaration for non-static variable 'quiet' [-Wmissing-variable-declarations]
bool quiet = false;                             /* true ==> quiet mode */
     ^
./txzchk.h:77:1: note: declare 'static' if the variable is not intended to be used outside of this translation unit
bool quiet = false;                             /* true ==> quiet mode */
^
```

### Solution

We simply add `static` to the variable as it's not needed outside `txzchk`. The
same holds for `chkentry` and various others.


### A slightly more complicated example


```c
jnum_test.c:42:11: warning: no previous extern declaration for non-static variable 'test_count' [-Wmissing-variable-declarations]
int const test_count = TEST_COUNT;
          ^
jnum_test.c:42:1: note: declare 'static' if the variable is not intended to be used outside of this translation unit
int const test_count = TEST_COUNT;
^
jnum_test.c:44:7: warning: no previous extern declaration for non-static variable 'test_set' [-Wmissing-variable-declarations]
char *test_set[TEST_COUNT+1] = {
      ^
jnum_test.c:44:1: note: declare 'static' if the variable is not intended to be used outside of this translation unit
char *test_set[TEST_COUNT+1] = {
^
jnum_test.c:493:20: warning: no previous extern declaration for non-static variable 'test_result' [-Wmissing-variable-declarations]
struct json_number test_result[TEST_COUNT+1] = {
                   ^
jnum_test.c:493:1: note: declare 'static' if the variable is not intended to be used outside of this translation unit
struct json_number test_result[TEST_COUNT+1] = {
^
```

### Solution

At first glance it would appear we could just declare it static but if we do
this we will find that `jnum_chk` will fail to compile. The solution here is,
because these variables are declared in `jnum_chk.h`, `#include "jnum_chk.h"`
and run `make depend`. But this in turn will create another warning:


```c
./jnum_chk.h:95:13: warning: unused function 'chk_test' [-Wunused-function]
static bool chk_test(int testnum, struct json_number *item, struct json_number *test, size_t len, bool strict);
            ^
./jnum_chk.h:96:13: warning: unused function 'check_val' [-Wunused-function]
static void check_val(bool *testp, char const *type, int testnum, bool size_a, bool size_b, intmax_t val_a, intmax_t val_b);
            ^
./jnum_chk.h:97:13: warning: unused function 'check_uval' [-Wunused-function]
static void check_uval(bool *testp, char const *type, int testnum, bool size_a, bool size_b, uintmax_t val_a, uintmax_t val_b);
            ^
./jnum_chk.h:98:13: warning: unused function 'check_fval' [-Wunused-function]
static void check_fval(bool *testp, char const *type, int testnum, bool size_a, bool size_b,
            ^
./jnum_chk.h:100:13: warning: unused function 'usage' [-Wunused-function]
static void usage(int exitcode, char const *prog, char const *str, int expected, int argc) __attribute__((noreturn));
            ^
```

The solution is to add an include guard to the file. It would not be a good
solution to create a header file for `jnum_test` because even if this would not
be unnecessary overkill these variables are also used in jnum_chk.c as well.

Now running from the main directory `make clobber all` will work.

### See also

These were fixed in commit 89f8a4b9d9b6f3533b3577398dbd559f09e27ecc.


## Issue: warning: 'return' will never be executed
### Status: varies (see discussion below)
### Example

```c
txzchk.c:1040:2: warning: 'return' will never be executed [-Wunreachable-code-return]
        return;
        ^~~~~~
1 warning generated.
```

This was likely a typo though how I do not know. In any event it should have
been `not_reached()` which it now is.

### An example that we ignore


```c
dbg_example.c:87:12: warning: 'return' will never be executed [-Wunreachable-code-return]
    return 2; /* this return is never reached */
           ^
```

We ignore this as it serves as a demonstration to show that the exit code should
be 2.


### See also

Fixed in commit e217a44d892759a82b683f465db482df4ab790d8. Also note comit
ab5579e0473199fc676b37e37e68b032338caf5b which is the commit for the
`dbg_example.c` example above.
