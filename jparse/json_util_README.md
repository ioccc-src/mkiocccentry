# Command line utilities that read JSON

## Introduction

There is a general lack of JSON aware command line tools that allows
someone to obtain information from within a JSON document.  By
"obtaining information" we refer to the ability to extract data,
either as a whole, or in part, that contained within a [JSON
document](./json_README.md#json-document).

There exists a multiple JSON APIs (Application Program Interfaces)
for various programming languages: JavaScript, C, Java, Python,
Perl, Go, Rust, PHP, etc.   This [mkiocccentry
repo](https://https://github.com/ioccc-src/mkiocccentry) contains
a rich JSON parser API C program may use, as well as a semantical
JSON check interface for C.  However all those APIs require the
user to "program" in a specific language in order to do something
as simple as obtain a selective [JSON values](./json_README.md#json-value)
from a [JSON document](./json_README.md#json-document).

Please see [json_README.md](./json_README.md) for some
important **JSON terms** used in this document!


## Scope

In this document, we will concern ourselves with only [Valid
JSON](./json_README.md#valid-json) in the form of a [JSON
document](./json_README.md#json-document).  This is necessary because
we can only extract information with any degree of certainty from
[Valid JSON](./json_README.md#valid-json).

We consider command line utilities that read information from a [JSON
document](./json_README.md#json-document).  We are not considering
utilities that either create or modify JSON in this document.

If a command line utility is given an **invalid JSON document**,
the utility shall exit non-zero with an error message indicating
that the [JSON document](./json_README.md#json-document) was not
valid JSON.  Therefore are mainly concerns with reading various
information from an existing [JSON
document](./json_README.md#json-document) that contains [Valid
JSON](./json_README.md#valid-json).


## Common command line options

In this document, we assume that all utilities have the following options:

```
	-h		Print help and exit
	-V		Print version and exit
	-v level	Verbosity level (def: 0)
	-J level	JSON verbosity level (def: 0)
	-q		Quiet mode, do not print to stdout (def: print stuff to stdout)
```

Additional command line options will be added on a utility-by-utility basis.

In this document, we assume that all utilities have the following options:


## Common exit codes

```
Exit codes:
    0	all is OK and file is valid JSON
...
    2	-h and help string printed or -V and version string printed
    3	invalid command line, invalid option or option missing an argument
    4	file does not exist, not a file, or unable to read the file
    5	file contents is not valid JSON
...
 >=10	internal error
```

Additional exit codes will be added on a utility by utility basis.


## Command line utilities

We present 3 **Command line utilities**:

- [jfmt utility](#jfmt-utility)
- [jval utility](#val-utility)
- [jnamval utility](#namval-utility)


## jfmt utility

Given a [JSON document](./json_README.md#json-document), we need a
utility to print the JSON in a "canonical"-like well formatted JSON.
The goal of such formatting is to make is easier for a human to see
the JSON structure.

The **jfmt** utility will write the entire [JSON
document](./json_README.md#json-document) to a file, (defaulting
to standard output) in a "canonical"-like well formatted JSON.  The
output of the **jfmt** utility **MUST** be [Valid
JSON](./json_README.md#valid-json).

Consider the following `top_level_array2.json` [JSON document](./json_README.md#json-document):

```json
["string",42,3.1415,6.02214076e23,{"member_name":"member_value"},["str",31,2.718,2.99792458e8,{"mname":21701},[0,1,2,3,false,true,null,{"one_tooth":"ree"}],false,true,null]]
```

One way to format `top_level_array2.json` is the way that
[jsonlint.com](https://jsonlint.com) formats such a document.
Another is to put each JSON element on its own line:

```json
[
    "string",
    42,
    3.1415,
    6.02214076e23,
    {
	"member_name" : "member_value"
    },
    [
	"str",
	31,
	2.718,
	2.99792458e8,
	{
	    "mname" : 21701
	},
	[
	    0,
	    1,
	    2,
	    3,
	    false,
	    true,
	    null,
	    {
		"one_tooth" : "ree"
	    }
	],
	false,
	true,
	null
    ]
]
```

And we write "easier for a human to see the JSON structure"  because
some [JSON documents](./json_README.md#json-document) are large,
deep and complex enough to make an attempt to format them difficult
for a human to understand them.

Should one wish to have more than one style of output, then:

```
	-s style	use JSON output style (def: use common style
```

could be added as an option to the **jfmt** command line to
change from the default JSON style.


### jfmt command line options

In addition to the [Common command line options](#common-command-line-options),
we recommend the following command line options for **jfmt**:

```
	-L <num>[{t|s}]	Print JSON level followed writespace (def: don't print)
	-L tab		Alias for: '-L 1t'

			Trailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces.
			Not specifying 's' or 't' implies spaces.

	-I <num>{[t|s]}	When printing JSON syntax, indent levels (def: indent with 4 spaces)
	-I tab		Alias for '-I 1t'

			Trailing 't' implies indent with number of tabs whereas trailing 's' implies spaces.
			Not specifying 's' or 't' implies spaces.

	-l lvl		Print specific JSON levels (def: print at any level)

			If lvl is a number (e.g.: -l 3), level must == number.
			If lvl is a number followed by : (e.g. '-l 3:'), level must be >= number.
			If lvl is a : followed by a number (e.g. '-l :3'), level must be <= number.
			If lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range.


	-o ofile	Output formatted JSON to ofile (def: standard output, - ==> standard output)

	file.json	JSON file to parse (- ==> read from stdin)
```

It could be argued the default for `-I stuff` should be tab or 2 spaces or something else.
We do not want to indent too far for reasonable levels of JSON.  2 spaces might be too little
while tab and 8 spaces might be too much.

Additional command line options may be added.

For example, one might wish to add additional command line options
to vary the style of JSON formatting.  This could be done, however,
every one of those formatting style variations, increases the
complexity of testing.

We recommend command line simplicity be considered for the **jfmt** utility.

We also recommend that the `jfmt(1)` man page contain a warning that
users should not depend on the exact output of **jfmt** and that
future versions of the utility may change the way [Valid JSON](./json_README.md#valid-json)
is format JSON.


### jfmt exit codes

In addition to the [Common exit codes](#common-exit-codes),
we recommend the following exit codes for **jfmt**:

```
    1	error in writing to ofile
```

Other exit codes probably should fall under the "_internal error_" category
and should fall into the "_>= 10_" range.


### jfmt examples

In these examples, we write JSON in an inconsistent style.
Feel free to improve on this style.


#### jfmt example 0

```sh
jfmt jparse/test_jparse/test_JSON/good/42.json
```

should print on standard output (pretend we have 4 space indenting):

```json
{
    "foo" : [
        { "foo" : [
	    { "bar" : 42 }
            ]
        }
    ]
}
```

It is not clear how best to indent.  This might be more clear if
the `-L stuff` is used to also print the JSON tree levels.


#### jfmt example 1

```sh
jfmt jparse/test_jparse/test_JSON/good/dyfi_zip.geo.json
```

should print on standard output:

```json
{
    "features" : [
	{
	    "geometry" : {
		"coordinates" : [
		    -149.7791,
		    61.1512],
		"type" : "Point"
	    },
	    "type" : "Feature",
	    "properties" : {
		"population": 0,
		"nresp": 1,
		"name": "99507<br>0.330",
		"cdi": 2,
		"dist": 195
	    }
	}
    ],
    "type" : "FeatureCollection"
}
```

Again, it is unclear how best to indent things such as "[{ .. }]" as
these compound [JSON values](./json_README.md#json-value) are at different
JSON tree levels.

It would be better if the formatted lines of **jfmt** corresponded,
line by line` with the use of `-L stuff` (printing JSON tree levels).


#### jfmt example 2

```sh
jfmt -L 4s jparse/test_jparse/test_JSON/good/foo.json
```

This is a useful way for the user to better understand JSON levels.
Assuming we don't make a mistake in reading the debugging output from `jparse -J 3`
we guess that this might print:

```
1    {
2       "foo" : "bar",
2	"foo" : [
3	    "bar",
3	    "bar",
3	    "bar"
2        ]
1    }
```


## jval utility

Given a [JSON document](./json_README.md#json-document), we need a
utility that prints [JSON values](./json_README.md#json-value) that
match or do not match certain values or value ranges.

While **jval** command, without any "_args_" on the command line will
cause the printing of any [JSON values](./json_README.md#json-value)
that are not otherwise restricted by a `-t type` or `-l lvl` option.

The presence of "_args_" creates match conditions that may further
restrict which [JSON values](./json_README.md#json-value) are printed.
When using the **jval** command with "_args_", match criteria is setup
to further select which [JSON values](./json_README.md#json-value)
are printed.


### jval command line options

In addition to the [Common command line options](#common-command-line-options),
we recommend the following command line options for **jval**:

```
	-L <num>[{t|s}]	Print JSON level followed writespace (def: don't print)
	-L tab		Alias for: '-L 1t'

			Trailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces.
			Not specifying 's' or 't' implies spaces.

	-t type		Match only JSON values of a given comma-separated type (def: simple, or if -n, num)

				int	integer values
				float	floating point values
				exp	exponential notation values
				num	alias for 'int,float,exp'
				bool	boolean values
				str	string values
				null	null values
				simple	alias for 'int,float,exp,bool,str,null'

	-l lvl		Print values at specific JSON levels (def: print at any level)

			If lvl is a number (e.g.: -l 3), level must == number.
			If lvl is a number followed by : (e.g. '-l 3:'), level must be >= number.
			If lvl is a : followed by a number (e.g. '-l :3'), level must be <= number.
			If lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range.

	-Q		Print JSON strings surrounded by double quotes (def: do not)
	-D		print JSON strings as decoded strings (def: print JSON strings as encoded strings)
	-d		Match the JSON decoded values (def: match as given in the JSON document)
	-i		invert match, print values that do not match (def: print values that do match)
	-s		match subsrrings (def: match entire values)
	-f		fold case (def: case matters when matching strings)
	-c		print total match count only (def: print values)

			Using -c with either -C or -L is an error.

	-C		print match count followed matched value (def: print values)

			Using -C with either -c or -L is an error.

	-g		Match using grep-like extended regular expressions (def: match values)

			To match from the beginning, start name_arg with '^'.
			To match to the end, end name_arg with '$'.
			Using -g with any of: -s, -n or -R is an error.

	-n		match numerical values (def: match strings in file.json)

			The _arg_ command line arguments must be numerical and match the -t type in effect.
			When used with -i, only numerical values that do not match are printed.
			Using -n with any of: -s, -f or -g is an error.

	-R interval	Match a range using a range type (def: gele)

			    gtlt	range match >  1st value <  2nd value
			    gelt	range match >= 1st value <  2nd value
			    gtle	range match >  1st value <= 2nd value
			    gele	range match >= 1st value <= 2nd value

			    lt		range match <  1st value    2nd value must be .
			    le		range match <= 1st value    2nd value must be .
			    gt		range match >  1st value    2nd value must be .
			    ge		range match >= 1st value    2nd value must be .

			    open	alias for gtlt
			    closed	alias for gele

			The number of _arg_s must be an even number >= 2.
			Using -R with either -s or -g is an error.

	file.json	JSON file to parse (- ==> read from stdin)
	[arg]		match argument(s)
```

By default, matching is performed on how the value is represented in the JSON document.

Because of the complexity of trying to describe a complex JSON value on
the command line, let alone the problem of describing complex value ranges,
the `-t type` does **NOT** include complex JSON types.

Using `-R` __WITHOUT__ `-n` implies that string value ranges are
compared.  Use string comparison functions such as `strcmp(3)`, and
`strcasecmp(3)` to example string value ranges.

If no "_arg_" is given on the command line then any value may be
considered, unless `-i` inverts the match in which case no value
is to be considered.

Without `-n`, matching is string based.  Such strings matches are
as given in the JSON document, unless `-d` is also given in which
case the JSON decoded string is matched.

Numerical matching (use of `-n`) solves the problem of trying to
match various ways to encode a JSON number.  Thus, matching with the
numerical value of _200_ would match JSON numbers such as:

```
200
200.0
2e2
200.000
2.0e2
0.2e3
```

Numerical matching is limited to the ability of the hardware to
encode a numerical value.  An attempt to try and match the numerical
value of:

```
1e100000000000000000000000000000000
```

is likely to fail.


### jval exit codes

In addition to the [Common exit codes](#common-exit-codes),
we recommend the following exit codes for **jval**:

```
    1	no matchs found
...
    3	invalid command line, invalid option, option missing argument, invalid number of args
...
    6	-n used and unable to convert arg into a numerical value
    7	invalid range args
```

Other exit codes probably should fall under the "_internal error_" category
and should fall into the "_>= 10_" range.

[JSON values](./json_README.md#json-value) are printed in order
that they are encoded in the JSON parse tree.


### jval examples

Level values are a guess made while looking at the output of `jparse -J 3`.
The actual level values depend on the actual JSON parse tree.


#### jval example 0

```sh
jval jparse/test_jparse/test_JSON/good/42.json
```

As there are no _arg_s, all values match:

```
foo
foo
bar
42
```


#### jval example 1

```sh
jval -L 4s jparse/test_jparse/test_JSON/good/42.json
```

should print with JSON levels:

```
2    foo
5    foo
8    bar
8    42
```

**IMPORTANT NOTE**: We observe that:

```sh
jparse -J 3 jparse/test_jparse/test_JSON/good/42.json
```

prints a different level for this [JSON member](./json_README.md#json-member):


```json
{ "bar" : 42 }
```

The same level value of _8_ is the same for both the [JSON name](./json_README.md#json-name)
of _bar_ and the value of _42_:

```
JSON tree[3]:	lvl: 8	type: JTYPE_STRING	len{qPa}: 3	value:	"bar"
JSON tree[3]:	lvl: 8	type: JTYPE_NUMBER	val+8: 42
```

This is correct.  Remember that in JSON member structure, there is a convenience
pointer to the [JSON name](./json_README.md#json-name).  This this is **NOT**
the level _bar_:

```
JSON tree[3]:	lvl: 7	type: JTYPE_MEMBER	name: "bar"		<<-- NOT THIS LEVEL !!!
```

The `jval(1)` will need to help explain JSON tree levels.  We highly recommend that
the command:

```sh
jfmt -L tab foo.json
```

be used to help the user understand JSON tree levels when using `-l lvl`.


#### jval example 2

```sh
jval -Q jparse/test_jparse/test_JSON/good/top_level_array.json
```

Here, `-Q` prints [JSON strings](./json_README.md#json-string) within double quotes:

```
"hi"
21701
2.718
6.023e23
true
false
null
1
2
3
"hello"
true
false
6.67430e-11
4
5
6
null
"a"
"b"
```

The use of double quotes helps distinguish, for example, a JSON
`null` from a [JSON string](./json_README.md#json-string) of "null".

It is important to note that what **jval** prints is the actual characters
from the [JSON document](./json_README.md#json-document), not some
machine interpreted value.  For example, even though in JSON, `2.0` could be
represented as in integer, what **jval** must print is the literal
characters from the [JSON document](./json_README.md#json-document).
This can be done by printing from structure elements such as `as_str`,
or if a decoded string is needed, from the `str` element and `-D` was given.

Consider the following [JSON document](./json_README.md#json-document), `hi-hello.json`:

```json
"hi\nhello"
```

The command:

```sh
jval -Q hi-hello.json
```

should print the [JSON encoded string](./json_README.md#json-encoded-string):

```
"hi\nhello"
```

Whereas the command:

```sh
jval -Q -D hi-hello.json
```

should print the [JSON decoded string](./json_README.md#json-decoded-string):

```
"hi
hello"
```


#### jval example 3

To print only integers:

```sh
jval -Q -t int jparse/test_jparse/test_JSON/good/top_level_array.json
```

will print:

```
21701
1
2
3
4
5
6
```

Care must be taken as to which [JSON values](./json_README.md#json-value) are printed.
Just because a value as an integer value does not mean that value should be printed
under the `-t int` option.

The `struct json_number` elements `is_floating` and `is_e_notation` need to be considered.

Consider this JSON:

```json
[31, 31.0, 3.1e1]
```

The command:

```sh
jval -Q -t int jparse/test_jparse/test_JSON/good/top_level_array.json
```

will only print:

```
31
```

The command:

```sh
jval -Q -t float jparse/test_jparse/test_JSON/good/top_level_array.json
```

will only print:

```
31.0
```

And the command:

```sh
jval -Q -t exp jparse/test_jparse/test_JSON/good/top_level_array.json
```

will only print:

```
3.1e1
```


#### jval example 4

The presence of "_args_" creates match conditions that may further
restrict which [JSON values](./json_README.md#json-value) are printed.
The default match criteria is when the  "_arg_" is an exact match:

```sh
jval parse/test_jparse/test_JSON/good/h2g2.json 42
```

This should print 3 lines:

```
42
42
42
```

When we add `-Q`:

```sh
jval -Q parse/test_jparse/test_JSON/good/h2g2.json 42
```


we see why there are 3 lines:

```
42
"42"
42
```

The 2nd _"42"_, with `-Q`, shows that it is a [JSON
string](./json_README.md#json-string) and not an integer.  This
points out that, by default, "_arg_" matching is based on the strings
in the [JSON document](./json_README.md#json-document).

Combined with `-t type`, allows for further restriction:

```sh
jval -Q -t str parse/test_jparse/test_JSON/good/h2g2.json 42
```

produces:

```
"42"
```


#### jval example 5

Multiple "_arg_", by default, setup multiple ways for a match.
One may think of multiple "_args_" are matching in an "OR"-like way.

```sh
jval -Q -t str parse/test_jparse/test_JSON/good/h2g2.json 42 number
```

produces:

```
"number"
"42"
```

Or for example:

```sh
jval -Q -t str parse/test_jparse/test_JSON/good/h2g2.json 42 number name
```

produces:

```
"number"
"42"
"name"
"name"
"name"
```


#### jval example 6

The **jval** offers 2 ways to count mactches instead of simply printing
 [JSON values](./json_README.md#json-value) that match:

 ```sh
 jval -Q -t str -c parse/test_jparse/test_JSON/good/h2g2.json 42 number name
 ```

 produces:

```
5
```

because there are 5 total matches.

With `-C`, the match count per "_arg_" may be printed:

 ```sh
 jval -Q -t str -C parse/test_jparse/test_JSON/good/h2g2.json 42 number name foo
 ```

produces:

```
1	"number"
1	"42"
3	"name"
```

Observe that the "_arg_" of "_foo_" had 0 matches and was not listed.

Without any "_args_" using `-c` just counts [JSON values](./json_README.md#json-value):

```sh
jval -c jparse/test_jparse/test_JSON/good/foo.json
```

produces:

```
6
```

Using `-C` with no "_args_" is a special case.  The [JSON
value](./json_README.md#json-value) is printed after a tab.  So
with `-C` and no  "_args_" we may see the counts of the individual
[JSON values](./json_README.md#json-value):

```sh
jval -C -Q jparse/test_jparse/test_JSON/good/foo.json
```

```
2	"foo"
4	"bar"
```

Observe in this case, the `-Q` is useful because what is bring
printed are counts with [JSON values](./json_README.md#json-value),
not "_args_".

Consider this case:

```sh
jval -C -Q jparse/test_jparse/test_JSON/good/h2g2.json 42
```

We have two types of "42" to consider, and thus different match counts:

```
2	42
1	"42"
```

Here the `-Q` is useful to distinguish between the integer and the
[JSON string](./json_README.md#json-string) being matched.


#### jval example 7

The `-i` option inverts matches, so **jval** would print
[JSON values](./json_README.md#json-value) that do not match an "_arg_":

```sh
jval -i jparse/test_jparse/test_JSON/good/foo.json bar
```

produces:

```
foo
foo
```

Use of `-i` without any "_args_" does nothing:

```sh
jval -i jparse/test_jparse/test_JSON/good/foo.json
```

produces:

```
```

The `-t type` restricts the inversion of the match.  So:

```sh
jval -i -t int jparse/test_jparse/test_JSON/good/top_level_array.json 23209 1 2 3
```

produces:

```
4
5
6
```

because those are the [JSON values](./json_README.md#json-value) of type "int"
that is **NOT** match one of the "_args_".


#### jval example 8

Be default, "_arg_" must match the entire [JSON value](./json_README.md#json-value).
With `-s`, substring matches are allowed:

```sh
json -s jparse/test_jparse/test_JSON/good/top_level_array.json e
```

produces:

```
6.023e23
true
false
hello
true
false
6.67430e-11
```

Each of those [JSON values](./json_README.md#json-value) have an "_e_" in them.

Had the type been restricted to strings:

```sh
json -s -t str jparse/test_jparse/test_JSON/good/top_level_array.json e
```

only the [JSON strings](./json_README.md#json-string) with an "_e_" would have been printed:

```
hello
```


#### jval example 9

By default, **jval** matches [JSON strings](./json_README.md#json-string)
based on their [JSON encoded string](./json_README.md#json-encoded-string)
values.  With `-d`, such matching is on the [JSON decoded
string](./json_README.md#json-decoded-string) value:

```sh
jval -Q -d jparse/test_jparse/test_JSON/good/hi-hello.json 'hi
hello'
```

This produces:

```
"hi
hello"
```


#### jval example 10

By default, **jval** matches [JSON strings](./json_README.md#json-string)
where the case of the string matters:

```sh
jval -Q -s jparse/test_jparse/test_JSON/good/hello-world.json Hello
```

will print:

```
"Hello, World!
"
```

However:

```sh
jval -Q -s jparse/test_jparse/test_JSON/good/hello-world.json hello
```

produces nothing:

```
```

The use of `-f` causes string matching to be case independent:

```sh
jval -Q -s jparse/test_jparse/test_JSON/good/h2g2.json killers
```

produces:

```
"killers"
"Killers"
```


#### jval example 11

The `-g` option causes **jval** to use regular expressions to match:

```sh
jval -g party.json 'B.*Bag'
```

produces:

```
Bag End, Bagshot Row, Hobbiton, Westfarthing, the Shire, Middle-earth
Bilbo Baggins
```

Observe that the regular expression, by default, matches sub-strings.

With `-f` the regular expression matching becomes case independent:

```sh
jval -g -f party.json 'b.*bag'
```

produces:

```
Bag End, Bagshot Row, Hobbiton, Westfarthing, the Shire, Middle-earth
Bilbo Baggins
```

The use of regular expression anchors may force the match of the
entire [JSON value](./json_README.md#json-value):

```sh
jval -g party.json '^P.*t$'
```

produces just:

```
Proudfoot
Proudfeet
```


#### jval example 12

By default, the matching of [JSON values](./json_README.md#json-value)
by "_args_" is done on the actual text of the [JSON
document](./json_README.md#json-document).  Or when `-d`, [JSON
decoded strings](./json_README.md#json-decoded-string) are considered.

The `-n` option changes **jval** to consider the numerical value
of the [JSON value](./json_README.md#json-value) instead.

Recall that:

```sh
jval -Q parse/test_jparse/test_JSON/good/h2g2.json 42
```

produces:

```
42
"42"
42
```

With `-n` the numerical value is considered:

```sh
jval -n parse/test_jparse/test_JSON/good/h2g2.json 42
```

produces:

```
42
42
```

Observe that the [JSON string](./json_README.md#json-string) containing
42 is not a match because it does not have a numerical value.

And for example:

```sh
jval -n top_level_array.json 2 3 5
```

produces:

```
2
3
5
```

because only those numerical [JSON values](./json_README.md#json-value) match.

Only values that can be represented by a machine can be matched.

The following command:

```sh
jval -n googolplex.json 1e10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```

will print an error message to standard error and exit with a value of 6.

However when comparing strings (without `-n`):

```sh
jval googolplex.json 1e10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```

produces:

```
1e10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```

It is an error to give a non-numerical "_arg_" when using `-n`.

Not giving any "_arg_" with `-n` is equivalent to using `-t num` and matching all numerical values.
On the other hand, using `-n -t bool`, for example, will match nothing because only numerical values
are matched with `-n`.

The numerical value may be further restricted by using `-t type`.  Observe that:

```sh
jval two-array.json 2
```

produces:

```
2
2.0
2e0
2.0e0
2.00
2.00e0
0.2e1
20e-1
20.0e-1
```

Whereas:

```sh
jval -t int two-array.json 2
```

produces:

```
2
```

And:

```sh
jval -t float two-array.json 2
```

produces:

```
2.0
2.00
```

And whereas:

```sh
jval -t int,float two-array.json 2
```


produces:

```
2
2.0
2.00
```


#### jval example 13

The `-R interval` option causes **jval** to treat pairs of "_args_" as a range, matching only
of the given [JSON value](./json_README.md#json-value) fall within a given range,
With `-R` the number of  "_args_" on the command line must be an even number >= 2.

There are 8 different types of range intervals supported.

These 4 intervals support bounded ranges:

```
gtlt	range match >  1st value <  2nd value
gelt	range match >= 1st value <  2nd value
gtle	range match >  1st value <= 2nd value
gele	range match >= 1st value <= 2nd value
```

These 4 intervals support half-open ranges:

```
lt		range match <  1st value    2nd value must be .
le		range match <= 1st value    2nd value must be .
gt		range match >  1st value    2nd value must be .
ge		range match >= 1st value    2nd value must be .
```

The last 4: "lt", "le", "gt", and "ge" cause the 2nd pair to be ignored
except that the corresponding "_arg"_ must be "_._" (dot).

Without `-m`, the range comparison is string based according to `strcmp(3)` or
`strcasecmp(3)` if `-f` is also used.  For example:

```sh
jval -R gele JSON_misfeature_number_7.json a c
```

The strings that are >= "a" and <= "c" are printed:

```
avalue
bvalue
```

Multiple ranges are allowed:

```sh
jval -R gtlt JSON_misfeature_number_7.json a c m o
```

produces:

```
avalue
cvalue
name1
name2
```

Half-open ranges are allowed:

```sh
jval -R lt JSON_misfeature_number_7.json name2 .
```

Here the "_._" is required.  This produces:

```
name1
avalue
bvalue
```


#### jval example 14

The `-R interval` with `-n` checks for numerical ranges of numeric
[JSON values](./json_README.md#json-value):

```sh
jval -R closed -m jparse/test_jparse/test_JSON/good/top_level_array.json 1 3
```

produces:

```
1
2
2.718
3
```

And:

```sh
jval -R lt jparse/test_jparse/test_JSON/good/top_level_array.json 1 .
```

produces:

```
6.67430e-11
```


## jnamval

The **jnamval** utility operates only on [JSON
members](./json_README.md#json-member).  This utility is concerned
only with [JSON member names](./json_README.md#json-member-name)
and their associated [JSON member values](./json_README.md#json-member-value) only.


### jnamval command line options

The options for the **jnamval** utility are very similar to the
[jval utility](#jval-utility).

In addition to the [Common command line options](#common-command-line-options),
and the [jval command line options](#jval-command-line-options),
we recommend the following command line options for **jnamval**:

```
        -t type         Match only JSON values of a given comma-separated type (def: simple, or if -n, num)
	...

			These types are only valid with -N, -M or if there are no args after file.json:

				member		JSON members
				object		JSON objects
				array		JSON arrays
				compound	alias for 'member,object,array'
				any		alias for 'simple,compound'

	-N		March based on JSON member names (def: match JSON member values)
	-H		March name heirarchies (def: with -H match any JSON member name, else JSON member value)

			Use of -H implies -N.
			Using -H with any of: -n or -R is an error.

	-p parts	Parts of a JSON member to print (def: print JSON member values)

				n		print JSON member names
				v		print JSON member values
				b		print JSON member names and values
				j		print JSON member om JSON syntax

				name		alias for n
				value		alais for v
				both		alais for b
				json		alais for j
```

The `-t type` applies to the type of the [JSON member
value](./json_README.md#json-member-value) in a given [JSON
members](./json_README.md#json-member).  Because of the difficulty
in expressing [JSON member values](./json_README.md#json-member-value)
for types member, object, array, compound, and any, those types
can only be used if there are no "_args_" on the command line.

By default, **jnamval** prints [JSON member
values](./json_README.md#json-member-value), just like the [jval
utility](#jval-utility) only prints [JSON
values](./json_README.md#json-value).  With the use of `-p parts`
[JSON member names](./json_README.md#json-member-name) and/or [JSON
member values](./json_README.md#json-member-value) may be printed.
Furthermore, with `-p j` or `-p json`, the JSON syntax may be printed
like the [jfmt utility](#jfmt-utility) does.

By default, **jnamval** uses "_args_" to search the values, ranges,
numerical values, and numerical ranges.  Multiple "_args_" on the
command line, by default, behave the same way as the [jval
utility](#jval-utility).  With `-H`, the meaning of more than "_arg_"
changes to searching for a given [JSON
members](./json_README.md#json-member) hierarchy.



### jnamval exit codes

The exit codes for **jnamval** (that are < 10) are identical to
the [jval exit codes](#jval-exit-codes).


### jnamval examples

Level values are a guess made while looking at the output of `jparse -J 3`.
The actual level values depend on the actual JSON parse tree.

We presume that the same concepts described in [jval examples](#jval-examples)
apply directly or indirectly to **jnamval**.  We will focus on command line
options unique to **jnamval** as well as some variations from how
[jval](#jval) operates.


#### jnamval example 0

Unlike [jval](#jval), **jnamval** operates only on [JSON
members](./json_README.md#json-member):

```sh
jnamval jparse/test_jparse/test_JSON/good/42.json
```

Because `-t type` defaults to simple, and `-p parts` defaults
to [JSON member values](./json_README.md#json-member-value),
this command produces:

```
42
```

Only 1 of the 3 [JSON members](./json_README.md#json-member)
had a [JSON member value](./json_README.md#json-member-value)
that was of a simple type.


#### jnamval example 1

The `-p type` option can change what **jnamval** prints from the
default [JSON member value](./json_README.md#json-member-value).

For example:

```sh
jnamval -t json jparse/test_jparse/test_JSON/good/42.json
```

produces:

```
{ "bar" : 42 }
```

We recommend that when printing with `-t json`, the resulting JSON
be formatted to fit in a single line with spaces between the JSON
syntax tokens.

And for example:

```sh
jnamval -t name -Q jparse/test_jparse/test_JSON/good/42.json
```

produces:

```
"bar"
```

And for example:

```sh
jnamval -t both -Q jparse/test_jparse/test_JSON/good/42.json
```

produces:

```
"bar" : 42
```

Observe that the `-t both` prints the [JSON member
name](./json_README.md#json-member-name) followed by a space,
followed by a "_:_} (ASCII colon), followed by the [JSON member
value](./json_README.md#json-member-value).


#### jnamval example 2

With **jnamval**, the `-t type` includes compound types such as:

- member for [JSON members](./json_README.md#json-member)
- object for [JSON objects](.json_README.md#json-object)
- array for [JSON array](.json_README.md#json-array)
- any for any part of JSON

We recommend that when printing with compound types, the resulting
JSON be formatted to fit in a single line with spaces between the
JSON syntax tokens.

Consider this command:

```sh
jnamval -t array jparse/test_jparse/test_JSON/good/42.json
```

This should produce:

```
"foo" : [ { "foo" : [ { "bar" : 42 } ] } ]
"foo" : [ { "bar" : 42 } ]
```


#### jnamval example 3

While **jnamval** is able to search for [JSON member
values](./json_README.md#json-member-value) in the same
way as [jval](#jval), the use of "_args_" as match criteria
is limited to only JSON simple types.

The use of `-t type` where type is a compound type is allowed ONLY
when there are no "_args_" after the "_file.json_" OR when `-N` is used.

When `-N` is used, we search for [JSON member
names](./json_README.md#json-member-name), which are [JSON
strings](./json_README.md#json-string), so any search "_args_" to
the [JSON member names](./json_README.md#json-member-name).  This
allows for a  `-t type` where type is a compound type with "_args_"
after the "_file.json_".

For example:

```sh
jnamval -N -t array jparse/test_jparse/test_JSON/good/party.json hobbit
```

produces:

```
[ { "name" : "Bilbo Baggins", "age" : 111, "inheritance" : false }, { "name": "Frodo Baggins", "age": 33, "inheritance" : true } ]
```

If one wanted to pretty print the results, pipe the output into [jfmt](#jfmt):

```sh
jnamval -N -t array jparse/test_jparse/test_JSON/good/party.json hobbit | jfmt -
```

produces:

```json
[
    {
	"name" : "Bilbo Baggins",
	"age" : 111,
	"inheritance" : false
    },
    {
	"name" : "Frodo Baggins",
	"age" : 33,
	"inheritance" : true
    }
]
```


#### jnamval example 4

With `-N` and "_args_" one may search for certain [JSON member
values](./json_README.md#json-member-value) that match and print
their corresponding [JSON member names](./json_README.md#json-member-name):

```sh
jnamval -N -s -i -Q jparse/test_jparse/test_JSON/good/party.json gandalf
```

produces:

```
"name"
"alias"
```

With `-N -t any` and no "_args_", one may simply print the list of [JSON member
values](./json_README.md#json-member-value):

```sh
jnamval -N -t any -Q jparse/test_jparse/test_JSON/good/dyfi_geo_10km.geo.json
```

produces:

```
"features"
"geometry"
"coordinates"
"type"
"type"
"properties"
"stddev"
"nresp"
"name"
"cdi"
"dist"
"type"
```


#### jnamval example 5

The `-H` option (which implies `-N`) causes "_arg_" to only match a hierarchy
of [JSON members](./json_README.md#json-member).

Consider this command:

```sh
jnamval -H jparse/test_jparse/test_JSON/good/dyfi_geo_10km.geo.json features properties stddev
```

A search for a [JSON members](./json_README.md#json-member) with a
[JSON member name](./json_README.md#json-member-name) of "_features_" is performed.

For every [JSON member name](./json_README.md#json-member-name) of "_features_"
that is found, a search under the given JSON node is then made for
[JSON members](./json_README.md#json-member) with a
[JSON member name](./json_README.md#json-member-name) "_properties_".

Then for every [JSON member name](./json_README.md#json-member-name) of "_properties_"
that is found, a search under the given JSON node is then made for a
[JSON members](./json_README.md#json-member) with a
[JSON member name](./json_README.md#json-member-name) of "_stddev_" is name.

Since "_stddev_" is the last "_arg_", then the [JSON member
value](./json_README.md#json-member-value) is examined to determine
if it is of the correct type.  As `-t type` defaults in this case to simple,
the command produces:

```
0.33
```

The check of the [JSON member value](./json_README.md#json-member-value)
is only performed on the last "_arg_".  All prior "_args_", to be
in a hierarchy, had to be some form on compound type otherwise their
value would not be able to contain lower level [JSON
member](./json_README.md#json-member).

While this command:

```sh
jnamval -Q jparse/test_jparse/test_JSON/good/party.json guest_list name
```

will produce:

```
"Bilbo Baggins"
"Frodo Baggins"
"Gandalf"
"Meriadoc Brandybuck"
"Peregrin Took"
"Gollum"
"Proudfoot"
```

Consider this command:

```sh
jnamval -H -Q jparse/test_jparse/test_JSON/good/party.json guest_list name
```

This will only produce:

```
"Gandalf"
"Meriadoc Brandybuck"
"Peregrin Took"
"Gollum"
"Proudfoot"
"Sackville-Baggins"
```

This is because with `-H`, the "name" was restricted to those being under "guest_list".
