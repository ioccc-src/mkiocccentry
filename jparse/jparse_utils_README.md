# Command line utilities that parse, read and process JSON in different ways

We provide a number of utilities that work with JSON, or which perform actions
according to the so-called JSON spec, which we describe below. These utilities
are described below. Other tools are in the pipeline and when they are
developed this file will document them as well.

If you want more information on the `jparse` repo, see the [jparse repo
README.md](https://github.com/xexyl/jparse/blob/master/README.md).

For information on the `jparse` library, see
[jparse_library_README.md](https://github.com/xexyl/jparse/blob/master/jparse_library_README.md).

For information on the testing suite see
[test_jparse/README.md](https://github.com/xexyl/jparse/blob/master/test_jparse/README.md).

We also do recommend that you read the
[json_README.md](https://github.com/xexyl/jparse/blob/master/json_README.md)
document to better understand the JSON terms used in this repo.

# Table of Contents

- [Common command line options](#common)
- [jparse stand-alone tool](#jparse-tool)
- [jstrdecode: a tool to JSON decode command line strings](#jstrdecode)
- [jstrencode: a tool to convert JSON decoded strings into normal strings](#jstrencode)

<div id="common"></div>

# Common command line options

All the tools provide a few options that do the same thing, regardless of the
tool used. These are as follows:

## Help/usage: `-h`

This shows the help/usage string of the tool and then exits.


## Verbosity/debug level: `-v level`

This sets the verbosity level. This is NOT the same thing as the JSON debug
level which is only used in tools that actually use the `jparse` library
(parsing JSON).

This option can provide valuable information but it is not normally needed.

## Quiet mode: `-q`

This sets quiet mode, which will silence some of the output of the tool. In
particular, it silences `msg()`, `warn()` and `warnp()` in the debug library, if
`-v 0` (the default verbosity level). The default is loud. :-)

### Print version string: `-V`

This option prints the version of the tool, the jparse UTF-8 library and the
jparse library version and then it exits.


### Terminate parsing of options: `--`

If you need to pass to the program a string that starts with a `-`, then you can
terminate the parsing of options by specifying `--`.


<div id="jparse-tool"></div>

# jparse stand-alone tool:

As a tool by itself `jparse` takes a block of memory from either a file (stdin
or a text file) or a string (if the `-s` option is used) and parses it as JSON,
exiting 0 if it is valid JSON or non-zero if something went wrong, for instance
if it is invalid JSON.

<div id="jparse-synopsis"></div>

## jparse synopsis:


```sh
jparse [-h] [-v level] [-J level] [-q] [-V] [-s] -- arg
```

The `-J` option increases the verbosity of the JSON parser.

If `-s` is passed the arg is expected to be a string; otherwise it is expected
to be a file.


<div id="jparse-examples"></div>

## jparse examples:

### Parse the JSON string `{ "test_mode" : false }`:

```sh
jparse -s '{ "test_mode" : false }'
```

With `-v 1` it would report `valid JSON`.


### Parse input from stdin (send EOF, usually ctrl-d or ^D, to parse):

```sh
jparse -
[]
^D
```

### Parse just a negative number:

```sh
jparse -s -- -5
```

### Parse party.json file:

```sh
jparse party.json
```


### Attempt to parse invalid JSON:


```sh
jparse -s "\n"
```

This might show something like:

```
syntax error in file - at line 1 column 1: <\>
Warning: jparse: JSON parse tree is NULL
ERROR[1]: jparse: invalid JSON
```

This would work, however:

```sh
jparse -s '"\n"'
```


<div id="jparse-exit-codes"></div>

## jparse exit codes

If the JSON is valid the exit status of `jparse` is 0. Different non-zero values
are for different error conditions, or help or version string printed.



<div id="jstrdecode"></div>

# jstrdecode: a tool to JSON decode command line strings

This tool decodes command line strings according to the so-called [JSON data
interchange syntax -
ECMA-404](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/).
It will also decode UTF-8 or UTF-16 codepoints, in the JSON form of of `\uxxxx`
or `\uxxxx\uxxxx`, to their proper Unicode symbol, although if your system
cannot show it, or it is invalid, you might see a character indicating this.


<div id="jstrdecode-synopsis"></div>

## jstrdecode synopsis


```sh
 jstrdecode [-h] [-v level] [-q] [-V] [-t] [-n] [-N] [-Q] [-e] [-d] [-E level] [arg ...]
```


Unlike the `jparse` utility, no JSON parsing functions are called, so there is
no `-J level` option.

Use of `-Q` will enclose output in double quotes whereas the use of `-e`
will enclose each decoded string with escaped double quotes. Use of `-Q` and
`-e` together will surround the entire output with unescaped quotes and each
decoded arg will be surrounded with escaped (backslashed) quotes. To not require
surrounding the input strings with double quotes (`"`s), use the `-d` option.

If you use `-N` it ignores all newlines in input. This does not mean that the
JSON allows for unescaped newlines but rather newlines on the command line are
ignored, as if the args are concatenated. Use of this option allows one to pipe
a command to the program without getting an error message, for instance:

```sh
echo foo bar | jstrdecode -N
```

or

```sh
printf "foo\nbar" | jstrdecode -N
```

A more complicated example is given in the examples below.

If you need to not have a trailing newline in the output, use the `-n` option.

`-E` is kind of undocumented but kind of documented: play with it to see what it
does, should you wish!  :-)

Running it with `-t` performs some sanity checks on JSON decode/encode
functionality and this is not usually needed, although the test suite does do
this.

If no string is given on the command line it expects you to type something on
`stdin`, ending it with EOF.



<div id="jstrdecode-examples"></div>

## jstrdecode examples

### Decode `"\"\""`:


```sh
jstrdecode '"\"\""'
```

This will show:

```
""
```

### Ignore newlines in input:

```sh
printf '"foo\nbar"' | jstrdecode -N

echo '"foo bar"' | jstrdecode -N
```

Note that the above two commands will result in different output.

### Enclose output in double quotes, not requiring double quotes:

```sh
jstrdecode -d -Q foo bar baz
```

### Enclose each decoded string with escaped quotes:

```sh
jstrdecode -e '"foo bar"' '"baz"'
```

This will show:

```
\"foo bar\"\"baz\"
```

### Enclose output with double quotes and each decoded string with escaped quotes:

```sh
jstrdecode -Q -e '"foo bar"' '"baz"'
```

This will show:

```
"\"foo bar\"\"baz\""
```



### Unicode examples:

Palaeontologists might like to try `U+1F996` and `U+F995`:

```sh
jstrdecode '"\uD83E\uDD96\uD83E\uDD95"'
```

which will show a T-Rex and a Sauropod (includes Brontosaurus, Diplodocus,
Brachiosaurus):

```
🦖🦕
```

whereas fantasy lovers might like to try `U+1F409`, `U+1FA84` and `U+1F9D9`:

```sh
jstrdecode '"\uD83D\uDC09\uD83E\uDE84\uD83E\uDDD9"'
```

which will show a dragon, a wand and a mage:

```
🐉🪄🧙
```

whereas volcanologists might like to try `U+1F30B`:

```sh
jstrdecode '"\uD83C\uDF0B"'
```

which will show a volcano:


```
🌋
```

whereas alcoholics :-) might like to try `U+1F37B`:

```sh
jstrdecode '"\uD83C\uDF7B"'
```

which will show clinking beer mugs:

```
🍻
```

assuming, in all cases, your system supports displaying such emojis.

Of course you may also use this program to decode diacritics and characters from
other languages.


### More jstrdecode information and examples:

For more information and examples, see the man page:

```sh
man ./man/man1/jstrdecode.1
```

from the repo directory, or if installed:

```sh
man jstrdecode
```

**NOTE**: After doing a `make all`, this tool may be found as: `./jstrdecode`.
If you run `make install` (as root or via sudo) you can just do: `jstrdecode`.

<div id="jstrdecode-exit-codes"></div>

## jstrdecode exit codes

If the decoding is successful the exit status of `jstrdecode` is 0, otherwise 1.
Different non-zero values are for different error conditions, or help or version
string printed.

<div id="jstrencode"></div>

# jstrencode: a tool to convert JSON decoded strings to normal strings

This tool converts data into JSON encoded strings according to the so-called
[JSON data interchange syntax -
ECMA-404](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/).

Something to note is that this command will not convert an emoji or Unicode
character to its `\uxxxx` form. In fact, for reasons we do not comprehend, this
is perfectly valid JSON:

```json
{ "🔥" "🐉" }
```


<div id="jstrencode-synopsis"></div>

## jstrencode synopsis:


```sh
jstrencode [-h] [-v level] [-q] [-V] [-t] [-n] [-N] [-Q] [-e] [-E level] [arg ...]
```

Unlike the `jparse` utility, no JSON parsing functions are called, so there is
no `-J level` option.

Use `-Q` if you do not want to encode double quotes that enclose the
concatenation of the args.

If you use `-N` it ignores all newlines in input. This does not mean that the
JSON allows for unescaped newlines but rather newlines on the command line are
ignored, as if the args are concatenated. Use of this option allows one to pipe
a command to the program without getting, for example, a `\n` at the end of the
output. For instance:

```sh
echo foo bar | jstrencode -N
```

or

```sh
printf "foo\nbar\n" | jstrencode -N
```

If you need to not have a trailing newline in the output, use the `-n` option.

Use `-e` to not output double quotes that enclose each arg.

`-E` is kind of undocumented but kind of documented: play with it to see what it
does, should you wish!  :-)

Running it with `-t` performs some sanity checks on JSON decode/encode
functionality and this is not usually needed, although the test suite does do
this.

If no string is given on the command line it expects you to type something on
`stdin`, ending it with EOF.


<div id="jstrencode-examples"></div>


## jstrencode examples

### Decode `""`:


```sh
jstrencode '""'
```

This will show:

```
\"\"
```

### Decode `"\"`:

```sh
jstrencode '"\"'
```

That will show:

```
\"\\\"
```


### Ignore newlines in input:

If you need to ignore newlines in input, use the `-N` option. For example, here
is the same command with and without the `-N` option:

```sh
$ echo '"\"' | jstrencode -N
\"\\\"

$ echo '"\"' | jstrencode
\"\\\"\n
```

### Skip double quotes that enclose each arg concatenation

```sh
jstrencode -Q '"foo"bar"' baz
```

This will show:

```
foo\"bar\"baz
```

instead of:

```
\"foo\"bar\"baz
```


### Skip double quotes that enclose each arg:

```sh
jstrencode -e '"foo"bar"' 'baz'
```

This will show:

```
foo\"barbaz
```

instead of:

```
\"foo\"bar\"baz
```


A more convoluted example:


```sh
jstrencode -e '"foo"\"bar"' 'baz'
```

This will show:

```
foo\"\\\"barbaz
```

instead of:

```
\"foo\"\\\"bar\"baz
```


### Skip double quotes that enclose the arg concatenation:

```sh
jstrencode -Q '"foo bar"' 'baz'
```

This will show:

```
foo bar\"baz
```

instead of:

```
\"foo bar\"baz
```

### More jstrencode information and examples

For more information and examples, see the man page:

```sh
man ./man/man1/jstrencode.1
```

from the repo directory, or if installed:

```sh
man jstrencode
```


**NOTE**: After doing a `make all`, this tool may be found as: `./jstrencode`.
If you run `make install` (as root or via sudo) you can just do: `jstrencode`.


<div id="jstrencode-exit-codes"></div>

## jstrencode exit codes

If the encoding is successful the exit status of `jstrencode` is 0, otherwise 1.
Different non-zero values are for different error conditions, or help or version
string printed.




<!--
## XXX - the below is for tools that have not yet been written      - XXX ##
## XXX - uncomment the below once the tools have been written and   - XXX ##
## XXX - documented                                                 - XXX ##

This document was prior to setting up this repo and it describes tools to be
written (that were being worked on until one of us got sick). As such these
tools have **NOT** been written yet. Additionally there might be some changes
in how they work, when it is time to write them. As well, another utility is
going to be created. All of these will happen at a later time as there
are other things that are of higher priority. When the tools exist then this
note will be either removed (if the tools are complete) or modified (if the
tools are still being worked on but do exist).


## Introduction to this document

**NOTE**: Please see [json_README.md](./json_README.md) for some important **JSON
terms** used in this document.

There is a general lack of JSON aware command line tools that allow someone to
obtain information from within a JSON document.  By "obtaining information" we
refer to the ability to extract (and if desired, format) data, either as a whole
or in part, that is contained within a [JSON
document](./json_README.md#json-document).

There does exist multiple [JSON](./json_README.md) [APIs (Application Program
Interfaces)](https://en.wikipedia.org/wiki/API) for various programming
languages such as: JavaScript, C, Java, Python, Perl, Go, Rust, PHP, etc.  This
[jparse repo](https://github.com/xexyl/jparse/) is
a rich JSON parser API C programs can use, as well as a semantic JSON check
interface for C.  However all those APIs require the user to "program" in a
specific language in order to do something as simple as obtain a selective [JSON
values](./json_README.md#json-value) from a [JSON
document](./json_README.md#json-document). With these tools we attempt to
address some of these problems.



## Scope of this document

In this document, we will concern ourselves only with [Valid
JSON](./json_README.md#valid-json) in the form of a [JSON
document](./json_README.md#json-document), where a document can be a file or
stdin. It must be [valid JSON](./json_README.md#valid-json) only because we can
only extract information with any degree of certainty from [Valid
JSON](./json_README.md#valid-json).

We consider command line utilities that _read_ information from a [JSON
document](./json_README.md#json-document) in the form of either a file or stdin.
In this document _we are not_ considering utilities that either create or modify
JSON.

If a command line utility is given an **invalid JSON document**, the utility
shall exit non-zero with an error message indicating that the [JSON
document](./json_README.md#json-document) is not valid JSON.  Therefore we're
only concerned with reading various information from an existing [JSON
document](./json_README.md#json-document) or stdin that contains [Valid
JSON](./json_README.md#valid-json).


## Common command line options

In this document, we assume that all utilities have at least the following
options:

```
	-h		Print help and exit
	-V		Print version and exit
	-v level	Verbosity level (def: 0)
	-J level	JSON verbosity level (def: 0)
	-q		Quiet mode, do not print to stdout (def: print stuff to stdout)
	-L <num>[{t|s}]	Print JSON level followed by a number of tabs or spaces (def: don't print levels)
	-L tab		Alias for: '-L 1t'

			Trailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces.
			Not specifying 's' or 't' implies spaces.

	-l lvl		Print specific JSON levels (def: print at any level)

			If lvl is a number (e.g.: -l 3), level must == number.
			If lvl is a number followed by : (e.g. '-l 3:'), level must be >= number.
			If lvl is a : followed by a number (e.g. '-l :3'), level must be <= number.
			If lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range.

	-m		Maximum depth to traverse in the JSON tree
	-K		Run test mode and exit
	-o ofile	Output formatted JSON to ofile (def: standard output, - ==> standard output)
	-F fmt		Change the JSON format style (def: use default)

			tty		When output is to a TTY, use colour, otherwise use simple
			simple		Each line has one JSON level determined by '[]'s and '{}'s
			colour		Simple plus ANSI colour syntax highlighting
			color		Alias for colour (colour excluding you :-) )
			1line		One line output
			nows		One line output, no extra whitespace

```

Additional command line options will be added on a utility by utility basis.



## Common exit codes

In this document, we assume that all utilities have at least the following exit
codes:

```
Exit codes:
    0	all is OK and file is valid JSON
    1	error writing to output file (if -o specified)
    2	-h and help string printed or -V and version string printed
    3	invalid command line, invalid option or option missing an argument
    4	file does not exist, not a file, or unable to read the file
    5	file contents is not valid JSON
    6	test mode failed
...
 >=10	internal error
```

Additional exit codes will be added on a utility by utility basis.
-->
