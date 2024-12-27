# Information on the jparse C library

In this document we describe the `jparse` C library so you can get an idea of
how to use it in a C application that needs to parse and process valid JSON
documents, whether in a file (on disk or stdin, or even over a network) or a
string.

For information on the `jparse` repo, see the [jparse repo
README.md](https://github.com/xexyl/jparse/blob/master/README.md).

For information on the `jparse` utilities see
[jparse_utils_README.md](https://github.com/xexyl/jparse/blob/master/jparse_utils_README.md).

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
- [jparse&lparen;3&rparen; details](#jparse-details)
    - [struct json: the core struct](#struct-json)
    - [enum item_type: the different JSON types](#enum-item-type)
    - [JSON structs](#json-structs)
        - [`struct json_number`: JSON numbers](#struct-json-number)
        - [`struct json_string`: JSON strings](#struct-json-string)
        - [`struct json_boolean`: JSON booleans](#struct-json-boolean)
        - [`struct json_null`: JSON `null`](#struct-json-null)
        - [`struct json_member`: JSON members](#struct-json-member)
        - [`struct json_object`: JSON objects](#struct-json-object)
        - [`struct json_array`: JSON arrays](#struct-json-array)
        - [`struct json_elements`: JSON elements](#struct-json-elements)
- [JSON debug output](#json-debug-output)
    - [JTYPE_NUMBER: JSON number debug output](#jtype-number-debug-output)
    - [JTYPE_STRING: JSON string debug output](#jtype-string-debug-output)
    - [JTYPE_BOOL: JSON boolean debug output](#jtype-boolean-debug-output)
    - [JTYPE_NULL: JSON null debug output](#jtype-null-debug-output)
    - [JTYPE_MEMBER: JSON member debug output](#jtype-member-debug-output)
    - [JTYPE_OBJECT: JSON object debug output](#jtype-object-debug-output)
    - [JTYPE_ARRAY: JSON array debug output](#jtype-array-debug-output)
    - [JTYPE_ELEMENTS: JSON elements debug output](#jtype-elements-debug-output)
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


<div id="linking-jparse"></div>

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

<div id="jparse-details"></div>

# jparse(3) details

Here we will give a few details on the core of the `jparse` library, though we
recommend you check the header files and source files noted below, for more
thorough details.

For much more details, please see the header file
[json_parse.h](https://github.com/xexyl/jparse/blob/master/json_parse.h) and the
source file
[json_parse.c](https://github.com/xexyl/jparse/blob/master/json_parse.c).


<div id="struct-json"></div>

## struct json: the core struct

In brief, for now at least, the core struct `json`, is defined as:

```c
/*
 * struct json - item for the JSON parse tree
 *
 * For the parse tree we have this struct and its associated union.
 */
struct json
{
    enum item_type type;		/* union item specifier */
    union json_union {
	struct json_number number;	/* JTYPE_NUMBER - value is number (integer or floating point) */
	struct json_string string;	/* JTYPE_STRING - value is a string */
	struct json_boolean boolean;	/* JTYPE_BOOL - value is a JSON boolean */
	struct json_null null;		/* JTYPE_NULL - value is a JSON null value */
	struct json_member member;	/* JTYPE_MEMBER - value is a JSON member: name : value */
	struct json_object object;	/* JTYPE_OBJECT - value is a JSON { members } */
	struct json_array array;	/* JTYPE_ARRAY - value is a JSON [ elements ] */
	struct json_elements elements;	/* JTYPE_ELEMENTS - zero or more JSON values */
    } item;

    /*
     * JSON parse tree links
     */
    struct json *parent;	/* parent node in the JSON parse tree, or NULL if tree root or unlinked */
};
```

Please read the comments for more details. Below we will include the enum and
give a brief list of each struct.


<div id="enum-item-type"></div>

## enum item_type: the different JSON types

The enum `item_type` corresponds to structs in the form of `json_foo` (such as
`struct json_string` for `JTYPE_STRING`) that are contained in the `struct json`
listed above. Below we will list the structs, with the purpose of each.
For more details please see the comments in
[json_parse.h](https://github.com/xexyl/jparse/blob/master/json_parse.h).

The `enum item_type` is as follows:

```c
/*
 * item_type - JSON item type - an enum for each union item member in struct json
 */
enum item_type {
    JTYPE_UNSET	    = 0,    /* JSON item has not been set - must be the value 0 */
    JTYPE_NUMBER,	    /* JSON item is a number - see struct json_number */
    JTYPE_STRING,	    /* JSON item is a string - see struct json_string */
    JTYPE_BOOL,		    /* JSON item is a boolean - see struct json_boolean */
    JTYPE_NULL,		    /* JSON item is a null - see struct json_null */
    JTYPE_MEMBER,	    /* JSON item is a member */
    JTYPE_OBJECT,	    /* JSON item is a { members } */
    JTYPE_ARRAY,	    /* JSON item is a [ elements ] */
    JTYPE_ELEMENTS,	    /* JSON item for building a JSON array */
};
```

<div id="json-structs"></div>

## JSON structs

The following is a list of each structure for the various JSON
types. These structures each correspond to a `JTYPE_` of the enum `item_type`,
found above, defined in
[json_parse.h](https://github.com/xexyl/jparse/blob/master/json_parse.h).

Every struct has the booleans `parsed` and `converted`, indicating,
respectively, whether or not the JSON was parsed successfully and if the data
could be converted. An example where it might be parsed successfully but it
could not be converted is if the number is so large it does not fit in a C type.

Please be advised that when `parsed == false` the fields in the struct might be
invalid. Thus you must check the boolean of parsed and only use values if
`parsed == true`. We provide the following convenience macros:

```c
#define VALID_JSON_NODE(item) ((item) != NULL && (((item)->parsed == true) || ((item)->converted == true)))
#define PARSED_JSON_NODE(item) ((item) != NULL && ((item)->parsed == true))
#define CONVERTED_PARSED_JSON_NODE(item) ((item) != NULL && (((item)->parsed == true) && ((item)->converted == true)))
#define CONVERTED_JSON_NODE(item) ((item) != NULL && (item)->converted == true)
```

to help you here.

For now (at least), there are no details about the structures; that might come
later but you are invited to look at the header file
[json_parse.h](https://github.com/xexyl/jparse/blob/master/json_parse.h) as well
as how they are used in the various source files, especially
[json_parse.c](https://github.com/xexyl/jparse/blob/master/json_parse.c), for
more details.

The structures are documented next.

<div id="struct-json-number"></div>

### `struct json_number`: JSON numbers

This structure is for JSON numbers, where a JSON number is:

```
({JSON_INTEGER}|{JSON_INTEGER}{JSON_FRACTION}|{JSON_INTEGER}{JSON_FRACTION}{JSON_EXPONENT})
```

where `{JSON_INTEGER}` is the regex:

```regex
-?([1-9][0-9]*|0)
```

and `{JSON_FRACTION}` is the regex:

```regex
\.[0-9]+
```

and `{JSON_EXPONENT}` is the regex:

```regex
[Ee][-+]?[0-9]+
```

This structure is quite complicated but we believe that the comments for each
member will help you in determining how the structure is used and how you can
use it as well:


```c
char *as_str;		/* allocated copy of the original allocated JSON number, NUL terminated */
char *first;		/* first whitespace character */

size_t as_str_len;		/* length of as_str */
size_t number_len;		/* length of JSON number, w/o leading or trailing whitespace and NUL bytes */

bool is_negative;		/* true ==> value < 0 */

bool is_floating;		/* true ==> as_str had a '.' in it such as 1.234, false ==> no '.' found */
bool is_e_notation;		/* true ==> e notation used such as 1e10, false ==> no e notation found */
bool is_integer;		/* true ==> converted to some integer type below */

/* integer values */

bool int8_sized;		/* true ==> converted JSON integer to C int8_t */
int8_t as_int8;		/* JSON integer value in int8_t form, if int8_sized == true */

bool uint8_sized;		/* true ==> converted JSON integer to C uint8_t */
uint8_t as_uint8;		/* JSON integer value in uint8_t form, if uint8_sized == true */

bool int16_sized;		/* true ==> converted JSON integer to C int16_t */
int16_t as_int16;		/* JSON integer value in int16_t form, if int16_sized == true */

bool uint16_sized;		/* true ==> converted JSON integer to C uint16_t */
uint16_t as_uint16;		/* JSON integer value in uint16_t form, if uint16_sized == true */

bool int32_sized;		/* true ==> converted JSON integer to C int32_t */
int32_t as_int32;		/* JSON integer value in int32_t form, if int32_sized == true */

bool uint32_sized;		/* true ==> converted JSON integer to C uint32_t */
uint32_t as_uint32;		/* JSON integer value in uint32_t form, if uint32_sized == true */

bool int64_sized;		/* true ==> converted JSON integer to C int64_t */
int64_t as_int64;		/* JSON integer value in int64_t form, if int64_sized == true */

bool uint64_sized;		/* true ==> converted JSON integer to C uint64_t */
uint64_t as_uint64;		/* JSON integer value in uint64_t form, if uint64_sized == true */

bool int_sized;		/* true ==> converted JSON integer to C int */
int as_int;			/* JSON integer value in int form, if int_sized == true */

bool uint_sized;		/* true ==> converted JSON integer to C unsigned int */
unsigned int as_uint;	/* JSON integer value in unsigned int form, if uint_sized == true */

bool long_sized;		/* true ==> converted JSON integer to C long */
long as_long;		/* JSON integer value in long form, if long_sized == true */

bool ulong_sized;		/* true ==> converted JSON integer to C unsigned long */
unsigned long as_ulong;	/* JSON integer value in unsigned long form, if long_sized == true */

bool longlong_sized;	/* true ==> converted JSON integer to C long long */
long long as_longlong;	/* JSON integer value in long long form, if longlong_sized longlong_sized == true */

bool ulonglong_sized;	/* true ==> converted JSON integer to C unsigned long long */
unsigned long long as_ulonglong;	/* JSON integer value in unsigned long long form, if ulonglong_sized a== true */

bool ssize_sized;		/* true ==> converted JSON integer to C ssize_t */
ssize_t as_ssize;		/* JSON integer value in ssize_t form, if ssize_sized == true */

bool size_sized;		/* true ==> converted JSON integer to C size_t */
size_t as_size;		/* JSON integer value in size_t form, if size_sized == true */

bool off_sized;		/* true ==> converted JSON integer to C off_t */
off_t as_off;		/* JSON integer value in off_t form, if off_sized == true */

bool maxint_sized;		/* true ==> converted JSON integer to C maxint_t */
intmax_t as_maxint;		/* JSON integer value in as_maxint form, if maxint_sized == true */

bool umaxint_sized;		/* true ==> converted JSON integer to C umaxint_t */
uintmax_t as_umaxint;	/* JSON integer value in as_umaxint form, if umaxint_sized == true */

/* floating point values */

bool float_sized;		/* true ==> converted JSON float to C float */
float as_float;		/* JSON floating point value in float form, if float_sized  == true */
bool as_float_int;		/* if float_sized == true, true ==> as_float is an integer */

bool double_sized;		/* true ==> converted JSON float to C double */
double as_double;		/* JSON floating point value in double form, if double_sized  == true */
bool as_double_int;		/* if double_sized == true, true ==> as_double is an integer */

bool longdouble_sized;	/* true ==> converted JSON float to C long double */
long double as_longdouble;	/* JSON floating point value in long double form, if longdouble_sized  == true */
bool as_longdouble_int;	/* if longdouble_sized == true, true ==> as_longdouble is an integer */
```

<div id="struct-json-string"></div>

### `struct json_string`: JSON strings

This struct is for JSON strings which we have as the regex:

```regex
\"([^"\x01-\x1f]|\\\")*\"
```

Now some might claim this is not quite correct for JSON strings but this is to
simplify the lexer. The parse routines do impose further restrictions. This is
done in the complicated function `json_decode()` found in
[json_parse.c](https://github.com/xexyl/jparse/blob/master/json_parse.c), which
calls the function `decode_json_string()`; `json_decode()` is called by the
function `json_conv_string()`; a simplified interface to `json_decode()` is
`json_decode_str()`.

For more details on a valid JSON string, please see the section [JSON encoded
string in json_README.md](json_README.md#json-encoded-string).

The `struct json_string` is also quite complicated but we believe the comments
should be enough to understand the struct and its members:


```c
char *as_str;		/* allocated non-decoded JSON string, NUL terminated (perhaps sans JSON '"'s) */
char *str;			/* allocated decoded JSON string, NUL terminated */

size_t as_str_len;		/* length of as_str, not including final NUL */
size_t str_len;		/* length of str, not including final NUL */

bool quote;			/* The original JSON string included surrounding '"'s */

bool same;			/* true => as_str same as str, JSON decoding not required */

bool slash;			/* true ==> / was found after decoding */
bool posix_safe;		/* true ==> all chars are POSIX portable safe plus + and maybe / after decoding */
bool first_alphanum;	/* true ==> first char is alphanumeric after decoding */
bool upper;			/* true ==> UPPER case chars found after decoding */
```

<div id="codepoints"></div>

#### On encoding/decoding UTF-8/UTF-16 codepoints:

A note on encoding/decoding UTF-8/UTF-16 codepoints is in order. The
`json_decode()` function decodes a JSON string according to the so-called JSON
spec and that function has the **encoding** of UTF-8/UTF-16 codepoints. The
`json_encode()` function encodes a JSON string according to the so-called JSON
spec and the function does **NOT** encode UTF-8/UTF-16 codepoints, but it
should. We might call this a mis-feature that should be corrected, but for now
this is not the case. The [documentation on
jstrdecode](jparse_utils_README.md#jstrdecode) and the [documentation on
jstrencode](jparse_utils_README.md#jstrencode) give more details on this.


<div id="struct-json-boolean"></div>

### `struct json_boolean`: JSON booleans

This struct is, as the name implies, for JSON booleans: that is `false` or
`true` (**NOT** `"false"` or `"true"`!). This struct is very simple.

The most important member is probably `value`, which is `true` or `false`, but
like with others, we do have:

```c
char *as_str;		/* allocated JSON boolean string, NUL terminated */
size_t as_str_len;		/* length of as_str */
```

<div id="struct-json-null"></div>

### `struct json_null`: JSON `null`

This struct is for the JSON `null` (**NOT** `"null"`) and it is also quite
simple. The `value` is actually a NULL (void) pointer. The struct also does have
the usual:

```c
char *as_str;		/* allocated JSON null string, NUL terminated */
size_t as_str_len;		/* length of as_str */
```

<div id="struct-json-member"></div>

### `struct json_member`: JSON members

Now we get to something more complicated, in that JSON members have more than
one component.

A JSON member is in the form of:

```json
"name" : value
```

<div id="json-value"></div>

where `name` is a valid JSON string and where `value` is any JSON value such as:

* JSON object
* JSON array
* JSON string
* JSON number
* JSON boolean
* JSON null

The struct has the following members, as a convenience for accessing JSON member
name information:

 * The `name_as_str` is a pointer copy of `name->item.string.as_str` pointer.
 * The `name_str` is a pointer copy of `name->item.string.str` pointer.
 * The `name_as_str_len` is a copy of `name->item.string.as_str_len`.
 * The `name_str_len` is a copy of `name->item.string.str_len`.

The `name` and `value` are each a `struct json *`.


<div id="struct-json-object"></div>

### `struct json_object`: JSON objects

A JSON object is one of:

```
{ }
```

or

```
{ members }
```

The pointer to the i-th JSON member in the JSON object, if `i < len`, is:

```c
foo.set[i-1]
```

The number of JSON members in the JSON object is, as hinted at above, the
`intmax_t` called `len`. The member `set`, a `struct json **`, is the set of
JSON members in the object. The member `s`, which is a `struct dyn_array *`, is
the dynamic array managed storage for the JSON object.

<div id="struct-json-array"></div>

### `struct json_array`: JSON arrays

**PLEASE NOTE**: this struct **MUST** be the same as `struct json_elements`!

A JSON array is of the form:

```
[ ]
```

or

```
[ values ]
```

Like a JSON object, the pointer to the i-th JSON value in the JSON array, if `i
< len`, is:

```c
foo.set[i-1]
```

And just like a JSON object, the length is an `intmax_t` called `len`, the set
of JSON values belonging to the array is a `struct json **` called `set` and the
dynamic array managed storage of the array is a `struct dyn_array *` called `s`.

<div id="struct-json-elements"></div>

### `struct json_elements`: JSON elements

**IMPORTANT NOTE**: the `struct json_elements` **MUST** be identical to `struct
json_array`!

A JSON element is 0 or more [JSON values](#json-value).

Given that the struct is identical to the `struct json_array`, we refer you to
the details on the [struct json_array](#struct-json-array).


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



<div id="json-debug-output"></div>

# JSON debug output

In order to help with seeing what JSON data was parsed, the library has debug
output code. In programs that use the `jparse(3)` library, there exists the `-J
level` option. Although some of it might be clear, when it comes to numbers and
strings, there are many flags that we describe below. Other types we also
describe, though there isn't much to those.

The printing code is in a number of functions (the static `fpr*()` functions) that
are called by `vjson_fprint()` found in
[json_util.c](https://github.com/xexyl/jparse/blob/master/json_util.c).

We will try and simplify this as much as possible, without sacrificing details
(as much as possible) but this might not be that easy to do.

In all cases, if you see the `p` flag it means the data was parsed successfully.
If you see the `c` flag it means the data was converted. It is possible for JSON
to be parsed successfully but not be converted, for instance if the number is so
big it does not fit in a C type. These flags will be shown in the examples
below.

In all cases if you see text in the form of `JSON tree[%d]` it is the debug
level that is not a forced level; otherwise, if it is forced it'll just be `JSON
tree node`.

In all cases the `lvl` indicates the depth.

In all cases the `type JTYPE_FOO` indicates the JSON type, for instance
`JTYPE_NUMBER` for numbers, `JTYPE_STRING` for strings, `JTYPE_NULL` for `null`
etc.

In the case of JSON types like arrays that have other members/objects, the debug
output will show those as well.

The other things depend on the type of JSON data.

<div id="jtype-number-debug-output"></div>

## JTYPE_NUMBER: JSON number debug output

There are a quite a few flags that indicate certain things when parsing JSON
numbers. The general form of the debug output is:

The output in general is in the form of:

```
JSON tree[3]:	lvl: 0	type: JTYPE_NUMBER	{p,c:-I8163264illlSSomffiddildldi}: value:	-5
```

where `type: JTYPE_NUMBER` indicates it is a number of some kind, and the flags
identify details about the number and the `value` is the value of the number,
assuming it was converted.

The flags are described below and are associated with the value.
The example above are the flags for a `JTYPE_NUMBER`, although there are others,
all of which will be described below; the code that prints these flags comes
from the `fprnumber()` function.

The flags after the parsed and converted flags (as described above), if present,
are in the following order:

0. `-`: a negative number.

1. `F`: the number is a floating point number.

2. `E`: the number is in E notation (e.g. `1e10`).

3. `I`: the number was converted to some integer type (see below).

4. `8`: the number was converted to `int8_t`.

5. `u8`: the number was converted to `uint8_t`.

6. `16`: the number was converted to `int16_t`.

7. `u16`: the number was converted to `uint16_t`.

8. `32`: the number was converted to `int32_t`.

9. `u32`: the number was converted to `uint32_t`.

10. `64`: the number was converted to `int64_t`.

11. `u64`: the number was converted to `uint64_t`.

12. `i`: the number was converted to `signed int`.

13. `ui`: the number was converted to `unsigned int` (cannot be < 0).

14. `l`: the number was converted to `long int`.

15. `ul`: the number was converted to `unsigned long int`.

16. `ll`: the number was converted to `long long int`.

17. `ull`: the number was converted to `unsigned long long int`.

18. `SS`: the number was converted to `ssize_t`.

19. `s`: the number was converted to `size_t`.

20. `o`: the number was converted to `off_t`.

21. `m`: the number was converted to `intmax_t`.

22. `um`: the number was converted to `uintmax_t`.

23. `f`: the number was converted to `float`.

24. `d`: the number was converted to `double`.

25. `di`: if `double_sized` (flag `d`, JSON float converted to `double`) set,
then `as_double` is an integer.

26. `ld`: the number was converted to `long double`.

27. `ldi`: if `longdouble_sized` (flag `ld`, JSON float converted to `long
double)` set, then `as_longdouble` is an integer.

<div id="jtype-string-debug-output"></div>

## JTYPE_STRING: JSON string debug output

There are a number of flags that indicate certain things when parsing JSON
strings. The general form of the debug output is:

```
JSON tree[5]:	lvl: 0	type: JTYPE_STRING	len{p,c:qPa}: 3	value:	"foo"
```

where `type: JTYPE_STRING` indicates it is a JSON string, `len{...}: 3`
indicates a length of 3 with the flags described below, and the value is the
string `"foo"`.

The code that prints this comes from the function `fprstring()`.

The following flags, if present, mean the below, in the following order, and are
for `JTYPE_STRING`:


0. `q`: the original string JSON string included surrounding double quotes (`"`s)

1. `=`: the encoded string is identical to the decoded string (JSON decoding was
not required).

2. `/`: `/` was found after decoding.

3. `p`: all chars are POSIX portable safe plus `+` and maybe `/` after decoding

4. `a`: first char is alphanumeric after decoding

5. `U`: UPPER case chars found after decoding.


<div id="jtype-bool-debug-output"></div>

## JTYPE_BOOL: JSON boolean debug output

For JSON booleans, it is quite simple, with the form of either:

```
JSON tree[3]:	lvl: 0	type: JTYPE_BOOL	{pc}value: false
```

for `false` or:

```
JSON tree[3]:	lvl: 0	type: JTYPE_BOOL	{pc}value: true
```

for `true` where `JTYPE_BOOL` is for JSON booleans.

The code that prints this comes from the function `fprboolean()`.

<div id="jtype-null-debug-output"></div>

## JTYPE_NULL: JSON null debug output

For JSON null (`null`, not a string), it looks like:

```
JSON tree[3]:	lvl: 0	type: JTYPE_NULL	{pc}: value: null
```

where `JTYPE_NULL` indicates a `null` object.

This is done in `fprnull()`.


<div id="jtype-member-debug-output"></div>

## JTYPE_MEMBER: JSON member debug output

The general form for debug output of JSON members is:

```
JSON tree[5]:   lvl: 2  type: JTYPE_MEMBER      {pc}name: "foo"
```

where `type: JTYPE_MEMBER` indicates that it is a JSON member and `name:
"foo"` indicates the name is `"foo"`.

This is done in `fprmember()`.

<div id="jtype-object-debug-output"></div>

## JTYPE_OBJECT: JSON object debug output

The general form for a JSON object is:

```
JSON tree[5]:   lvl: 0  type: JTYPE_OBJECT      {pc}len: 2
```

where `JTYPE_OBJECT` indicates it is a JSON object and `len` indicates the
length, or the number of JSON members in the object. If 0, the object has 0
members.


<div id="jtype-array-debug-output"></div>

## JTYPE_ARRAY: JSON array debug output

For JSON arrays the general form is:

```
JSON tree[5]:   lvl: 0  type: JTYPE_ARRAY       {pc}len: 1
```

where `JTYPE_ARRAY` indicates it is a JSON array and the `len` indicates the
number of JSON values in the JSON array. If 0, the array is empty.

**NOTE**: the structure `json_array` **MUST** be the same as the structure
`json_elements` because the function `json_parse_array()` converts by just
changing the JSON item type.

<div id="jtype-elements-debug-output"></div>

## JTYPE_ELEMENTS: JSON elements debug output

For JSON elements the form is:

```
JSON tree[5]:   lvl: 0  type: JTYPE_ELEMENTS    {pc}len: 4
```

where `JTYPE_ELEMENTS` indicates JSON elements and the `len` is the  number of
JSON values in the JSON elements. If 0, it is empty (no values).

**NOTE**: the structure, `json_elements` **MUST** be identical to the structure
`json_array` because the function `json_parse_array()` converts by changing the
JSON item type.



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
